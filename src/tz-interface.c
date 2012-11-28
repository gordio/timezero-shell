#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include "flash.h"
#include "utils.h"


// Выполнить команду
void
tzExec(const char *cmd)
{
	char *tmp;

	tmp = g_strconcat("if (tz) { tz.", cmd, "}", NULL);
	webkit_web_view_execute_script(webView, tmp);

	free(tmp);
}

// Установить значение переменной
void
tzSetVar(const char *var, const char *value)
{
	char *cmd;

	cmd = g_strconcat("SetVariable(\"", var, "\", \"", value, "\")", NULL);
	tzExec(cmd);

	free(cmd);
}

// Открыть окно с информацией о персонаже
void
tz_info_open(const char *nick)
{
	char *cmd;

	cmd = g_strconcat("getUserInfo(\"", nick, "\");", NULL);
	tzExec(cmd);

	free(cmd);
}

// Просмотреть идущий бой
void
tz_battle_show(const unsigned int *battle_id)
{
	char *cmd;

	cmd = g_strconcat("lookBattle(\"", battle_id, "\");", NULL);
	tzExec(cmd);

	free(cmd);
}

// Войти в бой
void
tz_battle_join(const int side, const unsigned int battle_id, int bloodbtl)
{
	char *cmd;

	cmd = g_strconcat("joinBattle(\"", battle_id, "\", \"", side, "\", \"", bloodbtl, "\");", NULL);
	tzExec(cmd);

	free(cmd);
}

// Обновить размеры флеш окна (актуально при растягивании)
void
tzFlashResize()
{
	tzExec("resizeStage();");
}

void
tzLogout(void)
{
	tzExec("gameLogOut();");
}

/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
