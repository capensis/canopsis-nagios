#include <stdio.h>
#include <stdlib.h>
#include <ltdl.h>

#include "test.h"
#include "nagios.h"

extern int event_broker_options;

int main (void)
{
	void *handle;
	int result = 0;

	printf("Init LTDL\n");
	result = lt_dlinit();
	if(result)
		return ERROR;
	printf(" + Ok\n");

	printf("Load module ...\n");
	//handle = lt_dlopen("/home/william/Projets/ndoutils-1.5.2/src/ndomod-3x.o");
	handle = lt_dlopen("/home/wpain/Bureau/gittmp/neb2amqp/src/neb2amqp.o");

	if (!handle) {
		printf(" + Error: %s\n", lt_dlerror());
		return ERROR;
    	}
	printf(" + Ok\n");


	printf("Unload module\n");
	lt_dlclose(handle);
	if(result)
		return ERROR;
	printf(" + Ok\n");

	printf("Deinit LTDL\n");
	result = lt_dlexit();
	if(result)
		return ERROR;

	printf(" + Ok\n");

	printf("Bye Bye\n");
	return 0;
}
