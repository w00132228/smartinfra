#include "log_level.h"
#include <string.h>

const char* log_level_to_string(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        case LOG_LEVEL_OFF:   return "OFF";
        default:              return "UNKNOWN";
    }
}

LogLevel log_level_from_string(const char* str) {
    if (str == NULL) return LOG_LEVEL_INFO;
    
    if (strcmp(str, "DEBUG") == 0) return LOG_LEVEL_DEBUG;
    if (strcmp(str, "INFO") == 0)  return LOG_LEVEL_INFO;
    if (strcmp(str, "WARN") == 0)  return LOG_LEVEL_WARN;
    if (strcmp(str, "ERROR") == 0) return LOG_LEVEL_ERROR;
    if (strcmp(str, "OFF") == 0)   return LOG_LEVEL_OFF;
    
    return LOG_LEVEL_INFO;
}

