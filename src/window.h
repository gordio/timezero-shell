/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _WINDOW_H
#  define _WINDOW_H
#include <gtk/gtk.h>

extern GtkWidget *window;
extern GtkWidget *main_panels;
extern GtkWidget *flash_panel;

void create_main_window();

void fullscreen_toggle();
void tab_refresh();
void ev_destroy();

#endif