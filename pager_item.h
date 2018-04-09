#pragma once
/*
 * pager_item - held by the dock_icon object
 */

#include <gtk/gtk.h>

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>

#include "drawing.h"
#include "eric_window.h"
#include "dock_objects.h"

void pager_item_name_changed( WnckWindow* window, pager_item* item );
void pager_item_icon_changed( WnckWindow* window, pager_item* item );
void pager_item_state_changed( WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, pager_item* item );
pager_item* pager_item_create( WnckWindow* window, dock_icon* parent );
int pager_item_mouse_move( pager_item* item, double mx, double my );
GtkWidget* pager_item_create_widget( pager_item* item, int width, int height );
void pager_item_destroy_widget( pager_item* item );
