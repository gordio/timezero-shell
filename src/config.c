#include <glib.h>
#include "general.h"

#include "config.h"


struct CONF_t {
	char *name;
	char *value;
};

struct CONF_t CONF[] = {
	{"fullscreen_flash_size", "200"},
	{"windowed_flash_size", "200"},
	{"auto_reconnect", "true"},
};

char *
conf_get(char *name) {
	for (uint i = 0; i < countof(CONF); ++i) {
		if (g_ascii_strcasecmp(CONF[i].name, name) == 0) {
			return CONF[i].value;
		}
	}

	// Can't find
	return NULL;
}

bool
conf_set(char *name, char *value) {
	for (uint i = 0; i < countof(CONF); ++i) {
		if (g_ascii_strcasecmp(CONF[i].name, name) == 0) {
			// FIXME: Pointer! Copy and delete old
			CONF[i].name = value;
			return true;
		}
	}

	// Error
	return false;
}

bool
conf_save() {

	// TODO: Save config to file

	return false;
}

bool
conf_load() {

	// TODO: Load config from file

	return false;
}