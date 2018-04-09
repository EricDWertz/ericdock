/*
 * dock_icon - Represents an icon on the dock
 * holds pager_items
 *
 * renders/handles events based on how many pager_items it contains
 */
#include "ericdock.h"
#include "dock_icon.h"
#include "tooltip_window.h"

#include <stdio.h>
#include <ctype.h>
#include <glib.h>
#include <gdk/gdkx.h>

//Refreshes the dock icon, if an item is provided try to use it otherwise use the top item
void dock_icon_refresh_icon( dock_icon* icon, pager_item* item )
{
    if( !icon->pager_items )
        return;

    pager_item* top_item = (pager_item*)icon->pager_items->data;

    if( item == NULL )
    {
        item = top_item;
    }

    if( GDK_IS_PIXBUF( icon->icon_pixbuf ) )
    {
        g_object_unref( icon->icon_pixbuf );
    }
    icon->icon_pixbuf = get_icon( item->window, (int)SCALE_VALUE( 32.0 ) );
}

static gboolean dock_icon_draw( GtkWidget* widget, cairo_t* cr, dock_icon* icon )
{
    int i, pager_count;
    double width, rx, ry;

    icon->w->text_color.alpha = 0.25;

    cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
    gdk_cairo_set_source_rgba( cr, &icon->w->text_color );
    if( icon->icon_state == ICON_STATE_HOVER )
    {
        draw_rounded_rect( cr, 0, 0, icon->width, icon->height, SCALE_VALUE( 2.0 ) );
        cairo_fill( cr );
    }
    if( icon->is_active )
    {
        draw_rounded_rect( cr, 0, 0, icon->width, icon->height, SCALE_VALUE( 2.0 ) );
        cairo_fill( cr );
    }

    if( !GDK_IS_PIXBUF( icon->icon_pixbuf ) )
    {
        printf( "Trying to draw without an icon\n" );
        dock_icon_refresh_icon( icon, NULL );
        return FALSE;
    }

    gdk_cairo_set_source_pixbuf( cr, icon->icon_pixbuf, ( icon->width - SCALE_VALUE( 32.0 ) ) / 2.0, SCALE_VALUE( 2.0 ) );
    cairo_paint( cr );

    pager_count = g_list_length( icon->pager_items );
    if( pager_count > 0 )
    {
        //Draw rectangles
        width = ( ( icon->width - SCALE_VALUE(8.0) ) / pager_count );  
        ry = SCALE_VALUE(36.0);
        icon->w->text_color.alpha = 0.5;

        //Shadow pass
        cairo_set_source_rgba( cr, 1.0 - icon->w->text_color.red, 1.0 - icon->w->text_color.green, 1.0 - icon->w->text_color.blue, icon->w->text_color.alpha*0.5 );
        rx = SCALE_VALUE(4.0);
        for( i = 0; i < pager_count; i++ )
        {
            cairo_rectangle( cr, rx+SCALE_VALUE( 2.0 ), ry+SCALE_VALUE( 1.0 ), width-SCALE_VALUE( 2.0 ), SCALE_VALUE( 4.0 ) );
            rx += width;
        }
        cairo_fill( cr );

        gdk_cairo_set_source_rgba( cr, &icon->w->text_color );
        rx = SCALE_VALUE(4.0);
        for( i = 0; i < pager_count; i++ )
        {
            cairo_rectangle( cr, rx+SCALE_VALUE( 1.0 ), ry, width-SCALE_VALUE( 2.0 ), SCALE_VALUE( 4.0 ) );
            rx += width;
        }
        cairo_fill( cr );
    }

    icon->w->text_color.alpha = 1.0;

    return TRUE;
}

static void dock_icon_clicked( GtkButton* button, dock_icon* icon )
{
    dock_icon_activate( icon, gdk_x11_get_server_time( gtk_widget_get_window( GTK_WIDGET( button ) ) ), TRUE );
} 

gboolean dock_icon_leave( GtkWidget* widget, GdkEvent* event, dock_icon* icon )
{
    icon->icon_state = ICON_STATE_NORMAL;
    gtk_widget_queue_draw( widget );

    return FALSE;
}

gboolean dock_icon_motion( GtkWidget* widget, GdkEvent* event, dock_icon* icon )
{
    icon->icon_state = ICON_STATE_HOVER;
    gtk_widget_queue_draw( widget );

    return FALSE;
}

void dock_icon_remove( dock_icon* icon )
{
    gtk_widget_destroy( icon->button );
}

dock_icon* dock_icon_create( WnckWindow* window, eric_window* w )
{
    dock_icon* icon = malloc( sizeof( dock_icon ) );
    icon->class_group = wnck_window_get_class_group( window );
    icon->instance_name = wnck_window_get_class_instance_name( window );
    icon->icon_pixbuf = get_icon( window, (int)SCALE_VALUE( 32.0 ) );
    icon->pager_items = NULL;
    icon->icon_state = ICON_STATE_NORMAL;
    icon->w = w;

    printf( "Attempted dock icon get%s \n", wnck_class_group_get_id( icon->class_group ) );

    icon->width = SCALE_VALUE( BAR_HEIGHT - 6.0 );
    icon->height = SCALE_VALUE( BAR_HEIGHT - 4.0 );

    icon->button = gtk_button_new();
    gtk_widget_set_size_request( icon->button, icon->width, icon->height );
    gtk_widget_add_events( icon->button, GDK_POINTER_MOTION_MASK
            | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK
            | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK );
    gtk_widget_set_app_paintable( icon->button, TRUE );
    g_signal_connect( G_OBJECT( icon->button ), "draw", G_CALLBACK( dock_icon_draw ), (void*)icon );
    g_signal_connect( G_OBJECT( icon->button ), "clicked", G_CALLBACK( dock_icon_clicked ), (void*)icon );
    g_signal_connect( G_OBJECT( icon->button ), "leave-notify-event", G_CALLBACK( dock_icon_leave ), (void*)icon );
    g_signal_connect( G_OBJECT( icon->button ), "motion-notify-event", G_CALLBACK( dock_icon_motion ), (void*)icon );

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
            tooltip_window_clear_pager_list();
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

gchar* get_icon_from_desktop( const char* name )
{
    GKeyFile* key_file = g_key_file_new();
    gchar file[256];
    gchar* out;

    sprintf( file, "/usr/share/applications/%s.desktop", name );
    if( !g_key_file_load_from_file( key_file, file, G_KEY_FILE_NONE, NULL ) )
        return NULL;

    out = g_key_file_get_string( key_file, G_KEY_FILE_DESKTOP_GROUP, "Icon", NULL );
    g_key_file_free( key_file );

    return out;
}

/* Gets the pixbuf from a desktop file's icon name. Based on the same function * from matchbox-desktop
 */
GdkPixbuf* get_icon( WnckWindow* window, guint size )
{
    static GtkIconTheme* theme = NULL;
    GdkPixbuf* pixbuf = NULL;
    GdkPixbuf* copy = NULL;
    gchar* stripped = NULL;
    const gchar* class_name = wnck_window_get_class_group_name( window );
    const gchar* instance_name = wnck_window_get_class_instance_name( window );
    gchar instance_lower[256] = {0};
    gchar class_lower[256] = {0};
    gint width, height;
    int unref = 0;


    if (theme == NULL) {
        theme = gtk_icon_theme_get_default();
    }

    printf( "class: %s\ninstance%s\n", 
            class_name,
            instance_name );
    if( class_name != NULL && instance_name != NULL )
    {
        int i;
        for( i = 0; class_name[i] && i < 256; i++ )
            class_lower[i] = tolower( class_name[i] );
        for( i = 0; instance_name[i] && i < 256; i++ )
            instance_lower[i] = tolower( instance_name[i] );

        //Use the window's icon
        if( strcmp( class_lower, instance_lower ) != 0 )
        {
            pixbuf = wnck_window_get_icon( window );
            unref = 0;
        }
    }

    //Check for desktop file entry
    if( pixbuf == NULL )
    {
        gchar* icon_name = get_icon_from_desktop( instance_name );
        if( icon_name != NULL )
        {
            pixbuf = gtk_icon_theme_load_icon( theme, icon_name, 
                            size, GTK_ICON_LOOKUP_FORCE_SIZE, NULL );
            unref = 1;
        }
    }

    /* Always try and send back something */
    if (pixbuf == NULL)
    {
        pixbuf = wnck_window_get_icon( window );
        unref = 0;
    }

    if (pixbuf == NULL)
    {
        pixbuf = gtk_icon_theme_load_icon(theme, "application-x-executable",
                                          size, 0, NULL);
        unref = 1;
    }

    width = gdk_pixbuf_get_width(pixbuf);
    height = gdk_pixbuf_get_height(pixbuf);

    if (width != size || height != size) {
        GdkPixbuf* temp = pixbuf;
        pixbuf = gdk_pixbuf_scale_simple(temp,
                                         size,
                                         size,
                                         GDK_INTERP_HYPER);
        if( unref )
            g_object_unref(temp);
    }

    g_free(stripped);

    copy = gdk_pixbuf_copy( pixbuf );
    if( unref )
        g_object_unref( pixbuf );
    return copy;
}

/* From matchbox-desktop */
char* strip_extension (const char *file)
{
    char *stripped, *p;

    stripped = g_strdup (file);

    p = strrchr (stripped, '.');
    if (p &&
        (!strcmp (p, ".png") ||
         !strcmp (p, ".svg") ||
         !strcmp (p, ".xpm")))
        *p = 0;

    return stripped;
}

