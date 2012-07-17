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