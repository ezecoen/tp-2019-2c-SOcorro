#ifndef hilolay_h__
#define hilolay_h__
#include "hilolay_alumnos.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <commons/string.h>


/* Interface for programs, this is what the programs should use and is implemented in hilolay_internal */

// TODO: Not working in program without struct, even though it has typedef
typedef struct hilolay_t {
	int tid;
} hilolay_t;

typedef struct hilolay_attr_t {
	int attrs;
} hilolay_attr_t;

typedef struct hilolay_sem_t {
	char *name;
} hilolay_sem_t;

typedef struct s_config{
	uint32_t puerto_suse;
	char* ip_suse;
}s_config;

typedef struct suse_wait_t{
	uint32_t tid;
	uint32_t size_id;
	char* id_semaforo;
}suse_wait_t;

typedef struct suse_signal_t{
	uint32_t tid;
	uint32_t size_id;
	char* id_semaforo;
}suse_signal_t;
//Enum de operaciones
typedef enum operaciones_t{
	INIT,
	CREATE,
	SCHEDULE_NEXT,
	JOIN,
	CLOSE,
	WAIT,
	SIGNAL
}operaciones_t;

//VARIABLES GLOBALES
s_config* configuracion;
t_config* g_config;
_Bool config_levantada = false;

int socket_suse;

//FUNCIONES
int suse_create(int tid);
int suse_schedule_next(void);
int suse_join(int tid);
int suse_close(int tid);
int suse_wait(int, char *);
int suse_signal(int, char *);

uint32_t conectar_socket_a(char* ip, uint32_t puerto);
s_config* leer_config(char* path);

/**
 * Initializes the library. Implemented in the client interface.
 *
 * Must call hilolay.c#_init function
 */
void hilolay_init(void);

/**
 * Starts an hilolay thread
 */
int hilolay_create(hilolay_t *thread, const hilolay_attr_t *attr, void *(*start_routine)(void *), void *arg);

/**
 * Forces a Thread scheduled swap
 */
int hilolay_yield(void);

/**
 * Joins the given `thread` to the current thread.
 */
int hilolay_join(hilolay_t *thread);

/**
 * Returns the tid of the current thread.
 */
int hilolay_get_tid(void);

/**
 * Initializes a named hilolay semaphore.
 */
hilolay_sem_t* hilolay_sem_open(char *name);

/**
 * Closes a named hilolay semaphore.
 */
int hilolay_sem_close(hilolay_sem_t*);

/**
 * Does a wait operation over a semaphore. It blocks the thread if no resources available.
 */
int hilolay_wait(hilolay_sem_t *sem);

/**
 * Does a signal operation over a semaphore.
 */
int hilolay_signal(hilolay_sem_t *sem);

/**
 * Returns and closes a thread
 * Note: Replaces a return statement on the original thread that called hilolay_init.
 */
int hilolay_return(int value);
#endif // hilolay_h__
