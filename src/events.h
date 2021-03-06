#ifndef __EVENTS_H
#define __EVENTS_H

#include <gtk/gtk.h>




// Chat
bool tag_time_cb();
bool tag_nick_cb();

bool chat_text_view_event_cb();
bool nick_label_cb();
void room_click_info_cb();

void set_cursor_hand_cb(GtkWidget *w, GdkEvent *e, gpointer user_data);
void unset_cursor_hand_cb(GtkWidget *w, GdkEvent *e, gpointer user_data);


#endif
