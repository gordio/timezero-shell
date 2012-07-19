/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _FLASH_H
#  define _FLASH_H
#  include <webkit/webkit.h>

GtkWidget *create_flash_frame();
extern WebKitWebView *webView;
extern bool *tz_fullscreen;

void tz_info_open();
void tz_battle_show();
void tz_battle_join();

void tzExec();
void tzSetVar();

void tz_log_out(void);
#endif							/* end guard: _FLASH_H */
