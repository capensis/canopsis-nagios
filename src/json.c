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
 
  json_t* connector 		= json_string(g_options.connector);
  json_t* connector_name	= json_string(g_options.eventsource_name);
  json_t* event_type 		= json_string("check");
  json_t* source_type		= json_string("resource");
  json_t* component			= json_string(c->host_name);
  //json_t* address			= json_string(host_object->address);
  json_t* resource			= json_string(c->service_description);
  json_t* timestamp			= json_integer((int) c->timestamp.tv_sec);
  json_t* state				= json_integer(c->state);
  json_t* state_type		= json_integer(c->state_type);
  json_t* output			= json_string(c->output);
  json_t* long_output		= json_string(c->long_output);
  json_t* perf_data			= json_string(c->perf_data);
  json_t* check_type		= json_integer(c->check_type);
  json_t* current_attempt	= json_integer(c->current_attempt);
  json_t* max_attempts		= json_integer(c->max_attempts);
  json_t* execution_time	= json_real(c->execution_time);
  json_t* latency			= json_real(c->latency);
  json_t* command_name		= json_string(c->command_name);
  
  json_object_set(jdata, "connector",		connector);
  json_object_set(jdata, "connector_name",	connector_name);
  json_object_set(jdata, "event_type",		event_type);
  json_object_set(jdata, "source_type",		source_type);
  json_object_set(jdata, "component",		component);
  //json_object_set(jdata, "address",			address);
  json_object_set(jdata, "resource",		resource);
  json_object_set(jdata, "timestamp", 		timestamp);
  json_object_set(jdata, "state",			state);
  json_object_set(jdata, "state_type",		state_type);
  json_object_set(jdata, "output",			output);
  json_object_set(jdata, "long_output",		long_output);
  json_object_set(jdata, "perf_data",		perf_data);
  json_object_set(jdata, "check_type",		check_type);
  json_object_set(jdata, "current_attempt",	current_attempt);
  json_object_set(jdata, "max_attempts",	max_attempts);
  json_object_set(jdata, "execution_time",	execution_time);
  json_object_set(jdata, "latency",			latency);
  json_object_set(jdata, "command_name",	command_name);
  
  json_decref(connector);
  json_decref(connector_name);
  json_decref(event_type);
  json_decref(source_type);
  json_decref(component);
  //json_decref(address);
  json_decref(resource);
  json_decref(timestamp);
  json_decref(state);
  json_decref(state_type);
  json_decref(output);
  json_decref(long_output);
  json_decref(perf_data);
  json_decref(check_type);
  json_decref(current_attempt);
  json_decref(max_attempts);
  json_decref(execution_time);
  json_decref(latency);
  json_decref(command_name);
  
  char * json = json_dumps( jdata, 0 );
  sprintf (buffer, "%s", json);
  free(json);
  
  json_decref(jdata);

}

void
nebstruct_host_check_data_to_json (char *buffer,
				   nebstruct_host_check_data * c)
{

  host *host_object = c->object_ptr;

  int cstate = c->state;
  // Set to Critical
  if (cstate >= 1){
      cstate = 2;
  }
 
  json_t* jdata;
  
  jdata = json_object();
 
  json_t* connector 		= json_string(g_options.connector);
  json_t* connector_name	= json_string(g_options.eventsource_name);
  json_t* event_type 		= json_string("check");
  json_t* source_type		= json_string("component");
  json_t* component			= json_string(c->host_name);
  //json_t* address			= json_string(host_object->address);
  json_t* timestamp			= json_integer((int) c->timestamp.tv_sec);
  json_t* state				= json_integer(cstate);
  json_t* state_type		= json_integer(c->state_type);
  json_t* output			= json_string(c->output);
  json_t* long_output		= json_string(c->long_output);
  json_t* perf_data			= json_string(c->perf_data);
  json_t* check_type		= json_integer(c->check_type);
  json_t* current_attempt	= json_integer(c->current_attempt);
  json_t* max_attempts		= json_integer(c->max_attempts);
  json_t* execution_time	= json_real(c->execution_time);
  json_t* latency			= json_real(c->latency);
  json_t* command_name		= json_string(c->command_name);
  
  json_object_set(jdata, "connector",		connector);
  json_object_set(jdata, "connector_name",	connector_name);
  json_object_set(jdata, "event_type",		event_type);
  json_object_set(jdata, "source_type",		source_type);
  json_object_set(jdata, "component",		component);
  //json_object_set(jdata, "address",			address);
  json_object_set(jdata, "timestamp", 		timestamp);
  json_object_set(jdata, "state",			state);
  json_object_set(jdata, "state_type",		state_type);
  json_object_set(jdata, "output",			output);
  json_object_set(jdata, "long_output",		long_output);
  json_object_set(jdata, "perf_data",		perf_data);
  json_object_set(jdata, "check_type",		check_type);
  json_object_set(jdata, "current_attempt",	current_attempt);
  json_object_set(jdata, "max_attempts",	max_attempts);
  json_object_set(jdata, "execution_time",	execution_time);
  json_object_set(jdata, "latency",			latency);
  json_object_set(jdata, "command_name",	command_name);
  
  json_decref(connector);
  json_decref(connector_name);
  json_decref(event_type);
  json_decref(source_type);
  json_decref(component);
  //json_decref(address);
  json_decref(timestamp);
  json_decref(state);
  json_decref(state_type);
  json_decref(output);
  json_decref(long_output);
  json_decref(perf_data);
  json_decref(check_type);
  json_decref(current_attempt);
  json_decref(max_attempts);
  json_decref(execution_time);
  json_decref(latency);
  json_decref(command_name);
  
  char * json = json_dumps( jdata, 0 );
  sprintf (buffer, "%s", json);
  free(json);
  
  json_decref(jdata);

}
