CC=gcc
CFLAGS=-c -std=c99 -Wall -Werror -Iinclude -I../commons/include
LDFLAGS=-lev

COMMONS = ../commons/socket-handler.o ../commons/comm_lib.o
SOURCES = src/matrixterminal.c src/lk204_handler.c
OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = matrixterminal2

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(COMMONS) -o $@

%.o: %.cc
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

