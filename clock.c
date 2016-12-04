/*
 * Clock drawing functions
 */
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "ericdock.h"
#include "clock.h"

double clock_alpha = 0.75;
double clock_height = 0.18;

int displaymode=0;

char* weekday_names[7]=
{
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

char* month_names[12]=
{
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

int oldsec;
gboolean clock_refresh(gpointer data)
{
    time_t rawtime;
    time ( &rawtime );
    struct tm * timeinfo;

    timeinfo = localtime ( &rawtime );
    if( timeinfo->tm_sec != oldsec );
        gtk_widget_queue_draw( GTK_WIDGET( data ) );

    return TRUE;
}

void clock_init( gpointer window )
{
    g_timeout_add_seconds(1,clock_refresh, window);
}

void clock_draw_timestring(cairo_t* cr, double x, double y, int blurpass)
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    //tm_sec tm_min tm_hour
    int hour=timeinfo->tm_hour;
    int minute=timeinfo->tm_min;
    int second=timeinfo->tm_sec;
    //hour+=minute/60;
    char ampm[3]="am";
    if(hour>=12) 
    {
    	hour-=12;
    	ampm[0]='p';
    }
    if(hour==0)
    {
    	hour=12;
    }
    oldsec = timeinfo->tm_sec;
    
    if(blurpass==1) cairo_translate(cr,-2.0,0);
    
    cairo_text_extents_t extents;
    double text_x = x;
    char timestring[64];
    cairo_set_font_face(cr,cairo_toy_font_face_create("Source Sans Pro",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL));
    
    cairo_set_font_size(cr, SCALE_VALUE( clock_height ) );
    sprintf(timestring,"%i:%02i%s",hour, minute, ampm);
    cairo_text_extents(cr,timestring,&extents);
    text_x-=extents.x_advance;
    cairo_move_to(cr,text_x, y - SCALE_VALUE( 4.0 ) );
    cairo_text_path(cr,timestring);

    sprintf(timestring,"%i/%02i/%02i", 
            timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_year + 1900 );
    cairo_text_extents(cr,timestring,&extents);
    text_x = x - extents.x_advance;
    cairo_move_to(cr,text_x, y - extents.y_bearing + SCALE_VALUE( 4.0 ) );
    cairo_text_path(cr,timestring);

    //sprintf(timestring,"%s, %s %i, %i",weekday_names[timeinfo->tm_wday],month_names[timeinfo->tm_mon],timeinfo->tm_mday,timeinfo->tm_year+1900);
    //cairo_text_extents(cr,timestring,&extents);
    //text_x= x - extents.x_advance;
    //cairo_move_to(cr,text_x, y + SCALE_VALUE( 32.0 ) );
    //cairo_text_path(cr,timestring);
    
    //cairo_set_font_size(cr,32.0);
    //sprintf(timestring,"%i:%02i",hour,minute);
    //cairo_text_extents(cr,timestring,&extents);
    //text_x-=extents.x_advance;
    //cairo_move_to(cr,text_x, y + 16.0 );
    //cairo_text_path(cr,timestring);
    
    
    
    //cairo_set_font_size(cr,16.0);
    
    if(blurpass==1) 
    {
    	cairo_set_line_width(cr,4.0);
    	cairo_stroke_preserve(cr);
    }
    cairo_fill(cr);
}

void clock_draw( cairo_t* cr, double x, double y, eric_window* w )
{
    //cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, clock_alpha);
    //if(clock_alpha!=0) draw_clock(cr,2.0);  
    
    w->text_color.red = 1.0 - w->text_color.red;
    w->text_color.green = 1.0 - w->text_color.green;
    w->text_color.blue = 1.0 - w->text_color.blue;
    w->text_color.alpha = 0.25 * clock_alpha;
    gdk_cairo_set_source_rgba( cr, &w->text_color );
    clock_draw_timestring( cr, x + SCALE_VALUE( 1.0 ), y + SCALE_VALUE( 1.0 ), 0 );

    w->text_color.red = 1.0 - w->text_color.red;
    w->text_color.green = 1.0 - w->text_color.green;
    w->text_color.blue = 1.0 - w->text_color.blue;
    w->text_color.alpha = clock_alpha;
    gdk_cairo_set_source_rgba( cr, &w->text_color );
    clock_draw_timestring( cr, x, y, 0 );
}
