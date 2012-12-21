/*--------------------------------
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
# ---------------------------------*/

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef fifo_h
#define fifo_h

#define EVENT_RK_MAX_SIZE 8192
#define EVENT_MSG_MAX_SIZE 8192

////////// Structures
typedef struct sEvent event;
typedef struct sFifo fifo;

struct sEvent {
  char * msg;
  char * rk;
  event * pNext;
  event * pPrev;
};

struct sFifo {
  event * first;
  event * last;
  int size;
  int max_size;
  char * file_path;
  FILE * pFile;
  int file_lock;
};

////////// Headers

fifo * fifo_init(int max_size, char * file_path);
event * event_init(const char * rk, const char * msg);
event * event_init_nomalloc(const char * rk, const char * msg);

int push(fifo * pFifo, event * pEvent);
event * shift(fifo * pFifo);
event * pop(fifo * pFifo);

void fifo_open_file(fifo * pFifo);
int load(fifo * pFifo);
int csync(fifo * pFifo);
int clear(fifo * pFifo);


void free_event(event * pEvent);
void free_fifo(fifo * pFifo);

#endif