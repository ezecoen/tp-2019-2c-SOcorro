#include <stdio.h>
#include <stdlib.h>
#include "muse.h"
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>

int main(int argc, char **argv) {
//	INICIANDO
	path_de_config = string_duplicate(argv[1]);
	iniciar_log(path_de_config);
	leer_config(path_de_config);
	init_estructuras();

//	//aver creamo un segmento xd
//	segmento* seg = malloc(sizeof(segmento));
//	seg->compartido = 0;
//	seg->mmapeado = 0;
//	seg->nombre = string_new();
//	string_append(&seg->nombre,"segmento 1");
//	t_list* paginas = list_create();
//	pagina* pag = malloc(sizeof(pagina));
//	pag->modificado = 0;
//	pag->num_pag = 0;
//	pag->presencia = 1;
//	pag->tamanio_en_uso = 0;
//	pag->pedacito_de_memoria = upcm;
//	list_add(paginas,pag);
//	t_list* heap = list_create();
//	heap_metadata* heap_m = malloc(sizeof(heap_metadata));
//	heap_m->isFree = true;
//	heap_m->size = 100;
//	list_add(heap,heap_m);
//	seg->heap_metadatas = heap;
//	seg->paginas = paginas;

//	SERVIDOR
	uint32_t servidor = crear_servidor(configuracion->puerto);
	while(1){
		esperar_conexion(servidor);
	}

	free_final();
	return EXIT_SUCCESS;
}
void init_estructuras(){
	upcm = malloc(configuracion->tam_mem);
	tabla_de_segmentos = list_create();
}
void free_final(){
	free(upcm);
	free(path_de_config);
	//free tabla de segmentos
}
void iniciar_log(char* path){//0 es archivo, 1 es consola
	char* nombre = string_new();
	string_append(&nombre,path);
	string_append(&nombre,".log");
	logg = log_create(nombre,"muse",0,LOG_LEVEL_TRACE);
	free(nombre);
}
s_config* leer_config(char* path){
	g_config = config_create(path);
	configuracion = malloc(sizeof(s_config));

	configuracion->puerto = config_get_int_value(g_config,"LISTEN_PORT");
	log_info(logg,"LISTEN_PORT: %d",configuracion->puerto);

	configuracion->tam_mem = config_get_int_value(g_config,"MEMORY_SIZE");
	log_info(logg,"MEMORY_SIZE: %d",configuracion->tam_mem);

	configuracion->tam_pag = config_get_int_value(g_config,"PAGE_SIZE");
	log_info(logg,"	PAGE_SIZE​: %d",configuracion->tam_pag);

	configuracion->tam_swap = config_get_int_value(g_config,"SWAP_SIZE");
	log_info(logg,"SWAP_SIZE​: %d",configuracion->tam_swap);

	config_destroy(g_config);
	return configuracion;
}
void muse_alloc(uint32_t tamanio){
	//me fijo si hay lugar disponible
	//busco si ya existe un segmento del que me pidio memoria
	//si no existe creo el segmento
	//agrego las paginas necesarias
}
void esperar_conexion(uint32_t servidor){
	uint32_t socket = aceptar_cliente(servidor);
	pthread_t cliente;
	puts("creo hilo");
	pthread_create(&cliente,NULL,(void*)ocupate_de_esta,(void*)socket);
	pthread_detach(cliente);
}

void ocupate_de_esta(int socket){
	uint32_t operacion;
	recv(socket,&operacion,4,0);
	switch (operacion) {
		case MUSE_INIT:;
			//recibo int pid, mando char* id
			uint32_t pid;
			recv(socket,&pid,4,0);
			char* pid_char = string_itoa(pid);
			//hay q conseguir la ip de el cliente xd
			char* id = string_new();
			struct sockaddr_in addr;
			uint32_t addrlen = sizeof(addr);
			getpeername(socket, (struct sockaddr *)&addr, &addrlen);
			char* ip = inet_ntoa(addr.sin_addr);
			string_append(&id,ip);
			string_append(&id,"-");
			string_append(&id,pid_char);
			//mando el id :)
			mandar_char(id,socket,operacion);
			//hay q liberar lo de sockaddr_in?
			free(pid_char);
			free(id);
			break;
		case MUSE_ALLOC:

			break;
		case MUSE_FREE:

			break;
		case MUSE_GET:

			break;
		case MUSE_CPY:

			break;
		case MUSE_MAP:

			break;
		case MUSE_SYNC:

			break;
		case MUSE_UNMAP:

			break;
		case MUSE_CLOSE:

			break;
		default:
			//xd?
			break;
	}
}


