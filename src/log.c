/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "general.h"
#include "main.h"

#include "log.h"

#ifdef ENABLE_CLI_COLORS
#define CLI_COLOR_RED "\e[31m"
#define CLI_COLOR_GREEN "\e[31m"
#define CLI_COLOR_GRAY "\e[37m"
#define CLI_COLOR_YELLOW "\e[33m"
#define CLI_COLOR_DISABLE "\e[m"
#else
#define CLI_COLOR_RED ""
#define CLI_COLOR_GREEN ""
#define CLI_COLOR_GRAY ""
#define CLI_COLOR_YELLOW ""
#define CLI_COLOR_DISABLE ""
#endif


/* ERROR */
void
elog(const char *fmt, ...) {
	char *msg = malloc(8196); // рефакторовец, не забудь латать течь
	va_list ptr; // извлечение аргумента ptr

	// Инициализация ptr, он становится указателем на первый аргумент,
	// следующий за строкой форматирования
	va_start(ptr, fmt);
	if (asprintf(&msg, " [%sE%s] %s\n", CLI_COLOR_RED, CLI_COLOR_DISABLE, fmt) == -1) {
		return;
	}
	vfprintf(stderr, msg, ptr); // вывод сообщения
	fflush(stderr); // сброс буфера вывода
	va_end(ptr);

	free(msg);
}

/* WARNING */
void
wlog(const char *fmt, ...) {
	char *msg = malloc(8196);
	va_list ptr;

	va_start(ptr, fmt);
	if (asprintf(&msg, " [%sW%s] %s\n", CLI_COLOR_YELLOW, CLI_COLOR_DISABLE, fmt) == -1) {
		return;
	}
	vfprintf(stderr, msg, ptr); // вывод сообщения
	fflush(stderr); // сброс буфера вывода
	va_end(ptr);

	free(msg);
}

/* INFO */
void
ilog(const char *fmt, ...) {
	char *msg;
	va_list ptr;

	va_start(ptr, fmt);
	if (asprintf(&msg, " [I] %s\n", fmt) == -1) {
		return;
	}
	vfprintf(stdout, msg, ptr);
	fflush(stdout);
	va_end(ptr);

	free(msg);
}

/* VERBOSE */
void
vlog(const char *fmt, ...) {
	if (!verbose) {
		return;
	}

	char *msg;
	va_list ptr;

	va_start(ptr, fmt);
	if (asprintf(&msg, "%s [V] %s%s\n", CLI_COLOR_GRAY, fmt, CLI_COLOR_DISABLE) == -1) {
		return;
	}
	vfprintf(stdout, msg, ptr);
	fflush(stdout);
	va_end(ptr);

	free(msg);
}
