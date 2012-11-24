#ifndef __UTILS_H
#define __UTILS_H

#include <gtk/gtk.h>

#define HASH_TABLESIZE 11


GtkWidget * al_list_widget_create();
GtkWidget * list_nickbox_create();
void gtk_button_add_image(GtkButton *button, char *image_path);
bool adjustment_is_bottom(GtkAdjustment *a);

int get_rank_num_from_ranks();

void rem_substr(char *str, const char const *substr);
char * strdup();
char * escape_str();
char * rep_substr(const char *str, const char *old, const char *new);
int str_hash(const char const *str, int hash_tablesize);


#endif
/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix ft=c: */
