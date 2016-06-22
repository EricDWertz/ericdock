#include <gdk/gdkx.h>
#include <X11/Xlib.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "eric_window.h"

GdkFilterReturn handle_x11_event( GdkXEvent *xevent, GdkEvent *event, gpointer data )
{
    XEvent* xev = (XEvent*)xevent;
    if( xev->type == KeyPress )
    {
        printf( "Got a key press event!\n" );
    }

    return GDK_FILTER_CONTINUE;
}

int main( int argc, char* argv[] )
{
    gtk_init( &argc, &argv );

    eric_window* w = eric_window_create( 640, 480, "" );
    gtk_window_set_type_hint( GTK_WINDOW( w->window ), GDK_WINDOW_TYPE_HINT_DOCK );

    gtk_widget_show_all( w->window );

    Display* dpy = GDK_DISPLAY_XDISPLAY( gdk_display_get_default() );
    Window xwin = RootWindow( dpy, DefaultScreen( dpy ) );
    XGrabKey( dpy, AnyKey, ControlMask, xwin, True, GrabModeAsync, GrabModeAsync );

    gdk_window_add_filter( NULL, handle_x11_event, NULL );

    WnckScreen* screen;
    GList* window_list;
    screen = wnck_screen_get_default();
    wnck_screen_force_update( screen );

    for( window_list = wnck_screen_get_windows( screen ); window_list != NULL; window_list = window_list->next )
    {
        WnckWindow* window = WNCK_WINDOW( window_list->data );
        printf( "%s%s\n", wnck_window_get_name( window ), wnck_class_group_get_name( wnck_window_get_class_group( window ) ) );
    }

    gtk_main();
    return 0;
}
