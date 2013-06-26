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

#include "events.h"

extern int event_broker_options;

extern struct options g_options;

int
verify_event_broker_options ()
{
  int errors = 0;
  
  /*if (!(event_broker_options & BROKER_PROGRAM_STATE))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_PROGRAM_STATE (%i) event_broker_option enabled to work.",
	      BROKER_PROGRAM_STATE);
      errors++;
    }*/
    
  /*if (!(event_broker_options & BROKER_TIMED_EVENTS))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_TIMED_EVENTS (%i) event_broker_option enabled to work.",
	      BROKER_TIMED_EVENTS);
      errors++;
    }*/
    
  if (!(event_broker_options & BROKER_SERVICE_CHECKS))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_SERVICE_CHECKS (%i) event_broker_option enabled to work.",
	      BROKER_SERVICE_CHECKS);
      errors++;
    }
    
  if (!(event_broker_options & BROKER_HOST_CHECKS))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_HOST_CHECKS (%i) event_broker_option enabled to work.",
	      BROKER_HOST_CHECKS);
      errors++;
    }
    
  /*if (!(event_broker_options & BROKER_LOGGED_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_LOGGED_DATA (%i) event_broker_option enabled to work.",
	      BROKER_LOGGED_DATA);
      errors++;
    }*/
    
  /*if (!(event_broker_options & BROKER_COMMENT_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_COMMENT_DATA (%i) event_broker_option enabled to work.",
	      BROKER_COMMENT_DATA);
      errors++;
    }
    
  if (!(event_broker_options & BROKER_DOWNTIME_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_DOWNTIME_DATA (%i) event_broker_option enabled to work.",
	      BROKER_DOWNTIME_DATA);
      errors++;
    }
    
  if (!(event_broker_options & BROKER_STATUS_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_STATUS_DATA (%i) event_broker_option enabled to work.",
	      BROKER_STATUS_DATA);
      errors++;
    }
    
  if (!(event_broker_options & BROKER_ADAPTIVE_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_ADAPTIVE_DATA (%i) event_broker_option enabled to work.",
	      BROKER_ADAPTIVE_DATA);
      errors++;
    }
    
  if (!(event_broker_options & BROKER_EXTERNALCOMMAND_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_EXTERNALCOMMAND_DATA (%i) event_broker_option enabled to work.",
	      BROKER_EXTERNALCOMMAND_DATA);
      errors++;
    }
    
  if (!(event_broker_options & BROKER_STATECHANGE_DATA))
    {
      n2a_logger (LG_CRIT,
	      "need BROKER_STATECHANGE_DATA (%i) event_broker_option enabled to work.",
	      BROKER_STATECHANGE_DATA);
      errors++;
    }*/

  return errors == 0;
}

void
n2a_register_callbacks ()
{
  n2a_logger (LG_INFO, "Register callbacks");
  //neb_register_callback (NEBCALLBACK_PROCESS_DATA,			g_options.nagios_handle, 0, event_process);
  //neb_register_callback (NEBCALLBACK_PROGRAM_STATUS_DATA,	g_options.nagios_handle, 0, event_program_status);

  neb_register_callback (NEBCALLBACK_SERVICE_CHECK_DATA,    g_options.nagios_handle, 0, n2a_event_service_check);
  neb_register_callback (NEBCALLBACK_HOST_CHECK_DATA,       g_options.nagios_handle, 0, n2a_event_host_check);

  //neb_register_callback (NEBCALLBACK_ACKNOWLEDGEMENT_DATA,	g_options.nagios_handle, 0, event_acknowledgement);
  //neb_register_callback (NEBCALLBACK_DOWNTIME_DATA, 		g_options.nagios_handle, 0, event_downtime);
  //neb_register_callback (NEBCALLBACK_COMMENT_DATA, 			g_options.nagios_handle, 0, event_comment);
}

void
n2a_deregister_callbacks ()
{
  n2a_logger (LG_INFO, "Deregister callbacks");
  //neb_deregister_callback (NEBCALLBACK_PROCESS_DATA,			event_process);
  //neb_deregister_callback (NEBCALLBACK_PROGRAM_STATUS_DATA, 	event_program_status);

  neb_deregister_callback (NEBCALLBACK_SERVICE_CHECK_DATA,		n2a_event_service_check);
  neb_deregister_callback (NEBCALLBACK_HOST_CHECK_DATA,			n2a_event_host_check);
  
  //neb_deregister_callback (NEBCALLBACK_ACKNOWLEDGEMENT_DATA,	event_acknowledgement);
  //neb_deregister_callback (NEBCALLBACK_DOWNTIME_DATA,			event_downtime);
  //neb_deregister_callback (NEBCALLBACK_COMMENT_DATA,			event_comment);
}
