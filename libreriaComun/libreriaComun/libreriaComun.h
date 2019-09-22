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

typedef enum{
	EXIT,
	CHAR,
	ERROR,
	EXITOSO,
}t_comando;

typedef struct{
	uint32_t size_descripcion;
	char* descripcion;
}t_error;

typedef struct{
	uint32_t size_descripcion;
	char* descripcion;
}t_exitoso;

/**
* @NAME: char_length
* @DESC: Devuelve el tamaño de un char, incluido el '\0'
*/
uint32_t char_length(char* string);
uint32_t conectar_socket_a(char* ip, uint32_t puerto);
struct sockaddr_in  direccionServidor;
struct sockaddr_in  direccionCliente;
uint32_t tamanoDireccion;
uint32_t servidor;
void mandar(uint32_t tipo,void* algo,uint32_t _socket);
char* recibir_char(uint32_t _socket);
void mandar_char(char* _char, uint32_t _socket,uint32_t comando);
t_error* crear_error(char* descripcion);
void error_destroy(t_error* error);
void* serializar_paquete_error(t_error* error);
t_error* deserializar_paquete_error(void* magic);
t_exitoso* crear_exitoso(char* descripcion);
void exitoso_destroy(t_exitoso* exitoso);
void* serializar_paquete_exitoso(t_exitoso* exitoso);
t_exitoso* deserializar_paquete_exitoso(void* magic);
uint64_t timestamp();
uint32_t aceptar_cliente(uint32_t servidor);
uint32_t crear_servidor(uint32_t puerto);
#endif /* LIBRERIA_COMUN_H_ */
