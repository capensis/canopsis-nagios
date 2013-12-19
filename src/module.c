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
#include "logger.h"
#include "xutils.h"

#include "broker.h"
#include "neb2amqp.h"
#include "module.h"

NEB_API_VERSION (CURRENT_NEB_API_VERSION)

static char *g_args = NULL;

extern int event_broker_options;

struct options g_options;

static void n2a_parse_arguments (const char *args_orig);

/* this function gets called when the module is loaded by the event broker */
int nebmodule_init (
        int flags __attribute__ ((__unused__)),
        char *args,
        nebmodule *handle)
{
    /* Set Nagios handle */
    g_options.nagios_handle = handle;

    /* Init default options */
    g_options.eventsource_name = "Central";
    g_options.hostname = "127.0.0.1";
    g_options.port = 5672;
    g_options.userid = "guest";
    g_options.password = "guest";
    g_options.virtual_host = "canopsis";
    g_options.exchange_name = "canopsis.events";
    g_options.exchange_type = NULL;
    g_options.log_level = 0;
    g_options.connector = "nagios";
    g_options.max_size = 8192;
    g_options.cache_size = 10000;
    g_options.autosync = 60;
    g_options.autoflush = 60;
    g_options.flush_interval = 5;
    g_options.rate = 5000;
    g_options.flush = -1;
    g_options.purge = FALSE;
    g_options.cache_file = "/tmp/neb2amqp.cache";
    g_options.pFifo = NULL;

    g_options.acknowledgement = FALSE;
    g_options.downtime = FALSE;
    g_options.hostgroups = FALSE;
    g_options.servicegroups = FALSE;
    g_options.custom_variables = FALSE;
    g_options.ssl = FALSE;

    /* Parse module options */
    n2a_parse_arguments (args);

    /* Init module */
    n2a_logger (LG_INFO, "NEB2amqp %s by Capensis. (connector: %s)", VERSION, g_options.connector);
    n2a_logger (LG_INFO, "Please visit us at http://www.canopsis.org/");

    if (!n2a_verify_event_broker_options ())
    {
        n2a_logger (LG_CRIT, "Fatal: bailing out. Please fix event_broker_options.");
        n2a_logger (LG_CRIT, "Hint: your event_broker_options are set to %d. Try setting it to -1.", event_broker_options);
        return 1;
    }
 
    /* TODO */
    g_options.pFifo = n2a_fifo_init (g_options.cache_size, g_options.cache_file);
    n2a_amqp_check ();

    n2a_register_callbacks ();

    n2a_logger (LG_INFO, "successfully finished initialization");

    return 0;
}

int nebmodule_deinit (
        int flags __attribute__ ((__unused__)),
        int reason __attribute__ ((__unused__)))
{
    n2a_logger (LG_INFO, "deinitializing");
  
    n2a_deregister_callbacks ();

    n2a_amqp_disconnect ();

    n2a_free_fifo (g_options.pFifo);
 
    xfree (g_args);

    return 0;
}

/* This code is part of Check_MK (GPL v2).
 * The official homepage is at http://mathias-kettner.de/check_mk
 */
static void n2a_parse_arguments (const char *args_orig)
{
    char *save = NULL;
    char *token = NULL;

    if (!args_orig)
    {
        /* no arguments, use default options */
        return;
    }

    g_args = xstrdup (args_orig);
    save = g_args;

    while (0 != (token = n2a_next_field (&g_args)))
    {
        /* find = */
        char *part = token;
        char *left = n2a_next_token (&part, '=');
        char *right = n2a_next_token (&part, 0);

        if (right == NULL)
        {
            char *subpart = left;
            char *subleft = n2a_next_token (&subpart, ':');
            char *subright = n2a_next_token (&subpart, 0);

            if (subright == NULL)
            {
                g_options.hostname = subleft;
            }
            else
            {
                g_options.hostname = subright;
                g_options.port = strtol (subleft, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting port number to %d", g_options.port);
            }

            n2a_logger (LG_DEBUG, "Setting hostname to %s", g_options.hostname);
        }
        else
        {
            if (strcmp (left, "debug") == 0)
            {
                g_options.log_level = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting debug level to %d", g_options.log_level);
            }
            else if (strcmp (left, "purge") == 0)
            {
                if (strncasecmp (right, "y", 1) == 0 || strncasecmp (right, "t", 1) == 0)
                {
                    g_options.purge = TRUE;
                }
                else if (strncasecmp (right, "f", 1) == 0 || strncasecmp (right, "n", 1) == 0)
                {
                    g_options.purge = FALSE;
                }
                else
                {
                    char *sav;
                    int r = strtol (right, &sav, 10);

                    if (right == sav)
                    {
                        g_options.purge = FALSE;
                    }
                    else
                    {
                        switch (r)
                        {
                            case 1:
                                g_options.purge = TRUE;
                                break;

                            case 0:
                            default:
                                g_options.purge = FALSE;
                                break;
                        }
                    }
                }

                n2a_logger (LG_DEBUG, "Setting purge to '%s'", g_options.purge ? "true": "false");
            }
            else if (strcmp (left, "rate") == 0)
            {
                int r = strtol (right, NULL, 10);

                if (r > 0)
                {
                    g_options.rate = r * 1000;

                    n2a_logger (LG_DEBUG, "Setting rate to %dms", r);
                }
                else
                {
                    n2a_logger (LG_DEBUG, "Wrong value for option 'rate', leave it to %dms",
                        g_options.rate / 1000
                    );
                }
            }
            else if (strcmp (left, "flush") == 0)
            {
                int r = strtol (right, NULL, 10);

                if (r > 0 || r == -1)
                {
                    g_options.flush = r;

                    n2a_logger (LG_DEBUG, "Setting flush to %d messages", r);
                }
                else
                {
                    n2a_logger (LG_DEBUG, "Wrong value for option 'flush', leave it to %d messages",
                        g_options.flush
                    );
                }
            }
            else if (strcmp (left, "max_size") == 0)
            {
                g_options.max_size = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting max_size buffer to %d bits",
                    g_options.max_size
                );
            }
            else if (strcmp (left, "autoflush") == 0)
            {
                g_options.autoflush = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting autoflush to %ds", g_options.autoflush);
            }
            else if (strcmp (left, "flush_interval") == 0)
            {
                g_options.flush_interval = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting flush_interval to %ds", g_options.autoflush);
            }
            else if (strcmp (left, "cache_size") == 0)
            {
                g_options.cache_size = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting cache_size to %d",
                    g_options.cache_size
                );
            }
            else if (strcmp (left, "cache_file") == 0)
            {
                g_options.cache_file = right;

                n2a_logger (LG_DEBUG, "Setting cache_file to '%s'",
                    g_options.cache_file
                );
            }
            else if (strcmp (left, "autosync") == 0)
            {
                g_options.autosync = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting autosync to %ds",
                    g_options.autosync
                );
            }
            else if (strcmp (left, "name") == 0)
            {
                g_options.eventsource_name = right;

                n2a_logger (LG_DEBUG, "Setting g_eventsource_name to %s",
                    g_options.eventsource_name
                );
            }
            else if (strcmp (left, "userid") == 0)
            {
                g_options.userid = right;

                n2a_logger (LG_DEBUG, "Setting userid to %s", g_options.userid);
            }
            else if (strcmp (left, "password") == 0)
            {
                g_options.password = right;

                n2a_logger (LG_DEBUG, "Setting password to %s", g_options.password);
            }
            else if (strcmp (left, "virtual_host") == 0)
            {
                g_options.virtual_host = right;

                n2a_logger (LG_DEBUG, "Setting virtual_host to %s", g_options.virtual_host);
            }
            else if (strcmp (left, "exchange_name") == 0)
            {
                g_options.exchange_name = right;

                n2a_logger (LG_DEBUG, "Setting exchange_name to %s", g_options.exchange_name);
            }
            else if (strcmp (left, "exchange_type") == 0)
            {
                g_options.exchange_type = right;

                n2a_logger (LG_DEBUG, "Setting exchange_type to %s", g_options.exchange_type);
            }
            else if (strcmp (left, "connector") == 0)
            {
                g_options.connector = right;

                n2a_logger (LG_DEBUG, "Setting connector to %s", g_options.connector);
            }
            else if (strcmp (left, "port") == 0)
            {
                g_options.port = strtol (right, NULL, 10);

                n2a_logger (LG_DEBUG, "Setting port to %d", g_options.port);
            }
            else if (strcmp (left, "host") == 0)
            {
                char *subpart = right;
                char *subleft = n2a_next_token (&subpart, ':');
                char *subright = n2a_next_token (&subpart, 0);

                if (subright == NULL)
                {
                    g_options.hostname = subleft;
                }
                else
                {
                    g_options.hostname = subright;
                    g_options.port = strtol (subleft, NULL, 10);

                    n2a_logger (LG_DEBUG, "Setting port number to %d", g_options.port);
                }

                n2a_logger (LG_DEBUG, "Setting hostname to %s", g_options.hostname);
            }
            else if (strcmp (left, "hostgroups") == 0)
            {
                g_options.hostgroups = strtol (right, NULL, 2);

                n2a_logger (LG_ERR, "Setting hostgroups to '%s'", g_options.hostgroups ? "true": "false");
            }
            else if (strcmp (left, "servicegroups") == 0)
            {
                g_options.servicegroups = strtol (right, NULL, 2);

                n2a_logger (LG_ERR, "Setting servicegroups to '%s'", g_options.servicegroups ? "true": "false");
            }
            else if (strcmp (left, "ssl") == 0)
            {
                g_options.ssl = strtol (right, NULL, 2);

                n2a_logger (LG_ERR, "Setting ssl to '%s'", g_options.ssl ? "true": "false");
            }
            else if (strcmp (left, "acknowledgement") == 0)
            {
                g_options.acknowledgement = strtol (right, NULL, 2);

                n2a_logger (LG_ERR, "Setting acknowledgement to '%s'", g_options.acknowledgement ? "true": "false");
            }
            else if (strcmp (left, "downtime") == 0)
            {
                g_options.downtime = strtol (right, NULL, 2);

                n2a_logger (LG_ERR, "Setting downtime to '%s'", g_options.downtime ? "true": "false");
            }
            else if (strcmp (left, "custom_variables") == 0)
            {
                g_options.custom_variables = strtol (right, NULL, 2);

                n2a_logger (LG_ERR, "Setting custom_variables to '%s'", g_options.custom_variables ? "true": "false");
            }
            else
            {
                n2a_logger (LG_ERR, "Ignoring invalid option %s=%s", left, right);
            }
        }
    }

    g_args = save;
}
