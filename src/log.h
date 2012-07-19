/*
 * TimeZero™ Shell - GTK Shell for online game www.TimeZero.ru
 * Copyright (C) 2011-2012  Gordienko Oleg
 * LICENCE: Creative Commons Attribution-NonCommercial-ShareAlike 2.0 (CC BY-NC-SA 2.0)
 * http://creativecommons.org/licenses/by-nc-sa/2.0/
 */

#ifndef _LOG_H
#define _LOG_H

void elog(const char *fmt, ...); // Error
void wlog(const char *fmt, ...); // Warning
void ilog(const char *fmt, ...); // Info
void vlog(const char *fmt, ...); // Verbose

#endif /* end guard: _LOG_H */
