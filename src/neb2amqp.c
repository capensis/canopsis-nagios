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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

#include <sys/time.h>

#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_private.h>
#include <amqp_tcp_socket.h>
#include <amqp_ssl_socket.h>

#include "neb2amqp.h"
#include "module.h"
#include "logger.h"
#include "xutils.h"

extern struct options g_options;

struct timeval now;

static bool amqp_errors = false;
/*static bool first = true;*/

static int amqp_last_connect = 0;
static int amqp_wait_time = 10;

static int fifo_last_sync = 0;
static int fifo_last_flush = 0;

bool amqp_connected = false;

bool blackout = false;

static amqp_connection_state_t conn = NULL;

bool n2a_toggle_blackout (void)
{
    if (!blackout)
    {
        n2a_logger (LG_INFO, "AMQP: !!!! BLACKOUT !!!!");
        n2a_amqp_disconnect();
        blackout = true;
    }
    else
    {
        n2a_logger (LG_INFO, "AMQP: Disable blackout");
        blackout = false;
    }

    return blackout;
}

void n2a_on_error (int x, char const *context)
{
    if (x < 0)
    {
        const char *errstr = amqp_error_string2 (-x);
        n2a_logger (LG_ERR, "AMQP: %s: %s\n", context, errstr);

        amqp_errors = true;
    }
}

void n2a_on_amqp_error (amqp_rpc_reply_t x, char const *context)
{
    switch (x.reply_type)
    {
        case AMQP_RESPONSE_NORMAL:
            return;

        case AMQP_RESPONSE_NONE:
            n2a_logger (LG_ERR, "AMQP: %s: missing RPC reply type!\n", context);
            break;

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        {
            const char *err = amqp_error_string2 (x.library_error);
            n2a_logger (LG_ERR, "AMQP: %s: %s\n", context, err);
            break;
        }

        case AMQP_RESPONSE_SERVER_EXCEPTION:
            switch (x.reply.id)
            {
                case AMQP_CONNECTION_CLOSE_METHOD:
                {
                    amqp_connection_close_t *m = (amqp_connection_close_t *) x.reply.decoded;

                    n2a_logger (LG_ERR, "AMQP: %s: server connection error %d, message: %.*s\n",
                        context,
                        m->reply_code,
                        (int) m->reply_text.len,
                        (char *) m->reply_text.bytes
                    );
                    break;
                }

                case AMQP_CHANNEL_CLOSE_METHOD:
                {
                    amqp_channel_close_t *m = (amqp_channel_close_t *) x.reply.decoded;
                    n2a_logger (LG_ERR, "AMQP: %s: server channel error %d, message: %.*s\n",
                        context,
                        m->reply_code,
                        (int) m->reply_text.len,
                        (char *) m->reply_text.bytes
                    );
                    break;
                }

                default:
                    n2a_logger (LG_ERR, "AMQP: %s: unknown server error, method id 0x%08X\n",
                        context,
                        x.reply.id
                    );
                    break;
            }

            break;
    }

    amqp_errors = true;
}

bool n2a_amqp_connect (void)
{
    amqp_errors = false;
    amqp_socket_t *socket = NULL;

    gettimeofday (&now, NULL);
    amqp_last_connect = (int) now.tv_sec;
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    if (conn)
    {
        amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection (conn);
    }

    n2a_logger (LG_DEBUG, "AMQP: Init connection");
    conn = amqp_new_connection ();
     
    n2a_logger (LG_DEBUG, "AMQP: Creating socket");
    if (!g_options.ssl)
        socket = amqp_tcp_socket_new(conn);
    else
        socket = amqp_ssl_socket_new(conn);
 
    n2a_logger (LG_DEBUG, "AMQP: Opening socket");
    n2a_on_error (amqp_socket_open_noblock (socket, g_options.hostname, g_options.port, &timeout), "Opening socket");

    if (!amqp_errors)
    {
        n2a_logger (LG_DEBUG, "AMQP: Login");
        n2a_on_amqp_error (
            amqp_login (
                conn,
                g_options.virtual_host,
                0, 131072, 0,
                AMQP_SASL_METHOD_PLAIN,
                g_options.userid,
                g_options.password
            ),
            "Logging in"
        );
    }

    if (!amqp_errors)
    {
        n2a_logger (LG_DEBUG, "AMQP: Open channel");
        amqp_channel_open (conn, 1);
        n2a_on_amqp_error (amqp_get_rpc_reply (conn), "Opening channel");
    }

    if (!amqp_errors && NULL != g_options.exchange_type)
    {
        n2a_logger (LG_DEBUG, "AMQP: setting exchange type");
        amqp_exchange_declare(conn, 1, amqp_cstring_bytes(g_options.exchange_name), amqp_cstring_bytes(g_options.exchange_type), 0, 0, amqp_empty_table);
        n2a_on_amqp_error (amqp_get_rpc_reply(conn), "Declaring exchange");
    }

    if (!amqp_errors)
    {
        n2a_logger (LG_INFO, "AMQP: Successfully connected");
        amqp_connected = true;

        /* Force fifo flush */
        fifo_last_flush = 0;
    }
    else
    {
        amqp_connected = false;
    }

    return amqp_connected;
}

void n2a_fifo_check (void)
{
    int timestamp = 0;
    int elapsed = 0;

    gettimeofday (&now, NULL);
    timestamp = now.tv_sec;

    /* Flush queue*/
    elapsed = timestamp - fifo_last_flush;

    if (amqp_connected && g_options.pFifo->size > 0 && elapsed >= g_options.flush_interval)
    {
        int size = g_options.pFifo->size;
        int i;

        int flush = g_options.flush;

        n2a_logger (LG_DEBUG, "AMQP: Shift queue, size: %d", size);

        if (flush == -1)
        {
            flush = (int) (g_options.cache_size / 5);
      
            if (flush > 1000)
            {
                flush = 1000;
            }
        }

        for (i = 0; i < flush; i++)
        {
            event *pEvent = n2a_shift (g_options.pFifo);

            /* End of fifo */
            if (pEvent == NULL)
            {
                break;
            }

            if (!n2a_amqp_publish (pEvent->rk, pEvent->msg))
            {
                n2a_prepend (g_options.pFifo, pEvent);
                break;
            }
            else
            {
                n2a_free_event (pEvent);
            }
        }

        n2a_logger (LG_INFO, "AMQP: %d/%d events shifted from Queue, new size: %d", i, size, g_options.pFifo->size);
        fifo_last_flush = timestamp;
    }

    /* Save queue */
    elapsed = timestamp - fifo_last_sync;

    if (elapsed >= g_options.autosync)
    {
        n2a_csync (g_options.pFifo);

        fifo_last_sync = timestamp;
    }
}

bool n2a_amqp_check (void)
{
    int timestamp, elapsed;

    if (blackout)
    {
        return false;
    }

    if (amqp_connected)
    {
        return amqp_connected;
    }

    amqp_connected = false;

    gettimeofday (&now, NULL);
    timestamp = (int) now.tv_sec;
    elapsed = timestamp - amqp_last_connect;

    if ((amqp_last_connect == 0) || (!amqp_connected && elapsed >= amqp_wait_time))
    {
        n2a_logger (LG_DEBUG, "AMQP: Re-connect to amqp ...");
        n2a_amqp_connect ();
    }  

    return amqp_connected;
}

void n2a_amqp_disconnect (void)
{
    amqp_errors = false;
  
    if (amqp_connected)
    {
        n2a_logger (LG_DEBUG, "AMQP: Closing channel");

        n2a_on_amqp_error (
            amqp_channel_close (conn, 1, AMQP_REPLY_SUCCESS),
            "Closing channel"
        );

        n2a_logger (LG_DEBUG, "AMQP: Closing connection");
        n2a_on_amqp_error (
            amqp_connection_close (conn, AMQP_REPLY_SUCCESS),
            "Closing connection"
        );

        n2a_logger (LG_DEBUG, "AMQP: Ending connection");
        n2a_on_error (amqp_destroy_connection (conn), "Ending connection");
      
        conn = NULL;
        amqp_connected = false;

        n2a_logger (LG_INFO, "AMQP: Successfully disconnected");
    }
    else
    {
        n2a_logger (LG_INFO, "AMQP: Impossible to disconnect, not connected");
    }
}

bool n2a_amqp_publish (const char *routingkey, const char *message)
{
    amqp_basic_properties_t props;
    int result;

    props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CONTENT_ENCODING_FLAG;
    props.content_type = amqp_cstring_bytes ("application/json");
    props.content_encoding = amqp_cstring_bytes ("UTF-8");
    props.delivery_mode = 2;  /* persistent delivery mode */

    result = amqp_basic_publish (conn,
        1,
        amqp_cstring_bytes (g_options.exchange_name),
        amqp_cstring_bytes (routingkey),
        0,
        0,
        &props,
        amqp_cstring_bytes (message)
    );

    n2a_on_error (result, "Publishing");

    if (amqp_errors)
    {
        n2a_logger (LG_ERR, "AMQP: Error on publish");
        n2a_amqp_disconnect ();
        return false;
    }

    return true;
}

int n2a_send_event (const char *routingkey, const char *message)
{
    int amqp_state = n2a_amqp_check ();

    n2a_fifo_check ();

    if (amqp_state && g_options.pFifo->size == 0)
    {
        if (!n2a_amqp_publish (routingkey, message))
        {
            return n2a_push (g_options.pFifo, n2a_event_init (routingkey, message));
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return n2a_push (g_options.pFifo, n2a_event_init(routingkey, message));
    }
}
