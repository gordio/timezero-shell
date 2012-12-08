#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"
#include "main.h"

#include "log.h"

#ifdef ENABLE_CLI_COLORS
#define CLI_COLOR_RED "\033[31m"
#define CLI_COLOR_GREEN "\033[31m"
#define CLI_COLOR_GRAY "\033[37m"
#define CLI_COLOR_YELLOW "\033[33m"
#define CLI_COLOR_DISABLE "\033[m"
#else
#define CLI_COLOR_RED ""
#define CLI_COLOR_GREEN ""
#define CLI_COLOR_GRAY ""
#define CLI_COLOR_YELLOW ""
#define CLI_COLOR_DISABLE ""
#endif


/* ERROR */
void
elog(const char *fmt, ...)
{
	va_list ptr; // извлечение аргумента ptr

	// Инициализация ptr, он становится указателем на первый аргумент,
	// следующий за строкой форматирования
	va_start(ptr, fmt);

	int size = snprintf(NULL, 0, " [%sE%s] %s\n", CLI_COLOR_RED, CLI_COLOR_DISABLE, fmt) + 1;
	char *msg = malloc(size);

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
wlog(const char *fmt, ...)
{
	va_list ptr;

	va_start(ptr, fmt);

	int size = snprintf(NULL, 0, " [%sW%s] %s\n", CLI_COLOR_YELLOW, CLI_COLOR_DISABLE, fmt) + 1;
	char *msg = malloc(size);

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
ilog(const char *fmt, ...)
{
	va_list ptr;

	va_start(ptr, fmt);

	int size = snprintf(NULL, 0, " [I] %s\n", fmt) + 1;
	char *msg = malloc(size);

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
vlog(const char *fmt, ...)
{
	if (!verbose) {
		return;
	}

	va_list ptr;

	va_start(ptr, fmt);

	int size = snprintf(NULL, 0, "%s [V] %s%s\n", CLI_COLOR_GRAY, fmt, CLI_COLOR_DISABLE) + 1;
	char *msg = malloc(size);

	if (asprintf(&msg, "%s [V] %s%s\n", CLI_COLOR_GRAY, fmt, CLI_COLOR_DISABLE) == -1) {
		return;
	}
	vfprintf(stdout, msg, ptr);
	fflush(stdout);
	va_end(ptr);

	free(msg);
}

/* vim: set fdm=marker ts=4 sw=4 tw=100 fo-=t ff=unix: */
