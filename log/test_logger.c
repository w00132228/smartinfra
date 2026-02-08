#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* test_thread(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 100; i++) {
        LOG_INFO("Thread %d: Log message %d", thread_id, i);
        usleep(1000);
    }
    
    return NULL;
}

int main() {
    printf("Testing logger module...\n");
    
    // 创建测试配置文件
    LogConfig config;
    log_config_init(&config);
    config.level = LOG_LEVEL_DEBUG;
    config.max_file_size = 1024; // 1KB for testing
    config.max_files = 5;
    
    // 使用配置初始化日志
    logger_init_with_config(&config);
    
    // 测试各级别日志
    LOG_DEBUG("This is a debug message");
    LOG_INFO("This is an info message");
    LOG_WARN("This is a warning message");
    LOG_ERROR("This is an error message");
    
    // 测试热配置
    logger_set_level(LOG_LEVEL_WARN);
    LOG_DEBUG("This debug should not appear");
    LOG_WARN("This warning should appear");
    
    // 测试多线程
    pthread_t threads[4];
    int thread_ids[4] = {1, 2, 3, 4};
    
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, test_thread, &thread_ids[i]);
    }
    
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 测试文件轮转
    for (int i = 0; i < 1000; i++) {
        LOG_INFO("Filling log file, message %d", i);
    }
    
    // 重新加载配置
    log_config_save_to_file(&config, "test_log.conf");
    logger_reload_config("test_log.conf");
    
    LOG_INFO("After reload config");
    
    // 清理
    logger_shutdown();
    
    printf("Logger test completed. Check ./logs/app.log*\n");
    printf("Log files should rotate when exceeding 1KB\n");
    
    return 0;
}

