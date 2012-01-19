#ifndef _neb2amqp_h_
#define _neb2amqp_h_

void amqp_main (const char *hostname,
                int port,
                const char *vhost,
                const char *exchange,
                const char *routingkey,
                const char *userid,
                const char *password,
                const char *message);

#endif
