#--------------------------------
# Copyright (c) 2011 "Capensis" [http://www.capensis.com]
#
# This file is part of Canopsis.
#
# Canopsis is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Canopsis is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Canopsis.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------

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

