#include "nagios.h"
#include "module.h"
#include "logger.h"
#include "events.h"

extern int event_broker_options;
extern void *g_nagios_handle;

int
verify_event_broker_options()
{
    int errors = 0;
    if (!(event_broker_options & BROKER_PROGRAM_STATE))
    {
        logger( LG_CRIT, "need BROKER_PROGRAM_STATE (%i) event_broker_option enabled to work.", BROKER_PROGRAM_STATE );
        errors++;
    }
    if (!(event_broker_options & BROKER_TIMED_EVENTS))
    {
        logger( LG_CRIT, "need BROKER_TIMED_EVENTS (%i) event_broker_option enabled to work.", BROKER_TIMED_EVENTS );
        errors++;
    }
    if (!(event_broker_options & BROKER_SERVICE_CHECKS))
    {
        logger( LG_CRIT, "need BROKER_SERVICE_CHECKS (%i) event_broker_option enabled to work.", BROKER_SERVICE_CHECKS );
        errors++;
    }
    if (!(event_broker_options & BROKER_HOST_CHECKS))
    {
        logger( LG_CRIT, "need BROKER_HOST_CHECKS (%i) event_broker_option enabled to work.", BROKER_HOST_CHECKS );
        errors++;
    }
    if (!(event_broker_options & BROKER_LOGGED_DATA))
    {
        logger( LG_CRIT, "need BROKER_LOGGED_DATA (%i) event_broker_option enabled to work.", BROKER_LOGGED_DATA );
        errors++;
    }
    if (!(event_broker_options & BROKER_COMMENT_DATA))
    {
        logger( LG_CRIT, "need BROKER_COMMENT_DATA (%i) event_broker_option enabled to work.", BROKER_COMMENT_DATA );
        errors++;
    }
    if (!(event_broker_options & BROKER_DOWNTIME_DATA))
    {
        logger( LG_CRIT, "need BROKER_DOWNTIME_DATA (%i) event_broker_option enabled to work.", BROKER_DOWNTIME_DATA );
        errors++;
    }
    if (!(event_broker_options & BROKER_STATUS_DATA))
    {
        logger( LG_CRIT, "need BROKER_STATUS_DATA (%i) event_broker_option enabled to work.", BROKER_STATUS_DATA );
        errors++;
    }
    if (!(event_broker_options & BROKER_ADAPTIVE_DATA))
    {
        logger( LG_CRIT, "need BROKER_ADAPTIVE_DATA (%i) event_broker_option enabled to work.", BROKER_ADAPTIVE_DATA );
        errors++;
    }
    if (!(event_broker_options & BROKER_EXTERNALCOMMAND_DATA))
    {
        logger( LG_CRIT, "need BROKER_EXTERNALCOMMAND_DATA (%i) event_broker_option enabled to work.", BROKER_EXTERNALCOMMAND_DATA );
        errors++;
    }
    if (!(event_broker_options & BROKER_STATECHANGE_DATA))
    {
        logger( LG_CRIT, "need BROKER_STATECHANGE_DATA (%i) event_broker_option enabled to work.", BROKER_STATECHANGE_DATA );
        errors++;
    }

    return errors == 0;
}

void
register_callbacks()
{
    neb_register_callback(NEBCALLBACK_PROCESS_DATA,		g_nagios_handle, 0, event_process); // used for starting threads
    neb_register_callback(NEBCALLBACK_PROGRAM_STATUS_DATA,		g_nagios_handle, 0, event_program_status);

    neb_register_callback(NEBCALLBACK_SERVICE_CHECK_DATA, 	g_nagios_handle, 0, event_service_check);
    neb_register_callback(NEBCALLBACK_HOST_CHECK_DATA,       	g_nagios_handle, 0, event_host_check);

    neb_register_callback(NEBCALLBACK_ACKNOWLEDGEMENT_DATA,	g_nagios_handle, 0, event_acknowledgement);
    neb_register_callback(NEBCALLBACK_DOWNTIME_DATA,		g_nagios_handle, 0, event_downtime);
    neb_register_callback(NEBCALLBACK_COMMENT_DATA,		g_nagios_handle, 0, event_comment);
}

void
deregister_callbacks()
{
    neb_deregister_callback(NEBCALLBACK_PROCESS_DATA,		event_process);
    neb_deregister_callback(NEBCALLBACK_PROGRAM_STATUS_DATA,event_program_status);

    neb_deregister_callback(NEBCALLBACK_SERVICE_CHECK_DATA,	event_service_check);
    neb_deregister_callback(NEBCALLBACK_HOST_CHECK_DATA,	event_host_check);

    neb_deregister_callback(NEBCALLBACK_ACKNOWLEDGEMENT_DATA,	event_acknowledgement);
    neb_deregister_callback(NEBCALLBACK_DOWNTIME_DATA,		event_downtime);
    neb_deregister_callback(NEBCALLBACK_COMMENT_DATA,		event_comment);
}
