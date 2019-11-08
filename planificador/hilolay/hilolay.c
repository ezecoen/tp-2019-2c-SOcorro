#include "hilolay.h"
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
	//Aca tengo que decirle a suse que inicialice las cosas (estados del planificador, semaforos, etc)
	//tambien hay que hacer el connect con suse
	init_internal(&hiloops);
	//se fija si ya se levanto la config, sino la levanta
	//por ahora el path esta hardcodeado
	leer_config("/home/utnso/tp-2019-2c-SOcorro/planificador/hilolay/Debug/hilolay.config");
	int sock = conectar_socket_a(configuracion->ip_suse,configuracion->puerto_suse);
	if(sock > 0){
		socket_suse = sock;
		//envio el init a suse
		void* mensaje = malloc(8);
		int op = INIT;
		int pid = getpid();
		memcpy(mensaje,&op,4);
		memcpy(mensaje+4,&pid,4);
	}
	else{
		perror("error en la conexion: ");
	}
}
uint32_t conectar_socket_a(char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM,0);
	if (connect(cliente,(void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		printf("Error al conectar a ip %s y puerto %d\n",ip,puerto);
		return -1;
	}
	return cliente;
}
s_config* leer_config(char* path){
	if(config_levantada){
		return configuracion;
	}
	g_config = config_create(path);
	configuracion = malloc(sizeof(s_config));

	configuracion->puerto_suse = config_get_int_value(g_config,"PUERTO_SUSE");
	configuracion->ip_suse = string_duplicate(config_get_string_value(g_config,"IP_SUSE"));
	puts("config de hilolay levantada");

	config_destroy(g_config);
	return configuracion;
}

