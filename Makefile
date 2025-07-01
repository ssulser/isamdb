# compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99 -MMD -MP
DEBUG = -g -DDEBUG=1

# Zielprogramm
TARGET = main

#
SOURCES = main.c isam.c serialize.c
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
	-rm mydata*

-include $(DEPS)
