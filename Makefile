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

CC     = gcc

CFLAGS = -fPIC -shared

INCLUDES = -Ilib/jansson-2.3.1/src/ -Ilib/librabbitmq/ -Ilib/iniparser/src/ -Ilib/ -Isrc/

SUFFIXES = .o .c .h .a .so

# Ar settings to build the library
AR      = ar
ARFLAGS = cq

RM      = rm -f
ECHO    = echo

COMPILE.c=$(CC) $(INCLUDES) $(CFLAGS) -c
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)

SRCS_JSON = $(wildcard lib/jansson-2.3.1/src/*.c)
SRC_RMQ   = $(wildcard lib/librabbitmq/*.c)
SRC_INI   = $(wildcard lib/iniparser/src/*.c)
SRC_N2A   = $(wildcard src/*.c)

OBJS_JSON = $(SRCS_JSON:.c=.o)
OBJS_RMQ  = $(SRC_RMQ:.c=.o)
OBJS_INI  = $(SRC_INI:.c=.o)

default:    libjansson.a librabbitmq.a libiniparser.a neb2amqp.o

libjansson.a: $(OBJS_JSON)
	@($(AR) $(ARFLAGS) libjansson.a $(OBJS_JSON))

librabbitmq.a: $(OBJS_RMQ)
	@($(AR) $(ARFLAGS) librabbitmq.a $(OBJS_RMQ))

libiniparser.a: $(OBJS_INI)
	@($(AR) $(ARFLAGS) libiniparser.a $(OBJS_INI))

neb2amqp.o: libjansson.a librabbitmq.a libiniparser.a
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $(SRC_N2A) libjansson.a librabbitmq.a	libiniparser.a
	@($(ECHO) "\n$@ compiled successfuly!")
	

clean:
	$(RM) $(OBJS_JSON) $(OBJS_RMQ) $(OBJS_INI)

verryclean: 
	$(RM) $(OBJS_JSON) $(OBJS_RMQ) $(OBJS_INI) libjansson.a librabbitmq.a libiniparser.a neb2amqp.o
