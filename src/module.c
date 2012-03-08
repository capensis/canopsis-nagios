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
#include "logger.h"
#include "strutil.h"

#include "broker.h"
#include "neb2amqp.h"

#include "module.h"

NEB_API_VERSION (CURRENT_NEB_API_VERSION)

extern int event_broker_options;

struct options g_options;

void parse_arguments (const char *args_orig);

/* this function gets called when the module is loaded by the event broker */
int
nebmodule_init (int flags __attribute__ ((__unused__)), char *args, void *handle)
{
  // Set Nagios handle
  g_options.nagios_handle = handle;

  // Init default options
  g_options.eventsource_name = "Central";
  g_options.hostname = "127.0.0.1";
  g_options.port = 5672;
  g_options.userid = "guest";
  g_options.password = "guest";
  g_options.virtual_host = "canopsis";
  g_options.exchange_name = "canopsis.events";
  g_options.routing_key = "secret";
  g_options.log_level = 0;

  // Parse module options
  parse_arguments (args);

  // Init module
  logger (LG_INFO, "NEB2amqp %s by Capensis.", VERSION);
  logger (LG_INFO, "Please visit us at http://www.canopsis.org/");

  if (! verify_event_broker_options ()) {
      logger (LG_CRIT, "Fatal: bailing out. Please fix event_broker_options.");
      logger (LG_CRIT, "Hint: your event_broker_options are set to %d. Try setting it to -1.", event_broker_options);
      return 1;
   }
 
  amqp_connect ();

  register_callbacks ();

  logger (LG_INFO, "successfully finished initialization");

  return 0;
}

int
nebmodule_deinit (int flags __attribute__ ((__unused__)), int reason
		  __attribute__ ((__unused__)))
{
  logger (LG_INFO, "deinitializing");
  
  amqp_disconnect ();
  
  deregister_callbacks ();
  
  return 0;
}

// This code is part of Check_MK (GPL v2).
// The official homepage is at http://mathias-kettner.de/check_mk
void
parse_arguments (const char *args_orig)
{

  if (!args_orig)
    return;			// no arguments, use default options

  char *args = strdup (args_orig);
  char *token;
  while (0 != (token = next_field (&args)))
    {
      /* find = */
      char *part = token;
      char *left = next_token (&part, '=');
      char *right = next_token (&part, 0);
      if (right == NULL)
	{
	  char *subpart = left;
	  char *subleft = next_token (&subpart, ':');
	  char *subright = next_token (&subpart, 0);
	  if (subright == NULL)
	    {
	      g_options.hostname = subleft;
	    }
	  else
	    {
	      g_options.hostname = subright;
	      g_options.port = strtol (subleft, NULL, 10);
	      logger (LG_DEBUG, "Setting port number to %d", g_options.port);
	    }
	  logger (LG_DEBUG, "Setting hostname to %s", g_options.hostname);
	}
      else
	{
	  if (strcmp (left, "debug") == 0)
	    {
	      g_options.log_level = strtol (right, NULL, 10);
	      logger (LG_DEBUG, "Setting debug level to %d", g_options.log_level);
	    }
	  else if (strcmp (left, "name") == 0)
	    {
	      g_options.eventsource_name = right;
	      logger (LG_DEBUG, "Setting g_eventsource_name to %s",
		      g_options.eventsource_name);
	    }
	  else if (strcmp (left, "userid") == 0)
	    {
	      g_options.userid = right;
	      logger (LG_DEBUG, "Setting userid to %s", g_options.userid);
	    }
	  else if (strcmp (left, "password") == 0)
	    {
	      g_options.password = right;
	      logger (LG_DEBUG, "Setting password to %s", g_options.password);
	    }
	  else if (strcmp (left, "virtual_host") == 0)
	    {
	      g_options.virtual_host = right;
	      logger (LG_DEBUG, "Setting virtual_host to %s", g_options.virtual_host);
	    }
	  else if (strcmp (left, "exchange_name") == 0)
	    {
	      g_options.exchange_name = right;
	      logger (LG_DEBUG, "Setting exchange_name to %s", g_options.exchange_name);
	    }
	  else if (strcmp (left, "routing_key") == 0)
	    {
	      g_options.routing_key = right;
	      logger (LG_DEBUG, "Setting routing_key to %s", g_options.routing_key);
	    }
	  else if (strcmp (left, "port") == 0)
	    {
	      g_options.port = strtol (right, NULL, 10);
	      logger (LG_DEBUG, "Setting port to %d", g_options.port);
	    }
	  else if (strcmp (left, "host") == 0)
	    {
	      char *subpart = right;
	      char *subleft = next_token (&subpart, ':');
	      char *subright = next_token (&subpart, 0);
	      if (subright == NULL)
		{
		  g_options.hostname = subleft;
		}
	      else
		{
		  g_options.hostname = subright;
		  g_options.port = strtol (subleft, NULL, 10);
		  logger (LG_DEBUG, "Setting port number to %d", g_options.port);
		}
	      logger (LG_DEBUG, "Setting hostname to %s", g_options.hostname);
	    }
	  else
	    {
	      logger (LG_ERR, "Ignoring invalid option %s=%s", left, right);
	    }
	}
    }
}
