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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>

#include "logger.h"
#include "nagios.h"

#include "fifo.h"

////////// Functions
fifo * fifo_init(int max_size, char * file_path){
  n2a_logger (LG_DEBUG, "FIFO: Init");
  n2a_logger (LG_DEBUG, "FIFO:  + max_size:  %d", max_size);
  n2a_logger (LG_DEBUG, "FIFO:  + file_path: %s", file_path);

  fifo * pFifo = (fifo *)malloc(sizeof(fifo));

  pFifo->first = NULL;
  pFifo->last = NULL;
  pFifo->pFile = NULL;
  pFifo->size = 0;
  pFifo->file_lock = false;
  pFifo->dirty = false;

  pFifo->file_path = strdup (file_path);

  pFifo->max_size = max_size;
  
  int loaded = load(pFifo);

  n2a_logger (LG_INFO, "FIFO: %d events loaded", loaded);

  return pFifo;
}

event * event_init(const char * rk, const char * msg){
  return event_init_nomalloc(strdup (rk), strdup (msg));
}

event * event_init_nomalloc(const char * rk, const char * msg){
  event * pEvent = (event *)malloc(sizeof(event));

  // Todo: check size
  //int size;
  //size = sizeof(char) * (strlen(msg)+1);

  pEvent->pNext = NULL;
  pEvent->pPrev = NULL;

  pEvent->msg = (char *)msg;
  pEvent->rk = (char *)rk;

  return pEvent;
}

int fifo_open_file(fifo * pFifo){
  if (! pFifo->pFile){
    n2a_logger (LG_DEBUG, "FIFO: Open fifo file '%s'", pFifo->file_path);

    pFifo->pFile = fopen(pFifo->file_path, "r+");
    if (! pFifo->pFile)
      pFifo->pFile = fopen(pFifo->file_path, "w+");

    if (! pFifo->pFile)
      n2a_logger (LG_ERR, "FIFO: Impossible to open '%s'", pFifo->file_path);
    else
      n2a_logger (LG_INFO, "FIFO: File successfully opened");

  }else{
    n2a_logger (LG_DEBUG, "FIFO: '%s' already open", pFifo->file_path);
  }

  if (! pFifo->pFile)
    return 0;

  return 1;
}

int load(fifo * pFifo){
  n2a_logger (LG_DEBUG, "FIFO: Load events from file");

  int i = 0;

  if (pFifo->file_lock){
    n2a_logger (LG_WARN, "FIFO: Fifo is locked");
    return 0;
  }

  if (! fifo_open_file(pFifo))
    return 0;

  pFifo->file_lock = true;

  rewind(pFifo->pFile);

  while(1){
    char rk[EVENT_MSG_MAX_SIZE];
    char msg[EVENT_MSG_MAX_SIZE];

    // Read RK
    fgets(rk, EVENT_MSG_MAX_SIZE, pFifo->pFile);

    if (feof(pFifo->pFile))
      break;

    // Read event
    fgets(msg, EVENT_MSG_MAX_SIZE, pFifo->pFile);

    // Remove \n
    rk[strlen(rk)-1] = 0;
    msg[strlen(msg)-1] = 0;

    //printf("Load: %s -> %s\n", rk, msg);

    push(pFifo, event_init(rk, msg));
    
    i += 1;
  }
  
  pFifo->file_lock = false;

  return i;
}

int clear(fifo * pFifo){
  n2a_logger (LG_DEBUG, "FIFO: Remove fifo file");

  if (pFifo->pFile)
    fclose(pFifo->pFile);

  pFifo->pFile = NULL;
  return unlink(pFifo->file_path);
}

int csync(fifo * pFifo){

  if (! pFifo){
    n2a_logger (LG_ERR, "FIFO: Invalid fifo");
    return 0;    
  }

  if (! pFifo->dirty){
    n2a_logger (LG_DEBUG, "FIFO: Up to date");
    return 0;
  }

  n2a_logger (LG_INFO, "FIFO: Sync %d events to file", pFifo->size);
  
  if (pFifo->file_lock){
    n2a_logger (LG_WARN, "FIFO: Fifo file is locked");
    return 0;
  }

  pFifo->file_lock = true;

  clear(pFifo);

  if (! fifo_open_file(pFifo)){
    pFifo->file_lock = false;
    return 0;
  }

  int i = 0;

  if (pFifo->first){

    event * pEvent = pFifo->first;

    while(pEvent != NULL){
      fprintf(pFifo->pFile, "%s\n%s\n", pEvent->rk, pEvent->msg);
      i += 1;
      pEvent = pEvent->pNext;
    }
  }

  fflush(pFifo->pFile);
  
  n2a_logger (LG_INFO, "FIFO: %d events written to file", i);

  pFifo->file_lock = false;
  pFifo->dirty = false;
  
  return i;
}

int push(fifo * pFifo, event * pEvent ){

  n2a_logger (LG_DEBUG, "FIFO: Push (%d)", pFifo->size);

  if (pFifo->size >= pFifo->max_size){
    n2a_logger (LG_WARN, "FIFO: Queue is full, remove old event");
    free_event(shift(pFifo));
  }

  if (pFifo->first == NULL){
    pFifo->first = pEvent;
    pFifo->last = pEvent;
  }else{
    pEvent->pPrev = pFifo->last;
    pFifo->last->pNext = pEvent;
    pFifo->last = pEvent;
  }

  pFifo->size += 1;
  pFifo->dirty = true;

  return pFifo->size;
}

int prepand(fifo * pFifo, event * pEvent){

  n2a_logger (LG_DEBUG, "FIFO: Prepand (%d)", pFifo->size);

  if (pFifo->size >= pFifo->max_size){
    n2a_logger (LG_WARN, "FIFO: Queue is full, remove newest event");
    free_event(pop(pFifo));
  }

  if (pFifo->first == NULL){
    pFifo->first = pEvent;
    pFifo->last = pEvent;
  }else{

    pEvent->pPrev = NULL;
    pEvent->pNext = pFifo->first;
    
    pFifo->first->pPrev = pEvent;
    pFifo->first = pEvent;
  }

  pFifo->size += 1;
  pFifo->dirty = true;

  return pFifo->size;
}

event * shift(fifo * pFifo){
  if (pFifo->first){
    event * pEvent = pFifo->first;

    if (pEvent->pNext){
      pFifo->first = pEvent->pNext;
      pFifo->first->pPrev = NULL;
    }else{
      pFifo->first = NULL;
    }

    pEvent->pNext = NULL;
    pEvent->pPrev = NULL;

    pFifo->size -= 1;
    pFifo->dirty = true;

    return pEvent;
  }else{
    return NULL;
  }
}

event * pop(fifo * pFifo){
  if (pFifo->last){
    event * pEvent = pFifo->last;

    if (pEvent->pPrev){
      pFifo->last = pEvent->pPrev;
      pFifo->last->pNext = NULL;
    }else{
      pFifo->last = NULL;
    }

    pEvent->pNext = NULL;
    pEvent->pPrev = NULL;

    pFifo->size -= 1;
    pFifo->dirty = true;
    
    return pEvent;
  }else{
    return NULL;
  }
}

void free_event(event * pEvent){
  if (pEvent){
    if (pEvent->msg != NULL)
      free(pEvent->msg);
    if (pEvent->rk != NULL)
      free(pEvent->rk);
    free(pEvent);
  }
}

void free_fifo(fifo * pFifo){

  csync(pFifo);

  n2a_logger (LG_DEBUG, "FIFO: Freeing fifo");

  if (pFifo->first){
    event * pEvent = pFifo->first;
    event * pNext = NULL;

    do{
      pNext = pEvent->pNext;
      free_event(pEvent);
      pEvent = pNext;
    }while(pNext != NULL);
  }

  if (pFifo->pFile){
    n2a_logger (LG_DEBUG, "FIFO: Close file");
    fclose(pFifo->pFile);
  }

  free(pFifo->file_path);
  free(pFifo);
}

////////// For Debug
void print_event(event * pEvent){
  if (pEvent){
    printf("%s -> %s\n", pEvent->rk, pEvent->msg);
  }else{
    printf("Error: pEvent is Null");
  }
}

void print_fifo(fifo * pFifo){
  printf("Fifo Size: %d/%d\n", pFifo->size, pFifo->max_size);

  if (pFifo->first){
    int i = 0;
    event * pEvent = pFifo->first;

    while(pEvent != NULL){
      printf(" + ");
      print_event(pEvent);
      pEvent = pEvent->pNext;
      i+=1;
    }
  }
}