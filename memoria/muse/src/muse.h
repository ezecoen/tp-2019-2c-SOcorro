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
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <math.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/log.h>
#include <string.h>
#include <stdbool.h>
#include <sys/mman.h>
//ESTRUCTURAS
typedef struct programa_t{
	char* id_programa;
	t_list* tabla_de_segmentos;
}programa_t;

typedef struct segmento{
	uint32_t num_segmento;
	uint32_t base_logica;
	uint32_t tamanio;
	t_list* paginas;
	t_list* info_heaps;
	_Bool mmapeado;
	_Bool compartido;
	char* path_mapeo;
	uint32_t tamanio_mapeo;
	uint32_t paginas_liberadas;
}segmento;

typedef struct mapeo_t{
	char* path;
	uint32_t contador;
	t_list* paginas;
	int tamanio;
	int tamanio_de_pags;
}mapeo_t;

typedef struct heap_metadata{
	uint32_t size;
	_Bool is_free;

}__attribute__((packed)) heap_metadata ;

typedef struct heap_lista{
	int direccion_heap_metadata; //con respecto al segmento
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
	uint32_t posicion;
	_Bool bit_uso;//para clock
	_Bool bit_modificado;//para clock
}t_bit_memoria;

typedef struct t_bit_swap{
	_Bool ocupado;
	uint32_t posicion;
}t_bit_swap;

typedef struct pagina{
	uint32_t num_pagina;
	_Bool presencia;
	t_bit_memoria* bit_marco;
	t_bit_swap* bit_swap;
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

typedef struct memoria_liberada{
	char* programa_id;
	int memoria_liberada_acumulada;
}memoria_liberada;

typedef struct memoria_pedida{
	char* programa_id;
	int memoria_pedida_acumulada;
}memoria_pedida;

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
void* upcm;
void* swap;
uint32_t lugar_disponible;
pthread_mutex_t mutex_lugar_disponible;
t_list* tabla_de_programas;
pthread_mutex_t mutex_tabla_de_programas;
t_list* tabla_de_mapeo;
pthread_mutex_t mutex_tabla_de_mapeo;
int CANT_PAGINAS_MEMORIA;
int CANT_PAGINAS_MEMORIA_VIRTUAL;
pthread_mutex_t mutex_bitarray;
bitarray_nuestro* bitarray;
int posicion_puntero_clock;
pthread_mutex_t mutex_tabla_de_memoria;
t_list* tabla_de_memoria_liberada;
t_list* tabla_de_memoria_pedida;
struct sockaddr_in direccionServidor;
uint32_t servidor;
s_config* configuracion;
t_config* g_config;
t_log* logg;
t_log* log_metricas;

//SEMAFOROS
//cosas que va a haber que semaforizar
/*
 * las 4 tablas, lugar disp
 *
 *
 */


//FUNCIONES
void init_estructuras();
void iniciar_memoria_virtual(char* path);
void init_semaforos();
int log_2(double d);
void iniciar_log(char* path);
s_config* leer_config(char* path);
int redondear_double_arriba(double d);
int muse_alloc(muse_alloc_t* datos);
uint32_t base_logica_segmento_nuevo(t_list* tabla_de_segmentos);
segmento* buscar_segmento_con_espacio(t_list* tabla_de_segmentos,uint32_t tamanio);
segmento* buscar_segmento_propio_por_id(char* id);
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio);
_Bool encontrar_ultima_pagina(pagina* pag);
void reemplazar_heap_en_memoria(heap_lista* heap_de_lista,segmento* seg,heap_metadata* nuevo_heap_metadata);
t_bit_memoria* asignar_marco_nuevo();
void* obtener_puntero_a_marco(pagina* pag);
t_bit_memoria* ejecutar_clock_modificado();
t_bit_memoria* ejecutar_clock_modificado_2vuelta();
t_bit_memoria* buscar_0_0();
t_bit_memoria* buscar_0_1();
pagina* buscar_pagina_por_bit(t_bit_memoria* bit);
t_bit_swap* pasar_marco_a_swap(t_bit_memoria* bit);
int muse_free(muse_free_t* datos);
void* muse_get(muse_get_t* datos);
segmento* traer_segmento_de_direccion(t_list* tabla_de_segmentos,uint32_t direccion);
void paginas_de_map_en_memoria(int direccion,int tamanio,segmento* segmento_buscado);
int muse_cpy(muse_cpy_t* datos);
int muse_map(muse_map_t* datos);
t_list* buscar_mapeo_existente(char* path, int tamanio);
void* generar_padding(int padding);
int muse_sync(muse_sync_t* datos);
int muse_unmap(muse_unmap_t* datos);
void bajar_mapeo(char* path_mapeo, int tam_mapeo);
void mapeo_destroy(mapeo_t* _mapeo);
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
//serializaciones
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
void destroy_segmento(segmento* seg);
void destroy_programa(programa_t* prog);
void destroy_pagina(pagina* pag);
void metricas();
void metrica_del_sistema();
void metrica_por_programa(char* id_cliente);
void metrica_por_socket_conectado();
void acumular_espacio_liberado(char* programa, int cuanto);
void acumular_espacio_pedido(char* programa, int cuanto);
segmento* buscar_segmento_con_paginas_liberadas(int tamanio, t_list* tabla_segmentos);

#endif /* MUSE_H_ */
