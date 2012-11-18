#ifndef __CHAT_H
#define __CHAT_H

#include <gtk/gtk.h>


#define CHAT_TIME_COLOR "#A0A0A0"
#define CHAT_HIGHLIGHT_BG_COLOR "#29241B"
#define CHAT_NICK_COLOR "#FFFFFF"
#define CHAT_SYSTEM_COLOR "#AB8D68"
#define CHAT_COLOR1 "#E0E0E0"
#define CHAT_COLOR2 "#8BB8E8"
#define CHAT_COLOR3 "#E999FF"
#define CHAT_COLOR4 "#B0B0B0"
#define CHAT_COLOR5 "#A3FF8A"
#define CHAT_COLOR6 "#FF8AA3"
#define CHAT_COLOR7 "#729FCF"
#define CHAT_COLOR8 "#FFE68A"
#define CHAT_COLOR9 "#AB8AFF"
#define CHAT_COLOR10 "#8AFFE6"
#define CHAT_COLOR11 "#FFCB0F"
#define CHAT_COLOR12 "#FF577B"
#define CHAT_COLOR13 "#7BFF57"
#define CHAT_COLOR14 "#CAAFAF"

enum {
	MSG_VIEW_GENERAL,
	MSG_VIEW_PRIVATE,
	MSG_VIEW_CLAN,
	MSG_VIEW_ALLIANCE,
	MSG_VIEW_RADIO,
};

enum {
	CHAT_FULL_OFF,
	CHAT_OFF,
	CHAT_ON,
	CHAT_REFRESH_OFF,
	CHAT_REFRESH_ON,
};


extern GtkWidget *chat_main_box;
extern GtkWidget *chat_panel;
extern GtkWidget *room_label_building;
extern bool chatOn;
extern bool chatRefresh;
extern bool in_building;


GtkWidget * create_chat_frame();
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
