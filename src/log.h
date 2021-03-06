#ifndef __LOG_H
#define __LOG_H


#define LOG_FILE_NAME "timezero.log"

void elog(const char *fmt, ...); // Error
void wlog(const char *fmt, ...); // Warning
void ilog(const char *fmt, ...); // Info
void vlog(const char *fmt, ...); // Verbose


#endif
