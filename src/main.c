#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <webkit/webkit.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "general.h"
#include "window.h"
#include "flash.h"
#include "chat.h"
#include "autologin.h"

const char *tz_client_dir;

bool verbose = false, fullscreen = false, no_theme = false;
char *default_autologin;

char *tz_file_path = NULL;

#include "main.h"

static int initArgs();


int main(int argc, char **argv, char *envp[])
{
#if ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");
	textdomain(PACKAGE);
#endif

	const char *home_dir = g_getenv("HOME");
	if (!home_dir)
		home_dir = g_get_home_dir();

	// parse arguments
	if (!initArgs(argc, argv)) {
		return -1;
	}

	if (!tz_client_dir)
		tz_client_dir = g_strconcat(home_dir, G_DIR_SEPARATOR_S, ".timezero", G_DIR_SEPARATOR_S, NULL);

	if (chdir(tz_client_dir) == -1) {
		elog("Can't change work directory to: %s", tz_client_dir);
		return -1;
	}

	// Check tz.swf file exist in current dir
	tz_file_path = g_strconcat(tz_client_dir, "tz.swf", NULL);
	if (!g_file_test(tz_file_path, G_FILE_TEST_EXISTS)) {
		elog(_("File Don't exist: %s"), tz_file_path);
		return -2;
	}

	gtk_init(&argc, &argv);

	// create window and other
	create_main_window();

	gtk_main();
}

static int
initArgs(const int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("\e[1m Usage:\e[0m\n");
			printf("  %s --full-screen --autologin \"Nick Name\"\n", argv[0]);
			printf("\n");
			printf("\e[1m Options:\e[0m\n");
			printf("  --full-screen, -f     Start in full screen mode.\n");
			printf("  --autologin, -l       Auto LogIn  nickname.\n");
			printf("  --client-dir, -cd     TimeZero Client directory.\n");
			printf("  --no-theme, -nt       Use system GTK+ theme.\n");
			printf("  --auto-mine, -am      Use power Luke.\n");
			printf("\n");
			printf("\e[1m Extended options:\e[0m\n");
			printf("  --version, -v         Show version info.\n");
			printf("  --verbose, -V         Verbose mode.\n");
			printf("\n");
			printf(PRINT_VERSION);
			return FALSE;
		}
		if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-V") == 0) {
			verbose = TRUE;
			continue;
		}

		if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
			printf(PRINT_VERSION);
			return FALSE;
		}
		if (strcmp(argv[i], "--full-screen") == 0 || strcmp(argv[i], "-f") == 0) {
			fullscreen = TRUE;
			continue;
		}
		if (strcmp(argv[i], "--no-theme") == 0 || strcmp(argv[i], "-nt") == 0) {
			no_theme = TRUE;
			continue;
		}
		if (strcmp(argv[i], "--autologin") == 0 || strcmp(argv[i], "-l") == 0) {
			i++;
			if (i < argc) {
				default_autologin = g_strdup(argv[i]);
				continue;
			} else {
				wlog("Option '%s' usage: %s %s \"User Nick\"\n", argv[i-1], argv[0], argv[i-1]);
				i--; // return and parse
				continue;
			}
		}
		if (strcmp(argv[i], "--client-dir") == 0 || strcmp(argv[i], "-cd") == 0) {
			i++;
			if (i < argc) {
				tz_client_dir = argv[i];
				continue;
			} else {
				wlog("Option '%s' usage: %s %s \"/tmpfs/timezero/\"\n", argv[i-1], argv[0], argv[i-1]);
				i--; // return and parse
				continue;
			}
		}
		wlog("Unknown option '%s'. Ignored.\n", argv[i]);
	}
	return TRUE;
}
