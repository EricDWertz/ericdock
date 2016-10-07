#pragma once
/*
 * dock_icon - Represents an icon on the dock
 * holds pager_items
 *
 * renders/handles events based on how many pager_items it contains
 */
#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

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

#include "tooltip_window.h"

extern dock_icon* tooltip_window_icon;

dock_icon* dock_icon_create( WnckClassGroup* class_group )
{
    dock_icon* icon = malloc( sizeof( dock_icon ) );
    icon->class_group = class_group;
    icon->icon_pixbuf = wnck_class_group_get_icon( class_group );
    icon->pager_items = NULL;
    icon->icon_state = ICON_STATE_NORMAL;

    icon->width = SCALE_VALUE( BAR_HEIGHT - 6.0 );
    icon->height = SCALE_VALUE( BAR_HEIGHT );

    icon->selected_index = 0;

    return icon;
}

//Goes to the next pager item, looping back to the start if necessary
pager_item* dock_icon_get_next_pager_item( dock_icon* icon )
{
    pager_item* item;

    icon->selected_index++;
    if( icon->selected_index >= g_list_length( icon->pager_items ) )
    {
        icon->selected_index = 0;
    }

    item = g_list_nth_data( icon->pager_items, icon->selected_index );

    return item;
}

void dock_icon_clear_pager_item_state( dock_icon* icon )
{
    GList* pager_list;
    pager_item* item;

    for( pager_list = icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
    {
        item = pager_list->data;
        item->icon_state = ICON_STATE_NORMAL;
    }
}

/* Event whenever a dock icon is clicked or selected via keyboard
 * Will switch to app, or if more than one pager_items, show menu
 */
void dock_icon_activate( dock_icon* icon, Time time, int from_click )
{
    if( !icon->pager_items )
        return;

    if( g_list_length( icon->pager_items ) > 1 )
    {
        pager_item* item;

        dock_icon_clear_pager_item_state( icon );

        if( tooltip_window_icon != icon )
        {
            tooltip_window_icon = icon;
            tooltip_window_show();
            icon->selected_index = 0;
            item = (pager_item*)icon->pager_items->data;
        }
        else
        {
            item = dock_icon_get_next_pager_item( icon );
        }

        if( !from_click )
        {
            item->icon_state = ICON_STATE_HOVER;
            wnck_window_activate( item->window, time );
            gtk_widget_queue_draw( tooltip_window->window );
        }
    }
    else
    {
        //Activiate the only one we have
        pager_item* item = (pager_item*)icon->pager_items->data;
        wnck_window_activate( item->window, time );
        tooltip_window_hide();
    }

    if( icon->icon_state == ICON_STATE_ALERT )
        icon->icon_state = ICON_STATE_NORMAL;
}

void dock_icon_mouse_down( dock_icon* icon, double mx, double my, Time time )
{
    double it, ib, il, ir;

    il = icon->x; ir = icon->x + icon->width;
    it = icon->y; ib = icon->y + icon->height; 

    if( il < mx && mx < ir && it < my && my < ib )
    {
        dock_icon_activate( icon, time, TRUE );
    }
}
