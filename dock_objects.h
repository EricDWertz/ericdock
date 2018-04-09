#pragma once

#include <pango/pango-context.h>

//Definitions for dock_icon and pager_item structs

typedef struct
{
    WnckClassGroup* class_group;
    eric_window* w;
    GdkPixbuf* icon_pixbuf;
    GList* pager_items;
    double width, height;
    int icon_state;
    int is_active;
    int selected_index;
    gchar* instance_name;
    GtkWidget* button;
} dock_icon;

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
    dock_icon* parent;
    GtkWidget* button;
    PangoLayout* layout;
    eric_window* w;
} pager_item;
