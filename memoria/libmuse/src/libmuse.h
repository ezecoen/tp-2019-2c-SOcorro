#ifndef LIBMUSE_H_
#define LIBMUSE_H_

#include <stdint.h>
#include <stddef.h>

//ESTRUCTURAS
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
	uint32_t size_mensaje;
	char* mensaje;
}muse_char;

typedef struct{
	uint32_t size_paquete;
	void* paquete;
}muse_void;


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
int socket_muse;
int iniciado = -1;
char* muse_id;

//FUNCIONES
/**
* Inicializa la biblioteca de MUSE.
* @param id El Process (o Thread) ID para identificar el caller en MUSE.
* @param ip IP en dot-notation de MUSE.
* @param puerto Puerto de conexión a MUSE.
* @return Si pasa un error, retorna -1. Si se inicializó correctamente, retorna 0.
* @see Para obtener el id de un proceso pueden usar getpid() de la lib POSIX (unistd.h)
* @note Debido a la naturaleza centralizada de MUSE, esta función deberá definir
*  el ID del proceso/hilo según "IP-ID".
*/
int muse_init(int id, char* ip, int puerto);

/**
* Cierra la biblioteca de MUSE.
*/
void muse_close();

/**
* Reserva una porción de memoria contígua de tamaño `tam`.
* @param tam La cantidad de bytes a reservar.
* @return La dirección de la memoria reservada.
*/
uint32_t muse_alloc(uint32_t tam);

/**
* Libera una porción de memoria reservada.
* @param dir La dirección de la memoria a reservar.
*/
void muse_free(uint32_t dir);

/**
* Copia una cantidad `n` de bytes desde una posición de memoria de MUSE a una `dst` local.
* @param dst Posición de memoria local con tamaño suficiente para almacenar `n` bytes.
* @param src Posición de memoria de MUSE de donde leer los `n` bytes.
* @param n Cantidad de bytes a copiar.
* @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
*/
int muse_get(void* dst, uint32_t src, size_t n);

/**
* Copia una cantidad `n` de bytes desde una posición de memoria local a una `dst` en MUSE.
* @param dst Posición de memoria de MUSE con tamaño suficiente para almacenar `n` bytes.
* @param src Posición de memoria local de donde leer los `n` bytes.
* @param n Cantidad de bytes a copiar.
* @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
*/
int muse_cpy(uint32_t dst, void* src, int n);


/**
* Devuelve un puntero a una posición mappeada de páginas por una cantidad `length` de bytes el archivo del `path` dado.
* @param path Path a un archivo en el FileSystem de MUSE a mappear.
* @param length Cantidad de bytes de memoria a usar para mappear el archivo.
* @param flags
*          MAP_PRIVATE     Solo un proceso/hilo puede mappear el archivo.
*          MAP_SHARED      El segmento asociado al archivo es compartido.
* @return Retorna la posición de memoria de MUSE mappeada.
* @note: Si `length` sobrepasa el tamaño del archivo, toda extensión deberá estar llena de "\0".
* @note: muse_free no libera la memoria mappeada. @see muse_unmap
*/
uint32_t muse_map(char *path, size_t length, int flags);

/**
* Descarga una cantidad `len` de bytes y lo escribe en el archivo en el FileSystem.
* @param addr Dirección a memoria mappeada.
* @param len Cantidad de bytes a escribir.
* @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
* @note Si `len` es menor que el tamaño de la página en la que se encuentre, se deberá escribir la página completa.
*/
int muse_sync(uint32_t addr, size_t len);

/**
* Borra el mappeo a un archivo hecho por muse_map.
* @param dir Dirección a memoria mappeada.
* @param
* @note Esto implicará que todas las futuras utilizaciones de direcciones basadas en `dir` serán accesos inválidos.
* @note Solo se deberá cerrar el archivo mappeado una vez que todos los hilos hayan liberado la misma cantidad de muse_unmap que muse_map.
* @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
*/
int muse_unmap(uint32_t dir);

void pruebita();
uint32_t conectar_socket_a(char* ip, uint32_t puerto);
int handshake_muse(int id);
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
uint32_t muse_map(char *path, size_t length, int flags);
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



#endif
