#ifndef __MAIN_H
#define __MAIN_H

#include "log.h"

#ifndef VERSION
#  define VERSION "undef"
#endif
#define PRINT_VERSION " \033[1mBuild:\033[0m %s, %s \t\t\033[1mVersion:\033[0m %s\n", __DATE__, __TIME__, VERSION

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

extern char *tz_file_path;
extern const char *tz_client_dir;
extern bool verbose, debug, fullscreen, no_theme;
extern char *autoauth_login;


#endif
