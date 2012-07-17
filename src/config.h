#ifndef _config_H
#define _config_H
#include <stdbool.h>

char * conf_get(char *name);
bool conf_set(char *name, char *value);
bool conf_save();
bool conf_load();

#endif