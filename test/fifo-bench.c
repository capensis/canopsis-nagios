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
#include <time.h>

#include "nagios.h"
#include "module.h"

#include "fifo.h"

#define FIFO_PATH "/tmp/bench.fifo"

struct options g_options;

int write_to_all_logs(char * msg,unsigned long priority){
  printf("log: %s\n", msg);
  return 0;
}

struct timeval now(){
  struct timeval tv;
  gettimeofday (&tv, NULL);
  return tv;
}

long elapsed(struct timeval start){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec-start.tv_sec) * 1000000L + (tv.tv_usec-start.tv_usec);
}

////////// Main
int main(int argc, char *argv[]){

  event * pEvent;

  unlink(FIFO_PATH);

  fifo * pFifo = fifo_init(10, FIFO_PATH);

  push(pFifo, event_init("rk0","First Event"));
  push(pFifo, event_init("rk1","Bonjour1"));
  push(pFifo, event_init("rk2","Bonjour2"));
  push(pFifo, event_init("rk3","Bonjour3"));
  push(pFifo, event_init("rk4","Bonjour5"));
  push(pFifo, event_init("rk5","Bonjour6"));
  push(pFifo, event_init("rk6","Bonjour7"));
  push(pFifo, event_init("rk7","Bonjour8"));
  push(pFifo, event_init("rk8","Bonjour9"));
  push(pFifo, event_init("rk9","Last Event"));
  push(pFifo, event_init("rk10","MAX-SIZE")); //Fifo full
  push(pFifo, event_init("rk11","MAX-SIZE")); //Fifo full

  printf("ORIGINAL FIFO:\n");
  print_fifo(pFifo);

  printf("\nSHIFT:\n");
  pEvent = shift(pFifo);
  print_event(pEvent);
  free_event(pEvent);

  printf("\nPOP:\n");
  pEvent = pop(pFifo);
  print_event(pEvent);
  free_event(pEvent);


  printf("\nSECOND PUSH:\n");
  push(pFifo, event_init("rk12","push1"));
  push(pFifo, event_init("rk13","push2"));
  push(pFifo, event_init("rk14","push3")); //Fifo full

  printf("\nFINAL FIFO:\n");
  print_fifo(pFifo);

  csync(pFifo);

  free_fifo(pFifo);

  printf("\nRELOAD FIFO FROM HDD:\n");
  pFifo = fifo_init(10, FIFO_PATH);

  printf("\nFIFO AFTER LOAD:\n");
  print_fifo(pFifo);

  //printf("\nCLEAR HDD FIFO:\n");
  //clear(pFifo);

  free_fifo(pFifo);

  //////// BENCH
  struct timeval start;
  int i;
  int benchnb = 1000000;


  char * rk = "selector.engine.selector.component.SL_SUP_PPD";
  char * msg = "{'resource': None, 'display_name': 'Supervision Pr\xe9-prod', 'event_type': 'selector', 'long_output': '', 'timestamp': 1355325584, 'selector_id': '50784fa00972347a51f7a847', 'component': 'SL_SUP_PPD', 'connector': 'selector', 'state_type': 1, 'source_type': 'component', 'state': 2, 'connector_name': 'engine', 'output': '658 Ok, 24 Warning, 369 Critical', 'perf_data_array': [{'max': 1093, 'metric': 'cps_sel_state_0', 'value': 658}, {'max': 1093, 'metric': 'cps_sel_state_1', 'value': 24}, {'max': 1093, 'metric': 'cps_sel_state_2', 'value': 369}, {'max': 1093, 'metric': 'cps_sel_state_3', 'value': 42}, {'metric': 'cps_sel_total', 'value': 1093}, {'max': 1093, 'metric': 'cps_sel_component', 'value': 110}, {'max': 1093, 'metric': 'cps_sel_resource', 'value': 983}]}";

  //char * rk = "rk";
  //char * msg = "push";

  // Remove fifo
  unlink(FIFO_PATH);

  printf("\n");

  printf("init_fifo\n"); start = now();
  pFifo = fifo_init(benchnb, FIFO_PATH);
  printf(" + Elapsed: %ld us\n", elapsed(start));

  printf("Fifo Size: %d\n", pFifo->size);

  printf("push %d events\n", benchnb); start = now();
  for (i=0; i<benchnb; i++) {
    push(pFifo, event_init(rk, msg));
  }
  printf(" + Elapsed: %d ms (%f us/evt)\n", (int)(elapsed(start)/1000), (float)elapsed(start)/benchnb);

  printf("Fifo Size: %d\n", pFifo->size);

  printf("Sync\n"); start = now();
  csync(pFifo);
  printf(" + Elapsed: %d ms (%f us/evt)\n", (int)(elapsed(start)/1000), (float)elapsed(start)/benchnb);

  printf("free_fifo\n"); start = now();
  free_fifo(pFifo);
  printf(" + Elapsed: %d ms\n", (int)(elapsed(start)/1000));

  printf("init_fifo (with load)\n"); start = now();
  pFifo = fifo_init(benchnb, FIFO_PATH);
  printf(" + Elapsed: %d ms (%f us/evt)\n", (int)(elapsed(start)/1000), (float)elapsed(start)/benchnb);

  printf("Fifo Size: %d\n", pFifo->size);

  printf("Shift All\n"); start = now();
  while(pFifo->size)
    free_event(shift(pFifo));
  printf(" + Elapsed: %d ms (%f us/evt)\n", (int)(elapsed(start)/1000), (float)elapsed(start)/benchnb);

  printf("Fifo Size: %d\n", pFifo->size);

  printf("Sync\n"); start = now();
  csync(pFifo);
  printf(" + Elapsed: %ld us (%f us/evt)\n", elapsed(start), (float)elapsed(start)/benchnb);

  printf("free_fifo\n"); start = now();
  free_fifo(pFifo);
  printf(" + Elapsed: %ld us\n", elapsed(start));

  exit (0);
}
