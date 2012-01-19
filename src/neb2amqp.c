#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>

#include <amqp.h>
#include <amqp_framing.h>

void
amqp_main (const char *hostname,
           int port,
           const char *vhost,
           const char *exchange,
           const char *routingkey,
           const char *userid,
           const char *password,
           const char *message)
{
    int sockfd;

    amqp_connection_state_t conn;

    conn = amqp_new_connection();

    die_on_error(sockfd = amqp_open_socket(hostname, port), "Opening socket");
    amqp_set_sockfd(conn, sockfd);
    die_on_amqp_error(amqp_login(conn, vhost, 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, userid, password),
            "Logging in");
    amqp_channel_open(conn, 1); 
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

    {
        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
        props.content_type = amqp_cstring_bytes("text/plain");
        props.delivery_mode = 2; /* persistent delivery mode */
        die_on_error(amqp_basic_publish(conn,
                    1,  
                    amqp_cstring_bytes(exchange),
                    amqp_cstring_bytes(routingkey),
                    0,  
                    0,  
                    &props,
                    amqp_cstring_bytes(message)),
         "Publishing");
    }

    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");
}
