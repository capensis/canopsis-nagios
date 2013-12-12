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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "nagios.h"
#include "fifo.h"

fifo *n2a_fifo_init (int max_size, char *file_path)
{
    fifo *pFifo = NULL;
    int loaded = 0;

    n2a_logger (LG_DEBUG, "FIFO: Init");
    n2a_logger (LG_DEBUG, "FIFO:  + max_size:  %d", max_size);
    n2a_logger (LG_DEBUG, "FIFO:  + file_path: %s", file_path);

    pFifo = malloc (sizeof (fifo));

    pFifo->first = NULL;
    pFifo->last = NULL;
    pFifo->pFile = NULL;
    pFifo->size = 0;
    pFifo->file_lock = false;
    pFifo->dirty = false;
    pFifo->full = false;

    pFifo->file_path = strdup (file_path);

    pFifo->max_size = max_size;

    loaded = n2a_load (pFifo);

    n2a_logger (LG_INFO, "FIFO: %d events loaded", loaded);

    return pFifo;
}

event *n2a_event_init (const char *rk, const char *msg)
{
    return n2a_event_init_nomalloc (strdup (rk), strdup (msg));
}

event *n2a_event_init_nomalloc (const char *rk, const char *msg)
{
    event *pEvent = malloc (sizeof (event));

    /* TODO: check size
     *   int size;
     *   size = sizeof(char) * (strlen(msg)+1);
     */

    pEvent->pNext = NULL;
    pEvent->pPrev = NULL;

    pEvent->msg = (char *) msg;
    pEvent->rk = (char *) rk;

    return pEvent;
}

int n2a_fifo_open_file (fifo *pFifo)
{
    if (!pFifo->pFile)
    {
        n2a_logger (LG_DEBUG, "FIFO: Open fifo file '%s'", pFifo->file_path);

        pFifo->pFile = fopen (pFifo->file_path, "r+");

        if (!pFifo->pFile)
        {
            pFifo->pFile = fopen (pFifo->file_path, "w+");
        }

        if (!pFifo->pFile)
        {
            n2a_logger (LG_ERR, "FIFO: Impossible to open '%s'", pFifo->file_path);
        }
        else
        {
            n2a_logger (LG_DEBUG, "FIFO: File successfully opened");
        }
    }
    else
    {
        n2a_logger (LG_DEBUG, "FIFO: '%s' already open", pFifo->file_path);
    }

    return (!pFifo->pFile ? 0 : 1);
}

int n2a_load (fifo *pFifo)
{
    int i = 0;

    n2a_logger (LG_DEBUG, "FIFO: Load events from file");

    if (pFifo->file_lock)
    {
        n2a_logger (LG_WARN, "FIFO: Fifo is locked");
        return 0;
    }

    if (!n2a_fifo_open_file (pFifo))
    {
        return 0;
    }

    pFifo->file_lock = true;

    rewind (pFifo->pFile);

    while(!feof (pFifo->pFile))
    {
        char rk[EVENT_MSG_MAX_SIZE];
        char msg[EVENT_MSG_MAX_SIZE];

        /* Read RK */
        fgets (rk, EVENT_MSG_MAX_SIZE, pFifo->pFile);

        /* Read event */
        fgets (msg, EVENT_MSG_MAX_SIZE, pFifo->pFile);

        /* Remove trailing \n */
        rk[strlen (rk) - 1] = 0;
        msg[strlen (msg) - 1] = 0;

        n2a_push (pFifo, n2a_event_init (rk, msg));
    
        i += 1;
    }
  
    if (pFifo->pFile)
    {
        n2a_logger (LG_DEBUG, "FIFO: Close file");
        fclose (pFifo->pFile);
        pFifo->pFile = NULL;
    }

    pFifo->file_lock = false;

    return i;
}

int n2a_clear (fifo *pFifo)
{
    n2a_logger (LG_DEBUG, "FIFO: Remove fifo file");

    if (pFifo->pFile)
    {
        fclose (pFifo->pFile);
    }

    pFifo->pFile = NULL;

    return unlink (pFifo->file_path);
}

void n2a_check_size (fifo *pFifo)
{
    if (pFifo->size >= pFifo->max_size && !pFifo->full)
    {
        pFifo->full = true;
        n2a_logger (LG_WARN, "FIFO: Queue is full, drop events ...");
    }

    if (pFifo->size < pFifo->max_size)
    {
        pFifo->full = false;
    }
}

int n2a_csync (fifo *pFifo)
{
    int i = 0;

    if (!pFifo)
    {
        n2a_logger (LG_ERR, "FIFO: Invalid fifo");
        return 0;    
    }

    if (!pFifo->dirty)
    {
        n2a_logger (LG_DEBUG, "FIFO: Up to date");
        return 0;
    }

    n2a_logger (LG_DEBUG, "FIFO: Sync %d events to file", pFifo->size);
  
    /* TODO:
     *   - Use pthread locks
     *   - Or better, but more complicated : use lock-free algorithm based on
     *     compare-and-swap atomic instruction.
     */

    if (pFifo->file_lock)
    {
        n2a_logger (LG_WARN, "FIFO: Fifo file is locked");
        return 0;
    }

    pFifo->file_lock = true;

    n2a_clear (pFifo);

    if (!n2a_fifo_open_file (pFifo))
    {
        pFifo->file_lock = false;
        return 0;
    }

    if (pFifo->first)
    {
        event *pEvent = NULL;

        for (pEvent = pFifo->first; pEvent != NULL; pEvent = pEvent->pNext, ++i)
        {
            fprintf (pFifo->pFile, "%s\n%s\n", pEvent->rk, pEvent->msg);
        }
    }

    fflush (pFifo->pFile);
  
    n2a_logger (LG_INFO, "FIFO: %d events written to file", i);

    pFifo->file_lock = false;
    pFifo->dirty = false;
  
    return i;
}

int n2a_push (fifo *pFifo, event *pEvent)
{
    n2a_logger (LG_DEBUG, "FIFO: Push (%d)", pFifo->size);

    n2a_check_size (pFifo);

    if (pFifo->full)
    {
        n2a_free_event (n2a_shift (pFifo));
    }

    if (pFifo->first == NULL)
    {
        pFifo->first = pEvent;
        pFifo->last = pEvent;
    }
    else
    {
        pEvent->pPrev = pFifo->last;
        pFifo->last->pNext = pEvent;
        pFifo->last = pEvent;
    }

    pFifo->size += 1;
    pFifo->dirty = true;

    return pFifo->size;
}

int n2a_prepend (fifo *pFifo, event *pEvent)
{
    n2a_logger (LG_DEBUG, "FIFO: Prepand (%d)", pFifo->size);

    n2a_check_size (pFifo);

    if (pFifo->full)
    {
        n2a_free_event (n2a_pop (pFifo));
    }

    if (pFifo->first == NULL)
    {
        pFifo->first = pEvent;
        pFifo->last = pEvent;
    }
    else
    {
        pEvent->pPrev = NULL;
        pEvent->pNext = pFifo->first;

        pFifo->first->pPrev = pEvent;
        pFifo->first = pEvent;
    }

    pFifo->size += 1;
    pFifo->dirty = true;

    return pFifo->size;
}

event *n2a_shift (fifo *pFifo)
{
    if (pFifo->first)
    {
        event *pEvent = pFifo->first;

        if (pEvent->pNext)
        {
            pFifo->first = pEvent->pNext;
            pFifo->first->pPrev = NULL;
        }
        else
        {
            pFifo->first = NULL;
        }

        pEvent->pNext = NULL;
        pEvent->pPrev = NULL;

        pFifo->size -= 1;
        pFifo->dirty = true;

        return pEvent;
    }
    else
    {
        return NULL;
    }
}

event *n2a_pop (fifo *pFifo)
{
    if (pFifo->last)
    {
        event *pEvent = pFifo->last;

        if (pEvent->pPrev)
        {
            pFifo->last = pEvent->pPrev;
            pFifo->last->pNext = NULL;
        }
        else
        {
            pFifo->last = NULL;
        }

        pEvent->pNext = NULL;
        pEvent->pPrev = NULL;

        pFifo->size -= 1;
        pFifo->dirty = true;
    
        return pEvent;
    }

    return NULL;
}

void n2a_free_event (event *pEvent)
{
    if (pEvent)
    {
        if (pEvent->msg != NULL)
        {
            free (pEvent->msg);
        }

        if (pEvent->rk != NULL)
        {
            free (pEvent->rk);
        }

        free (pEvent);
    }
}

void n2a_free_fifo (fifo * pFifo)
{
    n2a_csync (pFifo);

    n2a_logger (LG_DEBUG, "FIFO: Freeing fifo");

    if (pFifo->first)
    {
        event *pEvent = pFifo->first;
        event *pNext = NULL;

        do
        {
            pNext = pEvent->pNext;
            n2a_free_event (pEvent);
            pEvent = pNext;
        }
        while (pNext != NULL);
    }

    if (pFifo->pFile)
    {
        n2a_logger (LG_DEBUG, "FIFO: Close file");
        fclose (pFifo->pFile);
        pFifo->pFile = NULL;
    }

    free (pFifo->file_path);
    free (pFifo);
}

#ifdef DEBUG

void n2a_print_event (event *pEvent)
{
    if (pEvent)
    {
        printf ("%s -> %s\n", pEvent->rk, pEvent->msg);
    }
    else
    {
        printf ("Error: pEvent is Null");
    }
}

void n2a_print_fifo (fifo *pFifo)
{
    printf ("Fifo Size: %d/%d\n", pFifo->size, pFifo->max_size);

    if (pFifo->first)
    {
        int i = 0;
        event *pEvent = pFifo->first;

        for (; pEvent != NULL; pEvent = pEvent->pNext, ++i)
        {
            printf (" + ");
            n2a_print_event(pEvent);
        }
    }
}

#endif
