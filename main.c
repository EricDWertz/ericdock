#include "eric_window.h"

int main( int argc, char* argv[] )
{
    gtk_init( &argc, &argv );

    eric_window* w = eric_window_create( 640, 480, "Test Eric Window So Kawaii" );

    gtk_widget_show_all( w->window );
    gtk_main();

    return 0;
}
