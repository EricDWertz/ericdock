/*
 * eric_window.h
 * creates a transluscent window using the current theme color
 */

#pragma once

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gio/gio.h>

#include <stdlib.h>
#include <string.h>

/*
 * TODO: add text color
 */

typedef struct
{
    GtkWidget* window;
    GdkRGBA background_color;
    GdkRGBA background_color_old;
    GdkRGBA background_color_new;
    double background_change_percentage;
} eric_window;

static void gdk_color_lerp( GdkRGBA* c1, GdkRGBA* c2, double s, GdkRGBA* out )
{
	out->red = c1->red + ( c2->red - c1->red ) * s;	
	out->green = c1->green + ( c2->green - c1->green ) * s;	
	out->blue = c1->blue + ( c2->blue - c1->blue ) * s;	
	out->alpha = c1->alpha + ( c2->alpha - c1->alpha ) * s;	
}

static gboolean eric_window_animation_timer( eric_window* w )
{
	w->background_change_percentage += 0.05;
	gdk_color_lerp( &w->background_color_old, &w->background_color_new, 
                        w->background_change_percentage, &w->background_color);
	gtk_widget_queue_draw( w->window );

	if( w->background_change_percentage >= 1.0 ) return FALSE;

	return TRUE;
}

static gboolean eric_window_draw( GtkWidget* widget, cairo_t* cr, eric_window* w )
{
	cairo_set_operator(cr,CAIRO_OPERATOR_SOURCE);
	w->background_color.alpha = 0.75;
	gdk_cairo_set_source_rgba( cr, &w->background_color );
	cairo_paint( cr );	
	
    return FALSE;
}


static void eric_window_screen_changed( GtkWidget *widget, GdkScreen *old_screen, gpointer userdata )
{
    GdkVisual *visual;
	
	GdkScreen* screen=gtk_widget_get_screen(widget);
	if(!screen) return;

	visual = gdk_screen_get_rgba_visual(screen);
	if(visual==NULL) visual=gdk_screen_get_system_visual(screen);

	gtk_widget_set_visual(widget,visual);
}

void eric_window_gsettings_value_changed( GSettings *settings, const gchar *key, eric_window* w )
{
    if( strcmp( key, "primary-color" ) == 0 )
    {
        w->background_color_old = w->background_color;
        gdk_rgba_parse( &w->background_color_new, g_settings_get_string( settings, "primary-color" ) );
        
        w->background_change_percentage = 0.0;
        g_timeout_add( 32, (gpointer)eric_window_animation_timer, w );
    }
}

eric_window* eric_window_create( int width, int height, char* title )
{
    eric_window* w = malloc( sizeof( eric_window ) );

    if( title == NULL )
        title = "eric window";

	w->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW( w->window ), title );
	gtk_window_resize( GTK_WINDOW( w->window ), width, height );
	gtk_widget_add_events( w->window, GDK_STRUCTURE_MASK );

    gtk_widget_set_app_paintable( w->window, TRUE );

    g_signal_connect( G_OBJECT( w->window ), "draw", G_CALLBACK(eric_window_draw), &w );
    g_signal_connect( G_OBJECT( w->window ), "screen-changed", G_CALLBACK(eric_window_screen_changed), &w );
	g_signal_connect( G_OBJECT( w->window ), "delete-event", gtk_main_quit, NULL );

    eric_window_screen_changed( w->window, NULL, NULL );

    /* GSettings Stuff */
    GSettings* gsettings;
    GSettingsSchema* gsettings_schema;

    gsettings_schema = g_settings_schema_source_lookup( g_settings_schema_source_get_default(),
                                                "org.gnome.desktop.background",
                                                TRUE );
    if( gsettings_schema )
    {
        g_settings_schema_unref (gsettings_schema);
        gsettings_schema = NULL;
        gsettings = g_settings_new ( "org.gnome.desktop.background" );
    }

    g_signal_connect_data( gsettings, "changed", G_CALLBACK( eric_window_gsettings_value_changed ), NULL, 0, 0 );
    gdk_rgba_parse( &w->background_color, g_settings_get_string( gsettings, "primary-color" ) );
    printf( "COLOR: %s\n", g_settings_get_string( gsettings, "primary-color" ) );

    return w;
}
