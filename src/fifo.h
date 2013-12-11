/*
 * Copyright (c) 2011 "Capensis" [http://www.capensis.com]
 * 
 * This file is part of Canopsis.
 * 
 * Canopsis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Canopsis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with Canopsis.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __CANONEB_FIFO_H
#define __CANONEB_FIFO_H

#define EVENT_RK_MAX_SIZE 8192
#define EVENT_MSG_MAX_SIZE 8192

#include <stdbool.h>

typedef struct sEvent event;
typedef struct sFifo fifo;

struct sEvent
{
    char *msg;
    char *rk;
    event *pNext;
    event *pPrev;
};

struct sFifo
{
    event *first;
    event *last;
    unsigned int size;
    unsigned int max_size;
    char *file_path;
    FILE *pFile;
    bool file_lock;
    bool dirty;
    bool full;
};

fifo *n2a_fifo_init (int max_size, char *file_path);
event *n2a_event_init (const char *rk, const char *msg);
event *n2a_event_init_nomalloc (const char *rk, const char *msg);

int n2a_push (fifo *pFifo, event *pEvent);
int n2a_prepend (fifo *pFifo, event *pEvent);
event *n2a_shift (fifo *pFifo);
event *n2a_pop (fifo *pFifo);

int n2a_fifo_open_file (fifo *pFifo);
int n2a_load (fifo *pFifo);
int n2a_csync (fifo *pFifo);
int n2a_clear (fifo *pFifo);
int n2a_check_size (fifo * pFifo);

void n2a_free_event (event *pEvent);
void n2a_free_fifo (fifo *pFifo);

#ifdef DEBUG
void n2a_print_event (event *pEvent);
void n2a_print_fifo (fifo *pFifo);
#endif

#endif /* __CANONEB_FIFO_H */
