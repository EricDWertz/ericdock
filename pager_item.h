#pragma once
/*
 * pager_item - held by the dock_icon object
 */

#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "drawing.h"
#include "eric_window.h"

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

void pager_item_name_changed( WnckWindow* window, pager_item* item );
void pager_item_icon_changed( WnckWindow* window, pager_item* item );
void pager_item_state_changed( WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, pager_item* item );
pager_item* pager_item_create( WnckWindow* window );
int pager_item_mouse_move( pager_item* item, double mx, double my );
void pager_item_draw( pager_item* item, cairo_t* cr, eric_window* w, cairo_pattern_t* pattern );
