#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include "flash.h"
#include "utils.h"


// Выполнить команду
void tzExec(const char *cmd)
{
	webkit_web_view_execute_script(webView, g_strconcat("tz.", cmd, NULL));
}

// Установить значение переменной
void tzSetVar(const char *var, const char *value)
{
	char *cmd = g_strconcat("SetVariable(\"", var, "\", \"", value, "\")", NULL);
	tzExec(cmd);
	free(cmd);
}

// Открыть окно с информацией о персонаже
void tz_info_open(const char *nick)
{
	tzExec(g_strconcat("getUserInfo(\"", nick, "\");", NULL));
}

// Просмотреть идущий бой
void tz_battle_show(const uint *battle_id)
{
	tzExec(g_strconcat("lookBattle(\"", battle_id, "\");", NULL));
}

// Войти в бой
void tz_battle_join(const int side, const guint battle_id, int bloodbtl)
{
	tzExec(g_strconcat("joinBattle(\"", battle_id, "\", \"", side, "\", \"", bloodbtl, "\");", NULL));
}

// Обновить размеры флеш окна (актуально при растягивании)
void tzFlashResize()
{
	tzExec("resizeStage();");
}

// Выйти из игры
void tzLogout(void)
{
	tzExec("gameLogOut();");
}
