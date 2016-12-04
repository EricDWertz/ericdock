#pragma once
/*
 * Global header for ericdock stuff
 */

#include "eric_window.h" 

#define ERIC_DOCK_FONT "Source Sans Pro Regular" 
#define ERIC_DOCK_TOOLTIP_SHADOW_RADIUS 16.0
#define ERIC_DOCK_TOOLTIP_WIDTH ERIC_DOCK_TOOLTIP_SHADOW_RADIUS + 320.0
#define ERIC_DOCK_TOOLTIP_ITEM_HEIGHT 24.0
#define UI_SCALE 1.0
#define SCALE_VALUE(x) (x)*UI_SCALE
#define BAR_HEIGHT 48.0

#define ICON_STATE_NORMAL 0
#define ICON_STATE_HOVER 1
#define ICON_STATE_ACTIVE 2
#define ICON_STATE_ALERT 3

extern GList* dock_icons;
extern eric_window* dock_window;
