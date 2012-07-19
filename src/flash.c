/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <gdk/gdkkeysyms.h>*/
#include <gtk/gtk.h>
#include <webkit/webkit.h>

#include "general.h"
#include "main.h"
#include "window.h"
#include "chat.h"
#include "autologin.h"
#include "tz-interface.h"
#include "events.h"
#include "utils.h"

WebKitWebView *webView;			// Фрейм с флешем
// static WebKitWebView *create_gtk_window_around_cb();
WebKitNavigationResponse open_url();
static bool script_alert_cb();

bool tz_fullscreen = false;
bool flash_init = false;
extern GtkWidget *flash_panel;

GtkWidget *chat_panel = NULL;


static void
webkit_set_proxy(const char *host, guint port, const char *user, const char *pwd)
{
	SoupURI *proxy = NULL;

	if (host) {
		proxy = soup_uri_new(NULL);
		soup_uri_set_scheme(proxy, SOUP_URI_SCHEME_HTTP);
		soup_uri_set_host(proxy, host);
		soup_uri_set_port(proxy, port);
		soup_uri_set_user(proxy, user);
		soup_uri_set_password(proxy, pwd);
	}

	g_object_set(webkit_get_default_session(), SOUP_SESSION_PROXY_URI, proxy, NULL);
}

GtkWidget *
create_flash_frame()
{
	char *user_agent;
	WebKitWebSettings *settings;
	char *strHTML;

	webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
	settings = webkit_web_settings_new();
	user_agent = g_strconcat("TimeZero Linux Shell/", VERSION, " (X11; Linux; ru-ru)", NULL);

	g_object_set(G_OBJECT(settings), "user-agent", user_agent, NULL);
	g_object_set(G_OBJECT(settings), "auto-load-images", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-developer-extras", true, NULL);
	g_object_set(G_OBJECT(settings), "default-encoding", "UTF-8", NULL);
	g_object_set(G_OBJECT(settings), "default-font-size", 12, NULL);
	g_object_set(G_OBJECT(settings), "default-monospace-font-size", 10, NULL);
	g_object_set(G_OBJECT(settings), "minimum-font-size", 8, NULL);
	g_object_set(G_OBJECT(settings), "enable-default-context-menu", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-dns-prefetching", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-fullscreen", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-java-applet", false, NULL);
	g_object_set(G_OBJECT(settings), "enable-page-cache", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-spell-checking", true, NULL);
	g_object_set(G_OBJECT(settings), "spell-checking-languages", "en", NULL);
	g_object_set(G_OBJECT(settings), "user-stylesheet-uri", "file://", NULL);
	g_object_set(G_OBJECT(settings), "enforce-96-dpi", true, NULL);
	g_object_set(G_OBJECT(settings), "javascript-can-access-clipboard", true, NULL);
	g_object_set(G_OBJECT(settings), "javascript-can-open-windows-automatically", true, NULL);
	// Apply the result
	webkit_web_view_set_settings(WEBKIT_WEB_VIEW(webView), settings);

	tz_file_path = g_strconcat("file://", tz_file_path, NULL);

	strHTML = g_strconcat("<html><head><style>* {margin:0;padding:0;}</style>\
<script>function recieveFromFlash(data) {alert(data);return true;}\
function initialize() {recieveFromFlash(\"Init,\")}</script></head><body>\
<embed id=\"tz\" src=\"", tz_file_path, "\" name=\"tz\" align=\"middle\" play=\"true\" \
loop=\"false\" quality=\"high\" allowScriptAccess=\"always\" width=\"100%\" \
height=\"100%\" scale=\"exactfit\" flashvars=\"language=ru\" \
allowscriptaccess=\"always\" swliveconnect=\"true\" quality=\"best\" \
bgcolor=\"#11100E\" name=\"plugin\" type=\"application/x-shockwave-flash\" \
pluginspage=\"http://www.macromedia.com/go/getflashplayer\">\
</embed><script>var tz = document.getElementById(\"tz\");</script></body></html>", NULL);

	webkit_web_view_load_html_string(WEBKIT_WEB_VIEW(webView), strHTML, tz_file_path);

	g_signal_connect(G_OBJECT(webView), "script-alert", G_CALLBACK(&script_alert_cb), NULL);
	g_signal_connect(G_OBJECT(webView), "new-window-policy-decision-requested", G_CALLBACK(&open_url), webView);

	//webkit_set_proxy("127.0.0.1", 8099, "", "");
	free(user_agent);
	free(strHTML);

	return GTK_WIDGET(webView);
}

// CALLBACKS
WebKitNavigationResponse
open_url(WebKitWebView *webView, WebKitWebFrame *frame, WebKitNetworkRequest *request)
{
	const char *uri = webkit_network_request_get_uri(request);

	if (strstr(uri, "battle") != NULL) {
		// TODO: Create battle preview widget
		wlog("Battle: %s", uri);
		return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
	}

	char *cmd = malloc(1024);

	sprintf(cmd, "chrome %s", uri);
	printf("%s\n", cmd);
	free(cmd);
	return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
}

static bool
script_alert_cb(WebKitWebView *webView, WebKitWebFrame *frame, char *data, gpointer user_data)
{
	vlog("Receive from flash: '%s'", data);

	if (g_str_has_prefix(data, "S,")) {
		parse_and_add_message(data);
	} else if (g_str_has_prefix(data, "A,")) {
		tz_list_add(data, false);
	} else if (g_str_has_prefix(data, "D,")) {
		tz_list_remove(data);
	} else if (g_str_has_prefix(data, "R,")) {
		tz_list_refresh(data);
	} else if (g_str_has_prefix(data, "info,")) {
		char *nick = strdup(data + 5);
		insert_nick_to_entry(nick, TRUE);
		free(nick);
	} else if (g_str_has_prefix(data, "Z,")) {
		parse_and_add_system_message(data);
	} else if (g_str_has_prefix(data, "fullscreen,1")) {
		tz_fullscreen = true;
		if (chat_panel) {
			gtk_widget_hide(chat_main_box);
		}
		al_window_show();
	} else if (g_str_has_prefix(data, "fullscreen,0")) {
		tz_fullscreen = false;
		if (chat_panel) {
			gtk_widget_show_all(chat_panel);
		} else {
			chat_panel = create_chat_frame();
			gtk_paned_pack2(GTK_PANED(main_panels), chat_panel, true, false);
			gtk_widget_show_all(chat_panel);
			gtk_widget_hide(room_label_building); // fixme: this bad
		}
		al_window_hide();
	} else if (g_str_has_prefix(data, "cmd,")) {
		recreate_cmd_popup_menu(data);
	} else if (g_str_has_prefix(data, "Start,")) {
		tz_chat_start(data);
		setChatState(CHAT_ON);
	} else if (g_str_has_prefix(data, "Stop,0")) {
		setChatState(CHAT_ON);
	} else if (g_str_has_prefix(data, "Stop,1")) {
		setChatState(CHAT_OFF);
	} else if (g_str_has_prefix(data, "Stop,2")) {
		setChatState(CHAT_OFF);
	} else if (g_str_has_prefix(data, "vip,")) {
		vlog("VIP: %s", g_strdup(data + 4));
	} else if (g_str_has_prefix(data, "Init,")) {
		flash_init = true;
	} else {
		wlog("Unknown data: %s", data);
	}

	return true;
}
