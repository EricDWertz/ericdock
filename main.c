#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <math.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "eric_window.h"

GList* dock_icons = NULL;
eric_window* dock_window = NULL;

#include "ericdock.h"
#include "clock.h"

//Structure to hold actual pager items
#include "pager_item.h"

//Stucture to hold class groups... which are icons on the dock
//Will need to have support for launchers and stuff in the future
#include "dock_icon.h"

#include "tooltip_window.h"
#include "xutils.h"

int screen_width;
int screen_height;


//Logic to add a window to the pager items.
//If a matching class group already exists it will be added to that, otherwise create
//a new class group and add the window to that.
void add_window_to_pager( WnckWindow* window )
{
    GList* icon_list;
    int found_class_group = 0;
    dock_icon *icon, *new_dock_icon;

    WnckClassGroup* class = wnck_window_get_class_group( window );
    gchar* instance_name = wnck_window_get_class_instance_name( window );
    for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
    {
        icon = (dock_icon*)icon_list->data;
        if( strcmp( icon->instance_name, instance_name ) == 0 )
        {
            icon->pager_items = g_list_append( icon->pager_items, pager_item_create( window ) );
            found_class_group = 1;
            break;
        }
    }

    if( !found_class_group )
    {
        //Add a new dock item
        new_dock_icon = dock_icon_create( window );
        new_dock_icon->pager_items = g_list_append( 
            new_dock_icon->pager_items,
            pager_item_create( window )
        );

        dock_icons = g_list_append( dock_icons, new_dock_icon );
    }
}

dock_icon* get_dock_icon_at_position( int pos )
{
    GList* icon_list;
    int i = 0;

    for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
    {
        if( i == pos )
            return (dock_icon*)icon_list->data;
        i++;
    }
    return NULL;
}

GdkFilterReturn handle_x11_event( GdkXEvent *xevent, GdkEvent *event, gpointer data )
{
    XEvent* xev = (XEvent*)xevent;
    dock_icon* icon;

    Display* dpy = GDK_DISPLAY_XDISPLAY( gdk_display_get_default() );

    if( xev->type == KeyPress )
    {
        if( xev->xkey.keycode == 133 || xev->xkey.keycode == 134 )
        {
            printf( "Got a super key press event!\n" );
        }
        else
        {
            if( xev->xkey.keycode >= 10 && xev->xkey.keycode <= 20 )
            {
                printf( "Got a number key press event!\n" );

                icon = get_dock_icon_at_position( xev->xkey.keycode - 10 );
                if( icon )
                {
                    dock_icon_activate( icon, xev->xkey.time, FALSE );
                }
            }

            //L key press to lock the screen TODO: adjust this command
            if( xev->xkey.keycode == 46 )
            {
                system( "/home/eric/EricOS/ericlock/ericlock" );
            }
        }
    }
    if( xev->type == KeyRelease )
    {
        if( xev->xkey.keycode >= 10 && xev->xkey.keycode <= 20 )
        {
            printf( "Got a number key release event!\n" );
        }
        else
        {
            if( xev->xkey.keycode == 133 || xev->xkey.keycode == 134 )
            {
                printf( "Got a super key release event!\n" );
                tooltip_window_hide();
            }
            else
            {
                printf( "Flusing events\n" );
                XAllowEvents( dpy, ReplayKeyboard, xev->xkey.time );
                XFlush( dpy );
            }
        }
    }

    return GDK_FILTER_CONTINUE;
}

static gboolean draw_dock_window( GtkWidget* widget, cairo_t* cr, eric_window* w )
{
    GList* icon_list = dock_icons;
    dock_icon* icon;
    double x, y;
    double rx, ry, width;
    int i, pager_count, icon_count;
    
    icon_count = g_list_length( dock_icons );
    x = floor( (double)(screen_width/2) - SCALE_VALUE( 47.0 * (double)icon_count / 2.0 ) );
    y = SCALE_VALUE( 5.0 );
    cairo_set_operator( cr, CAIRO_OPERATOR_OVER );
    for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
    {
        icon = (dock_icon*)icon_list->data;

        w->text_color.alpha = 0.25;
        gdk_cairo_set_source_rgba( cr, &w->text_color );
        if( icon->icon_state == ICON_STATE_HOVER )
        {
            draw_rounded_rect( cr, x, y-SCALE_VALUE( 2.0 ), SCALE_VALUE( 42.0 ), SCALE_VALUE( 42.0 ), SCALE_VALUE( 2.0 ) );
            cairo_fill( cr );
        }
        if( icon->is_active )
        {
            draw_rounded_rect( cr, x, y-SCALE_VALUE( 2.0 ), SCALE_VALUE( 42.0 ), SCALE_VALUE( 42.0 ), SCALE_VALUE( 2.0 ) );
            cairo_fill( cr );
        }
        if( !GDK_IS_PIXBUF( icon->icon_pixbuf ) )
            icon->icon_pixbuf = wnck_class_group_get_icon( icon->class_group );
        gdk_cairo_set_source_pixbuf( cr, icon->icon_pixbuf, x+SCALE_VALUE( 4.0 ), y );
        cairo_paint( cr );

        pager_count = g_list_length( icon->pager_items );
        if( pager_count > 0 )
        {
            //Draw rectangles
            width = (SCALE_VALUE(32.0) / pager_count );  
            rx = x + SCALE_VALUE(4.0);
            ry = y + SCALE_VALUE(34.0);
            w->text_color.alpha = 0.5;
            gdk_cairo_set_source_rgba( cr, &w->text_color );
            for( i = 0; i < pager_count; i++ )
            {
                cairo_rectangle( cr, rx+SCALE_VALUE( 1.0 ), ry, width-SCALE_VALUE( 2.0 ), SCALE_VALUE( 4.0 ) );
                rx += width;
            }
            cairo_fill( cr );
        }
        icon->x = x;
        icon->y = y;

        x += SCALE_VALUE( 47.0 );
    }
    clock_draw( cr, (double)screen_width-SCALE_VALUE(10), ( BAR_HEIGHT * UI_SCALE ) / 2.0, w );

    return FALSE;
}

void grab_keys()
{
    int i;
    Display* dpy = GDK_DISPLAY_XDISPLAY( gdk_display_get_default() );
    Window xwin = RootWindow( dpy, DefaultScreen( dpy ) );

    //Grab number keys 
    for( i = 10; i <= 20; i++ )
    {
        XGrabKey( dpy, i, Mod4Mask | Mod2Mask, xwin, True, GrabModeAsync, GrabModeAsync );
        XGrabKey( dpy, i, Mod4Mask, xwin, True, GrabModeAsync, GrabModeAsync );
    }
    XGrabKey( dpy, 133, AnyModifier, xwin, True, GrabModeAsync, GrabModeAsync );
    XGrabKey( dpy, 134, AnyModifier, xwin, True, GrabModeAsync, GrabModeAsync );

    gdk_window_add_filter( NULL, handle_x11_event, NULL );
}

gboolean window_mouse_move( GtkWidget* widget, GdkEvent* event, gpointer user )
{
    double mx, my;
    double it, ib, il, ir;
    int old_state, state_changed;
    dock_icon* icon;
    GList *icon_list;
    GdkEventMotion* e = (GdkEventMotion*)event;

    mx = e->x;
    my = e->y;
    if( widget == dock_window->window )
    {
        for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
        {
            icon = (dock_icon*)icon_list->data;
            old_state = icon->icon_state;
            it = 0; ib = SCALE_VALUE( 48 ); 
            il = icon->x; ir = icon->x + SCALE_VALUE( 42 );
            if( il < mx && mx < ir && it < my && my < ib )
            {
                icon->icon_state = ICON_STATE_HOVER;
            }
            else
            {
                icon->icon_state = ICON_STATE_NORMAL;
            }
            if( old_state != icon->icon_state )
                state_changed = 1;

            if( state_changed )
                gtk_widget_queue_draw( dock_window->window );
        }
    }

    return FALSE;
}

void dock_window_mouse_down( GtkWidget* widget, GdkEvent* event, gpointer user )
{
    GdkEventButton* e = (GdkEventButton*)event;
    if( e->button != 1 )
        return;

    double mx, my;
    dock_icon* icon;
    GList* icon_list;

    mx = e->x; my = e->y;
    for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
    {
        icon = (dock_icon*)icon_list->data;
        dock_icon_mouse_down( icon, mx, my, e->time );
    }
}

void setup_dock_window()
{
    //GdkScreen* screen = gdk_screen_get_default();
    GdkMonitor* mon = gdk_display_get_primary_monitor( gdk_display_get_default() );
    GdkRectangle mon_geom;
    gdk_monitor_get_geometry( mon, &mon_geom );
    screen_width = mon_geom.width;
    screen_height = mon_geom.height;

    dock_window = eric_window_create( screen_width, BAR_HEIGHT * UI_SCALE, "" );
    gtk_window_move( GTK_WINDOW( dock_window->window ), 0, screen_height - BAR_HEIGHT * UI_SCALE );
    gtk_window_set_type_hint( GTK_WINDOW( dock_window->window ), GDK_WINDOW_TYPE_HINT_DOCK );

    dock_window->draw_callback = draw_dock_window;

    gtk_widget_add_events( dock_window->window, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK );
    g_signal_connect( G_OBJECT( dock_window->window ), "motion-notify-event", G_CALLBACK(window_mouse_move), NULL );
    g_signal_connect( G_OBJECT( dock_window->window ), "button-press-event", G_CALLBACK(dock_window_mouse_down), NULL );

    gtk_widget_show_now( dock_window->window );

    //Init clock drawing
    clock_init( dock_window->window );
}

static void wnck_window_opened( WnckScreen* screen, WnckWindow* window, gpointer data )
{
    if( !wnck_window_is_skip_pager( window ) && !wnck_window_is_skip_tasklist( window ) )
    {
        add_window_to_pager( window );
        gtk_widget_queue_draw( dock_window->window );
    }
}

static void wnck_window_closed( WnckScreen* screen, WnckWindow* window, gpointer data )
{
    GList *icon_list, *pager_list;
    dock_icon* icon;
    pager_item* item;

    for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
    {
        icon = (dock_icon*)icon_list->data;
        for( pager_list = icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
        {
            item = (pager_item*)pager_list->data;
            if( item->window == window )
            {
                icon->pager_items = g_list_remove( icon->pager_items, item );
                free( item );
                printf( "Removed pager item\n" );
                break;
            }
        }
        if( icon->pager_items == NULL )
        {
            dock_icons = g_list_remove( dock_icons, icon );
            free( icon );
            printf( "Removed dock icon\n" );
            break;
        }
    }
    gtk_widget_queue_draw( dock_window->window );
}

static void wnck_active_window_changed( WnckScreen* screen, WnckWindow* prev_window, gpointer user )
{
    GList *icon_list, *pager_list;
    dock_icon* icon;
    pager_item* item;
    WnckWindow* active = wnck_screen_get_active_window( screen );
    int old_state, state_changed;
    int has_active;

    state_changed = 0;
    for( icon_list = dock_icons; icon_list != NULL; icon_list = icon_list->next )
    {
        icon = (dock_icon*)icon_list->data;
        old_state = icon->is_active;
        has_active = 0;
        for( pager_list = icon->pager_items; pager_list != NULL; pager_list = pager_list->next )
        {
            item = (pager_item*)pager_list->data;
            if( item->window == active )
            {
                has_active = 1;
                break;
            }
        }
        if( has_active )
            icon->is_active = 1;
        else
            icon->is_active = 0;

        if( icon->is_active != old_state )
            state_changed = 1; 
    }
    if( state_changed )
        gtk_widget_queue_draw( dock_window->window );
}

void init_wnck()
{
    WnckScreen* screen;
    screen = wnck_screen_get_default();
    g_signal_connect( screen, "window-opened", G_CALLBACK( wnck_window_opened ), NULL );
    g_signal_connect( screen, "window-closed", G_CALLBACK( wnck_window_closed ), NULL );
    g_signal_connect( screen, "active-window-changed", G_CALLBACK( wnck_active_window_changed ), NULL );
}

int main( int argc, char* argv[] )
{
    gtk_init( &argc, &argv );

    setup_dock_window();
    tooltip_window_create( dock_window->window );
    grab_keys();
    init_wnck();

    gtk_main();
    return 0;
}
