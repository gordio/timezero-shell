/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#include <glib.h>
#include "general.h"

#include "config.h"


struct _conf {
	char *name;
	char *value;
};

struct _conf CONF[] = {
	{"window-width", "MIN_WINDOW_WIDTH"},
	{"window-height", "650"},
	{"flash-fullscreen_size", "200"},
	{"flash-windowed_size", "200"},
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