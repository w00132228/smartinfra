#ifndef FILE_ROTATOR_H
#define FILE_ROTATOR_H

#include <stdio.h>
#include <stddef.h>

typedef struct FileRotator FileRotator;

FileRotator* file_rotator_create(const char* base_path, 
                                 size_t max_size, 
                                 int max_files);
void file_rotator_destroy(FileRotator* rotator);
int file_rotator_should_rotate(FileRotator* rotator);
int file_rotator_rotate(FileRotator* rotator);
FILE* file_rotator_get_current_file(FileRotator* rotator);
const char* file_rotator_get_current_path(FileRotator* rotator);

#endif // FILE_ROTATOR_H

