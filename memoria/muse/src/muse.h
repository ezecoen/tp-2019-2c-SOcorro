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
#include <stdbool.h>
//ESTRUCTURAS
typedef struct programa_t{
	char* id_programa;
	t_list* tabla_de_segmentos;
}programa_t;

typedef struct segmento{
	_Bool mmapeado;
	_Bool compartido;
	uint32_t num_segmento;
	uint32_t base_logica;
	uint32_t tamanio;
	t_list* paginas;
	t_list* info_heaps;
}segmento;

typedef struct heap_metadata{
	uint32_t size;
	_Bool is_free;

}__attribute__((packed)) heap_metadata ;

typedef struct heap_lista{
	int direccion_heap_metadata;
	int espacio;
	_Bool is_free;
	int indice;
}__attribute__((packed)) heap_lista ;


typedef struct bitarray_nuestro{
	t_list* bitarray_memoria;//se llena con t_bit_memoria
	uint32_t size_memoria;
	t_list* bitarray_memoria_virtual;//se llena con t_bit_swap
	uint32_t size_memoria_virtual;
}bitarray_nuestro;

typedef struct t_bit_memoria{
	_Bool ocupado;
	uint32_t bit_position;
	_Bool bit_uso;//para clock
	_Bool bit_modificado;//para clock
}t_bit_memoria;
typedef struct t_bit_swap{
	_Bool ocupado;
	uint32_t bit_position;
}t_bit_swap;


typedef struct pagina{
	uint32_t num_pagina;
	_Bool presencia;
	t_bit_memoria* bit_marco;
}pagina;
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

typedef struct muse_char{
	uint32_t size_mensaje;
	char* mensaje;
}muse_char;

typedef struct muse_void{
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

typedef enum t_comando_muse{
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
	MUSE_VOID=13,
	MUSE_SEG_FAULT=14
}t_comando_muse;

//VARIABLES GLOBALES
char* path_de_config;
void* upcm;
void* swap;
uint32_t lugar_disponible;
t_list* tabla_de_programas;
int DIR_TAM_DIRECCION;
int DIR_TAM_DESPLAZAMIENTO;
int DIR_TAM_PAGINA;
int CANT_PAGINAS_MEMORIA;
int CANT_PAGINAS_MEMORIA_VIRTUAL;
bitarray_nuestro* bitarray;
int posicion_puntero_clock;

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
uint32_t base_logica_segmento_nuevo(segmento* segmento_anterior);
segmento* buscar_segmento_con_espacio(t_list* tabla_de_segmentos,uint32_t tamanio);
segmento* buscar_segmento_propio_por_id(char* id);
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio);
_Bool encontrar_ultima_pagina(pagina* pag);
t_bit_memoria* asignar_marco_nuevo();
void* obtener_puntero_a_marco(t_bit_memoria* bit_marco);
t_bit_memoria* ejecutar_clock_modificado();
t_bit_memoria* buscar_0_0();
t_bit_memoria* buscar_0_1();
int no_obtener_direccion_virtual(uint32_t num_segmento,uint32_t num_pag,uint32_t offset);
void no_abrir_direccion_virtual(int direccion,uint32_t* destino_segmento,uint32_t* destino_pagina, uint32_t* destino_offset);
int muse_free(muse_free_t* datos);
void* muse_get(muse_get_t* datos);
segmento* traer_segmento_de_direccion(t_list* tabla_de_segmentos,uint32_t direccion);
void* traer_datos_de_memoria(segmento* segmento_buscado,uint32_t dir_pagina,uint32_t dir_offset);
int muse_cpy(muse_cpy_t* datos);
int muse_map(muse_map_t* datos);
int muse_sync(muse_sync_t* datos);
int muse_unmap(muse_unmap_t* datos);
int muse_close(char* id_cliente);
uint32_t crear_servidor(uint32_t puerto);
void mandar_char(char* _char, uint32_t _socket,uint32_t com);
uint32_t aceptar_cliente(uint32_t servidor);
void esperar_conexion(uint32_t servidor);
void ocupate_de_este(int socket);
void init_bitarray();
void destroy_bitarray();
t_bit_memoria* bit_libre_memoria();
t_bit_swap* bit_libre_memoria_virtual();
_Bool bit_libre(t_bit_memoria* bit);
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
void* list_last_element(t_list* lista);
pagina* buscar_pagina_por_numero(t_list* lista, int numero_de_pag);

#endif /* MUSE_H_ */
