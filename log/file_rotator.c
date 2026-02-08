#include "file_rotator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>

struct FileRotator {
    char base_path[256];
    char current_path[256];
    size_t max_size;
    int max_files;
    FILE* current_file;
    long current_size;
};

static void ensure_directory(const char* path) {
    char dir_path[256];
    strncpy(dir_path, path, sizeof(dir_path) - 1);
    dir_path[sizeof(dir_path) - 1] = '\0';
    
    char* dir = dirname(dir_path);
    if (access(dir, F_OK) != 0) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", dir);
        system(cmd);
    }
}

static int get_file_size(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return (int)st.st_size;
    }
    return 0;
}

static void remove_old_files(const char* base_path, int max_files) {
    char pattern[512];
    snprintf(pattern, sizeof(pattern), "%s.*", base_path);
    
    char* dir = dirname(strdup(base_path));
    DIR* dp = opendir(dir);
    if (!dp) return;
    
    struct dirent* entry;
    int count = 0;
    
    while ((entry = readdir(dp)) != NULL) {
        if (strstr(entry->d_name, base_path)) {
            count++;
        }
    }
    closedir(dp);
    
    if (count > max_files) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "ls -t %s.* | tail -n +%d | xargs rm -f", 
                base_path, max_files + 1);
        system(cmd);
    }
}

FileRotator* file_rotator_create(const char* base_path, 
                                 size_t max_size, 
                                 int max_files) {
    FileRotator* rotator = malloc(sizeof(FileRotator));
    if (!rotator) return NULL;
    
    strncpy(rotator->base_path, base_path, sizeof(rotator->base_path) - 1);
    strncpy(rotator->current_path, base_path, sizeof(rotator->current_path) - 1);
    rotator->max_size = max_size;
    rotator->max_files = max_files;
    rotator->current_file = NULL;
    rotator->current_size = 0;
    
    ensure_directory(base_path);
    
    rotator->current_size = get_file_size(base_path);
    rotator->current_file = fopen(base_path, "a");
    
    return rotator;
}

void file_rotator_destroy(FileRotator* rotator) {
    if (!rotator) return;
    
    if (rotator->current_file) {
        fclose(rotator->current_file);
    }
    free(rotator);
}

int file_rotator_should_rotate(FileRotator* rotator) {
    if (!rotator || !rotator->current_file) return 0;
    
    fflush(rotator->current_file);
    rotator->current_size = get_file_size(rotator->current_path);
    
    return rotator->current_size >= (long)rotator->max_size;
}

int file_rotator_rotate(FileRotator* rotator) 
{
    if (!rotator || !rotator->current_file) {
        return 0;
    }
    
    fclose(rotator->current_file);
    
    for (int i = rotator->max_files - 1; i > 0; i--) {
        char old_path[512] = { 0 };
        char new_path[512] = { 0 };
        
        if (i == 1) {
            snprintf(old_path, sizeof(old_path), "%s", rotator->base_path);
        } else {
            snprintf(old_path, sizeof(old_path), "%s.%d", rotator->base_path, i - 1);
        }
        snprintf(new_path, sizeof(new_path), "%s.%d", rotator->base_path, i);
        
        if (access(old_path, F_OK) == 0) {
            rename(old_path, new_path);
        }
    }
    
    remove_old_files(rotator->base_path, rotator->max_files);
    
    rotator->current_file = fopen(rotator->base_path, "w");
    strcpy(rotator->current_path, rotator->base_path);
    rotator->current_size = 0;
    
    return rotator->current_file != NULL;
}

FILE* file_rotator_get_current_file(FileRotator* rotator) 
{
    return rotator ? rotator->current_file : NULL;
}

const char* file_rotator_get_current_path(FileRotator* rotator) 
{
    return rotator ? rotator->current_path : NULL;
}

