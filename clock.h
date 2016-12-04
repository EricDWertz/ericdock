#pragma once
/*
 * Clock drawing functions
 */
#include <gtk/gtk.h>

#include "eric_window.h"

extern double clock_alpha;
extern double clock_height;
extern int displaymode;
extern char* weekday_names[7];
extern char* month_names[12];

gboolean clock_refresh(gpointer data);
void clock_init( gpointer window );
void clock_draw_timestring(cairo_t* cr, double x, double y, int blurpass);
void clock_draw( cairo_t* cr, double x, double y, eric_window* w );
