/*
 * sac-server.h
 *
 *  Created on: 2 oct. 2019
 *      Author: utnso
 */

#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#include <time.h>
#include <pthread.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>
#include <commons/config.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>

struct sockaddr_in  direccionServidor;
struct sockaddr_in  direccionCliente;
/*==	Variables Globales		==*/
int cantidad_de_bloques;
t_log* logger;
FILE* fs;

typedef struct{
	uint32_t punteros [1024];
}_bloques_de_punteros;

typedef struct{
	uint8_t estado;
	char nombre_de_archivo[71];
	uint32_t bloque_padre; //_bloque
	uint32_t tamanio_de_archivo;//maximo 4 gb
	uint64_t fecha_de_creacion;
	uint64_t fecha_de_modificacion;
	_bloques_de_punteros punteros_indirectos[1000];
}nodo;

typedef struct { //tiene que pesar 4096
	char id[3];
	uint32_t version;
	uint32_t bloque_inicio_bitmap;
	uint32_t tamanio_bitmap;
	char relleno[4081];
}header;

typedef struct{
	unsigned char bytes [4096];
}bloque;


/*==	Firmas de Funciones		==*/
void esperar_conexion(int servidor);
int aceptar_cliente(int servidor);
int crear_servidor(int puerto);
uint64_t timestamp();
void atender_cliente(int cliente);


#endif /* SAC_SERVER_H_ */
