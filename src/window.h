#ifndef __WINDOW_H
#define __WINDOW_H

#include <gtk/gtk.h>


extern GtkWidget *window;
extern GtkWidget *main_panels;
extern GtkWidget *flash_panel;


void create_main_window();

void fullscreen_toggle();
void tab_refresh();
void ev_destroy();


#endif
