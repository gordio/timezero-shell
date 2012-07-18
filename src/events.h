#ifndef _EVENTS_H
#define _EVENTS_H
#include <gtk/gtk.h>

// window
//void keypress_cb(GtkWidget *w, GdkEvent *e, gpointer user_data);

bool keypress_cb(GtkWidget *w, GdkEventKey *e, gpointer user_data);

void print_resize();

// Chat
bool tag_time_cb();
bool tag_nick_cb();

bool chat_text_view_event_cb();
bool nick_label_cb();
void room_click_info_cb();

void flash_resize_cb();

void set_cursor_hand_cb(GtkWidget *w, GdkEvent *e, gpointer user_data);
void unset_cursor_hand_cb(GtkWidget *w, GdkEvent *e, gpointer user_data);

#endif /* end guard: _EVENTS_H */
