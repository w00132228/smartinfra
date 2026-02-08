#ifndef LOG_LEVEL_H
#define LOG_LEVEL_H

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO  = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_OFF   = 4
} LogLevel;

const char* log_level_to_string(LogLevel level);
LogLevel log_level_from_string(const char* str);

#endif // LOG_LEVEL_H

