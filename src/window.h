#ifndef __WINDOW_H
#define __WINDOW_H

#include <gtk/gtk.h>


#define MIN_WINDOW_WIDTH 1004
#define MIN_WINDOW_HEIGHT 540


extern GtkWindow *window;
extern GtkWidget *main_panels;
extern GtkWidget *flash_panel;


// Main
void create_main_window();

// Tools
void fullscreen_toggle();
void tab_refresh();

// callbacks
bool event_cb(GtkWidget *w, GdkEvent *e, gpointer p);
bool keypress_cb(GtkWidget *w, GdkEventKey *e, gpointer p);
void flash_resize_cb(GtkContainer *c, gpointer p);
void destroy_cb(GtkWidget *w, gpointer o);


#endif
