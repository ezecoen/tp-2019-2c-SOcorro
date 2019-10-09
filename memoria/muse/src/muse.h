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
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>

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

typedef struct{
	uint32_t size_mensaje;
	char* mensaje;
}muse_char;

typedef struct{
	uint32_t size_paquete;
	void* paquete;
}muse_void;

typedef struct s_config{
	uint32_t puerto;
	uint32_t tam_mem;
	uint32_t tam_pag;
	uint32_t tam_swap;
	char* ip;
}s_config;

typedef enum{
	MUSE_INIT=0,//no tiene crear/serializar/etc
	MUSE_ALLOC=1,
	MUSE_FREE=2,
	MUSE_GET=3,
	MUSE_CPY=4,
	MUSE_MAP=5,
	MUSE_SYNC=6,
	MUSE_UNMAP=7,
	MUSE_CLOSE=8,//no tiene crear/serializar/etc
	MUSE_ERROR=9,//falta
	MUSE_EXITOSO=10,//falta
	MUSE_CHAR=11,
	MUSE_INT=12,//no tiene crear/serializar/etc xq es solo un int de 4bytes
	MUSE_VOID=13
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

struct sockaddr_in direccionServidor;
uint32_t servidor;
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
void* asignar_marco_nuevo();
t_bit* ejecutar_clock_modificado();
int obtener_direccion_virtual(uint32_t num_segmento,uint32_t num_pag,uint32_t offset);
void abrir_direccion_virtual(int direccion,int* destino_segmento,int* destino_pagina, int* destino_offset);
int muse_free(muse_free_t* datos);
void* muse_get(muse_get_t* datos);
int muse_cpy(muse_cpy_t* datos);
int muse_map(muse_map_t* datos);
int muse_sync(muse_sync_t* datos);
int muse_unmap(muse_unmap_t* datos);
int muse_close(char* id_cliente);
_Bool direccion_valida_cliente(int direccion,int tamanio,char* id_cliente);
uint32_t crear_servidor(uint32_t puerto);
void mandar_char(char* _char, uint32_t _socket,uint32_t com);
uint32_t aceptar_cliente(uint32_t servidor);
void esperar_conexion(uint32_t servidor);
void ocupate_de_este(int socket);
void init_bitarray();
void destroy_bitarray();
t_bit* bit_libre_memoria();
t_bit* bit_libre_memoria_virtual();
_Bool bit_libre(t_bit* bit);
muse_alloc_t* crear_muse_alloc(uint32_t tamanio,char* id);
void muse_alloc_destroy(muse_alloc_t* mat);
void* serializar_muse_alloc(muse_alloc_t* mat);
muse_alloc_t* deserializar_muse_alloc(void* magic);
muse_free_t* crear_muse_free(char* id,uint32_t direccion);
void muse_free_destroy(muse_free_t* mfr);
void* serializar_muse_free(muse_free_t* mft);
muse_free_t* deserializar_muse_free(void* magic);
muse_get_t* crear_muse_get(uint32_t tamanio, char* id,uint32_t direccion);
void muse_get_destroy(muse_get_t* mgt);
void* serializar_muse_get(muse_get_t* mgt);
muse_get_t* deserializar_muse_get(void* magic);
muse_cpy_t* crear_muse_cpy(uint32_t tamanio, char* id,uint32_t direccion, void* paquete);
void muse_cpy_destroy(muse_cpy_t* mct);
void* serializar_muse_cpy(muse_cpy_t* mct);
muse_cpy_t* deserializar_muse_cpy(void* magic);
muse_map_t* crear_muse_map(uint32_t tamanio, char* id, uint32_t flag, char* path);
void muse_map_destroy(muse_map_t* mmt);
void* serializar_muse_map(muse_map_t* mmt);
muse_map_t* deserializar_muse_map(void* magic);
muse_sync_t* crear_muse_sync(uint32_t tamanio, char* id, uint32_t direccion);
void muse_sync_destroy(muse_sync_t* mst);
void* serializar_muse_sync(muse_sync_t* mst);
muse_sync_t* deserializar_muse_sync(void* magic);
muse_unmap_t* crear_muse_unmap(char* id, uint32_t direccion);
void muse_unmap_destroy(muse_unmap_t* mut);
void* serializar_muse_unmap(muse_unmap_t* mut);
muse_unmap_t* deserializar_muse_unmap(void* magic);
muse_char* crear_muse_char(char* mensaje, uint32_t size_mensaje);
void muse_char_destroy(muse_char* mc);
void* serializar_muse_char(muse_char* mc);
muse_char* deserializar_muse_char(void* magic);
muse_void* crear_muse_void(void* paquete, uint32_t size_paquete);
void muse_void_destroy(muse_void* mv);
void* serializar_muse_void(muse_void* mv);
muse_void* deserializar_muse_void(void* magic);


#endif /* MUSE_H_ */