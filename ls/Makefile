P=ls
CFLAGS=-std=c99 -g -O3
INCLUDE=-I/usr/local/opt/argp-standalone/include
LDLIBS=-L/usr/local/opt/argp-standalone/ -largp
LDFLAGS=-rdynamic
CC=gcc

CFILES = $(wildcard *.c)

$(P): $(CFILES)
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) $(LDLIBS) -o $(P) $(CFILES)
