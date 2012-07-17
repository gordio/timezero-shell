#ifndef _CHAT_H
#define _CHAT_H

#include <gtk/gtk.h>

extern GtkTextView *msgView;
extern GtkWidget *chat_main_box;
extern GtkWidget *chat_panel;
extern GtkWidget *room_label_building;
extern bool chatOn;
extern bool chatRefresh;
extern bool in_building;

GtkWidget *create_chat_frame();
void recreate_cmd_popup_menu();
bool parse_and_add_message();
bool parse_and_add_system_message();
void chat_list_nick_add();
void setChatState();

void tz_chat_start(const char const *data);
void tz_list_refresh(const char const *data);
void tz_list_add(const char const *data, bool disable_refresh);
void tz_list_remove(const char const *data);
void tz_message_add(const char const *data);

void show_system_msg();

bool insert_nick_to_entry();
bool insert_to_entry();

#endif