/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _config_H
#define _config_H
#include <stdbool.h>

char * conf_get(char *name);
bool conf_set(char *name, char *value);
bool conf_save();
bool conf_load();

#endif