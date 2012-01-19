###################################
# Makefile for NEB2amqp
###################################

INCLUDES=-I./librabbitmq -I../lib/ -I./
SRCPATH=src

CC=gcc
MOD_CFLAGS=-fPIC
CFLAGS=-g -O2 -DHAVE_CONFIG_H
MOD_LDFLAGS=-shared -lstdc++
#LDFLAGS=-L../lib/rabbitmq-c/librabbitmq/librabbitmq.la
LDFLAGS=
SOURCES=$(wildcard *.c librabbitmq/*.c)
LIBS=
DBG_FLAGS=-DDEBUG=1

CP=@CP@

all: clean neb2amqp.o

neb2amqp.o:
	$(CC) $(MOD_LDFLAGS) $(MOD_CFLAGS) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS) $(LIBS) $(INCLUDES)

clean:
	rm -f *.o

install:

