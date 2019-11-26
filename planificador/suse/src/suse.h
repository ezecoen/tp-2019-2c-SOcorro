#ifndef SUSE_H_
#define SUSE_H_
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <sys/time.h>


/* ESTRUCTURAS -----------------------------------------------------------------------------------------------------*/
typedef struct{
	int p_id; //Process Id
	int t_id; //Thread id
	uint64_t tiempoDeInicio;
	double estimacion; //La inicial siempre es 0
	double estimacionAnterior;
	double realAnterior;
	t_list* tidsEsperando;
}tcb;

typedef struct{
	t_list* ults;
	int p_id;
	_Bool ejecutando; //Me sirve para saber si este hilo se esta ejecutando o no
}pcb;

typedef struct{
	t_list* estadoReady;
	tcb estadoExecute;
}programa;

typedef struct{
	uint32_t LISTEN_PORT;
	uint32_t METRICS_TIMER;
	int MAX_MULTIPROG;
	t_list* SEM_IDS;
	t_list* SEM_INIT;
	double ALPHA_SJF;
	t_list* SEM_MAX;
}p_config;

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

typedef struct{
	uint32_t idNumerico;//Lo lleno con el indice que se utiliza en la config
	char* id_semaforo;//Es el id que viene por la configuracion
	int valorInicial;//Este es el valor que voy a restar/sumar cuando haga un
	uint32_t valorMaximo;
	t_list* colaDeBloqueo;//Es una lista que sirve para ver que procesos se bloquearon por querer waiter este semaforo
}semaforo_t;

typedef enum operaciones_t{
	INIT,
	CREATE,
	SCHEDULE_NEXT,
	JOIN,
	CLOSE,
	WAIT,
	SIGNAL
}operaciones_t;//Enum de operaciones para el Switch



/* PROCEDIMIENTOS -------------------------------------------------------------------------------------------------*/
//FUNCIONES PARA PRUEBAS LOCALES
void funcionDePrueba();
void printearNumeroDeHilo(tcb*);
void crearHilo(int numero);
int randomEntre1y10(int numero);


//FUNCIONES PARA INICIALIZAR COSAS
p_config* leer_config(char* path);
void iniciar_log(char* path);
void inicializarSemaforos();
void inicializarEstadosComunes();
void inicializarOtrasListas();


//COSAS PARA EL SERVIDOR
int crearServidor();
void escucharServidor(int);
void ocupateDeEste(uint32_t cliente);
int aceptarConexion(int);


//FUNCIONES PARA HACER LO QUE ME PIDA HILOLAY, O SEA, LA IMPLEMENTACION DE SUSE
pcb* crearPCB(int pid);
tcb* crearTCB(uint32_t, uint32_t);
pcb* buscarProcesoEnListaDeProcesos(int pid);
void sacarDeNew(tcb* _tcb);
void sacarDeReady(tcb* _tcb,t_list* colaReady);
uint64_t timestamp();
void actualizarEstimacion(tcb* _tcb,uint64_t tiempoFin);
suse_signal_t* deserializar_suse_signal(void* magic);
suse_wait_t* deserializar_suse_wait(void* magic);

//OTRAS FUNCIONES
void mostrarMetricasDelSistema();


/* VARIABLES GLOBALES ---------------------------------------------------------------------------------------------*/
t_list* ultsAPlanificar;
t_list* listaDeProgramas; //Lista de programas ejecutando/a ejecutar
t_list* listaDeSemaforos;
t_log* logg;
t_log* metricas;
t_config* g_config;
p_config* configuracion;



/* SEMAFOROS ------------------------------------------------------------------------------------------------------*/
sem_t sem_mutConfig;
sem_t mut_new;
sem_t mut_multiprogramacion;
sem_t mut_numeroDePrograma;
sem_t mut_listaDeSemaforos;
sem_t mut_blocked;
sem_t mut_exit;


/* COSAS PARA EL PLANIFICADOR -------------------------------------------------------------------------------------*/
t_list* estadoNew;
t_list* estadoExit;
t_list* estadoBlocked;
#endif /* SUSE_H_ */
