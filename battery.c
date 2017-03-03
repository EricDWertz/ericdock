#include <stdlib.h>

#include "ericdock.h"
#include "battery.h"

//Battery stuff
typedef struct {
    double charge_percent;
    int status;
} Battery;

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
    if( strcmp( "Charging\n", buffer ) == 0 ) bat->status = 1;
    if( strcmp( "Discharging\n", buffer ) == 0 ) bat->status = 0;

    fclose( fp_charge_full );
    fclose( fp_charge_now );
    fclose( fp_status );
}

void draw_battery_icon( cairo_t* cr, double x, double y, double height )
{
    Battery bat;
    getbatteryinfo( &bat );
    //bat.charge_percent * 100.0
    //bat.status == 1 means charging

    cairo_set_line_width( cr, 2.0 );
    cairo_rectangle( cr, x+height*0.375, y+height*0.125, height*0.25, height*0.125 );
    cairo_rectangle( cr, x+height*0.25, y+height*0.25, height*0.5, height*0.75 );
    cairo_stroke( cr );

    if( bat.charge_percent <= 0.15 )
        cairo_set_source_rgb( cr, 0.9, 0.0, 0.0 );

    cairo_rectangle( cr, x+height*0.25+2.0, y+height*0.25+(height*0.75*(1.0-bat.charge_percent))+2.0, height*0.5-4.0, height*0.75*bat.charge_percent-4.0  );
    cairo_fill( cr );
}

