#include "log_writer.h"
#include "log_formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct LogWriter {
    FileRotator* rotator;
};

LogWriter* log_writer_create(const char* file_path,
                            size_t max_size,
                            int max_files) {
    LogWriter* writer = malloc(sizeof(LogWriter));
    if (!writer) return NULL;
    
    writer->rotator = file_rotator_create(file_path, max_size, max_files);
    if (!writer->rotator) {
        free(writer);
        return NULL;
    }
    
    return writer;
}

void log_writer_destroy(LogWriter* writer) {
    if (!writer) return;
    
    if (writer->rotator) {
        file_rotator_destroy(writer->rotator);
    }
    free(writer);
}

static int write_to_file(LogWriter* writer, const char* message) {
    if (!writer || !writer->rotator) return 0;
    
    FILE* file = file_rotator_get_current_file(writer->rotator);
    if (!file) return 0;
    
    if (file_rotator_should_rotate(writer->rotator)) {
        file_rotator_rotate(writer->rotator);
        file = file_rotator_get_current_file(writer->rotator);
        if (!file) return 0;
    }
    
    fprintf(file, "%s\n", message);
    fflush(file);
    
    return 1;
}

int log_writer_write(LogWriter* writer,
                    LogLevel level,
                    const char* file,
                    int line,
                    const char* function,
                    const char* message) {
    char header[512];
    char full_message[2048];
    
    log_format_header(header, sizeof(header), level, file, line, function);
    snprintf(full_message, sizeof(full_message), "%s%s", header, message);
    
    return write_to_file(writer, full_message);
}

void log_writer_flush(LogWriter* writer) {
    if (!writer || !writer->rotator) return;
    
    FILE* file = file_rotator_get_current_file(writer->rotator);
    if (file) {
        fflush(file);
    }
}

