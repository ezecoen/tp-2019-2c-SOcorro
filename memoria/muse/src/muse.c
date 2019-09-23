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
	lugar_disponible = configuracion->tam_mem+configuracion->tam_swap;
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
	logg = log_create(nombre,"muse",1,LOG_LEVEL_TRACE);
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
	log_info(logg,"PAGE_SIZE​: %d",configuracion->tam_pag);

	configuracion->tam_swap = config_get_int_value(g_config,"SWAP_SIZE");
	log_info(logg,"SWAP_SIZE​: %d",configuracion->tam_swap);

	config_destroy(g_config);
	return configuracion;
}
int muse_alloc(muse_alloc_t* datos){
	//me fijo si hay lugar disponible
	if(lugar_disponible>=datos->tamanio){
		//busco si ya existe un segmento del que me pidio memoria
		segmento* segmento_buscado = buscar_segmento_por_id(datos->id);
		if(segmento_buscado==NULL){//hay q crear el segmento
			uint32_t cantidad_de_paginas = paginas_necesarias_para_tamanio(datos->tamanio);
			segmento* segmento_nuevo = malloc(sizeof(segmento));
			segmento_nuevo->compartido = false;
			segmento_nuevo->mmapeado = false;
			segmento_nuevo->nombre = string_duplicate(datos->id);
			segmento_nuevo->heap_metadatas = metadata_nuevo(cantidad_de_paginas);
			segmento_nuevo->paginas = reservar_paginas(cantidad_de_paginas);
			if(segmento_nuevo->paginas == NULL){
				return 0;
			}
		}
		else{//se encontro un segmento que pertenece a ese id...

		}
		//retorno la direccion de memoria (virtual) que le asigne
		return 1;
	}
	else{//no hay lugar
		return 0;
	}
}
segmento* buscar_segmento_por_id(char* id){
	_Bool segmento_igual(segmento* seg){
		return string_equals_ignore_case(seg->nombre,id);
	}
	return list_find(tabla_de_segmentos,(void*)segmento_igual);
}
t_list* metadata_nuevo(uint32_t cantidad_de_paginas){
	t_list* retorno = list_create();
	heap_metadata* heap = malloc(sizeof(heap_metadata));
	heap->isFree=true;
	heap->size = cantidad_de_paginas;
	list_add(retorno,heap);
	return retorno;
}
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio){
	uint32_t pags = tamanio/configuracion->tam_pag;
	if(tamanio%configuracion->tam_pag>0){
		pags++;
	}
	return pags;
}
t_list* reservar_paginas(uint32_t cantidad_de_paginas){
	t_list* paginas = list_create();
	if(lugar_disponible>=cantidad_de_paginas*configuracion->tam_pag){
		lugar_disponible-=cantidad_de_paginas*configuracion->tam_pag;
		//solo resto el tamaño, total las paginas solo se cargan en memoria cuando se necesiten
		for(int i = 0;i<cantidad_de_paginas;i++){
			pagina* pag = malloc(sizeof(pagina));
			pag->presencia = false;
			pag->modificado = false;
			pag->num_pag = i;
			pag->tamanio_en_uso = 0;
			pag->pedacito_de_memoria=NULL;
			list_add(paginas,pag);
		}
	}
	else{
		//se pudre to2 xd?
	}
	return paginas;
}
int muse_free(muse_free_t* datos){
	return 0;
}
int muse_get(muse_get_t* datos){
	return 0;
}
int muse_cpy(muse_cpy_t* datos){
	return 0;
}
int muse_map(muse_map_t* datos){
	return 0;
}
int muse_sync(muse_sync_t* datos){
	return 0;
}
int muse_unmap(muse_unmap_t* datos){
	return 0;
}
void esperar_conexion(uint32_t servidor){
	uint32_t socket = aceptar_cliente(servidor);
	pthread_t cliente;
	puts("creo hilo");
	pthread_create(&cliente,NULL,(void*)ocupate_de_este,(void*)socket);
	pthread_detach(cliente);
}
void ocupate_de_este(int socket){
	_Bool exit_loop = false;
	uint32_t operacion;
	while(recv(socket,&operacion,4,MSG_WAITALL)!=-1 && exit_loop==false){
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
				log_info(logg,"nuevo cliente, id: %s",id);
				mandar_char(id,socket,operacion);
				//hay q liberar lo de sockaddr_in?
				free(pid_char);
				free(id);
				break;
			case MUSE_ALLOC:;
				uint32_t tam;
				recv(socket,&tam,4,0);
				void* vmat = malloc(tam);
				recv(socket,vmat,tam,0);
				muse_alloc_t* datos = deserializar_muse_alloc(vmat);
				uint32_t resultado = muse_alloc(datos);
				send(socket,&resultado,4,0);
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
				//somethin
				exit_loop = true;
				break;
			default:
				//xd?
				break;
		}
	}
	//aca ya hizo muse_close
}


