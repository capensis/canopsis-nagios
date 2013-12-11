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

#ifndef __CANODEB_NEB2AMQP_H
#define __CANODEB_NEB2AMQP_H

#include <amqp.h>
#include <stdbool.h>

#define AMQP_MSG_SIZE_MAX 8192

bool n2a_amqp_connect (void);
void n2a_amqp_disconnect (void);
bool n2a_amqp_publish (const char *routingkey, const char *message);
int n2a_send_event (const char *routingkey, const char *message);

void n2a_on_error (int x, char const *context);
void n2a_on_amqp_error (amqp_rpc_reply_t x, char const *context);

bool n2a_toggle_blackout (void);

#endif /* __CANODEB_NEB2AMQP_H */
