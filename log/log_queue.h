#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H

#include <pthread.h>

typedef struct {
    char message[2048];
    int level;
    char file[128];
    int line;
    char function[128];
} LogEntry;

typedef struct {
    LogEntry* entries;
    int capacity;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} LogQueue;

LogQueue* log_queue_create(int capacity);
void log_queue_destroy(LogQueue* queue);
int log_queue_push(LogQueue* queue, const LogEntry* entry);
int log_queue_pop(LogQueue* queue, LogEntry* entry);
int log_queue_is_empty(LogQueue* queue);
int log_queue_is_full(LogQueue* queue);

#endif // LOG_QUEUE_H

