#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

#include <sys/time.h>

#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_private.h>

#include "neb2amqp.h"
#include "logger.h"

extern char *hostname;
extern int port;
extern char *userid;
extern char *password;
extern char *virtual_host;
extern char *exchange_name;
extern char *routing_key;
extern char *g_eventsource_name;

int sockfd;
bool amqp_connected;
bool amqp_errors;
int amqp_lastconnect = 0;
int amqp_wait_time = 10;

amqp_connection_state_t conn;

void
on_error (int x, char const *context)
{

  if (x < 0)
    {
      char *errstr = amqp_error_string (-x);
      logger (LG_INFO, "AMQP: %s: %s\n", context, errstr);
      free (errstr);

      amqp_errors = true;
    }

}

void
on_amqp_error (amqp_rpc_reply_t x, char const *context)
{

  switch (x.reply_type)
    {
    case AMQP_RESPONSE_NORMAL:
      return;

    case AMQP_RESPONSE_NONE:
      logger (LG_INFO, "AMQP: %s: missing RPC reply type!\n", context);
      break;

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:
      logger (LG_INFO, "AMQP: %s: %s\n", context,
	      amqp_error_string (x.library_error));
      break;

    case AMQP_RESPONSE_SERVER_EXCEPTION:
      switch (x.reply.id)
	{
	case AMQP_CONNECTION_CLOSE_METHOD:
	  {
	    amqp_connection_close_t *m =
	      (amqp_connection_close_t *) x.reply.decoded;
	    logger (LG_INFO,
		    "AMQP: %s: server connection error %d, message: %.*s\n",
		    context, m->reply_code, (int) m->reply_text.len,
		    (char *) m->reply_text.bytes);
	    break;
	  }
	case AMQP_CHANNEL_CLOSE_METHOD:
	  {
	    amqp_channel_close_t *m =
	      (amqp_channel_close_t *) x.reply.decoded;
	    logger (LG_INFO,
		    "AMQP: %s: server channel error %d, message: %.*s\n",
		    context, m->reply_code, (int) m->reply_text.len,
		    (char *) m->reply_text.bytes);
	    break;
	  }
	default:
	  logger (LG_INFO,
		  "AMQP: %s: unknown server error, method id 0x%08X\n",
		  context, x.reply.id);
	  break;
	}
      break;
    }

  amqp_errors = true;

}


void
amqp_connect (const char *hostname,
	      int port,
	      const char *vhost,
	      const char *exchange, const char *userid, const char *password)
{
  amqp_errors = false;

  struct timeval tv;
  gettimeofday (&tv, NULL);
  int now = tv.tv_sec;

  //init amqp_lastconnect
  if (amqp_lastconnect == 0)
    amqp_lastconnect = now - amqp_wait_time - 5;

  if (!amqp_connected && (amqp_lastconnect + amqp_wait_time) <= now)
    {

      amqp_lastconnect = now;

      logger (LG_INFO, "AMQP: Init connection");
      conn = amqp_new_connection ();

      logger (LG_INFO, "AMQP: Opening socket");
      on_error (sockfd = amqp_open_socket (hostname, port), "Opening socket");

      if (!amqp_errors)
	{
	  amqp_set_sockfd (conn, sockfd);

	  logger (LG_INFO, "AMQP: Logging");
	  on_amqp_error (amqp_login
			 (conn, vhost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
			  userid, password), "Logging in");
	}

      if (!amqp_errors)
	{
	  logger (LG_INFO, "AMQP: Open channel");
	  amqp_channel_open (conn, 1);
	  on_amqp_error (amqp_get_rpc_reply (conn), "Opening channel");
	}

      amqp_connected = false;
      if (!amqp_errors)
	{
	  logger (LG_INFO, "AMQP: Successfully connected");
	  amqp_connected = true;
	}

    }

}

void
amqp_disconnect ()
{
  if (amqp_connected)
    {
      logger (LG_INFO, "AMQP: Closing channel");
      on_amqp_error (amqp_channel_close (conn, 1, AMQP_REPLY_SUCCESS),
		     "Closing channel");

      logger (LG_INFO, "AMQP: Closing connection");
      on_amqp_error (amqp_connection_close (conn, AMQP_REPLY_SUCCESS),
		     "Closing connection");

      logger (LG_INFO, "AMQP: Ending connection");
      on_error (amqp_destroy_connection (conn), "Ending connection");

      amqp_connected = false;
      logger (LG_INFO, "AMQP: Successfully disconnected");
    }
  else
    {
      logger (LG_INFO, "AMQP: Impossible to disconnect, not connected");
    }
}

void
amqp_publish (const char *exchange,
	      const char *routingkey, const char *message)
{

  if (amqp_connected)
    {

      amqp_basic_properties_t props;
      props._flags =
	AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
      props.content_type = amqp_cstring_bytes ("text/plain");
      props.delivery_mode = 2;	/* persistent delivery mode */

      on_error (amqp_basic_publish (conn,
				    1,
				    amqp_cstring_bytes (exchange),
				    amqp_cstring_bytes (routingkey),
				    0,
				    0,
				    &props,
				    amqp_cstring_bytes (message)),
		"Publishing");


      if (amqp_errors)
	{
	  //TODO: re-queue event
	  logger (LG_INFO, "AMQP: Error on publish");
	  amqp_disconnect ();
	  logger (LG_INFO, "AMQP: Try to reconnect ...");
	  amqp_connect (hostname, port, virtual_host, exchange_name, userid,
			password);
	}
    }
  else
    {
      amqp_connect (hostname, port, virtual_host, exchange_name, userid,
		    password);
    }

}
