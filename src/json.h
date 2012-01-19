#ifndef json_h
#define json_h

void nebstruct_program_status_data_to_json(char * buffer, nebstruct_program_status_data *c);
void nebstruct_service_check_data_to_json(char * buffer, nebstruct_service_check_data *c);
void nebstruct_host_check_data_to_json(char * buffer, nebstruct_host_check_data *c);
void nebstruct_acknowledgement_data_to_json(char * buffer, nebstruct_acknowledgement_data *c);
void nebstruct_downtime_data_to_json(char * buffer, nebstruct_downtime_data *c);
void nebstruct_comment_data_to_json(char * buffer, nebstruct_comment_data *c);

#endif
