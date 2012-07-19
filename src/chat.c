/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
/* TODO #include <gtkspell-2.0/gtkspell/gtkspell.h>*/

#include "general.h"
#include "autologin.h"
#include "window.h"
#include "utils.h"
#include "main.h"
#include "flash.h"
#include "events.h"
#include "tz-interface.h"
#include "chat/sort.h"


GtkWidget *btnScan, *btnSend, *btnSmiles, *btnCmd, *btnClear, *btn_config, *btnExit, *btnTranslit;
GtkEntry *msgEntry;
GtkWidget *chat_main_box, *chat_text_box, *bottom_buttons, *room_list, *tab_bar;

GtkWidget *msgView_system, *msgView_private, *msgView_radio, *msgView_clan, *msgView_alliance;
GtkTextMark *msgViewMark;

GtkWidget *room_box;
GtkWidget *room_label_location, *room_label_building;


static GtkWidget *cmdMenu;
static char *last_cmd_str;
static char *current_player_name;
bool chatOn = true, chatRefresh = true;

static bool in_building = false;

#include "chat.h"

// FIXME: Use undefined list for players in rppm
tzPlayer Room_player[MAX_ROOM_NICKS];
GtkWidget *Room_widget[MAX_ROOM_NICKS];

// Messages caches
static ulong MCC[1024];
static ulong MCS[512];


bool send_text();
bool send_cmd();

void chat_list_nick_parse_add();
void setChatState();


// CALLBACKS
static bool activate_msgEntry_cb();
static bool press_scan_info_cb();
static bool press_smiles_cb();
static bool press_cmd_cb();
static bool clear_cb();
void room_label_cb();


struct _roomList {
	GtkWidget *w;
	tzPlayer *p;

	struct roomList_t *next;
};


GtkWidget *
create_room_list_widget(void)
{
	vlog("Create room list.");

	GtkWidget *scroll;
	GtkWidget *event_box;
	GtkWidget *vbox_location_name;
	GdkColor color_bg;

	room_box = gtk_vbox_new(false, 1);
	gtk_container_set_border_width(GTK_CONTAINER(room_box), 0);

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), room_box);
	gtk_widget_set_size_request(GTK_WIDGET(room_box), 180, -1);
	gtk_widget_set_size_request(GTK_WIDGET(scroll), 228, -1);

	vbox_location_name = gtk_vbox_new(false, 0);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), vbox_location_name);
	g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(room_label_cb), NULL);

	// ROOM Label: location name
	room_label_location = gtk_label_new("");
	gtk_label_set_justify(GTK_LABEL(room_label_location), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(room_label_location), true);
	gtk_label_set_line_wrap_mode(GTK_LABEL(room_label_location), PANGO_WRAP_WORD);
	gtk_label_set_use_markup(GTK_LABEL(room_label_location), false);
	gtk_label_set_use_underline(GTK_LABEL(room_label_location), false);
	gtk_box_pack_start(GTK_BOX(vbox_location_name), room_label_location, false, false, 2);

	// ROOM Label: building name
	room_label_building = gtk_label_new("");
	gtk_label_set_justify(GTK_LABEL(room_label_building), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(room_label_building), true);
	gtk_label_set_line_wrap_mode(GTK_LABEL(room_label_building), PANGO_WRAP_WORD);
	gtk_label_set_use_markup(GTK_LABEL(room_label_building), false);
	gtk_label_set_use_underline(GTK_LABEL(room_label_building), false);
	gtk_box_pack_start(GTK_BOX(vbox_location_name), room_label_building, false, false, 2);
	gtk_widget_hide(room_label_building);

	// configure EventBox for ROOM Labels
	// fixme: find border color, and use
	gdk_color_parse("#252320", &color_bg);
#if GTK_CHECK_VERSION (3, 0, 0)
	gtk_widget_override_background_color(room_label_location, GTK_STATE_NORMAL, &color_bg);
#else
	gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &color_bg);
#endif
	g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(&set_cursor_hand_cb), NULL);
	g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(&unset_cursor_hand_cb), NULL);

	gtk_box_pack_start(GTK_BOX(room_box), event_box, false, false, 0);

	return scroll;
}

void
room_widget_redraw(void)
{
	gtk_widget_hide(room_box);

	// remove widgets
	for (uint i = 0; i < countof(Room_widget); i++) {
		if (Room_widget[i] != NULL) {
			gtk_widget_destroy(Room_widget[i]);
			Room_widget[i] = NULL;
		}
	}

	// FIXME: qsort very speedly, but all time random.
	// Good choice for use on room Refresh. And input/delete without redraw.
	// room_array_qsort_by_level(Room_player, 0, MAX_ROOM_NICKS-1);
	room_array_sort_by_level(Room_player);
	room_array_sort_by_level_by_rank(Room_player);

	// create widget player box
	for (int i = countof(Room_player)-1; i >= 0; --i) {
		if (Room_player[i].nick != NULL) {
			for (uint j = 0; j < countof(Room_widget); j++) {
				if (Room_widget[j] == NULL) {
					Room_widget[j] = list_nickbox_create(&Room_player[i]);
					gtk_box_pack_start(GTK_BOX(room_box), Room_widget[j], false, false, 1);
					break;
				}
			}
		}
	}

	gtk_widget_show_all(room_box);

	// hide building name
	if (!in_building) {
		gtk_widget_hide(room_label_building);
	}

	return;
}

static tzPlayer *
get_player_or_exist(char *nick)
{
	// search exist player item
	for (uint i = 0; i < countof(Room_player); ++i) {
		if (Room_player[i].nick != NULL) {
			if (strcmp(nick, Room_player[i].nick) == 0) {
				return &Room_player[i];
			}
		}
	}

	// search new empty player item
	for (uint i = 0; i < countof(Room_player); i++) {
		if (Room_player[i].nick == NULL) {
			return &Room_player[i];
		}
	}

	// On error
	return NULL;
}

void
tz_list_add(const char const *data, bool disable_refresh)
{
	vlog("Add(update) user to room: '%s'", data);

	// Example: 0/0/33/The Alliance/Pilot-Lucky/19/4570/0/1
	char *nick = malloc(MAX_NICK_SIZE*2+1); // temp nickname for find exist player element
	tzPlayer *p;

	// find exist or new player element
	if (1 == sscanf(data, "A,%*u/%*d/%*u//%[^/]/%*d/%*d/%*d/%*d", nick)) {
		p = get_player_or_exist(nick);
	} else if (1 == sscanf(data, "A,%*u/%*d/%*u/%*[^/]/%[^/]/%*d/%*d/%*d/%*d", nick)) {
		p = get_player_or_exist(nick);
	}

	if (!p) {
		elog("Can't find empty room slot. Please report to developer.");
		return;
	}

	// allocate memory
	if (p->nick == NULL) {
		p->nick = malloc(1024);
	}
	if (p->clan == NULL) {
		p->clan = malloc(1024);
	}

	if (8 == sscanf(data, "A,%llu/%d/%u//%[^/]/%d/%d/%d/%d",
					&p->battleid,
					&p->group,
					&p->state,
					p->nick,
					&p->level,
					&p->rank,
					&p->minlevel,
					&p->aggr)) {
		free(p->clan);
		p->clan = NULL;
	} else if (9 == sscanf(data, "A,%llu/%d/%u/%[^/]/%[^/]/%d/%d/%d/%d",
					&p->battleid,
					&p->group,
					&p->state,
					p->clan,
					p->nick,
					&p->level,
					&p->rank,
					&p->minlevel,
					&p->aggr)) {
	} else {
		elog("\n!!!ERROR PARSE ADD:%s\n\n", data);
		return;
	}

	// Обновляем игрока в автологине
	if (current_player_name && strcmp(current_player_name, nick) == 0) {
		al_list_update_by_player(p);
	}

	if (!disable_refresh) {
		room_widget_redraw();
	}
}

void
tz_list_remove(const char const *data)
{
	vlog("tz_list_remove: (%s)", data);
	char *nick = g_strdup(data + 2);

	// find and remove from players buf
	for (uint i = 0; i < countof(Room_player); i++) {
		if (Room_player[i].nick != NULL) {
			if (strcmp(nick, Room_player[i].nick) == 0) {
				vlog("ok, find: %s and remove.", Room_player[i].nick);
				free(Room_player[i].nick);
				Room_player[i].nick = NULL;
				if (Room_player[i].clan != NULL) {
					free(Room_player[i].clan);
					Room_player[i].clan = NULL;
				}
			}
		}
	}

	room_widget_redraw();
}

static void
clear_room_players_buffer()
{
	for (uint i = 0; i < countof(Room_player); i++) {
		if (Room_player[i].nick != NULL) {
			free(Room_player[i].nick);
			Room_player[i].nick = NULL;
			if (Room_player[i].clan != NULL) {
				free(Room_player[i].clan);
				Room_player[i].clan = NULL;
			}
		}
	}
}

void
tz_list_refresh(const char const *data)
{
	vlog("tz_list_refresh: `%s`", data);
	char *text = malloc(strlen(data) + 1);
	char *loc_text = malloc(strlen(data) + 1);


	if (2 != sscanf(data, "R,%[^\t]\t%[^\n],", loc_text, text)) {
		elog("Parse Refresh room info\n");
	}

	char *room_name_p = malloc(strlen(loc_text) + 1);
	char *room_building = malloc(strlen(loc_text) + 1);

	if (2 == sscanf(loc_text, "%[^|]|%[^\n]", room_name_p, room_building)) {
		gtk_label_set_text(GTK_LABEL(room_label_location), room_name_p);
		gtk_label_set_text(GTK_LABEL(room_label_building), room_building);
		in_building = true;
	} else {
		gtk_label_set_text(GTK_LABEL(room_label_location), loc_text);
		gtk_label_set_text(GTK_LABEL(room_label_building), "");
		in_building = false;
	}

	clear_room_players_buffer();

	// parse nicks list
	char *pch = NULL;
	char *cmd = NULL;

	pch = strtok(text, ",");

	while (pch != '\0' && pch != NULL) {
		cmd = g_strconcat("A,", pch, NULL);
		tz_list_add(cmd, true);

		pch = strtok(NULL, ",");

		g_free(cmd);
	}
	g_free(text);
	g_free(loc_text);
	g_free(room_name_p);
	g_free(room_building);

	room_widget_redraw();
	if (!in_building) {
		gtk_widget_hide(room_label_building);
	}
}

void
tz_chat_start(const char const *data) {
	if (current_player_name) {
		free(current_player_name);
	}

	current_player_name = malloc(MAX_NICK_SIZE*2+1);

	if (1 == sscanf(data, "Start,%*[^,],%[^,],%*d", current_player_name)) {
		vlog("Start chat. Current player: %s", current_player_name);
	} else {
		wlog("Start chat. Can't parse player in '%s'", data);
	}

	clear_room_players_buffer();
}


GtkWidget *
create_chat_frame()
{
	GtkWidget *scroll;
	GtkWidget *vseparator;
	GtkWidget *label;

	// Create chat containers
	chat_main_box = gtk_vbox_new(false, 0);
	gtk_container_set_border_width(GTK_CONTAINER(chat_main_box), 1);
	chat_text_box = gtk_hpaned_new();
	gtk_container_set_border_width(GTK_CONTAINER(chat_text_box), 1);
	gtk_box_pack_start(GTK_BOX(chat_main_box), chat_text_box, true, true, 0);

	tab_bar = gtk_notebook_new();
	/*gtk_widget_set_size_request(GTK_WIDGET(msgView), 400, -1);*/
	gtk_notebook_set_show_border(GTK_NOTEBOOK(tab_bar), true);


	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN);

	gtk_paned_pack1(GTK_PANED(chat_text_box), GTK_WIDGET(tab_bar), true, false);


	msgView_system = chat_msg_view_new();
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), msgView_system);
	label = gtk_label_new(_("General and system"));
	gtk_notebook_append_page(GTK_NOTEBOOK(tab_bar), scroll, label);

	msgView_private = chat_msg_view_new();
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), msgView_private);
	label = gtk_label_new(_("Private"));
	gtk_notebook_append_page(GTK_NOTEBOOK(tab_bar), GTK_WIDGET(scroll), label);

	msgView_radio = chat_msg_view_new();
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), msgView_radio);
	label = gtk_label_new(_("Radio"));
	gtk_notebook_append_page(GTK_NOTEBOOK(tab_bar), GTK_WIDGET(scroll), label);

	msgView_clan = chat_msg_view_new();
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), msgView_clan);
	label = gtk_label_new(_("Clan"));
	gtk_notebook_append_page(GTK_NOTEBOOK(tab_bar), GTK_WIDGET(scroll), label);

	msgView_alliance = chat_msg_view_new();
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scroll), msgView_alliance);
	label = gtk_label_new(_("Alliance"));
	gtk_notebook_append_page(GTK_NOTEBOOK(tab_bar), GTK_WIDGET(scroll), label);


	scroll = create_room_list_widget();
	gtk_paned_pack2(GTK_PANED(chat_text_box), GTK_WIDGET(scroll), false, false);


	// BUTTONS PANEL
	bottom_buttons = gtk_hbox_new(false, 2);	// Инициализируем панель

	btnScan = image_button_new("img/button/chat/scan.png", "",
							   _("Show scan alert msgView"), &press_scan_info_cb);
	gtk_widget_set_sensitive(btnScan, false);
	gtk_button_set_relief(GTK_BUTTON(btnScan), GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), btnScan, false, true, 2);

	msgEntry = GTK_ENTRY(gtk_entry_new());	// строка ввода
	gtk_entry_buffer_set_max_length(gtk_entry_get_buffer(msgEntry), MAX_CHAT_MESSAGE);
	g_object_set(gtk_widget_get_settings(GTK_WIDGET(msgEntry)), "gtk-entry-select-on-focus", false, NULL);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), GTK_WIDGET(msgEntry), true, true, 2);
	g_signal_connect(G_OBJECT(msgEntry), "activate", G_CALLBACK(&activate_msgEntry_cb), NULL);



	btnSmiles = image_button_new("img/button/chat/smiles.png", "", _("Smiles"), &press_smiles_cb);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), btnSmiles, false, true, 2);

	btnCmd = image_button_new("img/button/chat/cmd.png", "", _("Cmd"), &press_cmd_cb);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), btnCmd, false, true, 2);

	btnTranslit = image_toggle_button_new("img/button/chat/translit.png", "", _("Translit"), NULL);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), btnTranslit, false, true, 2);

	vseparator = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(bottom_buttons), vseparator, false, true, 0);

	btn_config = image_button_new("img/button/chat/filter.png", _("Preferences "),
								  _("Open preferences window"), NULL);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), btn_config, false, true, 2);

	btnExit = image_button_new("img/button/chat/exit.png", _("Exit "), _("Exit game"), &tzLogout);
	gtk_box_pack_start(GTK_BOX(bottom_buttons), btnExit, false, true, 2);

	gtk_box_pack_start(GTK_BOX(chat_main_box), bottom_buttons, false, true, 2);


	setChatState(CHAT_FULL_OFF);

	return chat_main_box;
}

bool
send_raw(const char *data)
{
	char *escaped_str = escape_str(data);
	char *js_command = g_strconcat("sendChat(\"", escaped_str, "\")", NULL);

	tzExec(js_command);

	free(escaped_str);
	free(js_command);

	return true;
}

bool
send_text(const char *text)
{
	// TODO Create translit function
	//char *msg;
	//if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnTranslit)) == 1) {
		//msg = str_translit(strdup(text));
	//} else {
		//msg = strdup(text);
	//}
	send_raw(text);
	//g_free(msg);

	return false;
}

bool
insert_to_start_entry(const char *text)
{
	GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(msgEntry));

	gtk_entry_buffer_insert_text(entry_buffer, 0, text, -1);
	gtk_editable_set_position(GTK_EDITABLE(msgEntry), -1);
	gtk_widget_grab_focus(GTK_WIDGET(msgEntry));
	return false;
}

bool
insert_to_entry(const char *text)
{
	GtkEntryBuffer *entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(msgEntry));

	gtk_entry_buffer_insert_text(entry_buffer, 9999, text, -1);
	gtk_editable_set_position(GTK_EDITABLE(msgEntry), -1);
	gtk_widget_grab_focus(GTK_WIDGET(msgEntry));

	return false;
}

bool
insert_nick_to_entry(const char const *nick, int steel_private)
{
	// Don't add self player name
	if (current_player_name && strcmp(nick, current_player_name) == 0) {
		return false;
	}

	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(msgEntry));
	char *text = g_strdup(gtk_entry_buffer_get_text(buffer));
	char *to_nick = g_strconcat("to [", nick, "] ", NULL);
	char *private_nick = g_strconcat("private [", nick, "] ", NULL);
	char *tmp;


	if (strstr(text, to_nick)) {
		// ok exist _to_
		tmp = replace_substr_in_str(text, to_nick, private_nick);
		free(text);
		text = tmp;
	} else if (strstr(text, private_nick)) {
		// ok exist _private_
		if (!steel_private) {
			// change to _to_
			tmp = replace_substr_in_str(text, private_nick, to_nick);
			free(text);
			text = tmp;
		}
	} else {
		// add new nickname to start string
		if (steel_private) {
			tmp = g_strconcat(private_nick, text, NULL);
		} else {
			tmp = g_strconcat(to_nick, text, NULL);
		}

		free(text);
		text = tmp;
	}

	gtk_entry_set_text(msgEntry, text);
	/*gtk_entry_buffer_set_text(buffer, text, 0);*/

	gtk_widget_grab_focus(GTK_WIDGET(msgEntry));
	gtk_editable_set_position(GTK_EDITABLE(msgEntry), -1);

	free(to_nick);
	free(private_nick);
	free(text);

	return false;
}

bool
parse_and_add_system_message(const char *str)
{
	ulong hash = 0;
	int status_code = 0;
	char status_code_str[9];
	int hours = 99, minutes = 99;
	char time[6];
	char text[MAX_CHAT_MESSAGE*2+1]; // Пока с запасом
	char *message;

	GtkTextIter iter;
	GtkTextBuffer *msgViewBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(msgView_system));

	// Parse
	if (5 != sscanf(str, "Z,%lx:%d:%d\t%i\t%[^\n]", &hash, &hours, &minutes, &status_code, text)) {
		elog("Message parse error: %s\n", str);
		return false;
	}

	// Check messages cache
	for (uint i = 0; i < countof(MCS); i++) {
		if (MCS[i] == hash) {
			vlog("Message exist in cache. Don't add");
			return false;
		}
	}

	// fixme bug with "Z,000819ae:21:10\t11\t\tsinon_woolf"

	switch (status_code) {
		case 14:
			message = g_strconcat("Вас просканировал персонаж: ", text, NULL);
			break;

		case 11:
			message = g_strconcat("Персонаж ", text, " сейчас вне сети ", NULL);
			break;

		default:
			sprintf(status_code_str, "%i", status_code);
			message = g_strconcat("Системка #", status_code_str, " and:", text, NULL);
	}

	sprintf(time, "%.2i:%.2i", hours, minutes);

	// Show message in text view
	gtk_text_buffer_get_end_iter(msgViewBuffer, &iter);

	if (gtk_text_buffer_get_char_count(msgViewBuffer) != 0) {
		gtk_text_buffer_insert(msgViewBuffer, &iter, "\n", -1);
	}

	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, time, -1, "time", NULL);
	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, " ", -1, "monospace", NULL);

	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, message, -1, "system", NULL);

	// clear temp memory
	free(message);

	return true;
}

bool
parse_and_add_message(const char *str)
{
	gulong position = 0;
	int hours = 99, minutes = 99, text_color_index = 0;
	int nick_spaces = 0;
	GtkTextIter iter;
	GtkWidget *view;
	GtkTextBuffer *msgViewBuffer;

	char time[6];
	char nick[MAX_NICK_SIZE*2+1];	// *2 for utf8 and +1 for '\0'
	char text[MAX_CHAT_MESSAGE*2+1];	// FIXME: Overflow (сообщения приходящие не ограниченны?)
	char message[MAX_CHAT_MESSAGE*2+1];	// Add *5 for monkey fix problem

	bool self_message = false;


	vlog("Start parse input message");
	if (5 != sscanf(str, "S,%lx:%d:%d [%[^]]] %[^\n]", &position, &hours, &minutes, nick, text)) {
		elog("Message parse error: %s\n", str);
		return false;
	}
	if (2 != sscanf(text, "%[^\t]\t%d", message, &text_color_index)) {
		elog("Color parse error: %s\n", text);
	}

	// Check messages cache
	for (uint i = 0; i < countof(MCC); i++) {
		if (MCC[i] == position) {
			vlog("Message exist in cache. Don't add");

			return false;
		}
	}

	if (current_player_name && strcmp(nick, current_player_name) == 0) {
		vlog("This message is my self.");
		self_message = true;
	}


	char *prvt_nick = g_strconcat("private [", current_player_name, "]", NULL);
	char *to_nick = g_strconcat("to [", current_player_name, "]", NULL);

	if (!self_message && strstr(str, prvt_nick)) {
		system("aplay private.wav &>/dev/null &");
	} else if (!self_message && strstr(str, to_nick)) {
		system("aplay radio.wav &>/dev/null &");
	}

	free(prvt_nick);
	free(to_nick);


	if (strstr(str, "private [radio]") != NULL) {
		rem_substr(message, "private [radio] ");
		view = msgView_radio;
	} else if (strstr(str, "private [clan]") != NULL) {
		rem_substr(message, "private [clan] ");
		view = msgView_clan;
	} else if (strstr(str, "private [alliance]") != NULL) {
		rem_substr(message, "private [alliance] ");
		view = msgView_alliance;
	} else if (strstr(str, "private [") != NULL) {
		view = msgView_private;
	} else {
		view = msgView_system;
	}

	// add hash to message cache
	for (uint i = 0; i < countof(MCC); i++) {
		if (MCC[i] == 0) {
			if (i < countof(MCC)) {
				MCC[i] = position;
				MCC[i+1] = 0;
			} else {
				MCC[i] = position;
				MCC[0] = 0;
			}
			break;
		}
	}


	//if (!gtk_window_is_active(GTK_WINDOW(window))) {
		//message_notify(nick, message);
	//}

	msgViewBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	gtk_text_buffer_get_end_iter(msgViewBuffer, &iter);

	// make time message text
	sprintf(time, "%.2i:%.2i", hours, minutes);

	// find max nick white spaces
	int nick_len = g_utf8_strlen(nick, -1);
	if (MAX_NICK_SIZE > nick_len) {
		nick_spaces = MAX_NICK_SIZE - nick_len;
	}

	vlog("Insert message to chat");
	if (gtk_text_buffer_get_char_count(msgViewBuffer) != 0) {
		gtk_text_buffer_insert(msgViewBuffer, &iter, "\n", -1);
	}

	// time
	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, time, -1, "time", NULL);

	// nickname spaces
	char *str_spaces = malloc(nick_spaces + 1); // +1 for space after time
	memset(str_spaces, ' ', nick_spaces + 1);
	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, str_spaces, nick_spaces + 1, "monospace", NULL);
	free(str_spaces);

	// nickname
	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, nick, -1, "nickname", NULL);
	gtk_text_buffer_insert(msgViewBuffer, &iter, ": ", -1);

	// message
	char tmp_color[4];
	sprintf(tmp_color, "c%i", text_color_index+1);
	gtk_text_buffer_insert_with_tags_by_name(msgViewBuffer, &iter, message, -1, tmp_color, NULL);

	msgViewMark = gtk_text_buffer_create_mark(msgViewBuffer, NULL, &iter, false);
	if (msgViewMark != NULL) {
		gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(view), msgViewMark);
		gtk_text_buffer_delete_mark(msgViewBuffer, msgViewMark);
	}

	return true;
}

void
setChatState(int state)
{
	// check before set
	#ifndef gtk_widget_set_sensitive
	#define gtk_widget_set_sensitive(w, s) if (gtk_widget_is_sensitive(w) != s) { \
		gtk_widget_set_sensitive(w, s); \
	}
	#endif

	if (state == CHAT_FULL_OFF) {
		gtk_widget_set_sensitive(GTK_WIDGET(msgEntry), false);
		gtk_widget_set_sensitive(btnCmd, false);
		gtk_widget_set_sensitive(btnClear, false);
		gtk_widget_set_sensitive(btnSmiles, false);
		gtk_widget_set_sensitive(btnTranslit, false);
	}
	if (state == CHAT_ON) {
		chatOn = true;
		gtk_widget_set_sensitive(btnSmiles, true);
		// FIXME: Don't implemented translit
		//gtk_widget_set_sensitive(btnTranslit), true);
			gtk_widget_set_sensitive(GTK_WIDGET(msgEntry), true);
			gtk_widget_set_sensitive(btnCmd, true);
			gtk_widget_set_sensitive(btnClear, true);
		if (!chatRefresh) {
			send_raw("//refreshoff");
		}
	} else if (state == CHAT_OFF) {
		chatOn = false;
		gtk_widget_set_sensitive(btnSmiles, false);
		//gtk_widget_set_sensitive(btnTranslit), false);
		clear_room_players_buffer();
	} else if (state == CHAT_REFRESH_OFF) {
		send_raw("//refreshoff");
		chatRefresh = false;
	} else if (state == CHAT_REFRESH_ON) {
		send_raw("//refreshon");
		chatRefresh = true;
	}

	room_widget_redraw();
	recreate_cmd_popup_menu(NULL);
}

void
recreate_cmd_popup_menu(const char *cmd_str)
{
	// FIXME: Уже и не помню. Что за бред?
	if (cmd_str == NULL) {
		if (last_cmd_str != NULL) {
			cmd_str = last_cmd_str;
		} else {
			return;
		}
	}

	if (last_cmd_str != cmd_str) {
		if (last_cmd_str != NULL) {
			free(last_cmd_str);
		}
		last_cmd_str = strdup(cmd_str);
	}

	GtkWidget *menu_item;
	//char menu_item_buf[128];

	//if (cmdMenu != NULL) {
		//gtk_widget_unref(cmdMenu);
		//gtk_menu_detach(GTK_MENU(cmdMenu));
		//cmdMenu = NULL;
	//}
	cmdMenu = gtk_menu_new();
	gtk_menu_set_reserve_toggle_size(GTK_MENU(cmdMenu), false);

	bool trade = false, snow = false, coin = false, flower = false, flower2 = false, flower3 = false, venom = false, love = false, love2 = false, love3 = false, dirt = false, clan = false, alliance = false, radio = false, list = false, kiss = false, kiss2 = false, kiss3 = false, battle = false;

	char *tmp = strdup(cmd_str);
	char *pch = NULL;

	pch = strtok(tmp, ",");

	while (pch != '\0' && pch != NULL) {
/* cmd trade snow coin flower flower2 flower3 battle venom love love2 love3 clan alliance list radio */
		if (g_ascii_strcasecmp(pch, "trade") == 0)
			trade = true;
		if (g_ascii_strcasecmp(pch, "snow") == 0)
			snow = true;
		if (g_ascii_strcasecmp(pch, "dirt") == 0)
			dirt = true;
		if (g_ascii_strcasecmp(pch, "coin") == 0)
			coin = true;
		if (g_ascii_strcasecmp(pch, "flower") == 0)
			flower = true;
		if (g_ascii_strcasecmp(pch, "flower2") == 0)
			flower2 = true;
		if (g_ascii_strcasecmp(pch, "flower3") == 0)
			flower3 = true;
		if (g_ascii_strcasecmp(pch, "battle") == 0)
			battle = true;
		if (g_ascii_strcasecmp(pch, "venom") == 0)
			venom = true;
		if (g_ascii_strcasecmp(pch, "kiss") == 0)
			kiss = true;
		if (g_ascii_strcasecmp(pch, "kiss2") == 0)
			kiss2 = true;
		if (g_ascii_strcasecmp(pch, "kiss3") == 0)
			kiss3 = true;
		if (g_ascii_strcasecmp(pch, "love") == 0)
			love = true;
		if (g_ascii_strcasecmp(pch, "love2") == 0)
			love2 = true;
		if (g_ascii_strcasecmp(pch, "love3") == 0)
			love3 = true;
		if (g_ascii_strcasecmp(pch, "clan") == 0)
			clan = true;
		if (g_ascii_strcasecmp(pch, "alliance") == 0)
			alliance = true;
		if (g_ascii_strcasecmp(pch, "radio") == 0)
			radio = true;
		if (g_ascii_strcasecmp(pch, "list") == 0)
			list = true;
		pch = strtok(NULL, ",");
	}

	if (chatOn) {
		gtk_menu_set_title(GTK_MENU(cmdMenu), "CMD");
		menu_item = gtk_menu_item_new_with_label("//stop - выключает чат");
		gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
		g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(send_raw), "//stop");
	} else {
		menu_item = gtk_menu_item_new_with_label("//start - включить чат");
		gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
		g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(send_raw), "//start");
	}

	if (chatOn == true) {
		if (chatRefresh) {
			menu_item = gtk_menu_item_new_with_label("//refreshoff - выключает обновление списка людей в локации");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(setChatState), GINT_TO_POINTER(CHAT_REFRESH_OFF));
		} else {
			menu_item = gtk_menu_item_new_with_label("//refreshon - включает обновление списка людей в локации");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(setChatState), GINT_TO_POINTER(CHAT_REFRESH_ON));
		}

		menu_item = gtk_menu_item_new_with_label("//ping - узнать время отклика сервера");
		gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
		g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(send_raw), "//ping");

		menu_item = gtk_menu_item_new_with_label("//clear - очистить окно");
		gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
		g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(clear_cb), NULL);

		menu_item = gtk_menu_item_new_with_label("to [] //info - открыть информацию об этом персонаже");
		gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
		g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//info");

		if (trade) {
			menu_item = gtk_menu_item_new_with_label("to [] //trade - предложить этому персонажу обмен");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//trade");
		}

		if (kiss) {
			menu_item = gtk_menu_item_new_with_label("to [] //kiss - послать воздушный поцелуй");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//kiss");
		}

		if (kiss2) {
			menu_item = gtk_menu_item_new_with_label("to [] //kiss2 - другой вариант воздушного поцелу");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//kiss2");
		}

		if (kiss3) {
			menu_item = gtk_menu_item_new_with_label("to [] //kiss3 - другой вариант воздушного поцелу");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//kiss3");
		}

		if (flower) {
			menu_item = gtk_menu_item_new_with_label("to [] //flower - послать девушке букет роз");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(insert_to_entry), "//flower");
		}

		if (flower2) {
			menu_item = gtk_menu_item_new_with_label("to [] //flower2 - послать девушке букет колокольчиков");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(insert_to_entry), "//flower2");
		}

		if (flower3) {
			menu_item = gtk_menu_item_new_with_label("to [] //flower3 - послать девушке букет ромашек");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(insert_to_entry), "//flower3");
		}

		if (coin) {
			menu_item = gtk_menu_item_new_with_label("to [] //coin - бросить медную монету");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//coin");
		}

		if (dirt) {
			menu_item = gtk_menu_item_new_with_label("to [] //dirt - бросить грязью");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//dirt");
		}

		if (venom) {
			menu_item = gtk_menu_item_new_with_label("to [] //venom - облить ядом");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//venom");
		}

		if (snow) {
			menu_item = gtk_menu_item_new_with_label("to [] //snow - бросить снежком");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(insert_to_entry), "//snow");
		}

		if (clan) {
			menu_item = gtk_menu_item_new_with_label("private [clan] - Написать в клан");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(insert_to_start_entry), "private [clan] ");
		}

		if (alliance) {
			menu_item = gtk_menu_item_new_with_label("private [alliance] - Написать в альянс");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(insert_to_start_entry), "private [alliance] ");
		}

		if (radio) {
			menu_item = gtk_menu_item_new_with_label("private [radio] - Написать в радиоволну");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate",
									 G_CALLBACK(insert_to_start_entry), "private [radio] ");
		}

		if (battle) {
			menu_item = gtk_menu_item_new_with_label("//battle - напасть на монстров");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(send_raw), "//battle");
		}

		if (list) {
			menu_item = gtk_menu_item_new_with_label("//list - список участников радиоволны");
			gtk_menu_shell_append(GTK_MENU_SHELL(cmdMenu), menu_item);
			g_signal_connect_swapped(menu_item, "activate", G_CALLBACK(send_raw), "//list");
		}
	}

	g_free(tmp);
	gtk_widget_show_all(cmdMenu);
}

// CALLBACKS
static bool
press_scan_info_cb(GtkWidget *w)
{
	g_debug("Event: show_scan_info");
	return false;
}

static bool
activate_msgEntry_cb(GtkWidget *w)
{
	const char *text = gtk_entry_get_text(GTK_ENTRY(w));
	char *msg;

	int cur_tab_indx = gtk_notebook_get_current_page(GTK_NOTEBOOK(tab_bar));
	if (cur_tab_indx == 2) {
		msg = g_strconcat("private [radio] ", text, NULL);
	} else if (cur_tab_indx == 3) {
		msg = g_strconcat("private [clan] ", text, NULL);
	} else if (cur_tab_indx == 4) {
		msg = g_strconcat("private [alliance] ", text, NULL);
	} else {
		msg = g_strconcat("", text, NULL);
	}

	send_text(msg);
	free(msg);

	gtk_entry_set_text(GTK_ENTRY(w), "");
	gtk_widget_grab_focus(GTK_WIDGET(msgEntry));
	return false;
}

static bool
press_smiles_cb(GtkWidget *w)
{
	g_debug("TODO Event: Smiles.");

	return false;
}

static bool
press_cmd_cb(GtkWidget *w, GdkEvent *e)
{
	if (cmdMenu != NULL) {
		// create_cmdMenu();
		gtk_menu_popup(GTK_MENU(cmdMenu), NULL, NULL, NULL, NULL, 2, 0);
	} else {
		elog("Don't exist info about commands. Try latter or report to developer.");
	}

	return false;
}

static bool
clear_cb(GtkWidget *w)
{
	MCC[0] = 0;

	return false;
}

void
room_label_cb(GtkWidget *w, gpointer *data)
{
	char *location;
	const char *room, *building;

	room = gtk_label_get_text(GTK_LABEL(room_label_location));

	if (in_building) {
		building = gtk_label_get_text(GTK_LABEL(room_label_building));

		location = g_strconcat(room, "|", building, NULL);
		insert_to_entry(location);

		free(location);
	} else {
		insert_to_entry(room);
	}
}