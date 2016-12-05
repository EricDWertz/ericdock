#pragma once
/*
 * dock_icon - Represents an icon on the dock
 * holds pager_items
 *
 * renders/handles events based on how many pager_items it contains
 */
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <time.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "pager_item.h"

typedef struct
{
    WnckClassGroup* class_group;
    GdkPixbuf* icon_pixbuf;
    GList* pager_items;
    double x, y;
    double width, height;
    int icon_state;
    int is_active;
    int selected_index;
} dock_icon;

dock_icon* dock_icon_create( WnckClassGroup* class_group );
pager_item* dock_icon_get_next_pager_item( dock_icon* icon );
void dock_icon_clear_pager_item_state( dock_icon* icon );
void dock_icon_activate( dock_icon* icon, Time time, int from_click );
void dock_icon_mouse_down( dock_icon* icon, double mx, double my, Time time );
