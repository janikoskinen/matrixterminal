CC=gcc
COMMON=../../common-components/c
CFLAGS=-fPIC -c -Wall -Werror -I../include -I$(COMMON)/include

UTILS = ../../common-components/c/utils.o

SOURCES = libmtdisplayer_test.c libmtdisplayer_weather.c
OBJECTS = $(SOURCES:.c=.so)
LIBDIR = /usr/local/lib/matrixterminal/

all:
	gcc $(CFLAGS) libmtdisplayer_test.c libmtdisplayer_weather.c
	gcc -g -shared -o libmtdisplayer_test.so libmtdisplayer_test.o -lc
	gcc -g -shared -o libmtdisplayer_weather.so libmtdisplayer_weather.o $(UTILS) -lc
install:
	cp $(OBJECTS) $(LIBDIR)

clean:
	rm -f *.o *.so

