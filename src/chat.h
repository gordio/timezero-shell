#ifndef __CHAT_H
#define __CHAT_H

#include <gtk/gtk.h>


#define CHAT_TIME_FG      "#A0A0A0"
#define CHAT_HIGHLIGHT_BG "#29241B"
#define CHAT_NICK_FG      "#FFFFFF"
#define CHAT_SYSTEM_FG    "#AB8D68"
#define CHAT_MESSAGE_FG   "#EAEAEA"
#define CHAT_FG_1         "#F2C4AB"
#define CHAT_FG_2         "#F2E7AB"
#define CHAT_FG_3         "#D9F2AB"
#define CHAT_FG_4         "#ABF2C4"
#define CHAT_FG_5         "#E7ABF2"
#define CHAT_FG_6         "#ABB6F2"
#define CHAT_FG_7         "#ABD9F2"
#define CHAT_FG_8         "#C4ABF2"

enum {
	GENERAL,
	PRIVATE,
	CLAN,
	ALLIANCE,
	RADIO,
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
bool parse_and_add_message();
bool parse_and_add_system_message();
void chat_list_nick_add();
void setChatState();

void tz_chat_start(const char const *data);
void tz_list_refresh(const char const *data);
void tz_list_add(const char const *data, bool disable_refresh);
void tz_list_remove(const char const *data);
void tz_message_add(const char const *data);

void update_cmd();
void update_cmd_menu(void);

void chat_set_tab(int index);
bool insert_nick_to_entry();
bool insert_to_entry();

#endif
