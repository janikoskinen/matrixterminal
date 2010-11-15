CC=gcc
CFLAGS=-fPIC -c -Wall -Werror -I../include -I../commons/include

SOURCES = libmtdisplayer_test.c

OBJECTS = $(SOURCES:.c=.so)

all:
	gcc -fPIC -g -c -Wall -I../include libmtdisplayer_test.c
	gcc -g -shared -o libmtdisplayer_test.so libmtdisplayer_test.o -lc

clean:
	rm -f *.o *.so

