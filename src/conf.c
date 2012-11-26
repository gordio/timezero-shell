#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <json.h>
#include "log.h"

#include "conf.h"

// Должен быть минимум 1 элемент
Conf CONF_DEF[] = {
	{"window.save_position", "1", NULL},
	{"window.width", "1004", NULL}, // MIN_WINDOW_WIDTH
	{"window.height", "650", NULL},
	{"flash.fullscreen_height", "600", NULL},
	{"flash.windowed_height", "200", NULL},
};


Conf CONF; // Глобальный конфиг


bool
conf_init(void)
{
	Conf *cur = &CONF;

	unsigned int conf_def_length = sizeof(CONF_DEF)/sizeof(Conf);

	// перебираем все стандартные значения заполняя глобальный CONF
	for (unsigned int i = 0; i < conf_def_length; i++) {
		// заполняем значения
		cur->name = g_strdup(CONF_DEF[i].name);
		cur->value = g_strdup(CONF_DEF[i].value);
		vlog("init: %s = %s", cur->name, cur->value);

		if (i+1 < conf_def_length) {
			// создаем очередной элемент
			cur->next = (Conf *)malloc(sizeof(Conf));
			// переключаемся на созданный элемент
			cur = cur->next;
		}
	}
	cur->next = NULL;
	
	conf_load();

	return true;
}

bool
conf_get(const char const *name, char **value)
{
	Conf *cur = &CONF;

	while (cur) {
		if (strcmp(name, cur->name) == 0) {
			*value = cur->value;
			vlog("get: %s = %s", name, cur->value);
			return true;
		}
		cur = cur->next;
	}

	vlog("get: %s = <not find>", name);
	return false;
}

bool
conf_get_int(const char const *name, int *value)
{
	char *str = NULL;

	if (conf_get(name, &str)) {
		if (str) {
			*value = atoi(str);
			return true;
		}
	}

	return false;
}

bool
conf_set(const char const *name, const char *value)
{
	Conf *cur = &CONF;

	while (cur) {
		if (strcmp(name, cur->name) == 0) {
			// освобождаем старую память
			if (cur->value) {
				free(cur->value);
			}

			// записываем новое значение
			cur->value = g_strdup(value);
			vlog("set: %s = %s", cur->name, cur->value);
			return true;
		}
		cur = cur->next;
	}

	// Создаем новый элемент
	cur = (Conf *)malloc(sizeof(Conf));
	cur->name = g_strdup(name);
	cur->value = g_strdup(value);
	cur->next = NULL;

	vlog("set: (new)%s = %s", cur->name, cur->value);
	return false;
}

bool
conf_set_int(const char const *name, int value)
{
	int size = snprintf(NULL, 0, "%i", value) + 1;
	char *buf = malloc(size);
	snprintf(buf, size, "%i", value);

	bool status;
	status = conf_set(name, buf);

	free(buf);

	return status;
}

bool
conf_save()
{
	json_object *root;

	root = json_object_new_object();
	Conf *cur = &CONF; // глобальный конфиг
	unsigned int conf_def_length = sizeof(CONF_DEF)/sizeof(Conf);

	while (cur) {
		// сохраняем только не совпадающие с дефолтом значения
		for (unsigned int i = 0; i < conf_def_length; i++) {
			if (strcmp(cur->name, CONF_DEF[i].name) == 0 &&
					strcmp(cur->value, CONF_DEF[i].value) != 0) {
				json_object_object_add(root, cur->name, json_object_new_string(cur->value));
				continue;
			}
		}
		cur = cur->next;
	}

	json_object_to_file(CONF_FILE_NAME, root);
	json_object_put(root);

	return false;
}

bool
conf_load()
{
	json_object *root;

	if (!g_file_test(CONF_FILE_NAME, G_FILE_TEST_EXISTS)) {
		return false;
	}

	root = json_object_from_file(CONF_FILE_NAME);

	json_object_object_foreach(root, key, val) {
		conf_set(key, json_object_get_string(val));
	}

	return true;
}
/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
