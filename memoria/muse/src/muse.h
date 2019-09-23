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

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t tamanio;
}muse_alloc_t;

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
}muse_free_t;

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
	uint32_t tamanio;
}muse_get_t;

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
	uint32_t size_paquete;
	void* paquete;
}muse_cpy_t;

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t size_path;
	char* path;
	uint32_t tamanio;
	uint32_t flag;
}muse_map_t;

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
	uint32_t tamanio;
}muse_sync_t;

typedef struct{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
}muse_unmap_t;

typedef struct{
	uint32_t puerto;
	uint32_t tam_mem;
	uint32_t tam_pag;
	uint32_t tam_swap;
}s_config;

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

//VARIABLES GLOBALES
char* path_de_config;
void* upcm;
uint32_t lugar_disponible;
t_list* tabla_de_segmentos;

s_config* configuracion;
t_config* g_config;
t_log* logg;

//SEMAFOROS


//FUNCIONES
void init_estructuras();
void free_final();
void iniciar_log(char* path);
s_config* leer_config(char* path);
int muse_alloc(muse_alloc_t* datos);
segmento* buscar_segmento_por_id(char* id);
t_list* metadata_nuevo(uint32_t cantidad_de_paginas);
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio);
t_list* reservar_paginas(uint32_t cantidad_de_paginas);
int muse_free(muse_free_t* datos);
int muse_get(muse_get_t* datos);
int muse_cpy(muse_cpy_t* datos);
int muse_map(muse_map_t* datos);
int muse_sync(muse_sync_t* datos);
int muse_unmap(muse_unmap_t* datos);
void ocupate_de_este(int socket);
void esperar_conexion(uint32_t servidor);

#endif /* MUSE_H_ */
