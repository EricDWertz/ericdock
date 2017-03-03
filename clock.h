#pragma once
/*
 * Clock drawing functions
 */
#include <gtk/gtk.h>

#include "eric_window.h"

gboolean clock_refresh(gpointer data);
void clock_init( gpointer window );
double clock_draw_timestring(cairo_t* cr, double x, double y, int blurpass);
double clock_draw( cairo_t* cr, double x, double y, eric_window* w );
