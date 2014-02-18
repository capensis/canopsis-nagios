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

#ifndef __CANONEB_EVENTS_H
#define __CANONEB_EVENTS_H

int n2a_event_void (int event_type __attribute__ ((__unused__)), void *data);

int n2a_event_process (int event_type __attribute__ ((__unused__)), void *data);
int n2a_event_program_status (int event_type __attribute__ ((__unused__)), void *data);

int n2a_event_service_check (int event_type __attribute__ ((__unused__)), void *data);
int n2a_event_host_check (int event_type __attribute__ ((__unused__)), void *data);

int n2a_event_acknowledgement (int event_type __attribute__ ((__unused__)), void *data);
int n2a_event_downtime (int event_type __attribute__ ((__unused__)), void *data);
int n2a_event_comment (int event_type __attribute__ ((__unused__)), void *data);

#endif /* __CANONEB_EVENTS_H */
