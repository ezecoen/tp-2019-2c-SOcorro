#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <../hilolay/hilolay.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>

/* ESTRUCTURAS -----------------------------------------------------------------------------------------------------*/
typedef struct{
	int p_id; //Process Id
	int t_id; //Thread id
	float estimacion; //La inicial siempre es 0
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
	uint32_t MAX_MULTIPROG;
	char SEM_IDS[2];
	uint32_t SEM_INIT[2];
	uint32_t ALPHA_SJF;
	uint32_t SEM_MAX[];
}p_config;

typedef enum{
	CREATE,
	SCHEDULE_NEXT,
	JOIN,
	CLOSE,
	WAIT,
	SIGNAL
};//Enum de operaciones para el Switch



/* PROCEDIMIENTOS -------------------------------------------------------------------------------------------------*/
//FUNCIONES PARA PRUEBAS LOCALES
void funcionDePrueba();
void printearNumeroDeHilo(tcb*);
void crearHilo(int numero);
int randomEntre1y10(int numero);


//FUNCIONES PARA INICIALIZAR COSAS
p_config* leer_config();
void iniciar_log();
void inicializarSemaforos();
void inicializarEstadosComunes();
void inicializarOtrasListas();


//COSAS PARA EL SERVIDOR
int crearServidor();
void escucharServidor(int);
void ocupateDeEste(uint32_t, uint32_t);
int aceptarConexion(int);


//FUNCIONES PARA HACER LO QUE ME PIDA HILOLAY, O SEA, LA IMPLEMENTACION DE SUSE
void planificarNuevoProceso(uint32_t pid, uint32_t tid)
pcb* crearPCB();
void crearTCB(uint32_t, uint32_t);


//OTRAS FUNCIONES



/* VARIABLES GLOBALES ---------------------------------------------------------------------------------------------*/
t_list* ultsAPlanificar;
t_list* listaDeProgramas; //Lista de programas ejecutando/a ejecutar
t_log* log;
t_config* g_config;
p_config* configuracion;
uint32_t numeroDePrograma; //vendria a ser como el "pid" de cada programa/proceso
uint32_t multiprogramacion;



/* SEMAFOROS ------------------------------------------------------------------------------------------------------*/
sem_t sem_mutConfig;
sem_t sem_mutNew;
sem_t mut_multiprogramacion;
sem_t mut_numeroDePrograma;



/* COSAS PARA EL PLANIFICADOR -------------------------------------------------------------------------------------*/
t_list* estadoNew;
t_list* estadoExit;
t_list* estadoBlocked;
