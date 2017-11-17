/*
 * System control functions
 *
 * Battery Icon
 * Volume
 * Screen brightness
 */
#include <stdlib.h>

#include "ericdock.h"
#include "control.h"
#include <math.h>

#define BRIGHTNESS_INTENSITY_FILE "/sys/bus/iio/devices/iio:device0/in_illuminance0_input"
#define BATTERY_CHARGE_FILE "/sys/class/power_supply/BAT0/charge_full"
#define KEYBOARD_BACKLIGHT_FILE "/sys/class/backlight/keyboard_backlight/brightness"

int brightness_support;

//Battery stuff
typedef struct {
    double charge_percent;
    int status;
    double dt;
    double dc;
} Battery;

//Brightness stuff
typedef struct {
    double scaled;
    double readings[10];
    double out;
    double target;
    double db;
    int moving;
} Brightness;

Brightness brightness;

void getbatteryinfo( Battery* bat )
{
    FILE* fp_charge_full;
    FILE* fp_charge_now;
    FILE* fp_status;
    int charge_full, charge_now;
    char buffer[256];

    fp_charge_full = fopen( "/sys/class/power_supply/BAT0/charge_full", "r" );
    fp_charge_now = fopen( "/sys/class/power_supply/BAT0/charge_now", "r" );
    fp_status = fopen( "/sys/class/power_supply/BAT0/status", "r" );

    fgets( buffer, 255, fp_charge_full );
    charge_full = atoi( buffer );
    fgets( buffer, 255, fp_charge_now );
    charge_now = atoi( buffer );
    fgets( buffer, 255, fp_status );

    bat->charge_percent = (double)charge_now / (double)charge_full;
    if( strcmp( "Full\n", buffer ) == 0 ) bat->status = 2;
    if( strcmp( "Charging\n", buffer ) == 0 ) bat->status = 1;
    if( strcmp( "Discharging\n", buffer ) == 0 ) bat->status = 0;

    fclose( fp_charge_full );
    fclose( fp_charge_now );
    fclose( fp_status );
}

void draw_battery_icon( cairo_t* cr, double x, double y, double height )
{
    static Battery bat;
    getbatteryinfo( &bat );
    //bat.charge_percent * 100.0
    //bat.status == 1 means charging

    cairo_set_line_width( cr, 2.0 );
    cairo_rectangle( cr, x+height*0.375, y+height*0.125, height*0.25, height*0.125 );
    cairo_rectangle( cr, x+height*0.25, y+height*0.25, height*0.5, height*0.75 );
    cairo_stroke( cr );

    if( bat.charge_percent <= 0.20 ) //Low battery
        cairo_set_source_rgb( cr, 0.9, 0.0, 0.0 );
    if( bat.status == 1 ) //Charging
        cairo_set_source_rgb( cr, 0.0, 0.5, 1.0 );
    if( bat.status == 2 ) //Full
        cairo_set_source_rgb( cr, 0.0, 0.7, 0.0 );

    cairo_rectangle( cr, x+height*0.25+2.0, y+height*0.25+(height*0.75*(1.0-bat.charge_percent))+2.0, height*0.5-4.0, height*0.75*bat.charge_percent-4.0  );
    cairo_fill( cr );
}

void add_brightness_reading( double r )
{
    int i;
    double avg;

    for( i = 9; i > 0; i-- )
        brightness.readings[i] = brightness.readings[i-1];
    brightness.readings[0] = r;

    avg = 0.0;
    for( i = 0; i < 10; i++ )
        avg += brightness.readings[i];

    brightness.scaled = avg/10.0;
    if( brightness.scaled < 0.15 )
        brightness.scaled = 0.15;
}

gboolean fade_brightness(gpointer data)
{
    brightness.out+=brightness.db;

    if( abs( brightness.out - brightness.target ) < 0.75 )
        brightness.out = brightness.target;
    brightness.moving = brightness.out != brightness.target;

    char buffer[80];
    sprintf( buffer, "xbacklight -set %f", brightness.out>2?brightness.out:2 );
    system( buffer );

    //printf( "current: %f, target: %f\n", brightness.out, brightness.target );

    return brightness.moving;
}

void update_auto_brightness()
{
    char buffer[256];
    FILE* f = fopen( BRIGHTNESS_INTENSITY_FILE, "r" );

    fgets( buffer, 255, f );
    add_brightness_reading( atof( buffer ) );

    //9.9323 ln(x) + 27.059
    brightness.target = (9.9323*log(brightness.scaled) + 27.059);

    if( brightness.target != brightness.out )
    {
        brightness.db = (brightness.target-brightness.out)/8;
        if( !brightness.moving )
        {
            brightness.moving = 1;
            g_timeout_add( 250, fade_brightness, NULL);
        }
    }

    fclose( f );
}

gboolean control_update(gpointer data)
{
    if(brightness_support)
        update_auto_brightness();

    return TRUE;
}

void control_init( int* battery, int* auto_brightness )
{
    FILE* f;

    //Check if there's a battery
    f = fopen( BATTERY_CHARGE_FILE, "r" );
    *battery = f != NULL;
    if( f )
        fclose(f);

    //Check for light sensor
    f = fopen( BRIGHTNESS_INTENSITY_FILE, "r" );
    brightness_support = f != NULL;
    if( auto_brightness )
        *auto_brightness = brightness_support;
    if( f )
        fclose(f);

    if( brightness_support )
    {
        int i;
        for( i = 0; i < 10; i++ )
            brightness.readings[i] = 5.0;
        brightness.moving = 0;
        brightness.out = 50;
    }

    g_timeout_add_seconds( 2, control_update, NULL);
}

