#ifndef _LOG_H
#define _LOG_H

void elog(const char *fmt, ...); // Error
void wlog(const char *fmt, ...); // Warning
void ilog(const char *fmt, ...); // Info
void vlog(const char *fmt, ...); // Verbose

#endif /* end guard: _LOG_H */
