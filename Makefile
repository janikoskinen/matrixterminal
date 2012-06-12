CC=gcc
COMMON_DIR=../common-components/c

CFLAGS=-c -std=c99 -Wall -Werror -Iinclude -I$(COMMON_DIR)/include
LDFLAGS=-Ldisplayers -rdynamic -lev -ljansson -ldl $(LIB_DISPLAYERS) -Wl,-R/usr/local/lib/matrixterminal

LIB_DISPLAYERS=-lmtdisplayer_test

COMMONS = $(COMMON_DIR)/socket-handler.o $(COMMON_DIR)/comm_lib.o $(COMMON_DIR)/utils.o

SOURCES = src/matrixterminal.c src/lk204_handler.c src/displayer_commons.c

OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = matrixterminal2

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(COMMONS) -o $@ $(LDFLAGS)

%.o: %.cc
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

