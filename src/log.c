#include "log.h"
#include <bits/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

struct logger {
    FILE *file;
    int level;
};

static struct logger l = {
    .file = NULL,
    .level = LEVEL_INFO
};

int open_logfile(const char * filename)
{
    l.file = fopen(filename, "a");
    if (l.file == NULL) {
	perror("open log file");
    }

    fprintf(l.file, "Opened logilfe...\n");
    return 0;
}

void log_set_loglevel(int level)
{
    l.level = level;
}

const char *log_get_log_level_str(int level)
{
    switch (level) {
	case LEVEL_TRACE:
	    return "TRACE";
	case LEVEL_DEBUG:
	    return "DEBUG";
	case LEVEL_INFO:
	    return "INFO";
	case LEVEL_WARNING:
	    return "WARNING";
	case LEVEL_ERROR:
	    return "ERROR";
	case LEVEL_CRITICAL:
	    return "CRITICAL";
	default:
	    return "??????";
    }
}


int log_msg(int level, const char *format, ...)
{
    int ret;
    va_list args;
    char fmt[128];
    char time_str[40];
    time_t raw_time;
    struct tm *time_info;
    struct timespec curr_time;

    time(&raw_time);
    time_info = localtime(&raw_time);
    clock_gettime(CLOCK_REALTIME, &curr_time);

    strftime(time_str, 40, "%d/%m/%Y %H:%M:%S", time_info);

    if (level < l.level) return -1;
    if (l.file == NULL) return -2;

    va_start(args, format);
    snprintf(fmt, 128,
	     "%s:%03ld | %s | %s\n",
	     time_str,
	     curr_time.tv_nsec / 1000000,
	     log_get_log_level_str(level),
	     format);

    ret = vfprintf(l.file, fmt, args);
    if (ret < 0) {
	perror("Log line");
    }

    va_end(args);
    fflush(l.file);
    return ret;
}

int close_logfile()
{
    fclose(l.file);
    return 0;
}
