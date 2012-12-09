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
#include "events.h"
#include "utils.h"
#include "conf.h"

WebKitWebView *webView;

bool tz_fullscreen = false;
extern GtkWidget *flash_panel;

GtkWidget *chat_panel = NULL;

// Обрабатывает входящую команду и выполняет нужные функции над ней
static void command_router(const char const *data);

// CALLBACKS
static WebKitNavigationResponse open_url_cb();
static bool script_alert_cb();


/* GTK Widget {{{ */
// Создает виджет верхнего фрейма, цепляет обработчики и прочий обвес, возвращая только виджет.
GtkWidget *
create_flash_frame(void)
{
	char *user_agent;
	WebKitWebSettings *settings;
	char *strHTML;

	webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
	settings = webkit_web_settings_new();
	user_agent = "TimeZero Linux Shell/" VERSION " (X11; Linux; ru-ru)";

	g_object_set(G_OBJECT(settings), "user-agent", user_agent, NULL);
	g_object_set(G_OBJECT(settings), "auto-load-images", true, NULL);
	g_object_set(G_OBJECT(settings), "default-encoding", "UTF-8", NULL);
	g_object_set(G_OBJECT(settings), "default-font-size", 12, NULL);
	g_object_set(G_OBJECT(settings), "default-monospace-font-size", 10, NULL);
	g_object_set(G_OBJECT(settings), "minimum-font-size", 8, NULL);
	g_object_set(G_OBJECT(settings), "enable-default-context-menu", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-fullscreen", true, NULL);
	g_object_set(G_OBJECT(settings), "enable-java-applet", false, NULL);
	g_object_set(G_OBJECT(settings), "enable-page-cache", true, NULL);
	g_object_set(G_OBJECT(settings), "enforce-96-dpi", true, NULL);
	g_object_set(G_OBJECT(settings), "javascript-can-access-clipboard", true, NULL);
	//g_object_set(G_OBJECT(settings), "javascript-can-open-windows-automatically", true, NULL);
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
	g_signal_connect(G_OBJECT(webView), "new-window-policy-decision-requested", G_CALLBACK(&open_url_cb), webView);

	//webkit_set_proxy("127.0.0.1", 8099, "", "");
	/*free(user_agent);*/
	free(strHTML);

	return GTK_WIDGET(webView);
}

static void
webkit_set_proxy(const char *host, unsigned int port, const char *user, const char *pass)
{
	SoupURI *proxy = NULL;

	if (host) {
		proxy = soup_uri_new(NULL);
		soup_uri_set_scheme(proxy, SOUP_URI_SCHEME_HTTP);
		soup_uri_set_host(proxy, host);
		soup_uri_set_port(proxy, port);
		soup_uri_set_user(proxy, user);
		soup_uri_set_password(proxy, pass);
	}

	g_object_set(webkit_get_default_session(), SOUP_SESSION_PROXY_URI, proxy, NULL);
}

static WebKitNavigationResponse
open_url_cb(WebKitWebView *webView, WebKitWebFrame *frame, WebKitNetworkRequest *request)
{
	int ret;
	const char *uri = webkit_network_request_get_uri(request);

	if (strstr(uri, "battle") != NULL) {
		// TODO: Create battle preview widget
		wlog("Battle: %s", uri);
		return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
	}

	// FIXME: Escape url
	// FIXME: Use terminal with fork
	char *cmd = malloc(1024);

	sprintf(cmd, "xdg-open \"%s\" &", uri);

	vlog("EXEC: %s", cmd);
	ret = system(cmd);

	if (ret == -1)
		elog("Execute problem for command '%s'", cmd);

	free(cmd);
	return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
}
static bool
script_alert_cb(WebKitWebView *webView, WebKitWebFrame *frame, char *data, gpointer user_data)
{
	command_router(data);
	return true;
}
/* }}} */


/* FLASH Логика, и API {{{ */
static void
command_router(const char const *data)
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
		// приват персу
		char *nick = strdup(data + 5);
		insert_nick_to_entry(nick, true);
		free(nick);
	} else if (g_str_has_prefix(data, "Z,")) {
		// Системное сообщение
		parse_and_add_system_message(data);
	} else if (g_str_has_prefix(data, "fullscreen,1")) {
		// login screen
		tz_fullscreen = true;

		if (chat_panel) {
			gtk_widget_hide(chat_panel);
		}

		al_window_show();
	} else if (g_str_has_prefix(data, "fullscreen,0")) {
		// игровой режим
		tz_fullscreen = false;

		if (chat_panel) {
			gtk_widget_show_all(chat_panel);
		} else {
			chat_panel = create_chat_frame();
			gtk_paned_pack2(GTK_PANED(main_panels), chat_panel, true, false);
			gtk_widget_show_all(chat_panel);
		}
		gtk_widget_hide(room_label_building);

		al_window_hide();
	} else if (g_str_has_prefix(data, "cmd,")) {
		// список команд
		update_cmd(data + 4);
	} else if (g_str_has_prefix(data, "Start,")) {
		// Чат запущен. Обновить все данные и при необходимости подгрузить историю.
		tz_chat_start(data);
		setChatState(CHAT_ON);
	} else if (g_str_has_prefix(data, "Stop,")) {
		// n == 0 - Выход их игры
		// n == 1 - Остановка чата пользователем
		// n == 2 - отвалился, зараза :D
		switch (atoi(data+5)) {
			case 0:
				setChatState(CHAT_ON);
				break;
			case 1:
				setChatState(CHAT_OFF);
				break;
			case 2:
				setChatState(CHAT_OFF);
				break;
			default:
				setChatState(CHAT_OFF);
				break;
		}
	} else if (g_str_has_prefix(data, "vip,")) {
		// Нужно придумать вкусняшки
		vlog("VIP: %s", g_strdup(data + 4));
	} else if (g_str_has_prefix(data, "Init,")) {
		// TODO: Работать с флешем только после инициализации
		vlog("Flash init done.");
	} else if (g_str_has_prefix(data, "dealer_logins,")) {
		// Вообще бесполезная опция, или может добавлять автоматически в игнор? :hehe:
		vlog("Ignore command 'dealer_logins'.");
	} else if (g_str_has_prefix(data, "dealer_sites,")) {
		// Пока игнорируем, у меня нет идей куда использовать. Все URL и так будут активны
		vlog("Ignore command 'dealer_sites'.");
	} else {
		wlog("Unknown data: %s", data);
	}
}

static void
tz_exec(const char const *cmd)
{
	char *tmp = NULL;

	tmp = g_strconcat("if ((typeof tz) != 'undefined') { tz.", cmd, "}", NULL);
	webkit_web_view_execute_script(webView, tmp);

	free(tmp);
}

void
tz_set_var(const char const *var, const char const *value)
{
	char *cmd = NULL;

	cmd = g_strconcat("SetVariable(\"", var, "\", \"", value, "\")", NULL);
	tz_exec(cmd);

	free(cmd);
}

void
tz_chat_send(const char const *text)
{
	char *escaped = NULL;
	char *cmd = NULL;

	escaped = escape_str(text);
	cmd = g_strconcat("sendChat(\"", escaped, "\")", NULL);
	free(escaped);

	tz_exec(cmd);

	free(cmd);
}

void
tz_chat_refresh(const bool enabled)
{
	if (enabled) {
		tz_exec("sendChat(\"//refreshon\")");
	} else {
		tz_exec("sendChat(\"//refreshoff\")");
	}
}

void
tz_info_open(const char const *nick)
{
	char *cmd = NULL;

	cmd = g_strconcat("getUserInfo(\"", nick, "\");", NULL);
	tz_exec(cmd);

	free(cmd);
}

void
tz_battle_show(const unsigned int *id)
{
	char *cmd = NULL;

	cmd = g_strconcat("lookBattle(\"", id, "\");", NULL);
	tz_exec(cmd);

	free(cmd);
}

void
tz_battle_join(const int side, const unsigned int id, const int blood)
{
	char *cmd = NULL;

	cmd = g_strconcat("joinBattle(\"", id, "\", \"", side, "\", \"", blood, "\");", NULL);
	tz_exec(cmd);

	free(cmd);
}

void
tz_flash_resize(void)
{
	tz_exec("resizeStage();");
}

void
tz_logout(void)
{
	tz_exec("gameLogOut();");
}
/* }}} */

/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
