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

#include "jansson.h"

extern struct options g_options;

char *
charnull (char *data)
{
  if (data == NULL)
    {
      data = "";
    }
  return data;
}

void
nebstruct_service_check_data_to_json (char *buffer,
				      nebstruct_service_check_data * c)
{

  service *service_object = c->object_ptr;
  host *host_object = service_object->host_ptr;

  json_t* jdata;
  jdata = json_object();
  
  json_object_set(jdata, "connector",		json_string("nagios"));
  json_object_set(jdata, "connector_name",	json_string(g_options.eventsource_name));
  json_object_set(jdata, "event_type",		json_string("check"));
  json_object_set(jdata, "source_type",		json_string("resource"));
  json_object_set(jdata, "component",		json_string(c->host_name));
  json_object_set(jdata, "address",			json_string(host_object->address));
  json_object_set(jdata, "resource",		json_string(c->service_description));
  json_object_set(jdata, "timestamp", 		json_integer((int) c->timestamp.tv_sec));
  json_object_set(jdata, "state",			json_integer(c->state));
  json_object_set(jdata, "state_type",		json_integer(c->state_type));
  json_object_set(jdata, "output",			json_string(c->output));
  json_object_set(jdata, "long_output",		json_string(c->long_output));
  json_object_set(jdata, "perf_data",		json_string(c->perf_data));
  json_object_set(jdata, "check_type",		json_integer(c->check_type));
  json_object_set(jdata, "current_attempt",	json_integer(c->current_attempt));
  json_object_set(jdata, "max_attempts",	json_integer(c->max_attempts));
  json_object_set(jdata, "execution_time",	json_real(c->execution_time));
  json_object_set(jdata, "latency",			json_real(c->latency));
  json_object_set(jdata, "command_name",	json_string(c->command_name));
  
  char* event_string = json_dumps( jdata, 0 );
  sprintf (buffer, "%s", event_string);
  
  free(event_string);
  json_decref(jdata); 

}

void
nebstruct_host_check_data_to_json (char *buffer,
				   nebstruct_host_check_data * c)
{

  host *host_object = c->object_ptr;

  int state = c->state;

  if (state >= 1)
    {
      // Set to Critical
      state = 2;
    }

  json_t* jdata;
  jdata = json_object();
  
  json_object_set(jdata, "connector",		json_string("nagios"));
  json_object_set(jdata, "connector_name",	json_string(g_options.eventsource_name));
  json_object_set(jdata, "event_type",		json_string("check"));
  json_object_set(jdata, "source_type",		json_string("component"));
  json_object_set(jdata, "component",		json_string(c->host_name));
  json_object_set(jdata, "address",			json_string(host_object->address));
  json_object_set(jdata, "resource",		json_string(""));
  json_object_set(jdata, "timestamp", 		json_integer((int) c->timestamp.tv_sec));
  json_object_set(jdata, "state",			json_integer(c->state));
  json_object_set(jdata, "state_type",		json_integer(c->state_type));
  json_object_set(jdata, "output",			json_string(c->output));
  json_object_set(jdata, "long_output",		json_string(c->long_output));
  json_object_set(jdata, "perf_data",		json_string(c->perf_data));
  json_object_set(jdata, "check_type",		json_integer(c->check_type));
  json_object_set(jdata, "current_attempt",	json_integer(c->current_attempt));
  json_object_set(jdata, "max_attempts",	json_integer(c->max_attempts));
  json_object_set(jdata, "execution_time",	json_real(c->execution_time));
  json_object_set(jdata, "latency",			json_real(c->latency));
  json_object_set(jdata, "command_name",	json_string(c->command_name));
  
  char* event_string = json_dumps( jdata, 0 );
  sprintf (buffer, "%s", event_string);
  
  free(event_string);
  json_decref(jdata); 

}
