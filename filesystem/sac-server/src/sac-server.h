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
#include <commons/bitarray.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

struct sockaddr_in  direccionServidor;
struct sockaddr_in  direccionCliente;

typedef struct{
	uint32_t punteros;
}puntero_a_bloque_de_puntero;

typedef struct{
	uint8_t estado;
	char nombre_de_archivo[71];
	uint32_t bloque_padre; //_bloque
	uint32_t tamanio_de_archivo;//maximo 4 gb
	uint64_t fecha_de_creacion;
	uint64_t fecha_de_modificacion;
	puntero_a_bloque_de_puntero punteros_indirectos[1000];
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

typedef struct{
	int op;
	void* argumentos;
}operacion;

typedef struct{
	uint32_t fd;
	uint32_t ptr_a_nodo;
}t_filedes;

/*==	Firmas de Funciones		==*/
void esperar_conexion(int servidor);
int crear_servidor(int puerto);
uint64_t timestamp();
void hace_algo(int cliente);
operacion* recibir_instruccion(int cliente);
void init_relleno(char* _relleno);
void escribir_header(bloque* disco);
header* levantar_header(bloque* _bloque);
void crear_fs(void);
int fileSize(char* filename);
void escribir_bitmap_inicio(bloque* disco,t_bitarray* bitarray);
t_bitarray* init_set_bitmap();
t_bitarray* levantar_bit_array(bloque* bloque);
header* levantar_header(bloque* _bloque);
void levantar_tabla_de_nodo(bloque* bloque);
void escribir_tabla_de_nodos(bloque* _bloque);
bool el_fs_esta_formateado(char* fs);
void init_fs(char* fs);
void load_fs(char* fs);
void atender_cliente(int cliente);
void _readdir(int cliente);
int dame_el_numero_de_bloque_de_nodo(char* nombre);
void crear_nodo(const char* path);
nodo* dame_el_primer_nodo_libre();
char* dame_el_nombre(char** nombres,int quien);
int _mknod(char* nombre);
int _mkdir(char* nombre);
nodo* dame_el_nodo_de(const char* _nombre);

/*==	Variables Globales		==*/
int cantidad_de_bloques;
int es_virgen;
t_log* logger;
FILE* fs;
int tam_del_fs;
int tam_de_bitmap;
bloque* primer_bloque_de_disco;
t_bitarray* bitarray;
header* _header;
nodo** tabla_de_nodos;
t_list* tabla_file_descriptors;
#endif /* SAC_SERVER_H_ */
