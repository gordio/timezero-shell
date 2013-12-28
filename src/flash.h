#ifndef __FLASH_H
#define __FLASH_H

#include <stdbool.h>
#include <webkit/webkit.h>


extern WebKitWebView *webView;
extern bool *tz_fullscreen;


// Возвращает виджет с флешкой внутри
GtkWidget *create_flash_frame(void);

// Выполнить tz.swf функцию
//void tz_exec(const char const *cmd);
// Установить переменную в tz.swf
void tz_set_var(const char const *var, const char const *value);

// Отправить сообщение
void tz_chat_send(const char const *text);
// Нужно ли обновлять чат?
void tz_chat_refresh(const bool enabled);

// Открыть окно информации о персонаже
void tz_info_open(const char const *nick);
// Включить просмотр боя
void tz_battle_show(const unsigned int *id);
// Присоединится к бою
void tz_battle_join(const int side, const unsigned int id, const int blood);

// Отправить сигнал о необходимости пересчета размера внутриностей флешки
void tz_flash_resize(void);


// Автоматический вход в игру
void tz_autoauth(char* login, char* password);

// отправить сигнал выхода
void tz_logout(void);


#endif
