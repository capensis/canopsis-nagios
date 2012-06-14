#include <stdio.h>
#include <stdlib.h>
#include <ltdl.h>
#include <unistd.h>
#include <sys/time.h>

#include "nagios.h"

int event_broker_options = -1;

int (*service_check_data)(int,void *);
int (*host_check_data)(int,void *);

int write_to_all_logs(char * msg,unsigned long priority){
	printf("log: %s\n", msg);
	return 0;
}

int neb_register_callback(int callback_type, void *mod_handle, int priority, int (*callback_func)(int,void *)){
	if 			(callback_type == NEBCALLBACK_SERVICE_CHECK_DATA){
		service_check_data = callback_func;
		
	}else if	(callback_type == NEBCALLBACK_HOST_CHECK_DATA){
		host_check_data = callback_func;
	}
	return 0;
}

int neb_deregister_callback(int callback_type, int (*callback_func)(int,void *)){
	return 0;
}

nebstruct_service_check_data build_service_check(char * host_name, char * description, char * plugin_output){
	nebstruct_service_check_data ds;
	
	struct timeval tv;
	gettimeofday(&tv,NULL);

	host host_object;
	host_object.address="127.0.0.1";
	
	service service_object;
	service_object.host_ptr = &host_object;

	ds.type=NEBTYPE_SERVICECHECK_PROCESSED;
	ds.flags=0;
	ds.attr=0;
	ds.timestamp=tv;

	ds.host_name=host_name;
	ds.service_description=description;
	
	ds.object_ptr=&service_object;
	
	ds.check_type=0;
	ds.current_attempt=1;
	ds.max_attempts=5;
	ds.state=0;
	ds.state_type=1;
	ds.timeout=0;
	ds.command_name="check_debug";
	ds.command_args="";
	ds.command_line="";
	ds.start_time=tv;
	ds.end_time=tv;
	ds.early_timeout=0;
	ds.execution_time=0.23;
	ds.latency=0.55;
	ds.return_code=0;
	ds.output=plugin_output;
	ds.long_output="";
	ds.perf_data="";

	return ds;
}

int main (void)
{
	int result = 0;

	char * args = "name=Debug 127.0.0.1";
	char * filename = "/home/wpain/Bureau/gittmp/neb2amqp/src/neb2amqp.o";
	int should_be_loaded = 1;
	
	nebmodule *module=NULL;
	module=(nebmodule *)malloc(sizeof(nebmodule));
	
	printf("Init nebmodule struct\n");
	
	module->filename=(char *)strdup(filename);
	module->args=(args==NULL)?NULL:(char *)strdup(args);
	module->should_be_loaded=should_be_loaded;
	module->is_currently_loaded=FALSE;
	module->module_handle=NULL;
	module->init_func=NULL;
	module->deinit_func=NULL;

	printf("Init LTDL\n");
	result = lt_dlinit();
	if(result)
		return ERROR;
	printf(" + Ok\n");

	printf("Load module ...\n");
	//handle = lt_dlopen("/home/william/Projets/ndoutils-1.5.2/src/ndomod-3x.o");
	module->module_handle = lt_dlopen(filename);

	if (!module->module_handle) {
		printf(" + Error: %s\n", lt_dlerror());
		return ERROR;
    	}
	printf(" + Ok\n");
	
	printf("Load init function...\n");
	module->init_func =   lt_dlsym(module->module_handle,"nebmodule_init");
	if(! module->init_func){
		printf(" + Error");
		return ERROR;
	}
	printf(" + Ok\n");
	
	printf("Load deinit function...\n");
	module->deinit_func = lt_dlsym(module->module_handle,"nebmodule_deinit");
	if(! module->deinit_func){
		printf(" + Error");
		return ERROR;
	}
	printf(" + Ok\n");


	printf("\nInit NEB ...\n");
	int (*initfunc)(int,char *,void *);
	initfunc=module->init_func;
	result=(*initfunc)(NEBMODULE_NORMAL_LOAD, module->args, module->module_handle);

	sleep(1);
	/// Do it !
	printf("\nPlay with NEB ...\n");
	
	nebstruct_service_check_data check1;
	
	check1 = build_service_check("host1", "service1", "mon output1");
	service_check_data(NEBCALLBACK_SERVICE_CHECK_DATA, (void *)&check1);
	
	/// End !
	sleep(1);
	
	printf("\ndeInit NEB ...\n");
	int (*deinitfunc)(int, int);
	deinitfunc=module->deinit_func;
	result=(*deinitfunc)(0, NEBMODULE_FORCE_UNLOAD);

	printf("Unload module\n");
	result = lt_dlclose(module->module_handle);
	if(result)
		return ERROR;
	printf(" + Ok\n");

	printf("Deinit LTDL\n");
	result = lt_dlexit();
	if(result)
		return ERROR;

	printf(" + Ok\n");

	printf("Free ...\n");
	free(module->filename);
	free(module->args);
	free(module);
	printf(" + Ok\n");
	
	printf("Bye Bye\n");
	return 0;
}

/*
==24438== HEAP SUMMARY:
==24438==     in use at exit: 5,278 bytes in 20 blocks
==24438==   total heap usage: 75 allocs, 55 frees, 928,637 bytes allocated
==24438== 
==24438== 21 bytes in 1 blocks are definitely lost in loss record 1 of 8
==24438==    at 0x4026864: malloc (vg_replace_malloc.c:236)
==24438==    by 0x40C5D1F: strdup (strdup.c:43)
==24438==    by 0x45B8BC6: ???
==24438==    by 0x45B901F: ???
==24438==    by 0x8048CCF: main (test.c:134)
==24438== 
==24414== LEAK SUMMARY:
==24414==    definitely lost: 21 bytes in 1 blocks
==24414==    indirectly lost: 0 bytes in 0 blocks
==24414==      possibly lost: 0 bytes in 0 blocks
==24414==    still reachable: 5,257 bytes in 19 blocks
==24414==         suppressed: 0 bytes in 0 blocks
*/
