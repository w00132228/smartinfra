#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H

#include <stdlib.h>
#include <string.h>
#include "log_level.h"

typedef struct {
    LogLevel level;
    char file_path[256];
    size_t max_file_size;
    int max_files;
    int async_enabled;
    int flush_interval;
} LogConfig;

void log_config_init(LogConfig* config);
int log_config_load_from_file(LogConfig* config, const char* path);
int log_config_save_to_file(const LogConfig* config, const char* path);

#endif // LOG_CONFIG_H

