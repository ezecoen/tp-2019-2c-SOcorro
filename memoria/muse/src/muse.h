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
#include <libreriaComun/libreriaComun.h>
#include <commons/config.h>
#include <commons/log.h>

//VARIABLES GLOBALES
uint32_t tam_mem;
void* upcm;
t_list* tabla_de_segmentos;

typedef struct{
	uint32_t puerto;
	uint32_t tam_mem;
	uint32_t tam_pag;
	uint32_t tam_swap;
}s_config;
s_config* configuracion;
t_config* g_config;
t_log* logg;

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
	uint32_t tamanio_en_uso;
	_Bool presencia;
	_Bool modificado;
	void* pedacito_de_memoria;
}pagina;

typedef enum{
	MUSE_INIT=0,
	MUSE_ALLOC=1,
	MUSE_FREE=2,
	MUSE_GET=3,
	MUSE_CPY=4,
	MUSE_MAP=5,
	MUSE_SYNC=6,
	MUSE_UNMAP=7,
	MUSE_CLOSE=8,
	MUSE_ERROR=9,
	MUSE_EXITOSO=10
}t_comando_muse;

//FUNCIONES
void init_estructuras();
void free_final();
void iniciar_log();
s_config* leer_config();
void muse_alloc(uint32_t tamanio);
void ocupate_de_esta(int socket);
void esperar_conexion(uint32_t servidor);

#endif /* MUSE_H_ */
