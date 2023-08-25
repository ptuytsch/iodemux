#include "log.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

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


int log_msg(int level, const char *format, ...)
{
    int ret;
    va_list args;
    char fmt[128];

    if (level < l.level) return -1;
    if (l.file == NULL) return -2;

    va_start(args, format);
    snprintf(fmt, 128, "%s\n", format);

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
