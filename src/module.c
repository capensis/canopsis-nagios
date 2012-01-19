/*****************************************************************************
 *
 * neb2socket.c - NEB Module to export data to Unix socket stream
 *
 * Copyright (c) 2011 Capensis
 *
 *****************************************************************************/

#include <sys/select.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>


#include "nagios.h"
#include "logger.h"
#include "broker.h"
#include "strutil.h"
#include "neb2amqp.h"


#include "module.h"

NEB_API_VERSION(CURRENT_NEB_API_VERSION)

extern int event_broker_options;

// Threads
int g_thread_running = 0;
int g_thread_pid = 0;
int g_should_terminate = FALSE;
pthread_t g_mainthread_id;
pthread_t *g_clientthread_id;
void* voidp;
pthread_mutex_t g_wait_mutex = PTHREAD_MUTEX_INITIALIZER;

char *hostname;
int port;
char *userid;
char *password;
char *virtual_host;
char *exchange_name;
char *routing_key;

int g_debug_level;
int g_max_fd_ever;

void *g_nagios_handle;
char *g_eventsource_name;

void parse_arguments(const char *args_orig);

//unsigned long g_max_cached_messages = 500000;
//unsigned long g_max_response_size = 100 * 1024 * 1024; // limit answer to 10 MB


/* this function gets called when the module is loaded by the event broker */
int
nebmodule_init(int flags __attribute__ ((__unused__)), char *args, void *handle) 
{
    g_nagios_handle = handle;

    g_eventsource_name = "Central";
    hostname = "127.0.0.1";
    port = 5672;
    userid = "guest";
    password = "guest";
    virtual_host = "canopsis";
    exchange_name = "canopsis.events";
    routing_key = "secret";
    g_max_fd_ever = 0;
    g_debug_level = 0;

    parse_arguments(args);

    logger(LG_INFO, "NEB2amqp %s by Capensis.", VERSION);
    logger(LG_INFO, "Please visit us at http://www.capensis.org/");

    if (!verify_event_broker_options())
    {
        logger(LG_CRIT, "Fatal: bailing out. Please fix event_broker_options.");
        logger(LG_CRIT, "Hint: your event_broker_options are set to %d. Try setting it to -1.", event_broker_options);
        return 1;
    }
    else if (g_debug_level > 0)
        logger(LG_INFO, "Your event_broker_options are sufficient for NEB2Socket.");

    register_callbacks();

    /* Unfortunately, we cannot start our socket thread right now.
       Nagios demonizes *after* having loaded the NEB modules. When
       demonizing we are losing our thread. Therefore, we create the
       thread the first time one of our callbacks is called. Before
       that happens, we haven't got any data anyway... */

    if (g_debug_level > 0)
        logger(LG_INFO, "successfully finished initialization");

    return 0;
}


int
nebmodule_deinit(int flags __attribute__ ((__unused__)), int reason __attribute__ ((__unused__)))
{
    logger(LG_INFO, "deinitializing");
    deregister_callbacks();
//	terminate_threads();
    return 0;
}

// FIXME: useless?
void *
main_thread(void *data __attribute__ ((__unused__)))
{

    g_thread_pid = getpid();
    logger(LG_INFO, "Main thread: Started");
    logger(LG_INFO, "Main thread: Wait Socket Connections ...");

    while (!g_should_terminate)
    {
        sleep (1);
        //logger(LG_INFO, "Main thread: Keepalive");
    }

    logger(LG_INFO, "Main thread: Terminated");
    return voidp;
}

void
start_threads()
{
    if (!g_thread_running)
    {
        logger(LG_INFO, "Start threads ...");
        pthread_create(&g_mainthread_id, 0, main_thread, (void *)0);
        g_thread_running = TRUE;
    }
}


void
terminate_threads()
{
    if (g_thread_running) 
    {
        logger(LG_INFO, "Stop threads ...");
        g_should_terminate = TRUE;

        logger(LG_INFO, "Waiting for main to terminate...");
        pthread_join(g_mainthread_id, NULL);

        g_thread_running = FALSE;
    }
}


// This code is part of Check_MK (GPL v2).
// The official homepage is at http://mathias-kettner.de/check_mk
void
parse_arguments(const char *args_orig)
{    
    /* set default socket path */
    //strncpy(g_socket_path, DEFAULT_SOCKET_PATH, sizeof(g_socket_path) - 1);

    if (!args_orig) 
        return; // no arguments, use default options
    
    char *args = strdup(args_orig);
    char *token;
    while (0 != (token = next_field(&args)))
    {
        /* find = */
        char *part = token;
        char *left = next_token(&part, '=');
        char *right = next_token(&part, 0);
        if (right == NULL) 
        {
            char *subpart = left;
            char *subleft = next_token(&subpart, ':');
            char *subright = next_token(&subpart, 0);
            if (subright == NULL)
            {
                hostname = subleft;
            }
            else
            {
                hostname = subright;
                port = strtol (subleft, NULL, 10);
                logger(LG_INFO, "Setting port number to %d", port);
            }
            logger(LG_INFO, "Setting hostname to %s", hostname);
    	}
        else 
        {
            if (strcmp(left, "debug") == 0)
            {
                g_debug_level = strtol (right, NULL, 10);
                logger(LG_INFO, "Setting debug level to %d", g_debug_level);
            }
            else if (strcmp(left, "name") == 0)
            {
                g_eventsource_name = right;
                logger(LG_INFO, "Setting g_eventsource_name to %s", g_eventsource_name);
            }
            else if (strcmp(left, "userid") == 0)
            {
                userid = right;
                logger(LG_INFO, "Setting userid to %s", userid);
            }
            else if (strcmp(left, "password") == 0)
            {
                password = right;
                logger(LG_INFO, "Setting password to %s", password);
            }
            else if (strcmp(left, "virtual_host") == 0)
            {
                virtual_host = right;
                logger(LG_INFO, "Setting virtual_host to %s", virtual_host);
            }
            else if (strcmp(left, "exchange_name") == 0)
            {
                exchange_name = right;
                logger(LG_INFO, "Setting exchange_name to %s", exchange_name);
            }
            else if (strcmp(left, "routing_key") == 0)
            {
                routing_key = right;
                logger(LG_INFO, "Setting routing_key to %s", routing_key);
            }
            else if (strcmp(left, "port") == 0)
            {
                port = strtol (right, NULL, 10);
                logger(LG_INFO, "Setting port to %d", port);
            }
            else if (strcmp(left, "host") == 0)
            {
                char *subpart = right;
                char *subleft = next_token(&subpart, ':');
                char *subright = next_token(&subpart, 0); 
                if (subright == NULL)
                {   
                    hostname = subleft;
                }   
                else
                {   
                    hostname = subright;
                    port = strtol (subleft, NULL, 10);
                    logger(LG_INFO, "Setting port number to %d", port);
                }
                logger(LG_INFO, "Setting hostname to %s", hostname);
            }
            else
            {
                logger(LG_INFO, "Ignoring invalid option %s=%s", left, right);
            }
        }
    } 
    // free(args); won't free, since we use pointers?
}

