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

#include "json.h"
#include "neb2amqp.h"
#include "xutils.h"

#include "events.h"

extern struct options g_options;

int g_last_event_program_status = 0;

/* Define a macro that will handle the split of messages */
#define split_message(message,field)                                                    \
do                                                                                      \
{                                                                                       \
    temp = ((int) xstrlen (message) / left + 1);                                        \
    i = 0;                                                                              \
                                                                                        \
    while (i < temp)                                                                    \
    {                                                                                   \
        char *json = NULL;                                                              \
        size_t len = 0;                                                                 \
                                                                                        \
        n2a_nebstruct_service_check_data_update_json (&jdata, message, field, left, i); \
                                                                                        \
        json = json_dumps (jdata, 0);                                                   \
        len = xstrlen (json);                                                           \
                                                                                        \
        buffer = xmalloc (len + 1);                                                     \
        snprintf (buffer, len + 1, "%s", json);                                         \
        n2a_send_event (key, buffer);                                                   \
                                                                                        \
        xfree (buffer);                                                                 \
        xfree (json);                                                                   \
                                                                                        \
        i++;                                                                            \
    }                                                                                   \
} while(0)

int n2a_event_service_check (int event_type __attribute__ ((__unused__)), void *data)
{
    nebstruct_service_check_data *c = (nebstruct_service_check_data *) data;

    if (c->type == NEBTYPE_SERVICECHECK_PROCESSED)
    {
        char *buffer = NULL;
        char *key = NULL;

        json_t *jdata = NULL;
        size_t message_size = 0;

        int nbmsg = n2a_nebstruct_service_check_data_to_json (c, &jdata, &message_size);

        key = n2a_str_join (".",
            g_options.connector,
            g_options.eventsource_name,
            "check",
            "resource",
            c->host_name,
            c->service_description,
            NULL
        );

        /* if the rk is too big */
        if (xstrlen (key) > g_options.max_size)
        {
            /* truncate it */
            key[g_options.max_size] = 0;

            /* then free available memory */
            key = realloc (key, strlen (key) + 1);
        }

        if (nbmsg == 1)
        {
            char *json = json_dumps (jdata, 0);
            buffer = xmalloc (message_size + 1);

            snprintf (buffer, message_size + 1, "%s", json);
            n2a_send_event (key, buffer);

            xfree (buffer);
            xfree (json);
        }
        else
        {
            int left = g_options.max_size - (int) message_size;

            size_t l_out = xstrlen (c->long_output);
            size_t out = xstrlen (c->output);
            size_t perf = xstrlen (c->perf_data);

            int msgs = ((int) l_out / left + 1) + ((int) out / left + 1) + ((int) perf / left + 1);

            int i = 0, temp = 0;

            n2a_logger (LG_INFO, "Data too long... sending %d messages for host: %s, service: %s", msgs, c->host_name, c->service_description);

            split_message (c->long_output, "long_output");
            split_message (c->output, "output");
            split_message (c->perf_data, "perf_data");
        }

        if (jdata != NULL)
        {
            json_decref (jdata);
        }

        xfree (key);
    }

    return 0;
}

int n2a_event_host_check (int event_type __attribute__ ((__unused__)), void *data)
{
    nebstruct_host_check_data *c = (nebstruct_host_check_data *) data;

    if (c->type == NEBTYPE_HOSTCHECK_PROCESSED)
    {
        char *buffer = NULL;
        char *key = NULL;

        n2a_nebstruct_host_check_data_to_json (&buffer, c);

        key = n2a_str_join (".",
            g_options.connector,
            g_options.eventsource_name,
            "check",
            "component",
            c->host_name,
            NULL
        );

        /* if the rk is too big */
        if (xstrlen (key) > g_options.max_size)
        {
            /* truncate it */
            key[g_options.max_size] = 0;

            /* then free available memory */
            key = realloc (key, strlen (key) + 1);
        }

        n2a_send_event (key, buffer);

        xfree (buffer);
        xfree (key);
    }

    return 0;
}

int n2a_event_void (int event_type __attribute__ ((__unused__)), void *data)
{
    return 0;
}


int n2a_event_process (int event_type __attribute__ ((__unused__)), void *data)
{
    struct nebstruct_process_struct *ps = (struct nebstruct_process_struct *) data;

    return 0;
}

int n2a_event_program_status (int event_type __attribute__ ((__unused__)), void *data)
{
    nebstruct_program_status_data *ps = (nebstruct_program_status_data *) data;

    /* Send program_status every 10sec min */
    if ((int) ps->timestamp.tv_sec >= (g_last_event_program_status + 10))
    {
        char buffer[AMQP_MSG_SIZE_MAX];

        /* TODO:
         * nebstruct_program_status_data_to_json (buffer, ps);
         * n2a_send_event (exchange_name, routingkey, buffer);
         */
        g_last_event_program_status = (int) ps->timestamp.tv_sec;
    }

    return 0;
}

int n2a_event_acknowledgement (int event_type __attribute__ ((__unused__)), void *data)
{
    nebstruct_acknowledgement_data *c = (nebstruct_acknowledgement_data *) data;

    n2a_logger (LG_DEBUG, "acktype: %d", c->type);

    if (c->type == NEBTYPE_ACKNOWLEDGEMENT_ADD)
    {
        char *buffer = NULL;
        char *key = NULL;

        n2a_logger (LG_DEBUG, "Event: event_acknowledgement ADD");
        printf ("ACK STATE: %d\n", c->state);

        if (c->acknowledgement_type == HOST_ACKNOWLEDGEMENT)
        {
            key = n2a_str_join (".",
                g_options.connector,
                g_options.eventsource_name,
                "ack",
                "component",
                c->host_name,
                NULL
            );
        }
        else if (c->acknowledgement_type == SERVICE_ACKNOWLEDGEMENT)
        {
            key = n2a_str_join (".",
                g_options.connector,
                g_options.eventsource_name,
                "ack",
                "resource",
                c->host_name,
                c->service_description,
                NULL
            );
        }

        /* if the rk is too big */
        if (xstrlen (key) > g_options.max_size)
        {
            /* truncate it */
            key[g_options.max_size] = 0;

            /* then free available memory */
            key = realloc (key, strlen (key) + 1);
        }

        n2a_nebstruct_acknolegement_data_to_json (&buffer, c);
        n2a_send_event (key, buffer);

        xfree (buffer);
        xfree (key);
    }
    else if (c->type == NEBTYPE_ACKNOWLEDGEMENT_REMOVE)
    {
        /* NOT IMPLEMENTED IN NAGIOS */
        n2a_logger (LG_DEBUG, "Event: event_acknowledgement REMOVE");
    }

    return 0;
}

int n2a_event_downtime (int event_type __attribute__ ((__unused__)), void *data)
{
    nebstruct_downtime_data *c = (nebstruct_downtime_data *) data;

    if (c->type == NEBTYPE_DOWNTIME_START)
    {
        n2a_logger (LG_DEBUG, "Event: event_downtime START");
    }
    else if (c->type == NEBTYPE_DOWNTIME_STOP)
    {
        n2a_logger (LG_DEBUG, "Event: event_downtime STOP");
    }
    else if (c->type == NEBTYPE_DOWNTIME_ADD)
    {
        char *buffer = NULL;
        char *key = NULL;

        n2a_logger (LG_DEBUG, "Event: event_downtime ADD");

        if (c->downtime_type == HOST_DOWNTIME)
        {
            key = n2a_str_join (".",
                g_options.connector,
                g_options.eventsource_name,
                "downtime",
                "component",
                c->host_name,
                NULL
            );
        }
        else if (c->downtime_type == SERVICE_DOWNTIME)
        {
            key = n2a_str_join (".",
                g_options.connector,
                g_options.eventsource_name,
                "downtime",
                "resource",
                c->host_name,
                c->service_description,
                NULL
            );
        }

        /* if the rk is too big */
        if (xstrlen (key) > g_options.max_size)
        {
            /* truncate it */
            key[g_options.max_size] = 0;

            /* then free available memory */
            key = realloc (key, strlen (key) + 1);
        }

        n2a_nebstruct_downtime_data_to_json (&buffer, c);
        n2a_send_event (key, buffer);

        xfree (buffer);
        xfree (key);
    }

    return 0;
}

int n2a_event_comment (int event_type __attribute__ ((__unused__)), void *data)
{
    nebstruct_comment_data *c = (nebstruct_comment_data *) data;

    if (c->type == NEBTYPE_COMMENT_ADD)
    {
        n2a_logger (LG_DEBUG, "Event: event_comment ADD");
    }
    else if (c->type == NEBTYPE_COMMENT_DELETE)
    {
        n2a_logger (LG_DEBUG, "Event: event_comment DELETE");
    }

    if (c->type == NEBTYPE_COMMENT_ADD || c->type == NEBTYPE_COMMENT_DELETE)
    {
        char buffer[AMQP_MSG_SIZE_MAX];

        /* TODO:
         * nebstruct_comment_data_to_json (buffer, c);
         * n2a_send_event (exchange_name, routingkey, buffer);
         */
    }

    return 0;
}


