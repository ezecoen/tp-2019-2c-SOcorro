#include "hilolay.h"
/* Lib implementation: It'll only schedule the last thread that was created */
int max_tid = 0;

int suse_create(int tid){
	int op = CREATE;
	void* mensaje = malloc(8);
	memcpy(mensaje,&op,4);
	memcpy(mensaje,&tid,4);
	send(socket_suse,mensaje,8,0);
	int resultado;
	recv(socket_suse,&resultado,4,0);
	return resultado;

	//Aca voy le voy a decir a suse que cree la estructura de un proceso nuevo y ademas cree la estructura de un hilo
	//Despues en suse, le tengo que agregar el hilo que cree a la lista de threads del proceso (ademas de mandarlo a la cola del planificador
	//que corresponda
	//Cada proceso va a tener: un PID, una lista de hilos y un atributo que me diga si tiene o no algun hilo ejecutando
	//Cada ult va a tener: un TID, la estimacion (que al principio siempre es 0), estimacion de proxima rafaga, ejecucion real anterior
}

int suse_schedule_next(void){
	//Aca es donde basicamente le digo a suse que corra el algoritmo SJF
	int op = SCHEDULE_NEXT;
	send(socket_suse,&op,4,0);
	int tid_return;
	recv(socket_suse,&tid_return,4,0);
	return tid_return;
}

int suse_join(int tid){
	int tid_actual = hilolay_get_tid();
	int op = JOIN;
	void* paquete = malloc(12);
	memcpy(paquete,&op,4);
	memcpy(paquete+4,&tid_actual,4);
	memcpy(paquete+8,&tid,4);
	send(socket_suse,paquete,12,0);
	int resultado;
	recv(socket_suse,&resultado,4,0);
	return resultado;
}

int suse_close(int tid){
	//Llaman a esta funcion cuando termina de ejecutar un hilo, de aca saco la info para el algoritmo SJF
	return 0;
}

int suse_wait(int tid, char *sem_name){
	//aca le digo a suse que decremente el valor de este semaforo, y si es menor a 0 tengo que hacer que bloquee el TID
	return 0;
}

int suse_signal(int tid, char *sem_name){
	//lo mismo que el wait, aumentando obviamente
	return 0;
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
		send(socket_suse,mensaje,8,0);
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

suse_wait_t* crear_suse_wait(int tid, char* id_semaforo){
	suse_wait_t* swt = malloc(sizeof(suse_wait_t));
	swt->tid = tid;
	swt->size_id = strlen(id_semaforo)+1;
	swt->id_semaforo = string_duplicate(id_semaforo);
	return swt;
}

void* serializar_suse_wait(suse_wait_t* swt){
	int bytes = sizeof(uint32_t)*2+ swt->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de swt y 2 int de comando y tamanio
	int comando = WAIT;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&swt->tid,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&swt->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,swt->id_semaforo,swt->size_id);
	puntero += swt->size_id;
	return magic;
}
suse_wait_t* deserializar_suse_wait(void* magic){
	suse_wait_t* mmt = malloc(sizeof(suse_wait_t));
	uint32_t puntero = 0;
	memcpy(&mmt->tid,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mmt->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mmt->id_semaforo = malloc(mmt->size_id);
	memcpy(mmt->id_semaforo,magic+puntero,mmt->size_id);
	puntero+=mmt->size_id;
	return mmt;
}
void suse_wait_destroy(suse_wait_t* swt){
	free(swt->id_semaforo);
	free(swt);
}

suse_signal_t* crear_suse_signal(int tid, char* id_semaforo){
	suse_signal_t* sst = malloc(sizeof(suse_signal_t));
	sst->tid = tid;
	sst->size_id = strlen(id_semaforo)+1;
	sst->id_semaforo = string_duplicate(id_semaforo);
	return sst;
}
void* serializar_suse_signal(suse_signal_t* swt){
	int bytes = sizeof(uint32_t)*2+ swt->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de swt y 2 int de comando y tamanio
	int comando = SIGNAL;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&swt->tid,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&swt->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,swt->id_semaforo,swt->size_id);
	puntero += swt->size_id;
	return magic;
}
suse_signal_t* deserializar_suse_signal(void* magic){
	suse_signal_t* mmt = malloc(sizeof(suse_signal_t));
	uint32_t puntero = 0;
	memcpy(&mmt->tid,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mmt->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mmt->id_semaforo = malloc(mmt->size_id);
	memcpy(mmt->id_semaforo,magic+puntero,mmt->size_id);
	puntero+=mmt->size_id;
	return mmt;
}
void suse_signal_destroy(suse_signal_t* swt){
	free(swt->id_semaforo);
	free(swt);
}

