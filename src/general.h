#ifndef __GENERAL_H
#define __GENERAL_H

#include <stdbool.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include "log.h"

#define countof(a) ((sizeof(a)/sizeof((a)[0])))

#define MIN_WINDOW_WIDTH 1004
#define MIN_WINDOW_HEIGHT 540
#define MAX_ROOM_NICKS 1500
#define MAX_CHAT_MESSAGE 2600
#define MAX_NICK_SIZE 16
#define MAX_CLAN_SIZE 30
#define NICK_SIZE 512 // size in memory for Nickname

#define VERSION "0.3"
#ifndef VERSION_EXT
#  define VERSION_EXT "?"
#endif
#define PRINT_VERSION " \033[1mBuild:\033[0m %s, %s \t\t\033[1mVersion:\033[0m %s-%s\n", __DATE__, __TIME__, VERSION, VERSION_EXT


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

// Player
typedef struct tzPlayer_t {
	unsigned long long int battleid;
	int group;
	unsigned int state;
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
	unsigned int level;
	unsigned int rank;
	char *key_file_path;
} tzLogin;

#endif
