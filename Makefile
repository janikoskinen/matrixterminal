CC=gcc

CFLAGS=-c -std=c99 -Wall -Werror -Iinclude -I../commons/include
LDFLAGS=-Ldisplayers -rdynamic -lev -ldl $(LIB_DISPLAYERS) -Wl,-R/usr/local/lib/matrixterminal

LIB_DISPLAYERS=-lmtdisplayer_test

COMMONS = ../commons/socket-handler.o ../commons/comm_lib.o

SOURCES = src/matrixterminal.c src/lk204_handler.c src/displayer_commons.c

OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = matrixterminal2

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(COMMONS) -o $@

%.o: %.cc
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

