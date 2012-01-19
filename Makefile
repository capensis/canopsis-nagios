###################################
# Makefile for NEB2amqp
###################################

all: clean neb2amqp.o

neb2amqp.o:
	cd src && make

clean:
	cd src && make clean
	rm -f *.o

install:

