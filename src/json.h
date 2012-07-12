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

#ifndef json_h
#define json_h

void nebstruct_service_check_data_to_json(char ** buffer, nebstruct_service_check_data *c);
void nebstruct_host_check_data_to_json(char ** buffer, nebstruct_host_check_data *c);

//void nebstruct_program_status_data_to_json(char * buffer, nebstruct_program_status_data *c);
//void nebstruct_acknowledgement_data_to_json(char * buffer, nebstruct_acknowledgement_data *c);
//void nebstruct_downtime_data_to_json(char * buffer, nebstruct_downtime_data *c);
//void nebstruct_comment_data_to_json(char * buffer, nebstruct_comment_data *c);

#endif
