/*
 * pager_item - held by the dock_icon object
 */

#include "pager_item.h"

#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <pango/pangocairo.h>

#include "ericdock.h"
#include "drawing.h"
#include "dock_icon.h"
#include "tooltip_window.h"

void pager_item_name_changed( WnckWindow* window, pager_item* item )
{
    PangoRectangle rect;

    strcpy( item->name, wnck_window_get_name( window ) );
    if( item->layout != NULL )
    {
        pango_layout_set_text( item->layout, item->name, strlen( item->name ) );
        pango_layout_get_pixel_extents( item->layout, NULL, &rect );
        item->text_height = rect.height;
    }
}

void pager_item_icon_changed( WnckWindow* window, pager_item* item )
{
    if( item->icon_pixbuf != NULL && !GDK_IS_PIXBUF( item->icon_pixbuf ) )
    {
        item->icon_pixbuf = NULL;
    }

    printf( "Icon changed %s\n", item->name );
    item->icon_pixbuf = get_icon( window, (int)SCALE_VALUE( 16.0 ) );
    dock_icon_refresh_icon( item->parent, item );
}

void pager_item_state_changed( WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, pager_item* item )
{
    printf( "Item state changed %s\n", item->name );
    //item->icon_pixbuf = wnck_window_get_mini_icon( window );
}

gboolean pager_item_leave( GtkWidget* widget, GdkEvent* event, pager_item* item )
{
    item->icon_state = ICON_STATE_NORMAL;
    gtk_widget_queue_draw( widget );

    return FALSE;
}

gboolean pager_item_motion( GtkWidget* widget, GdkEvent* event, pager_item* item )
{
    item->icon_state = ICON_STATE_HOVER;
    gtk_widget_queue_draw( widget );

    return FALSE;
}

static gboolean pager_item_draw( GtkWidget* widget, cairo_t* cr, pager_item* item )
{
    double text_y;

    if( !GDK_IS_PIXBUF( item->icon_pixbuf ) )
    {
        printf( "Attempting to draw pager item without icon\n" );
        item->icon_pixbuf = get_icon( item->window, (int)SCALE_VALUE( 16.0 ) );
    }

    if( item->icon_state == ICON_STATE_HOVER )
    {
        item->w->text_color.alpha = 0.25;
        gdk_cairo_set_source_rgba( cr, &item->w->text_color );
        draw_rounded_rect( cr, 0, 0, 
                item->width, item->height, SCALE_VALUE( 2.0 ) );
        cairo_fill( cr );
    }

    gdk_cairo_set_source_pixbuf( cr, item->icon_pixbuf, 0, 0 + SCALE_VALUE( 4.0 ) );
    cairo_paint( cr );

    //Draw text
    text_y = ( SCALE_VALUE(24.0) - item->text_height ) / 2.0;

    //Shadow Pass
    item->w->text_color.alpha = 1.0;
    cairo_set_source_rgba(cr,( 1.0 - item->w->text_color.red ), 
        ( 1.0 - item->w->text_color.green ),
        ( 1.0 - item->w->text_color.blue ),
        0.25);
    cairo_move_to( cr, SCALE_VALUE( 21.0 ), text_y + SCALE_VALUE( 1.0 ) );
    pango_cairo_layout_path( cr, item->layout );
    cairo_fill( cr );

    //Final Pass
    gdk_cairo_set_source_rgba( cr, &item->w->text_color );
    cairo_move_to( cr, SCALE_VALUE( 20.0 ), text_y );
    pango_cairo_layout_path( cr, item->layout );
    cairo_fill( cr );

    item->w->text_color.alpha = 1.0;

    return TRUE;
}

pager_item* pager_item_create( WnckWindow* window, dock_icon* parent )
{
    pager_item* item = malloc( sizeof( pager_item ) );
    item->window = window;
    item->parent = parent;
    strcpy( item->name, wnck_window_get_name( item->window ) );

    item->icon_state = ICON_STATE_NORMAL;
    
    item->icon_pixbuf = NULL;
    item->button = NULL;
    item->layout = NULL;

    g_signal_connect( G_OBJECT( window ), "name-changed", G_CALLBACK( pager_item_name_changed ), (gpointer)item );
    g_signal_connect( G_OBJECT( window ), "icon-changed", G_CALLBACK( pager_item_icon_changed ), (gpointer)item );
    g_signal_connect( G_OBJECT( window ), "state-changed", G_CALLBACK( pager_item_state_changed ), (gpointer)item );

    item->text_height = SCALE_VALUE( 16.0 );

    //Append to the icon
    parent->pager_items = g_list_append( parent->pager_items, item );

    return item;
}

static void pager_item_clicked( GtkButton* button, pager_item* item )
{
    wnck_window_activate( item->window, gdk_x11_get_server_time( gtk_widget_get_window( GTK_WIDGET( button ) ) ) );
    tooltip_window_hide();
}

GtkWidget* pager_item_create_widget( pager_item* item, int width, int height )
{
    PangoRectangle rect;

    if( GTK_IS_WIDGET( item->button ) )
    {
        gtk_widget_destroy( item->button );
    }

    item->width = width;
    item->height = height;
    item->button = gtk_button_new();

    if( !GDK_IS_PIXBUF( item->icon_pixbuf ) )
    {
        item->icon_pixbuf = get_icon( item->window, (int)SCALE_VALUE( 16.0 ) );
    }
    
    gtk_widget_set_size_request( item->button, item->width, item->height );
    gtk_widget_add_events( item->button, GDK_POINTER_MOTION_MASK
            | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK
            | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK );
    gtk_widget_set_app_paintable( item->button, TRUE );
    g_signal_connect( G_OBJECT( item->button ), "draw", G_CALLBACK( pager_item_draw ), (void*)item );
    g_signal_connect( G_OBJECT( item->button ), "clicked", G_CALLBACK( pager_item_clicked ), (void*)item );
    g_signal_connect( G_OBJECT( item->button ), "leave-notify-event", G_CALLBACK( pager_item_leave ), (void*)item );
    g_signal_connect( G_OBJECT( item->button ), "motion-notify-event", G_CALLBACK( pager_item_motion ), (void*)item );

    //Pango stuff
    item->layout = pango_layout_new( tooltip_window_get_pango_context() );
    pango_layout_set_wrap( item->layout, PANGO_WRAP_WORD );
    pango_layout_set_auto_dir( item->layout, FALSE );
    pango_layout_set_width( item->layout, ( item->width - SCALE_VALUE( 20.0 ) ) * PANGO_SCALE ); //Have to remove space for icon
    pango_layout_set_height( item->layout, item->height * PANGO_SCALE );
    pango_layout_set_ellipsize( item->layout, PANGO_ELLIPSIZE_END );

    pango_layout_set_text( item->layout, item->name, strlen( item->name ) );
    pango_layout_get_pixel_extents( item->layout, NULL, &rect );
    item->text_height = rect.height;

    return item->button;
}

void pager_item_destroy_widget( pager_item* item )
{
    if( GDK_IS_PIXBUF( item->icon_pixbuf ) )
    {
        g_object_unref( item->icon_pixbuf );
        item->icon_pixbuf = NULL;
    }
    //TODO: Destroy pango?
    item->layout = NULL;
    gtk_widget_destroy( item->button );
    item->button = NULL;
}
