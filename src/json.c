#include "nagios.h"
#include "module.h"
#include "logger.h"

extern char *g_eventsource_name;

char *
charnull(char *data)
{
    if (data == NULL)
    {
    	data = "";
    }
    return data;
}

void
nebstruct_service_check_data_to_json(char * buffer, nebstruct_service_check_data *c)
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
}\n",
    	g_eventsource_name,
    	c->host_name,
    	c->service_description,
    	(int)c->timestamp.tv_sec,
    	c->state,
    	c->state_type,
    	charnull(c->output),
    	charnull(c->long_output),
    	charnull(c->perf_data),

    	c->check_type,
    	c->current_attempt,
    	c->max_attempts,
    	c->execution_time,
    	c->latency,
    	charnull(c->command_name));
}

void
nebstruct_host_check_data_to_json(char * buffer, nebstruct_host_check_data *c)
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
}\n",
    	g_eventsource_name,
    	c->host_name,
    	(int)c->timestamp.tv_sec,
    	c->state,
    	c->state_type,
    	charnull(c->output),
    	charnull(c->long_output),
    	charnull(c->perf_data),

    	c->check_type,
    	c->current_attempt,
    	c->max_attempts,
    	c->execution_time,
    	c->latency,
    	charnull(c->command_name));
}

