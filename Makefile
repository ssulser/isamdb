# compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c89 -Iinclude -MMD -MP
DEBUG = -g -DDEBUG=1

# Zielprogramm
TARGET = main

#
SOURCES = main.c src/isam.c
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(SOURCES:.c=.d)

#default target
all: $(TARGET)

#Linken
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $^

# Kompilieren mit automatischen Abhängigkeitsdateien
%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -c $< -o $@


.phony: clean
clean:
	-rm *.o
	-rm $(TARGET)
	-rm *.d
	-rm src/*.o
	-rm src/*.d

-include $(DEPS)
