#include "logger.h"
#include "log_writer.h"
#include "log_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>

static struct {
    LogConfig config;
    LogWriter* writer;
    LogQueue* queue;
    pthread_t worker_thread;
    volatile int running;
    pthread_mutex_t config_mutex;
} g_logger = {0};

static void* log_worker_thread(void* arg) 
{
    (void)arg;
    
    while (g_logger.running || !log_queue_is_empty(g_logger.queue)) {
        LogEntry entry;
        
        if (log_queue_pop(g_logger.queue, &entry)) {
            pthread_mutex_lock(&g_logger.config_mutex);
            log_writer_write(g_logger.writer, entry.level,
                           entry.file, entry.line,
                           entry.function, entry.message);
            pthread_mutex_unlock(&g_logger.config_mutex);
        }
        
        usleep(g_logger.config.flush_interval * 1000);
    }
    
    return NULL;
}

static void create_worker_thread(void)
{
    if (!g_logger.config.async_enabled) return;
    
    g_logger.running = 1;
    pthread_create(&g_logger.worker_thread, NULL,
                  log_worker_thread, NULL);
}

static void init_logger_internal(void)
{
    log_config_init(&g_logger.config);
    pthread_mutex_init(&g_logger.config_mutex, NULL);
}

void logger_init(void)
{
    init_logger_internal();
    g_logger.writer = log_writer_create(g_logger.config.file_path,
                                       g_logger.config.max_file_size,
                                       g_logger.config.max_files);
    
    if (g_logger.config.async_enabled) {
        g_logger.queue = log_queue_create(4096);
        create_worker_thread();
    }
}

void logger_init_with_config(const LogConfig* config)
{
    init_logger_internal();
    
    if (config) {
        memcpy(&g_logger.config, config, sizeof(LogConfig));
    }
    
    g_logger.writer = log_writer_create(g_logger.config.file_path,
                                       g_logger.config.max_file_size,
                                       g_logger.config.max_files);
    
    if (g_logger.config.async_enabled) {
        g_logger.queue = log_queue_create(4096);
        create_worker_thread();
    }
}

void logger_shutdown(void)
{
    if (!g_logger.writer) return;
    
    g_logger.running = 0;
    
    if (g_logger.config.async_enabled && g_logger.queue) {
        pthread_join(g_logger.worker_thread, NULL);
        log_queue_destroy(g_logger.queue);
        g_logger.queue = NULL;
    }
    
    log_writer_flush(g_logger.writer);
    log_writer_destroy(g_logger.writer);
    g_logger.writer = NULL;
    
    pthread_mutex_destroy(&g_logger.config_mutex);
}

void logger_set_level(LogLevel level)
{
    pthread_mutex_lock(&g_logger.config_mutex);
    g_logger.config.level = level;
    pthread_mutex_unlock(&g_logger.config_mutex);
}

void logger_set_max_file_size(size_t size)
{
    pthread_mutex_lock(&g_logger.config_mutex);
    g_logger.config.max_file_size = size;
    pthread_mutex_unlock(&g_logger.config_mutex);
}

void logger_set_max_files(int count)
{
    pthread_mutex_lock(&g_logger.config_mutex);
    g_logger.config.max_files = count;
    pthread_mutex_unlock(&g_logger.config_mutex);
}

void logger_reload_config(const char* config_path)
{
    pthread_mutex_lock(&g_logger.config_mutex);
    
    LogConfig new_config;
    log_config_init(&new_config);
    
    if (log_config_load_from_file(&new_config, config_path)) {
        memcpy(&g_logger.config, &new_config, sizeof(LogConfig));
    }
    
    pthread_mutex_unlock(&g_logger.config_mutex);
}

void logger_log(LogLevel level, const char* file, int line, const char* function, const char* format, ...) 
{
    pthread_mutex_lock(&g_logger.config_mutex);
    LogLevel current_level = g_logger.config.level;
    pthread_mutex_unlock(&g_logger.config_mutex);
    
    if (level < current_level || !g_logger.writer) {
        return;
    }
    
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    if (g_logger.config.async_enabled && g_logger.queue) {
        LogEntry entry;
        entry.level = level;
        strncpy(entry.file, file, sizeof(entry.file) - 1);
        entry.line = line;
        strncpy(entry.function, function, sizeof(entry.function) - 1);
        strncpy(entry.message, message, sizeof(entry.message) - 1);
        
        log_queue_push(g_logger.queue, &entry);
    } else {
        pthread_mutex_lock(&g_logger.config_mutex);
        log_writer_write(g_logger.writer, level, file, line, function, message);
        pthread_mutex_unlock(&g_logger.config_mutex);
    }
}

