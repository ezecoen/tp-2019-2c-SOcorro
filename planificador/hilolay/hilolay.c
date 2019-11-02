#include <hilolay/alumnos.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Lib implementation: It'll only schedule the last thread that was created */
int max_tid = 0;

int suse_create(int tid){

}

int suse_schedule_next(void){

}

int suse_join(int tid){

}

int suse_close(int tid){

}

int suse_wait(int tid, char *sem_name){

}

int suse_signal(int tid, char *sem_name){

}

static struct hilolay_operations hiloops = {
		.suse_create = &suse_create,
		.suse_schedule_next = &suse_schedule_next,
		.suse_join = &suse_join,
		.suse_close = &suse_close,
		.suse_wait = &suse_wait,
		.suse_signal = &suse_signal
};

void hilolay_init(void){
	init_internal(&hiloops);
}
