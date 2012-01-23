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

extern char *g_eventsource_name;

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

  sprintf (buffer, "{\
\"connector\":		 \"nagios\",\
\"connector_name\":	 \"%s\",\
\"event_type\":		 \"check\",\
\"source_type\":	 \"resource\",\
\"component\":		 \"%s\",\
\"resource\":		 \"%s\",\
\"timestamp\":		 %d,\
\"state\":		 %d,\
\"state_type\":		 %d,\
\"output\":		 \"%s\",\
\"long_output\":	 \"%s\",\
\"perf_data\":		 \"%s\",\
\"check_type\":		 %d,\
\"current_attempt\":	%d,\
\"max_attempts\":	%d, \
\"execution_time\":	%.3lf, \
\"latency\":		%.3lf, \
\"command_name\":	\"%s\" \
}\n", g_eventsource_name, c->host_name, c->service_description, (int) c->timestamp.tv_sec, c->state, c->state_type, charnull (c->output), charnull (c->long_output), charnull (c->perf_data), c->check_type, c->current_attempt, c->max_attempts, c->execution_time, c->latency, charnull (c->command_name));
}

void
nebstruct_host_check_data_to_json (char *buffer,
				   nebstruct_host_check_data * c)
{

  sprintf (buffer, "{\
\"connector\":		 \"nagios\",\
\"connector_name\":	 \"%s\",\
\"event_type\":		 \"check\",\
\"source_type\":	 \"component\",\
\"component\":		 \"%s\",\
\"resource\":		 \"\",\
\"timestamp\":		 %d,\
\"state\":		 %d,\
\"state_type\":		 %d,\
\"output\":		 \"%s\",\
\"long_output\":	 \"%s\",\
\"perf_data\":		 \"%s\",\
\"check_type\":		 %d,\
\"current_attempt\":	%d,\
\"max_attempts\":	%d,\
\"execution_time\":	%.3lf,\
\"latency\":		%.3lf,\
\"command_name\":	\"%s\"\
}\n", g_eventsource_name, c->host_name, (int) c->timestamp.tv_sec, c->state, c->state_type, charnull (c->output), charnull (c->long_output), charnull (c->perf_data), c->check_type, c->current_attempt, c->max_attempts, c->execution_time, c->latency, charnull (c->command_name));
}
