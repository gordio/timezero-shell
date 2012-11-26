#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>

#define CONF_FILE_NAME "config.json"


typedef struct {
	char *name;
	char *value;

	void *next;
} Conf;


bool conf_init(void);

bool conf_get(const char const *name, char **value);
bool conf_get_int(const char const *name, int *value);
#define conf_get_char(a, b) conf_get(a,b)

bool conf_set(const char const *name, const char *value);
bool conf_set_int(const char const *name, int value);
#define conf_set_char(a, b) conf_set(a,b)

bool conf_save(void);
bool conf_load(void);



#endif
