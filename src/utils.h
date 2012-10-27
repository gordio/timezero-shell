#ifndef __UTILS_H
#define __UTILS_H

#include <gtk/gtk.h>


GtkWidget * al_list_widget_create();
GtkWidget * list_nickbox_create();
void gtk_button_add_image(GtkButton *button, char *image_path);

int get_rank_num_from_ranks();

void rem_substr(char *str, const char const *substr);
char * strdup();
char * rep_substr();
char * escape_str();


#endif
