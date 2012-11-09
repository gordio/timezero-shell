#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdbool.h>


typedef struct {
	char *name;
	char *value;

	void *next;
} Conf;


bool conf_init(void);
bool conf_get(const char const *name, char *value);


#endif
