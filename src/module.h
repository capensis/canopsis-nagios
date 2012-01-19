#ifndef module_h
#define module_h

#define FALSE 0
#define TRUE 1

int nebmodule_init(int flags __attribute__ ((__unused__)), char *args, void *handle);
void terminate_threads();
void start_threads();
int nebmodule_deinit(int flags __attribute__ ((__unused__)), int reason __attribute__ ((__unused__)));
void *main_thread(void *data __attribute__ ((__unused__)));

#endif
