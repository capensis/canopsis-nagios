#ifndef events_h
#define events_h

int event_void(int event_type __attribute__ ((__unused__)), void *data);

int event_process(int event_type __attribute__ ((__unused__)), void *data);
int event_program_status(int event_type __attribute__ ((__unused__)), void *data);

int event_service_check(int event_type __attribute__ ((__unused__)), void *data);
int event_host_check(int event_type __attribute__ ((__unused__)), void *data);

int event_acknowledgement(int event_type __attribute__ ((__unused__)), void *data);
int event_downtime(int event_type __attribute__ ((__unused__)), void *data);
int event_comment(int event_type __attribute__ ((__unused__)), void *data);

#endif
