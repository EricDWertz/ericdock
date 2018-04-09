#pragma once
/*
 * dock_icon - Represents an icon on the dock
 * holds pager_items
 *
 * renders/handles events based on how many pager_items it contains
 */
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <X11/Xlib.h>
#include <time.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "pager_item.h"
#include "eric_window.h"
#include "dock_objects.h"

dock_icon* dock_icon_create( WnckWindow* window, eric_window* w );
pager_item* dock_icon_get_next_pager_item( dock_icon* icon );
void dock_icon_clear_pager_item_state( dock_icon* icon );
void dock_icon_activate( dock_icon* icon, Time time, int from_click );
void dock_icon_mouse_down( dock_icon* icon, double mx, double my, Time time );
void dock_icon_remove( dock_icon* icon );
GdkPixbuf* get_icon( WnckWindow* window, guint size );
char * strip_extension (const char *file);
void dock_icon_refresh_icon( dock_icon* icon, pager_item* item );
