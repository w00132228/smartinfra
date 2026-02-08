#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "log_level.h"
#include "log_config.h"

void logger_init(void);
void logger_init_with_config(const LogConfig* config);
void logger_shutdown(void);

void logger_set_level(LogLevel level);
void logger_set_max_file_size(size_t size);
void logger_set_max_files(int count);
void logger_reload_config(const char* config_path);

void logger_log(LogLevel level,
               const char* file,
               int line,
               const char* function,
               const char* format, ...);

#define LOG_DEBUG(format, ...) \
    logger_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
    logger_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
    logger_log(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
    logger_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, format, ##__VA_ARGS__)

#endif // __LOGGER_H__

