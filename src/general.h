#ifndef _GENERAL_H
#define _GENERAL_H

#include <stdbool.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include "log.h"

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef NULL
#define NULL (0L)
#endif

#define uint unsigned int
#define countof(a) ((sizeof(a)/sizeof((a)[0])))

#define MIN_WINDOW_WIDTH 1004
#define MIN_WINDOW_HEIGHT 540
#define MAX_ROOM_NICKS 1500
#define MAX_CHAT_MESSAGE 2600
#define MAX_NICK_SIZE 16
#define MAX_CLAN_SIZE 30
#define NICK_SIZE 512 // size in memory for Nickname

#define LOG_FILE_NAME "timezero.log"
#define PRINT_VERSION " \e[1mVersion:\e[0m %s \t\t\e[1mBuild:\e[0m %s, %s\n", VERSION, __DATE__, __TIME__


#ifdef ENABLE_NLS
#include <libintl.h>
#include <locale.h>
#define _(str) gettext(str)
#define gettext_noop(str) str
#define N_(str) gettext_noop(str)
#define S_(str) sgettext(str)
#define PACKAGE "TimeZero"
#define LOCALEDIR "./locale"
#else
#define _(str) ((char*) (str))
#define N_(str) ((char*) (str))
#define S_(str) ((char*) (str))
#endif

#include "../build/config.h"


enum {
	CHAT_FULL_OFF,
	CHAT_OFF,
	CHAT_ON,
	CHAT_REFRESH_OFF,
	CHAT_REFRESH_ON,
};

// TODO
typedef struct tzTab_t {
	GtkWidget *main_panels;

	GtkWidget *flash_panel;
	WebKitWebView *webView;

	GtkWidget *chat_panel;
} tzTab;
// TODO
typedef struct tzWindow_t {
	GtkWidget window;
	tzTab tab[30];
} tzWindow;

// Player
typedef struct tzPlayer_t {
	unsigned long long int battleid;
	int group;
	uint state;
	char *clan;
	char *nick;
	int level;
	int rank;
	int minlevel;
	int aggr; // Фракция
	int stake;
} tzPlayer;

// Login
typedef struct tzLogin_t {
	char *login;
	char *password;
	char *clan;
	char *profession;
	uint level;
	uint rank;
	char *key_file_path;
} tzLogin;


#define CHAT_TIME_COLOR "#A0A0A0"
#define CHAT_NICK_COLOR "#EEEEEE"
#define CHAT_SYSTEM_COLOR "#8B6D48"

#endif /* end guard: _GENERAL_H */