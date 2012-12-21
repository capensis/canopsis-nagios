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
# Makefile for NEB2amqp Test
###################################

INCLUDES=-I../lib/ -I../src/ -I./
CC=gcc
#CFLAGS=-Wall -g -O2 -DHAVE_CONFIG_H
CFLAGS=-Wall -g -DHAVE_CONFIG_H

LDFLAGS=
LIBS=

BROKERLIBS=-ldl
BROKER_LDFLAGS=-Wall -rdynamic
# -lltdl

DBG_FLAGS=-DDEBUG=1
#gcc -Wall -g -O2 -DHAVE_CONFIG_H -o test test.c -Wl,-export-dynamic  -lltdl  -I../lib/ -I../src/ -I./
#gcc -Wall -g -O2 -DHAVE_CONFIG_H -DNSCORE -o nagios nagios.c broker.o .... -Wl,-export-dynamic    -lm  -lpthread -lltdl

all: clean test

test:
	$(CC) $(CFLAGS) -o $@ $@.c $(BROKER_LDFLAGS) $(LDFLAGS) $(BROKERLIBS) $(LIBS) $(INCLUDES)

clean:
	rm -f test

ini:
	gcc -g -o testini -I../lib/iniparser/src ../lib/iniparser/src/*.c ini.c
