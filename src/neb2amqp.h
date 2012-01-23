#ifndef _neb2amqp_h_
#define _neb2amqp_h_

#include <amqp.h>



void amqp_connect (const char *hostname,
           int port,
           const char *vhost,
           const char *exchange,
           const char *userid,
           const char *password);

void amqp_disconnect ();

void amqp_publish (const char *exchange,
                const char *routingkey,
                const char *message);


void on_error(int x, char const *context);

void on_amqp_error(amqp_rpc_reply_t x, char const *context);

#endif
