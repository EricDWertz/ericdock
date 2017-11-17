/*
 * System control functions
 *
 * Battery Icon
 * Volume
 * Screen brightness
 */
#pragma once

void control_init( int* battery, int* auto_brightness );
//void control_update();
void draw_battery_icon( cairo_t* cr, double x, double y, double height );
