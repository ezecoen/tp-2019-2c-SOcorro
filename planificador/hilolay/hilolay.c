#include <hilolay/alumnos.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Lib implementation: It'll only schedule the last thread that was created */
int max_tid = 0;

int suse_create(int tid){
	//Aca voy le voy a decir a suse que cree la estructura de un proceso nuevo y ademas cree la estructura de un hilo
	//Despues en suse, le tengo que agregar el hilo que cree a la lista de threads del proceso (ademas de mandarlo a la cola del planificador
	//que corresponda
	//Cada proceso va a tener: un PID, una lista de hilos y un atributo que me diga si tiene o no algun hilo ejecutando
	//Cada ult va a tener: un TID, la estimacion (que al principio siempre es 0), estimacion de proxima rafaga, ejecucion real anterior
}

int suse_schedule_next(void){
	//Aca es donde basicamente le digo a suse que corra el algoritmo SJF
}

int suse_join(int tid){
	//TODO:Preguntar que hace esto?????
}

int suse_close(int tid){
	//Llaman a esta funcion cuando termina de ejecutar un hilo, de aca saco la info para el algoritmo SJF
}

int suse_wait(int tid, char *sem_name){
	//aca le digo a suse que decremente el valor de este semaforo, y si es menor a 0 tengo que hacer que bloquee el TID
}

int suse_signal(int tid, char *sem_name){
	//lo mismo que el wait, aumentando obviamente
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

	//Aca tengo que decirle a suse que inicialice las cosas (estados del planificador, semaforos, etc)

}
