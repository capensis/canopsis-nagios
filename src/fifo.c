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

#include "fifo.h"

////////// Functions
fifo * fifo_init(int max_size, char * file_path){
  if (DEBUG) printf("Init fifo\n");
  if (DEBUG) printf(" + max_size:  %d\n", max_size);
  if (DEBUG) printf(" + file_path: %s\n", file_path);

  fifo * pFifo = (fifo *)malloc(sizeof(fifo));

  pFifo->first = NULL;
  pFifo->last = NULL;
  pFifo->pFile = NULL;
  pFifo->size = 0;
  pFifo->file_lock = 0;

  pFifo->file_path = strdup (file_path);

  pFifo->max_size = max_size;

  int loaded = 0;
  
  loaded = load(pFifo);
  if (loaded != 0)
    if (DEBUG)  printf("%d events loaded from fifo.", loaded);

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

void fifo_open_file(fifo * pFifo){
  if (! pFifo->pFile){
    if (DEBUG)  printf(" + Open fifo file '%s'\n", pFifo->file_path);
    pFifo->pFile = fopen(pFifo->file_path, "r+");
    if (! pFifo->pFile)
      pFifo->pFile = fopen(pFifo->file_path, "w+");
  }
}

int load(fifo * pFifo){
  if (DEBUG)  printf("Load events from fifo file '%s'\n", pFifo->file_path);
  int i = 0;

  if (pFifo->file_lock){
    if (DEBUG) printf(" + Fifo is locked\n");
    return 0;
  }

  pFifo->file_lock = 1;

  fifo_open_file(pFifo);

  rewind(pFifo->pFile);

  if (DEBUG)  printf(" + Read fifo file\n");
  while(1){
    char rk[EVENT_MSG_MAX_SIZE];
    char msg[EVENT_MSG_MAX_SIZE];

    fgets(rk, EVENT_MSG_MAX_SIZE, pFifo->pFile);

    if (feof(pFifo->pFile))
      break;

    fgets(msg, EVENT_MSG_MAX_SIZE, pFifo->pFile);

    // Remove \n
    rk[strlen(rk)-1] = 0;
    msg[strlen(msg)-1] = 0;

    //printf("Load: %s -> %s\n", rk, msg);

    push(pFifo, event_init(rk, msg));
    
    i += 1;
  }

  pFifo->file_lock = 0;

  if (DEBUG) printf(" + %d events loaded\n", i);
  return i;
}

int csync(fifo * pFifo){
  if (DEBUG) printf("Sync fifo to hdd\n");

  if (! pFifo){
    if (DEBUG) printf(" + Invalid fifo\n");
    return 0;    
  }
  
  if (pFifo->file_lock){
    if (DEBUG) printf(" + Fifo is locked\n");
    return 0;
  }

  pFifo->file_lock = 1;
  
  int i = 0;

  //Todo: tweak this
  clear(pFifo);

  fifo_open_file(pFifo);

  if (pFifo->first){

    event * pEvent = pFifo->first;

    while(pEvent != NULL){
      fprintf(pFifo->pFile, "%s\n%s\n", pEvent->rk, pEvent->msg);
      i += 1;
      pEvent = pEvent->pNext;
    }
  }
  if (DEBUG) printf(" + %d events synced\n", i);

  pFifo->file_lock = 0;
  
  return i;
}

int clear(fifo * pFifo){
  if (DEBUG) printf("Clear fifo file\n");
  if (pFifo->pFile){
    if (DEBUG) printf(" + Close\n");
    fclose(pFifo->pFile);
  }

  pFifo->pFile = NULL;
  if (DEBUG) printf(" + Unlink\n");
  return unlink(pFifo->file_path);
}

int push(fifo * pFifo, event * pEvent ){

  if (DEBUG) printf("Push event to fifo\n");
  if (pFifo->size >= pFifo->max_size){
    if (DEBUG) printf(" + Fifo si full, shift fisrt event\n");
    event * pEvent = shift(pFifo);
    free_event(pEvent);
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

  if (DEBUG) printf(" + Done, fifo size: %d\n", pFifo->size);

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
  if (DEBUG) printf("Freeing fifo\n");
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
    if (DEBUG) printf(" + Flush fifo file\n");
    fflush(pFifo->pFile);
    if (DEBUG) printf(" + Close fifo file\n");
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