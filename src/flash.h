#ifndef __FLASH_H
#define __FLASH_H

#include <webkit/webkit.h>

GtkWidget *create_flash_frame();
extern WebKitWebView *webView;
extern bool *tz_fullscreen;

void tz_info_open();
void tz_battle_show();
void tz_battle_join();

void tzExec();
void tzSetVar();

void tz_log_out(void);
#endif