kdlib = ../libkdtree.a

CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -lm -g -I..
LDFLAGS = $(kdlib) -lm

.PHONY: all
all: test test2

test: test.c $(kdlib) -lm
test2: test2.c $(kdlib) -lm

.PHONY: clean
clean:
	rm -f test test2
