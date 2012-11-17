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

#ifndef module_h
#define module_h

#include "nagios.h"

#define FALSE 0
#define TRUE 1

int nebmodule_init(int flags __attribute__ ((__unused__)), char *args, nebmodule *handle);
int nebmodule_deinit(int flags __attribute__ ((__unused__)), int reason __attribute__ ((__unused__)));

struct options {
	char *hostname;
	int port;
    int max_size;
	char *userid;
	char *password;
	char *virtual_host;
	char *exchange_name;
	char *connector;
	char *eventsource_name;

	int log_level;
	
	nebmodule * nagios_handle;
};


#endif
