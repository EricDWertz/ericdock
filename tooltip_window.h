#pragma once

#include <gtk/gtk.h>
#include "eric_window.h"

#include <pango/pango-context.h>

extern eric_window* tooltip_window;

void tooltip_window_hide();
gboolean tooltip_window_lose_focus( GtkWidget* widget, GdkEvent* event, gpointer user );
void tooltip_window_mouse_down( GtkWidget* widget, GdkEvent* event, gpointer user );
gboolean tooltip_window_mouse_move( GtkWidget* widget, GdkEvent* event, gpointer user );
gboolean tooltip_window_draw( GtkWidget* widget, cairo_t* cr, eric_window* w );
void tooltip_window_create( GtkWidget* dock_window );
void tooltip_window_update_geometry();
void tooltip_window_show();
PangoContext* tooltip_window_get_pango_context();
void tooltip_window_clear_pager_list();
