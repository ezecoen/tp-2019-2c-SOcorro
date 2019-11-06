#ifndef LIBRERIA_COMUN_H_
#define LIBRERIA_COMUN_H_


#include <time.h>
#include <pthread.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/error.h>
#include <commons/memory.h>
#include <commons/process.h>
#include <commons/temporal.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/node.h>
#include <commons/collections/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>


typedef enum {
	INIT_CLI,
	GETATTR,
	READDIR,
	OPEN,
	READ,
	MKNOD,
	MKDIR,
	CHMOD,
	UNLINK,
	ERROR,
	WRITE,
	EXITOSO,
	UTIMES,
	RMDIR,
	RENAME,
	TRUNCATE,
}operaciones;

typedef struct{
	uint32_t size_descripcion;
	char* descripcion;
}t_error;

typedef struct{
	uint32_t size_descripcion;
	char* descripcion;
}t_exitoso;

typedef struct{
	char* old;
	char* new;
	uint32_t old_size;
	uint32_t new_size;
}t_rename;

typedef struct{
	char* path;
	uint32_t size_path;
	uint32_t new_size;
}t_truncate;

typedef struct{
	uint32_t size; //4bytes
	uint64_t modif_time; //8bytes
	uint8_t tipo; //4bytes
}t_getattr;

typedef struct{
	uint32_t size_path;
	char* path;
	int crear;
	int crear_ensure;
	int truncate;
}t_open;

typedef struct{
	char* path;
	uint32_t size_path;
	char* buff;
	uint32_t size_buff;
	uint32_t offset;
}t_write;

typedef struct{
	int size_path;
	char* path;
	uint64_t utime;
}t_utime;

/**
* @NAME: char_length
* @DESC: Devuelve el tamaño de un char, incluido el '\0'
*/
int servidor;
t_log* logger;
uint32_t char_length(char* string);
uint32_t conectar_socket_a(char* ip, uint32_t puerto);
struct sockaddr_in  direccionServidor;
struct sockaddr_in  direccionCliente;
uint32_t tamanoDireccion;
void mandar(uint32_t tipo,void* algo,uint32_t _socket);
char* recibir_char(uint32_t _socket);
void mandar_char(char* _char, uint32_t _socket,uint32_t comando);
int recibir_op(int sock);
t_error* crear_error(char* descripcion);
void error_destroy(t_error* error);
void* serializar_paquete_error(t_error* error);
t_error* deserializar_paquete_error(void* magic);
t_exitoso* crear_exitoso(char* descripcion);
void exitoso_destroy(t_exitoso* exitoso);
void* serializar_paquete_exitoso(t_exitoso* exitoso);
t_exitoso* deserializar_paquete_exitoso(void* magic);
uint64_t timestamp();
int aceptar_cliente(int servidor);
int crear_servidor(int puerto);
uint32_t length_de_char_asterisco(char** arrays);
void* serializar_path(const char* path, operaciones comando);
char* deserializar_path (void* magic);
void* serializar_lista_ent_dir(t_list* lista);
t_list* deserializar_lista_ent_dir (void* magic, int tam_lista);
int tamanio_de_todos_las_ent_dir(t_list* lista);
char* recibir_path(int socket_cliente);
t_getattr* crear_getattr(uint32_t size, uint64_t modif_time, uint8_t tipo);
void* serializar_getattr(t_getattr* stat);
t_getattr* deserializar_getattr(void* magic);
t_open* crear_open(char* path, int flags);
t_utime* crear_utime (char* path, uint64_t utime);

#endif /* LIBRERIA_COMUN_H_ */
