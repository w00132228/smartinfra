#include "log_formatter.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

void log_format_timestamp(char* buffer, size_t size, time_t timestamp) {
    struct tm tm_time;
    localtime_r(&timestamp, &tm_time);
    
    snprintf(buffer, size,
             "%04d-%02d-%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday,
             tm_time.tm_hour,
             tm_time.tm_min,
             tm_time.tm_sec);
}

void log_format_header(char* buffer, size_t size,
                      LogLevel level,
                      const char* file,
                      int line,
                      const char* function) {
    time_t now = time(NULL);
    char time_str[64];
    
    log_format_timestamp(time_str, sizeof(time_str), now);
    
    const char* filename = strrchr(file, '/');
    if (!filename) filename = file;
    else filename++;
    
    snprintf(buffer, size,
             "[%s] [pid:%d] [tid:%lx] [%s] [%s:%d] [%s] - ",
             time_str,
             getpid(),
             (unsigned long)pthread_self(),
             log_level_to_string(level),
             filename,
             line,
             function);
}

const char* log_get_thread_id(void) {
    static __thread char tid_str[32] = {0};
    if (tid_str[0] == '\0') {
        snprintf(tid_str, sizeof(tid_str), "%lx", (unsigned long)pthread_self());
    }
    return tid_str;
}

