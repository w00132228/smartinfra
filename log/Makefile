CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -pthread
TARGET = logger_test
OBJS = log_level.o log_config.o file_rotator.o log_formatter.o \
       log_writer.o log_queue.o logger.o test_logger.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) test_log.conf
	rm -rf logs/

test: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

.PHONY: all clean test valgrind
