#include "log_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FILE_PATH "./logs/app.log"
#define DEFAULT_MAX_SIZE (100 * 1024 * 1024) // 100MB
#define DEFAULT_MAX_FILES 10
#define DEFAULT_FLUSH_INTERVAL 1000

void log_config_init(LogConfig* config)
{
    if (!config) {
        return;
    }
    
    config->level = LOG_LEVEL_INFO;
    strncpy(config->file_path, DEFAULT_FILE_PATH, sizeof(config->file_path) - 1);
    config->file_path[sizeof(config->file_path) - 1] = '\0';
    config->max_file_size = DEFAULT_MAX_SIZE;
    config->max_files = DEFAULT_MAX_FILES;
    config->async_enabled = 1;
    config->flush_interval = DEFAULT_FLUSH_INTERVAL;
}

static int parse_config_line(LogConfig* config, const char* line)
{
    char key[64] = {0};
    char value[256] = {0};
    
    if (sscanf(line, "%63[^=]=%255[^\n]", key, value) != 2) {
        return 0;
    }
    
    if (strcmp(key, "level") == 0) {
        config->level = log_level_from_string(value);
    } else if (strcmp(key, "file_path") == 0) {
        strncpy(config->file_path, value, sizeof(config->file_path) - 1);
    } else if (strcmp(key, "max_file_size") == 0) {
        config->max_file_size = atoll(value);
    } else if (strcmp(key, "max_files") == 0) {
        config->max_files = atoi(value);
    } else if (strcmp(key, "async_enabled") == 0) {
        config->async_enabled = atoi(value);
    } else if (strcmp(key, "flush_interval") == 0) {
        config->flush_interval = atoi(value);
    }
    
    return 1;
}

int log_config_load_from_file(LogConfig* config, const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return 0;
    
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        parse_config_line(config, line);
    }
    
    fclose(file);
    return 1;
}

int log_config_save_to_file(const LogConfig* config, const char* path) {
    FILE* file = fopen(path, "w");
    if (!file) return 0;
    
    fprintf(file, "level=%s\n", log_level_to_string(config->level));
    fprintf(file, "file_path=%s\n", config->file_path);
    fprintf(file, "max_file_size=%zu\n", config->max_file_size);
    fprintf(file, "max_files=%d\n", config->max_files);
    fprintf(file, "async_enabled=%d\n", config->async_enabled);
    fprintf(file, "flush_interval=%d\n", config->flush_interval);
    
    fclose(file);
    return 1;
}

