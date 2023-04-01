#ifndef LOG_H
#define LOG_H

void log_enter_context(char *name);
void log_leave_context(void);
void log_error(const char *const fmt, ...);
void log_warning(const char *const fmt, ...);
void log_debug(const char *const fmt, ...);
void log_info(const char *const fmt, ...);
void log_append_error(const char *const fmt, ...);
void log_append_warning(const char *const fmt, ...);
void log_append_debug(const char *const fmt, ...);
void log_append_info(const char *const fmt, ...);

#endif /* LOG_H */
