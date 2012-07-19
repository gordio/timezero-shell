/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _CHAT_H
#define _CHAT_H

#include <gtk/gtk.h>

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