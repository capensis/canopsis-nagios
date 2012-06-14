#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "test.h"
#include "nagios.h"

int event_broker_options = -1;

int main (void)
{
	void *handle;
    char *error = NULL;

	printf("Load module ...\n");
	//handle = lt_dlopen("/home/william/Projets/ndoutils-1.5.2/src/ndomod-3x.o");
	handle = dlopen("../src/neb2amqp.o", RTLD_LAZY);
    error = dlerror ();
    if (error != NULL) {
        printf (" + Error: %s\n", error);
        return ERROR;
    }

	printf(" + Ok\n");


	printf("Unload module\n");
	dlclose(handle);

    error = dlerror ();
    if (error != NULL) {
        printf (" + Error: %s\n", error);
        return ERROR;
    }
		
	printf(" + Ok\n");

	printf("Bye Bye\n");
	return 0;
}
