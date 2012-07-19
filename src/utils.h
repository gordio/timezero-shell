/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <gtk/gtk.h>

/* GTK */
GtkWidget *al_list_widget_create();
GtkWidget *image_button_new();
GtkWidget *image_toggle_button_new();
GtkWidget *chat_msg_view_new();
GtkWidget *list_nickbox_create();

int get_rank_num_from_ranks();

/* Strings */
void rem_substr();
char *replace_substr_in_str();
char *escape_str();

#endif /* end guard: _UTILS_H */