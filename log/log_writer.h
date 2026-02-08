#ifndef LOG_WRITER_H
#define LOG_WRITER_H

#include "log_level.h"
#include "file_rotator.h"

typedef struct LogWriter LogWriter;

LogWriter* log_writer_create(const char* file_path,
                            size_t max_size,
                            int max_files);
void log_writer_destroy(LogWriter* writer);
int log_writer_write(LogWriter* writer,
                    LogLevel level,
                    const char* file,
                    int line,
                    const char* function,
                    const char* message);
void log_writer_flush(LogWriter* writer);

#endif // LOG_WRITER_H

