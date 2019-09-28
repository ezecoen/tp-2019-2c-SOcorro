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
typedef struct segmento{
	char* nombre;
	_Bool mmapeado;
	_Bool compartido;
	uint32_t num_segmento;
	t_list* paginas;
}segmento;

typedef struct pagina{
	uint32_t num_pagina;
	int ultimo_heap_metadata_libre;
	_Bool presencia;
	_Bool modificado;
	void* datos;
}pagina;

typedef struct heap_metadata{
	uint32_t size;
	_Bool isFree;
}heap_metadata;

typedef struct {
	t_list* bitarray_memoria;
	uint32_t size_memoria;
	t_list* bitarray_memoria_virtual;
	uint32_t size_memoria_virtual;
}bitarray_nuestro;

typedef struct t_bit{
	bool bit_usado;
	uint32_t bit_position;
}t_bit;

typedef struct muse_alloc_t{
	uint32_t size_id;
	char* id;
	uint32_t tamanio;
}muse_alloc_t;

typedef struct muse_free_t{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
}muse_free_t;

typedef struct muse_get_t{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
	uint32_t tamanio;
}muse_get_t;

typedef struct muse_cpy_t{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
	uint32_t size_paquete;
	void* paquete;
}muse_cpy_t;

typedef struct muse_map_t{
	uint32_t size_id;
	char* id;
	uint32_t size_path;
	char* path;
	uint32_t tamanio;
	uint32_t flag;
}muse_map_t;

typedef struct muse_sync_t{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
	uint32_t tamanio;
}muse_sync_t;

typedef struct muse_unmap_t{
	uint32_t size_id;
	char* id;
	uint32_t direccion;
}muse_unmap_t;

typedef struct s_config{
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
void* swap;
uint32_t lugar_disponible;
t_list* tabla_de_segmentos;
int DIR_TAM_DIRECCION;
int DIR_TAM_DESPLAZAMIENTO;
int DIR_TAM_PAGINA;
int CANT_PAGINAS_MEMORIA;
int CANT_PAGINAS_MEMORIA_VIRTUAL;

bitarray_nuestro* bitarray;

s_config* configuracion;
t_config* g_config;
t_log* logg;

//SEMAFOROS


//FUNCIONES
void init_estructuras();
void free_final();
void iniciar_log(char* path);
s_config* leer_config(char* path);
void dec_a_bin(char destino[],int decimal, int tam);
int bin_a_dec(char* binario);
int redondear_double_arriba(double d);
int log_2(double d);
int muse_alloc(muse_alloc_t* datos);
segmento* buscar_segmento_por_id(char* id);
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio);
int reservar_lugar_en_segmento(segmento* seg,uint32_t tamanio);
_Bool asignar_marco_nuevo(void** destino);
int obtener_direccion_virtual(uint32_t num_segmento,uint32_t num_pag,uint32_t offset);
int muse_free(muse_free_t* datos);
int muse_get(muse_get_t* datos);
int muse_cpy(muse_cpy_t* datos);
int muse_map(muse_map_t* datos);
int muse_sync(muse_sync_t* datos);
int muse_unmap(muse_unmap_t* datos);
void ocupate_de_este(int socket);
void esperar_conexion(uint32_t servidor);
void init_bitarray();
void destroy_bitarray();
t_bit* bit_libre_memoria();
t_bit* bit_libre_memoria_virtual();
_Bool bit_libre(t_bit* bit);
muse_alloc_t* crear_muse_alloc(uint32_t tamanio,char* id);
void muse_alloc_destroy(muse_alloc_t* mat);
void* serializar_muse_alloc(muse_alloc_t* mat);
muse_alloc_t* deserializar_muse_alloc(void* magic);


#endif /* MUSE_H_ */
