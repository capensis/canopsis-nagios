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

#include "nagios.h"
#include "module.h"
#include "logger.h"
#include "xutils.h"

#include "json.h"
#include "neb2amqp.h"
#include "xutils.h"

#include "events.h"

extern struct options g_options;
extern int c_size;

int g_last_event_program_status = 0;

// Define a macro that will handle the split of messages
#define split_message(message,field)                                               \
do {                                                                               \
    temp = ((int)xstrlen(message)/left + 1);                                       \
    i = 0;                                                                         \
    while (i < temp) {                                                             \
        nebstruct_service_check_data_update_json(&jdata, message, field, left, i); \
        char *json = json_dumps(jdata, 0);                                         \
        size_t len = xstrlen (json);                                               \
        buffer = xmalloc (len + 1);                                                \
        snprintf (buffer, len + 1, "%s", json);                                    \
        if (c_size == -10000 || c_size / 2 == 0)                                   \
            amqp_publish(key, buffer);                                             \
        else                                                                       \
            n2a_record_cache (key, buffer);                                        \
        xfree(buffer);                                                             \
        xfree (json);                                                              \
        i++;                                                                       \
    }                                                                              \
} while(0);


int
n2a_event_service_check (int event_type __attribute__ ((__unused__)), void *data)
{
  //logger(LG_DEBUG, "Event: event_host_check");
  nebstruct_service_check_data *c = (nebstruct_service_check_data *) data;

  if (c->type == NEBTYPE_SERVICECHECK_PROCESSED)
    {
      //logger(LG_DEBUG, "SERVICECHECK_PROCESSED: %s->%s", c->host_name, c->service_description);
      char *buffer = NULL, *key = NULL;

      size_t l = xstrlen(g_options.connector) +
      xstrlen(g_options.eventsource_name) + xstrlen(c->host_name) + xstrlen(c->service_description) + 20;
      // "..check.ressource.." + \0 = 20 chars

      json_t *jdata = NULL;
      size_t message_size = 0;

      int nbmsg = nebstruct_service_check_data_to_json(c, &jdata, &message_size); 

      // DO NOT FREE !!!
      xalloca(key, xmin(g_options.max_size, (int)l) * sizeof(char));

      snprintf(key, xmin(g_options.max_size, (int)l),
                 "%s.%s.check.resource.%s.%s", g_options.connector,
                 g_options.eventsource_name, c->host_name,
                 c->service_description);

      if (nbmsg == 1) {
          char *json = json_dumps(jdata, 0);
          buffer = xmalloc (message_size + 1);

          snprintf (buffer, message_size + 1, "%s", json);

          if (c_size == -10000 || c_size / 2 == 0) 
              amqp_publish(key, buffer);
          else
              n2a_record_cache (key, buffer);

          xfree(buffer);
          xfree (json);
      } else {
          int left = g_options.max_size - (int)message_size;
          size_t l_out = xstrlen(c->long_output);
          size_t out = xstrlen(c->output);
          size_t perf = xstrlen(c->perf_data);
          int msgs = ((int)l_out/left + 1) + ((int)out/left + 1) + ((int)perf/left + 1);
          n2a_logger(LG_INFO, "Data too long... sending %d messages for host: %s, service: %s", msgs, c->host_name, c->service_description);
          int i, temp;
          split_message(c->long_output, "long_output");
          split_message(c->output, "output");
          split_message(c->perf_data, "perf_data");
      }

      if (jdata != NULL)
          json_decref (jdata);

    }

  return 0;
}

int
n2a_event_host_check (int event_type __attribute__ ((__unused__)), void *data)
{
  //logger(LG_DEBUG, "Event: event_service_check");
  nebstruct_host_check_data *c = (nebstruct_host_check_data *) data;

  if (c->type == NEBTYPE_HOSTCHECK_PROCESSED)
    {
      //logger(LG_DEBUG, "HOSTCHECK_PROCESSED: %s", c->host_name);
      char *buffer = NULL, *key = NULL;

      size_t l = xstrlen(g_options.connector) + xstrlen(g_options.eventsource_name) + xstrlen(c->host_name) + 20; 

//      nebstruct_host_check_data_to_json(&buffer, c); 

      // DO NOT FREE !!!
      xalloca(key, xmin(g_options.max_size, (int)l) * sizeof(char));

      snprintf(key, xmin(g_options.max_size, (int)l),
                 "%s.%s.check.component.%s", g_options.connector,
                 g_options.eventsource_name, c->host_name);

      if (c_size == -10000 || c_size / 2 == 0)
          amqp_publish(key, buffer);
      else
          n2a_record_cache (key, buffer);

      xfree(buffer);
    }

  return 0;
}

/*

int
event_void (int event_type __attribute__ ((__unused__)), void *data)
{
  return;
}


int
event_process (int event_type __attribute__ ((__unused__)), void *data)
{
  struct nebstruct_process_struct *ps = (struct nebstruct_process_struct *) data;
  
  return 0;
}

int
event_program_status (int event_type __attribute__ ((__unused__)), void *data)
{
  nebstruct_program_status_data *ps = (nebstruct_program_status_data *) data;
  //logger(LG_DEBUG, "Event: event_program_status (type: %i)", ps->type);

  //Send program_status every 10sec min
  if ((int) ps->timestamp.tv_sec >= (g_last_event_program_status + 10))
    {
      char buffer[AMQP_MSG_SIZE_MAX];
      //TODO
      //nebstruct_program_status_data_to_json(buffer, ps);
      //amqp_publish (exchange_name, routingkey, buffer);
      g_last_event_program_status = (int) ps->timestamp.tv_sec;
    }
  return 0;
}


int
event_acknowledgement (int event_type
		       __attribute__ ((__unused__)), void *data)
{
  //logger(LG_DEBUG, "Event: event_acknowledgement");
  nebstruct_acknowledgement_data *c = (nebstruct_acknowledgement_data *) data;
  
  if (c->type == NEBTYPE_ACKNOWLEDGEMENT_ADD)
    {
      n2a_logger(LG_DEBUG, "Event: event_acknowledgement ADD");

      char buffer[AMQP_MSG_SIZE_MAX];
      //TODO
      //nebstruct_acknowledgement_data_to_json(buffer, c);
      //amqp_publish (exchange_name, routingkey, buffer);

    }
  else if (c->type == NEBTYPE_ACKNOWLEDGEMENT_REMOVE)
    {
	  // NOT IMPLEMENTED IN NAGIOS :(
      //logger(LG_DEBUG, "Event: event_acknowledgement REMOVE");
    }

  return 0;
}

int
event_downtime (int event_type __attribute__ ((__unused__)), void *data)
{
  //logger(LG_DEBUG, "Event: event_downtime");
  nebstruct_downtime_data *c = (nebstruct_downtime_data *) data;
  
  if (c->type == NEBTYPE_DOWNTIME_START)
    {
      n2a_logger(LG_DEBUG, "Event: event_downtime START");
    }
  else if (c->type == NEBTYPE_DOWNTIME_STOP)
    {
      n2a_logger(LG_DEBUG, "Event: event_downtime STOP");
    }

  if (c->type == NEBTYPE_DOWNTIME_START || c->type == NEBTYPE_DOWNTIME_STOP)
    {
      char buffer[AMQP_MSG_SIZE_MAX];
      //TODO
      //nebstruct_downtime_data_to_json(buffer, c);
      //amqp_publish (exchange_name, routingkey, buffer);
    }

  return 0;
}

int
event_comment (int event_type __attribute__ ((__unused__)), void *data)
{
  //logger(LG_DEBUG, "Event: event_comment");
  nebstruct_comment_data *c = (nebstruct_comment_data *) data;
  
  if (c->type == NEBTYPE_COMMENT_ADD)
    {
      n2a_logger(LG_DEBUG, "Event: event_comment ADD");
    }
  else if (c->type == NEBTYPE_COMMENT_DELETE)
    {
      n2a_logger(LG_DEBUG, "Event: event_comment DELETE");
    }

  if (c->type == NEBTYPE_COMMENT_ADD || c->type == NEBTYPE_COMMENT_DELETE)
    {
      char buffer[AMQP_MSG_SIZE_MAX];
      //TODO
      //nebstruct_comment_data_to_json(buffer, c);
      //amqp_publish (exchange_name, routingkey, buffer);
    }

  return 0;
}
*/
