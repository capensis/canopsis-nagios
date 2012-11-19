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

/* this functions clears the neb cache */
void n2a_clear_cache (void);

/* this functions initialize the neb cache */
void n2a_init_cache (void);

/**
 * this function flushes the cache into a file on the disk.
 * this flush happens when 'autoflush' delays since last flush is elapsed unless
 * 'force' is set to TRUE while calling the function.
 * note: if 'autoflush' < 0 the automatic flush is disabled unless 'force' is
 * set to TRUE. if 'autoflush' == 0 the automatic flush is done at every call.
 * @param force: set to TRUE if you want to override the autoflush timeout
 */
void n2a_flush_cache (unsigned int force);

/**
 * this function stores the key and the message into the dictionary.
 * this adds 2 new keys into the dictionary.
 * @param key: routing key of the amqp message
 * @param message: amqp message
 */
void n2a_record_cache (const char *key, const char *message);

/**
 * this function depiles the messages already stored in memory and resent them
 * to the AMQP bus.
 * note: when one send fails, we stop the depiling process until next time...
 */
void n2a_pop_all_cache (void); 
