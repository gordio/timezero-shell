#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "conf.h"

// Должен быть минимум 1 элемент
Conf CONF_DEF[] = {
	{"window.save_position", "1", NULL},
	{"window.width", "1004", NULL}, // MIN_WINDOW_WIDTH
	{"window.height", "650", NULL},
	{"flash.fullscreen_size", "600", NULL},
	{"flash.windowed_size", "200", NULL},
};


Conf CONF; // Глобальный конфиг


bool
conf_init(void)
{
	Conf *cur = &CONF;

	// init first element
	unsigned int conf_def_length = sizeof(CONF_DEF)/sizeof(Conf);

	// перебираем все стандартные значения заполняя глобальный CONF
	for (unsigned int i = 0; i < conf_def_length; i++) {
		// заполняем значения
		cur->name = g_strdup(CONF_DEF[i].name);
		cur->value = g_strdup(CONF_DEF[i].value);

		if (i+1 < conf_def_length) {
			// создаем очередной элемент
			cur->next = (Conf *)malloc(sizeof(Conf));
			// переключаемся на созданный элемент
			cur = cur->next;
		}
	}
	cur->next = NULL;

	return true;
}

bool
conf_get(const char const *name, char *value)
{
	Conf *cur = &CONF;

	while (cur) {
		if (strcmp(name, cur->name) == 0) {
			printf("%s\n", cur->value);
			return true;
		}
		cur = cur->next;
	}

	// Can't find
	return false;
}

bool
conf_set(const char const *name, char *value)
{


	// Error
	return false;
}

bool
conf_save(const char const *file_name)
{

	// TODO: Save config to file

	return false;
}

bool
conf_load(const char const *file_name)
{

	// TODO: Load config from file

	return false;
}
/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
