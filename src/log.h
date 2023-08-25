#ifndef LOG_H
#define LOG_H

struct logger;

#define LEVEL_TRACE 10
#define LEVEL_DEBUG 20
#define LEVEL_INFO 30
#define LEVEL_WARNING 40
#define LEVEL_ERROR 50
#define LEVEL_CRITICAL 60

#define log_trace(...)     log_msg(LEVEL_TRACE, __VA_ARGS__)
#define log_debug(...)     log_msg(LEVEL_DEBUG, __VA_ARGS__)
#define log_info(...)      log_msg(LEVEL_INFO, __VA_ARGS__)
#define log_warning(...)   log_msg(LEVEL_WARNING, __VA_ARGS__)
#define log_error(...)     log_msg(LEVEL_ERROR, __VA_ARGS__)
#define log_critical(...)  log_msg(LEVEL_CRITICAL, __VA_ARGS__)

int open_logfile(const char * filename);
void log_set_loglevel(int level);
int log_msg(int level, const char *msg, ...);
int close_logfile();

#endif // LOG_H
