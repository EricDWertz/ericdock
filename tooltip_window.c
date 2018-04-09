#include "ericdock.h"

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "tooltip_window.h"
#include "drawing.h"

eric_window* tooltip_window = NULL;
dock_icon* tooltip_window_icon = NULL;

GtkWidget* pager_items_box;
PangoContext* pango_context;

gboolean tooltip_window_lose_focus( GtkWidget* widget, GdkEvent* event, gpointer user )
{
    tooltip_window_hide();
    return TRUE;
}

PangoContext* tooltip_window_get_pango_context()
{
    return pango_context;
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

    //Add Dock icon box
    pager_items_box = gtk_button_box_new( GTK_ORIENTATION_VERTICAL );
    gtk_button_box_set_layout( GTK_BUTTON_BOX( pager_items_box ), GTK_BUTTONBOX_CENTER );
    //gtk_button_box_set_child_non_homogeneous( GTK_BUTTON_BOX( dock_icons_box ), TRUE );
    gtk_container_add( GTK_CONTAINER( w->window ), pager_items_box );
    gtk_widget_show( pager_items_box );

    //Pango stuff
    char fontdesc[80];
    pango_context = gtk_widget_create_pango_context( w->window );
    sprintf( fontdesc, "Source Sans Pro Regular %ipx", (int)(SCALE_VALUE( 16 )) );
    PangoFontDescription* font = pango_font_description_from_string( fontdesc );
    pango_context_set_font_description( pango_context, font );

    tooltip_window = w;
}


void tooltip_window_update_geometry()
{
    int pager_count;
    int wx, wy;

    pager_count = g_list_length( tooltip_window_icon->pager_items ); 

    gtk_window_get_position( GTK_WINDOW( dock_window->window ), &wx, &wy );
    gtk_window_resize( GTK_WINDOW( tooltip_window->window ), SCALE_VALUE(ERIC_DOCK_TOOLTIP_WIDTH), SCALE_VALUE( pager_count * 22 ) );
    //TODO: Move window to appropriate dock icon here!
}

void tooltip_window_show()
{
    GList* pager_list;
    pager_item* item;
    GtkWidget* button;

    tooltip_window_update_geometry();
    gtk_widget_show_all( tooltip_window->window );
    gtk_widget_queue_draw( tooltip_window->window );

    for( pager_list = tooltip_window_icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
    {
        item = (pager_item*)pager_list->data;
        button = pager_item_create_widget( item, SCALE_VALUE( ERIC_DOCK_TOOLTIP_WIDTH ), SCALE_VALUE( 22 ));
        item->w = tooltip_window;

        gtk_container_add( GTK_CONTAINER( pager_items_box ), button );
        gtk_widget_show( button );
    }
}

void tooltip_window_clear_pager_list()
{
    GList* pager_list;
    pager_item* item;

    if( tooltip_window_icon != NULL )
    {
        //Destroy any pager item widgets
        for( pager_list = tooltip_window_icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
        {
            item = (pager_item*)pager_list->data;
            pager_item_destroy_widget( item );
        }
    }
}

void tooltip_window_hide()
{
    tooltip_window_clear_pager_list();

    tooltip_window_icon = NULL;
    gtk_widget_hide( tooltip_window->window );
}
