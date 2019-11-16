#ifndef HILOLAYEXTENDED_H_
#define HILOLAYEXTENDED_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <commons/config.h>
#include <commons/string.h>
#include <hilolay/alumnos.h>

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
int socket_suse;
_Bool config_levantada = false;

void hilolay_init();
int suse_create(int tid);
int suse_join(int tid);
int suse_schedule_next(void);
int suse_close(int);
int suse_wait(int, char *);
int suse_signal(int, char *);

uint32_t conectar_socket_a(char* ip, uint32_t puerto);
s_config* leer_config(char* path);

suse_signal_t* crear_suse_signal(int tid, char* id_semaforo);
void* serializar_suse_signal(suse_signal_t* swt);
void* serializar_suse_wait(suse_wait_t* swt);
suse_wait_t* crear_suse_wait(int tid, char* id_semaforo);

#endif /* HILOLAYEXTENDED_H_ */
