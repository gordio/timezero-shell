/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _AUTOLOGIN_H
#define _AUTOLOGIN_H


#define AL_FILE_NAME "autologins.json"
#define MAX_LOGIN_CHARS 80
#define MAX_PASSWORD_CHARS 80
#define MAX_AUTOLOGIN_ITEMS 20


void al_window_show();
void al_window_hide();

bool al_list_update_by_player();

#endif
