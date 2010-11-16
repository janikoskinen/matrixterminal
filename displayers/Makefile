CC=gcc
CFLAGS=-fPIC -c -Wall -Werror -I../include -I../../commons/include

SOURCES = libmtdisplayer_test.c
OBJECTS = $(SOURCES:.c=.so)
LIBDIR = /usr/local/lib/matrixterminal/

all:
	gcc $(CFLAGS) libmtdisplayer_test.c
	gcc -g -shared -o libmtdisplayer_test.so libmtdisplayer_test.o -lc

install:
	cp $(OBJECTS) $(LIBDIR)

clean:
	rm -f *.o *.so

