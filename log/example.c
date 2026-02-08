// example.c
#include <unistd.h>
#include "logger.h"


int main() {
    // 简单初始化
    logger_init();
    
    // 记录日志
    LOG_INFO("Application started");
    
    for (int i = 0; i < 10; i++) {
        LOG_DEBUG("Processing item %d", i);
        
        if (i == 5) {
            LOG_WARN("Halfway through processing");
        }
    }
    
    LOG_ERROR("Simulated error occurred");
    
    // 动态修改配置
    logger_set_level(LOG_LEVEL_WARN);
    logger_set_max_file_size(50 * 1024 * 1024); // 50MB
    
    LOG_DEBUG("This won't be logged after level change");
    LOG_WARN("This warning will be logged");
    
    // 重新加载配置文件
    logger_reload_config("/etc/myapp/log.conf");
    
    // 清理
    logger_shutdown();
    
    return 0;
}

