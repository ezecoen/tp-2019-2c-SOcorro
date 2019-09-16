/*
 * muse.h
 *
 *  Created on: 16 sep. 2019
 *      Author: utnso
 */

#ifndef MUSE_H_
#define MUSE_H_
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

//VARIABLES GLOBALES
void* malloc_gigante;
t_list* tabla_de_segmentos;


//SEMAFOROS


//ESTRUCTURAS
typedef struct{
	char* nombre;
	_Bool mmapeado;
	_Bool compartido;
	t_list* heap_metadatas;
	t_list* paginas;
}segmento;

typedef struct{
	uint32_t size;
	_Bool isFree;
}heap_metadata;

typedef struct{
	uint32_t num_pag;
	uint32_t tamaño_en_uso;
	_Bool presencia;
	_Bool modificado;
	void* pedacito_de_memoria;
}pagina;


//FUNCIONES


#endif /* MUSE_H_ */
