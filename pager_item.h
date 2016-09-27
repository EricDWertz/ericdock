#pragma once
/*
 * pager_item - held by the dock_icon object
 */

#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "drawing.h"

//Structure to hold actual pager items
typedef struct
{
    WnckWindow* window;
    char name[256];
    GdkPixbuf* icon_pixbuf;
    double x, y;
    double width, height;
    double text_height;
    int icon_state;
} pager_item;

void pager_item_name_changed( WnckWindow* window, pager_item* item )
{
    strcpy( item->name, wnck_window_get_name( window ) );
}

void pager_item_icon_changed( WnckWindow* window, pager_item* item )
{
    item->icon_pixbuf = wnck_window_get_mini_icon( window );
}

pager_item* pager_item_create( WnckWindow* window )
{
    pager_item* item = malloc( sizeof( pager_item ) );
    item->window = window;
    strcpy( item->name, wnck_window_get_name( item->window ) );
    item->icon_pixbuf = wnck_window_get_mini_icon( item->window );
    item->icon_state = ICON_STATE_NORMAL;

    g_signal_connect( G_OBJECT( window ), "name-changed", G_CALLBACK( pager_item_name_changed ), (gpointer)item );
    g_signal_connect( G_OBJECT( window ), "icon-changed", G_CALLBACK( pager_item_name_changed ), (gpointer)item );

    item->width = SCALE_VALUE( 320 );
    item->height = SCALE_VALUE( 24.0 );
    item->text_height = SCALE_VALUE( 16.0 );

    return item;
}

/* Returns bool if state has changed */
int pager_item_mouse_move( pager_item* item, double mx, double my )
{
    double it, ib, il, ir;
    int old_state, state_changed;

    old_state = item->icon_state;

    il = item->x; ir = item->x + item->width;
    it = item->y; ib = item->y + item->height; 

    if( il < mx && mx < ir && it < my && my < ib )
    {
        item->icon_state = ICON_STATE_HOVER;
    }
    else
    {
        item->icon_state = ICON_STATE_NORMAL;
    }

    return old_state != item->icon_state;
}

void pager_item_draw( pager_item* item, cairo_t* cr, eric_window* w, cairo_pattern_t* pattern )
{
    if( item->icon_state == ICON_STATE_HOVER )
    {
        w->text_color.alpha = 0.25;
        gdk_cairo_set_source_rgba( cr, &w->text_color );
        draw_rounded_rect( cr, item->x, item->y, 
                item->width, item->height, SCALE_VALUE( 2.0 ) );
        cairo_fill( cr );
    }

    gdk_cairo_set_source_pixbuf( cr, item->icon_pixbuf, item->x, item->y + SCALE_VALUE( 6.0 ) );
    cairo_paint( cr );

    cairo_set_source( cr, pattern );
    cairo_move_to( cr, item->x + SCALE_VALUE( 20.0 ), item->y + item->text_height  );
    cairo_text_path( cr, item->name );

    cairo_fill( cr );

}