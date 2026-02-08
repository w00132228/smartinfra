#include "log_queue.h"
#include <stdlib.h>
#include <string.h>

LogQueue* log_queue_create(int capacity) {
    LogQueue* queue = malloc(sizeof(LogQueue));
    if (!queue) return NULL;
    
    queue->entries = malloc(sizeof(LogEntry) * capacity);
    if (!queue->entries) {
        free(queue);
        return NULL;
    }
    
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    
    return queue;
}

void log_queue_destroy(LogQueue* queue) {
    if (!queue) return;
    
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    
    free(queue->entries);
    free(queue);
}

int log_queue_push(LogQueue* queue, const LogEntry* entry) {
    if (!queue || !entry) return 0;
    
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->count >= queue->capacity) {
        pthread_cond_wait(&queue->not_full, &queue->mutex);
    }
    
    memcpy(&queue->entries[queue->tail], entry, sizeof(LogEntry));
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->count++;
    
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
    
    return 1;
}

int log_queue_pop(LogQueue* queue, LogEntry* entry) {
    if (!queue || !entry) return 0;
    
    pthread_mutex_lock(&queue->mutex);
    
    while (queue->count <= 0) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }
    
    memcpy(entry, &queue->entries[queue->head], sizeof(LogEntry));
    queue->head = (queue->head + 1) % queue->capacity;
    queue->count--;
    
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    
    return 1;
}

int log_queue_is_empty(LogQueue* queue) {
    if (!queue) return 1;
    
    pthread_mutex_lock(&queue->mutex);
    int empty = (queue->count == 0);
    pthread_mutex_unlock(&queue->mutex);
    
    return empty;
}

int log_queue_is_full(LogQueue* queue) {
    if (!queue) return 0;
    
    pthread_mutex_lock(&queue->mutex);
    int full = (queue->count >= queue->capacity);
    pthread_mutex_unlock(&queue->mutex);
    
    return full;
}

