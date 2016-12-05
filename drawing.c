#include "drawing.h"
#include <math.h>

void draw_rounded_rect(cairo_t* cr,double x,double y,double w,double h,double r)
{
	cairo_move_to(cr,x+r,y);
	cairo_line_to(cr,x+w-r*2,y);
	cairo_arc(cr,x+w-r,y+r,r,-M_PI/2.0,0);
	cairo_line_to(cr,x+w,y+h-r*2);
	cairo_arc(cr,x+w-r,y+h-r,r,0,M_PI/2.0);
	cairo_line_to(cr, x+r,y+h);
	cairo_arc(cr,x+r,y+h-r,r,M_PI/2.0,M_PI);
	cairo_line_to(cr, x, y+r);
	cairo_arc(cr,x+r,y+r,r,M_PI,-M_PI/2.0);
	cairo_close_path(cr);
}

