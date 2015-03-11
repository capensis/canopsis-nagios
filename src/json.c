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

#include "nagios.h"
#include "module.h"
#include "logger.h"
#include "xutils.h"

#include "jansson.h"
#include "xutils.h"

extern struct options g_options;

#define charnull(data)      (data == NULL ? "" : data)

struct event_options_t
{
    char *name;
    void *option;
    json_t* (*callback) ();
};

void n2a_nebstruct_service_check_data_update_json (
        json_t **pdata,
        const char *message,
        const char *field,
        int size,
        int cpt)
{
    json_t *item;
    json_t *jdata = *pdata;

    char *temp = NULL;
    int offset = cpt * (size - 1); /* avoid the final \0 from previous snprintf */

    xalloca (temp, size);
    snprintf (temp, size, "%s", message + offset);

    json_object_del (jdata, field);
    item = json_string (temp);
    json_object_set (jdata, field, item);
    json_decref (item);
}

void n2a_nebstruct_add_hostgroups (json_t *jdata, host *host_object)
{
    /* add hostgroups to event */
    if (g_options.hostgroups)
    {
        objectlist *hostgroups = NULL;

        json_t *item = json_array ();
        json_object_set (jdata, "hostgroups", item);

        for (hostgroups = host_object->hostgroups_ptr;
             hostgroups != NULL;
             hostgroups = hostgroups->next
            )
        {
            hostgroup *group = hostgroups->object_ptr;

            json_t *groupname = json_string (group->group_name);
            json_array_append (item, groupname);
            json_decref (groupname);
        }

        json_decref (item);
    }
}

void n2a_nebstruct_add_servicegroups (json_t *jdata, service *service_object)
{
    /* add servicegroups to event */
    if (g_options.servicegroups)
    {
        objectlist *servicegroups = NULL;

        json_t *item = json_array ();
        json_object_set (jdata, "servicegroups", item);

        for (servicegroups = service_object->servicegroups_ptr;
             servicegroups != NULL;
             servicegroups = servicegroups->next
            )
        {
            servicegroup *group = servicegroups->object_ptr;

            json_t *groupname = json_string (group->group_name);
            json_array_append (item, groupname);
            json_decref (groupname);
        }

        json_decref (item);
    }
}

void n2a_nebstruct_add_custom_variables (json_t *jdata, customvariablesmember *cvars)
{
    /* now, add custom variables to event */
    if (g_options.custom_variables)
    {
        customvariablesmember *cvar = NULL;

        for (cvar = cvars; cvar != NULL; cvar = cvar->next)
        {
            json_t *item = json_string (cvar->variable_value);
            json_object_set (jdata, cvar->variable_name, item);
            json_decref (item);
        }
    }
}

json_t *n2a_bool_to_json (int boolean)
{
    return (boolean ? json_true () : json_false ());
}

json_t *n2a_string_to_truncated_json (char *string)
{
    if (xstrlen (string) > g_options.max_size)
    {
        return json_string ("");
    }
    else
    {
        return json_string (string);
    }
}

void n2a_add_options_to_event (struct event_options_t *options, json_t *jdata, int (*cmp) (void *option, void *userdata), void *userdata)
{
    int i;

    for (i = 0; options[i].name != NULL; ++i)
    {
        json_t *item = NULL;

        if (cmp == NULL || cmp (options[i].option, userdata))
        {
            if (options[i].callback == json_string)
            {
                item = json_string (*((char **) options[i].option));
            }
            else if (options[i].callback == json_integer)
            {
                item = json_integer (*((int *) options[i].option));
            }
            else if (options[i].callback == json_real)
            {
                item = json_real (*((double *) options[i].option));
            }

            json_object_set (jdata, options[i].name, item);
            json_decref (item);
        }
    }
}

int n2a_str_maxsize (void *option, void *userdata)
{
    char *string = *((char **) option);
    size_t length = *((size_t *) userdata);

    return (length <= xstrlen (string));
}

void n2a_nebstruct_add_urls (json_t *jdata, char *action_url, char *notes_url)
{
    if (g_options.urls)
    {
        struct event_options_t options[] =
        {
            {"action_url", &(action_url), json_string},
            {"notes_url",  &(notes_url),  json_string},
            {NULL, NULL, NULL}
        };

        n2a_add_options_to_event (options, jdata, NULL, NULL);
    }
}

int n2a_nebstruct_service_check_data_to_json (
        nebstruct_service_check_data *c,
        json_t **pdata,
        size_t *message_size)
{
    /* JSON data */
    json_t *jdata = json_object ();

    int nbmsg = 1;
    char *json = NULL;
    int left = 0;
    size_t rest = -1;

    /* Event data */
    service *service_object = c->object_ptr;
    host *host_object = service_object->host_ptr;

    char *event_type = "check";
    char *source_type = "resource";

    struct event_options_t options[] = {
        {"connector",       &(g_options.connector),        json_string},
        {"connector_name",  &(g_options.eventsource_name), json_string},
        {"event_type",      &(event_type),                 json_string},
        {"source_type",     &(source_type),                json_string},
        {"component",       &(c->host_name),               json_string},
        {"resource",        &(c->service_description),     json_string},
        {"address",         &(host_object->address),       json_string},
        {"timestamp",       &(c->timestamp.tv_sec),        json_integer},
        {"state",           &(c->state),                   json_integer},
        {"state_type",      &(c->state_type),              json_integer},
        {"check_type",      &(c->check_type),              json_integer},
        {"current_attempt", &(c->current_attempt),         json_integer},
        {"max_attempts",    &(c->max_attempts),            json_integer},
        {"execution_time",  &(c->execution_time),          json_real},
        {"latency",         &(c->latency),                 json_real},
        {"command_name",    &(c->command_name),            json_string},
        {NULL, NULL, NULL}
    };

    *pdata = jdata;

    n2a_add_options_to_event (options, jdata, NULL, NULL);
    n2a_nebstruct_add_hostgroups (jdata, host_object);
    n2a_nebstruct_add_servicegroups (jdata, service_object);
    n2a_nebstruct_add_custom_variables (jdata, service_object->custom_variables);
    n2a_nebstruct_add_urls (jdata, service_object->action_url, service_object->notes_url);

    /* now stringify JSON */

    json = json_dumps (jdata, 0);
    *message_size = xstrlen (json);
    xfree (json);

    left = g_options.max_size - (int) * message_size;

    rest = xstrlen (c->long_output) + xstrlen (c->output) + xstrlen (c->perf_data);

    if ((int) rest > left)
    {
        /* we work with int so we add 1 to the division */
        nbmsg = ((int) rest / left) + 1;
    }
    else
    {
        struct event_options_t additionals[] = {
            {"output",      &(c->output),      json_string},
            {"long_output", &(c->long_output), json_string},
            {"perf_data",   &(c->perf_data),   json_string},
            {NULL, NULL, NULL}
        };

        n2a_add_options_to_event (additionals, jdata, NULL, NULL);

        json = json_dumps (jdata, 0);
        *message_size = xstrlen (json);
        xfree (json);
    }

    /* we don't remove the reference on jdata because it's a weak reference. */

    return nbmsg;
}

int n2a_nebstruct_host_check_data_to_json (char **buffer, nebstruct_host_check_data *c)
{
    /* JSON data */
    json_t *jdata = json_object ();

    int nbmsg = 0;
    char *json = NULL;
    size_t ref = -1;

    /* Event data */
    host *host_object = c->object_ptr;

    char *event_type = "check";
    char *source_type = "component";

    struct event_options_t options[] = {
        {"connector",       &(g_options.connector),        json_string},
        {"connector_name",  &(g_options.eventsource_name), json_string},
        {"event_type",      &(event_type),                 json_string},
        {"source_type",     &(source_type),                json_string},
        {"component",       &(c->host_name),               json_string},
        {"address",         &(host_object->address),       json_string},
        {"timestamp",       &(c->timestamp.tv_sec),        json_integer},
        {"state",           &(c->state),                   json_integer},
        {"state_type",      &(c->state_type),              json_integer},
        {"check_type",      &(c->check_type),              json_integer},
        {"current_attempt", &(c->current_attempt),         json_integer},
        {"max_attempts",    &(c->max_attempts),            json_integer},
        {"execution_time",  &(c->execution_time),          json_real},
        {"latency",         &(c->latency),                 json_real},
        {"command_name",    &(c->command_name),            json_string},
        {NULL, NULL, NULL}
    };

    n2a_add_options_to_event (options, jdata, NULL, NULL);
    n2a_nebstruct_add_hostgroups (jdata, host_object);
    n2a_nebstruct_add_custom_variables (jdata, host_object->custom_variables);
    n2a_nebstruct_add_urls (jdata, host_object->action_url, host_object->notes_url);

    /* now stringify JSON */

    json = json_dumps (jdata, 0);
    ref = xstrlen (json);

    if ((int) ref > g_options.max_size)
    {
        size_t save = ref - g_options.max_size;
        struct event_options_t additionals[] = {
            {"output",      &(c->output),      json_string},
            {"long_output", &(c->long_output), json_string},
            {"perf_data",   &(c->perf_data),   json_string},
            {NULL, NULL, NULL}
        };

        n2a_add_options_to_event (additionals, jdata, n2a_str_maxsize, &save);

        xfree (json);
        json = json_dumps (jdata, 0);
    }

    ref = xstrlen (json);
    *buffer = xmalloc (ref + 1);

    snprintf (*buffer, ref + 1, "%s", json);
    xfree (json);
    json_decref (jdata);

    return nbmsg;
}

int n2a_nebstruct_acknowlegement_data_to_json (char **buffer, nebstruct_acknowledgement_data *c)
{
    /* JSON data */
    json_t *jdata = json_object ();

    char *s = NULL;
    size_t l = 0;

    /* Event data */
    char *event_type = "ack";

    char *source_type = NULL;
    char *ref_rk = NULL;

    if (c->acknowledgement_type == HOST_ACKNOWLEDGEMENT)
    {
        /* referer RK is the same as this one, but with event type check */
        ref_rk = n2a_str_join (".",
            g_options.connector,
            g_options.eventsource_name,
            "check",
            "component",
            c->host_name,
            NULL
        );

        source_type = "component";
    }
    else if (c->acknowledgement_type == SERVICE_ACKNOWLEDGEMENT)
    {
        /* referer RK is the same as this one, but with event type check */
        ref_rk = n2a_str_join (".",
            g_options.connector,
            g_options.eventsource_name,
            "check",
            "resource",
            c->host_name,
            c->service_description,
            NULL
        );

        source_type = "resource";
    }

    int cstate_type = 1;

    struct event_options_t options[] = {
        {"connector",      &(g_options.connector),        json_string},
        {"connector_name", &(g_options.eventsource_name), json_string},
        {"event_type",     &(event_type),                 json_string},
        {"source_type",    &(source_type),                json_string},
        {"component",      &(c->host_name),               json_string},
        {"resource",       &(c->service_description),     json_string},
        {"referer",        &(ref_rk),                     json_string},
        {"ref_rk",         &(ref_rk),                     json_string},
        {"author",         &(c->author_name),             json_string},
        {"state",          &(c->state),                   json_integer},
        {"state_type",     &(cstate_type),                json_integer},
        {"timestamp",      &(c->timestamp.tv_sec),        json_integer},
        {"output",         &(c->comment_data),            n2a_string_to_truncated_json},
        {NULL, NULL, NULL}
    };

    n2a_add_options_to_event (options, jdata, NULL, NULL);

    /* generate string */
    s = json_dumps (jdata, 0);
    l = xstrlen (s);

    /* write to buffer */
    *buffer = xmalloc (l + 1);
    snprintf (*buffer, l + 1, "%s", s);
    xfree (s);

    json_decref (jdata);
    return 1;
}

int n2a_nebstruct_downtime_data_to_json (char **buffer, nebstruct_downtime_data *c)
{
    /* JSON data */
    json_t *jdata = json_object ();

    char *s = NULL;
    size_t l = 0;

    /* Event data */
    char *event_type = "downtime";
    char *source_type = NULL;

    if (c->downtime_type == HOST_DOWNTIME)
    {
        source_type = "component";
    }
    else if (c->downtime_type == SERVICE_DOWNTIME)
    {
        source_type = "resource";
    }

    struct event_options_t options[] = {
        {"connector",      &(g_options.connector),        json_string},
        {"connector_name", &(g_options.eventsource_name), json_string},
        {"event_type",     &(event_type),                 json_string},
        {"source_type",    &(source_type),                json_string},
        {"component",      &(c->host_name),               json_string},
        {"resource",       &(c->service_description),     json_string},
        {"author",         &(c->author_name),             json_string},
        {"timestamp",      &(c->timestamp.tv_sec),        json_integer},
        {"output",         &(c->comment_data),            n2a_string_to_truncated_json},
        {"start",          &(c->start_time),              json_integer},
        {"end",            &(c->end_time),                json_integer},
        {"duration",       &(c->duration),                json_integer},
        {"entry",          &(c->entry_time),              json_integer},
        {"fixed",          &(c->fixed),                   n2a_bool_to_json},
        {"downtime_id",    &(c->downtime_id),             json_integer},
        {NULL, NULL, NULL}
    };

    n2a_add_options_to_event (options, jdata, NULL, NULL);

    /* generate string */
    s = json_dumps (jdata, 0);
    l = xstrlen (s);

    /* write to buffer */
    *buffer = xmalloc (l + 1);
    snprintf (*buffer, l + 1, "%s", s);
    xfree (s);

    json_decref (jdata);

    return 1;
}
