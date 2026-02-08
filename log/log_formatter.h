#ifndef LOG_FORMATTER_H
#define LOG_FORMATTER_H

#include "log_level.h"
#include <time.h>

void log_format_header(char* buffer, size_t size,
                      LogLevel level,
                      const char* file,
                      int line,
                      const char* function);
void log_format_timestamp(char* buffer, size_t size, time_t timestamp);
const char* log_get_thread_id(void);

#endif // LOG_FORMATTER_H

