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

int n2a_nebstruct_service_check_data_to_json (
        nebstruct_service_check_data *c,
        json_t **pdata,
        size_t *message_size)
{
    int nbmsg = 1;

    service *service_object = c->object_ptr;
    objectlist *servicegroups = NULL;

    host *host_object = service_object->host_ptr;
    objectlist *hostgroups = NULL;

    json_t *item = NULL;
    json_t *jdata = json_object ();

    char *json = NULL;
    int left = 0;
    size_t rest = 0;

    *pdata = jdata;

    item = json_string (g_options.connector);
    json_object_set (jdata, "connector", item);
    json_decref (item);

    item = json_string (g_options.eventsource_name);
    json_object_set (jdata, "connector_name", item);
    json_decref (item);

    item = json_string ("check");
    json_object_set (jdata, "event_type", item);
    json_decref (item);

    item = json_string ("resource");
    json_object_set (jdata, "source_type", item);
    json_decref (item);

    item = json_string (c->host_name);
    json_object_set (jdata, "component", item);
    json_decref (item);

    item = json_string (c->service_description);
    json_object_set (jdata, "resource", item);
    json_decref (item);

    item = json_integer ((int) c->timestamp.tv_sec);
    json_object_set (jdata, "timestamp", item);
    json_decref (item);

    item = json_integer (c->state);
    json_object_set (jdata, "state", item);
    json_decref (item);

    item = json_integer (c->state_type);
    json_object_set (jdata, "state_type", item);
    json_decref (item);

    item = json_string ("");
    json_object_set (jdata, "output", item);
    json_decref (item);

    item = json_string ("");
    json_object_set (jdata, "long_output", item);
    json_decref (item);

    item = json_string ("");
    json_object_set (jdata, "perf_data", item);
    json_decref (item);

    item = json_integer (c->check_type);
    json_object_set (jdata, "check_type", item);
    json_decref (item);

    item = json_integer (c->current_attempt);
    json_object_set (jdata, "current_attempt", item);
    json_decref (item);

    item = json_integer (c->max_attempts);
    json_object_set (jdata, "max_attempts", item);
    json_decref (item);

    item = json_real (c->execution_time);
    json_object_set (jdata, "execution_time", item);
    json_decref (item);

    item = json_real (c->latency);
    json_object_set (jdata, "latency", item);
    json_decref (item);

    item = json_string (c->command_name);
    json_object_set (jdata, "command_name", item);
    json_decref (item);

    /* add hostgroups to event */
    item = json_array ();
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

    /* add servicegroups to event */
    item = json_array ();
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
        item = json_string (c->long_output);
        json_object_set (jdata, "long_output", item);
        json_decref (item);

        item = json_string (c->output);
        json_object_set (jdata, "output", item);
        json_decref (item);

        item = json_string (c->perf_data);
        json_object_set (jdata, "perf_data", item);
        json_decref (item);

        json = json_dumps (jdata, 0);
        *message_size = xstrlen (json);
        xfree (json);
    }

    /* we don't remove the reference on jdata because it's a weak reference. */

    return nbmsg;
}

int n2a_nebstruct_host_check_data_to_json (char **buffer, nebstruct_host_check_data *c)
{
    host *host_object = c->object_ptr;
    objectlist *hostgroups = NULL;

    int nbmsg = 0;
    int cstate = (c->state >= 1 ? 2 : c->state);

    json_t *jdata = json_object ();
    json_t *item = NULL;

    char *json = NULL;
    size_t ref = -1;

    item = json_string (g_options.connector);
    json_object_set (jdata, "connector", item);
    json_decref (item);

    item = json_string (g_options.eventsource_name);
    json_object_set (jdata, "connector_name", item);
    json_decref (item);

    item = json_string ("check");
    json_object_set (jdata, "event_type", item);
    json_decref (item);

    item = json_string ("component");
    json_object_set (jdata, "source_type", item);
    json_decref (item);

    item = json_string (c->host_name);
    json_object_set (jdata, "component", item);
    json_decref (item);

    item = json_integer ((int) c->timestamp.tv_sec);
    json_object_set (jdata, "timestamp", item);
    json_decref (item);

    item = json_integer (cstate);
    json_object_set (jdata, "state", item);
    json_decref (item);

    item = json_integer (c->state_type);
    json_object_set (jdata, "state_type", item);
    json_decref (item);

    item = json_string (c->output);
    json_object_set (jdata, "output", item);
    json_decref (item);

    item = json_string (c->long_output);
    json_object_set (jdata, "long_output", item);
    json_decref (item);

    item = json_string (c->perf_data);
    json_object_set (jdata, "perf_data", item);
    json_decref (item);

    item = json_integer (c->check_type);
    json_object_set (jdata, "check_type", item);
    json_decref (item);

    item = json_integer (c->current_attempt);
    json_object_set (jdata, "current_attempt", item);
    json_decref (item);

    item = json_integer (c->max_attempts);
    json_object_set (jdata, "max_attempts", item);
    json_decref (item);

    item = json_real (c->execution_time);
    json_object_set (jdata, "execution_time", item);
    json_decref (item);

    item = json_real (c->latency);
    json_object_set (jdata, "latency", item);
    json_decref (item);

    item = json_string (c->command_name);
    json_object_set (jdata, "command_name", item);
    json_decref (item);

    /* add hostgroups to event */
    item = json_array ();
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

    /* now stringify JSON */

    json = json_dumps (jdata, 0);
    ref = xstrlen (json);

    if ((int) ref > g_options.max_size)
    {
        size_t save = ref - g_options.max_size;

        if (save <= xstrlen (c->long_output))
        {
            item = json_string ("");
            json_object_set (jdata, "long_output", item);
            json_decref (item);

            n2a_logger (LG_INFO, "long_output is too long! (host: %s)", c->host_name);
        }
        else if (save <= xstrlen (c->output))
        {
            item = json_string ("");
            json_object_set (jdata, "output", item);
            json_decref (item);

            n2a_logger (LG_INFO, "output is too long! (host: %s)", c->host_name);
        }
        else if (save <= xstrlen(c->perf_data))
        {
            item = json_string ("");
            json_object_set (jdata, "perf_data", item);
            json_decref (item);

            n2a_logger (LG_INFO, "perfdata is too long! (host: %s)", c->host_name);
        }

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
