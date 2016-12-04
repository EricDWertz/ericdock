#include "tooltip_window.h"
#include "ericdock.h"

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "dock_icon.h"
#include "drawing.h"

eric_window* tooltip_window = NULL;
dock_icon* tooltip_window_icon = NULL;

void tooltip_window_hide()
{
    tooltip_window_icon = NULL;
    gtk_widget_hide( tooltip_window->window );
}

gboolean tooltip_window_lose_focus( GtkWidget* widget, GdkEvent* event, gpointer user )
{
    tooltip_window_hide();
    return TRUE;
}

void tooltip_window_mouse_down( GtkWidget* widget, GdkEvent* event, gpointer user )
{
    GdkEventButton* e = (GdkEventButton*)event;
    if( e->button != 1 )
        return;

    double mx, my;
    double it, ib, il, ir;
    dock_icon* icon;
    pager_item* item;
    GList* pager_list;

    mx = e->x; my = e->y;
    for( pager_list = tooltip_window_icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
    {
        item = pager_list->data;
        if( my > item->y && my < item->y + SCALE_VALUE( 24.0 ) )
        {
            wnck_window_activate( item->window, e->time );
            tooltip_window_hide();
        }
    }
}

gboolean tooltip_window_mouse_move( GtkWidget* widget, GdkEvent* event, gpointer user )
{
    double mx, my;
    double it, ib, il, ir;
    int old_state, new_state, state_changed;
    dock_icon* icon;
    pager_item* item;
    GList *icon_list, *item_list;
    GdkEventMotion* e = (GdkEventMotion*)event;

    mx = e->x;
    my = e->y;

    for( item_list = tooltip_window_icon->pager_items; item_list != NULL; item_list = item_list->next )
    {
        item = item_list->data;


        if( pager_item_mouse_move( item, mx, my ) )
            state_changed = TRUE;
    }

    if( state_changed )
        gtk_widget_queue_draw( tooltip_window->window );

    return FALSE;
}

gboolean tooltip_window_draw( GtkWidget* widget, cairo_t* cr, eric_window* w )
{
    double x, y;
    GList* pager_list;
    pager_item* item;
    cairo_pattern_t* pattern;

    cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
    w->text_color.alpha = 1.0;
    cairo_set_font_face( cr, cairo_toy_font_face_create( ERIC_DOCK_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL ) );
    cairo_set_font_size( cr, SCALE_VALUE( 12.0 ) );

    x = SCALE_VALUE( 5.0 ); //margin-left
    y = SCALE_VALUE( 2.0 ); //margin-top
    pattern = cairo_pattern_create_linear( SCALE_VALUE( ERIC_DOCK_TOOLTIP_WIDTH - 21.0 ), 0, SCALE_VALUE( ERIC_DOCK_TOOLTIP_WIDTH - 5.0 ), 0 );
    cairo_pattern_add_color_stop_rgba( pattern, 0.0, w->text_color.red, w->text_color.green, w->text_color.blue, 1.0 );
    cairo_pattern_add_color_stop_rgba( pattern, 1.0, w->text_color.red, w->text_color.green, w->text_color.blue, 0.0 );
    for( pager_list = tooltip_window_icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
    {
        item = (pager_item*)pager_list->data;

        item->x = x;
        item->y = y;
        pager_item_draw( item, cr, w, pattern );

        y += SCALE_VALUE( 24.0 );
    }

    cairo_pattern_destroy( pattern );
}



void tooltip_window_create( GtkWidget* dock_window )
{
    eric_window* w = eric_window_create( 10, 10, "" );
    gtk_window_move( GTK_WINDOW( w->window ), 0, 1040 );
    gtk_window_set_type_hint( GTK_WINDOW( w->window ), GDK_WINDOW_TYPE_HINT_DIALOG );
    gtk_window_set_decorated( GTK_WINDOW( w->window ), FALSE );
    gtk_window_set_skip_pager_hint( GTK_WINDOW( w->window ), TRUE );
    gtk_window_set_transient_for( GTK_WINDOW( w->window ), GTK_WINDOW( dock_window ) );
    gtk_window_set_keep_above( GTK_WINDOW( w->window ), TRUE );
    
    gtk_widget_add_events( w->window, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK );
    g_signal_connect( G_OBJECT( w->window ), "focus-out-event", G_CALLBACK(tooltip_window_lose_focus), NULL );
    g_signal_connect( G_OBJECT( w->window ), "motion-notify-event", G_CALLBACK(tooltip_window_mouse_move), NULL );
    g_signal_connect( G_OBJECT( w->window ), "button-press-event", G_CALLBACK(tooltip_window_mouse_down), NULL );
    w->draw_callback = tooltip_window_draw;

    tooltip_window = w;
}


void tooltip_window_update_geometry()
{
    int pager_count;
    int wx, wy;

    pager_count = g_list_length( tooltip_window_icon->pager_items ); 

    gtk_window_get_position( GTK_WINDOW( dock_window->window ), &wx, &wy );
    gtk_window_resize( GTK_WINDOW( tooltip_window->window ), SCALE_VALUE(ERIC_DOCK_TOOLTIP_WIDTH), SCALE_VALUE( pager_count * 24 ) );
    gtk_window_move( GTK_WINDOW( tooltip_window->window ), wx + (int)tooltip_window_icon->x, wy - (int)SCALE_VALUE( pager_count * 24 ) );
}

void tooltip_window_show()
{
    int wx, wy;
    int w, h;

    tooltip_window_update_geometry();
    gtk_widget_show_all( tooltip_window->window );
    gtk_widget_queue_draw( tooltip_window->window );
}
