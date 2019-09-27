#include "libmuse.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <syscall.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
int muse_init(int id, char* ip, int puerto){
	int sock = conectar_socket_a(ip,puerto);
	if(sock == -1){
		puts("Nada de muse :(");
		return -1;
	}
	socket_muse = sock;
	muse_id = string_new();
	int iniciado = handshake_muse(id);//aca se setea muse_id
	//falta cerrar el sock
	return iniciado;
}

/**
 * Cierra la biblioteca de MUSE.
 */
void muse_close(){
	close(socket_muse);
	puts("Chau muse  :´(");
}

/**
 * Reserva una porción de memoria contígua de tamaño `tam`.
 * @param tam La cantidad de bytes a reservar.
 * @return La dirección de la memoria reservada.
 */
uint32_t muse_alloc(uint32_t tam){
	muse_alloc_t* mat = crear_muse_alloc(tam,muse_id);
	void* magic = serializar_muse_alloc(mat);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	uint32_t direccion;
	recv(socket_muse,&direccion,4,0);
	printf("direccion recibida: %d\n",direccion);
	return direccion;
}

/**
 * Libera una porción de memoria reservada.
 * @param dir La dirección de la memoria a reservar.
 */
void muse_free(uint32_t dir){
		muse_free_t* mft = crear_muse_free(muse_id,dir);
		void* magic = serializar_muse_free(mft);
		uint32_t tamanio_magic;
		memcpy(&tamanio_magic,magic+4,4);
		send(socket_muse,magic,tamanio_magic,0);
		_Bool resultado;
		recv(socket_muse,&resultado,4,0);
		// !! si recv = -1 que pasa con resultado? ??
		if (resultado)
			{
			printf("free realizado para: %d\n",dir);

			}else
			{
			printf("error al realizar el free para: %d\n",dir);
			}

}


/**
 * Copia una cantidad `n` de bytes desde una posición de memoria de MUSE a una `dst` local.
 * @param dst Posición de memoria local con tamaño suficiente para almacenar `n` bytes.
 * @param src Posición de memoria de MUSE de donde leer los `n` bytes.
 * @param n Cantidad de bytes a copiar.
 * @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
 */
int muse_get(void* dst, uint32_t src, size_t n){

		muse_get_t* mat = crear_muse_get(n,muse_id,src);
		void* magic = serializar_muse_get(mat);
		uint32_t tamanio_magic;
		memcpy(&tamanio_magic,magic+4,4);
		send(socket_muse,magic,tamanio_magic,0);
		uint32_t resultado;
		if(recv(socket_muse,&resultado,4,0) == 0)
		{
			printf("get hecho para : %d\n",src);
		} else {
			printf("error en get para : %d\n",src);
			return -1;
		}

	return 0;
}

/**
 * Copia una cantidad `n` de bytes desde una posición de memoria local a una `dst` en MUSE.
 * @param dst Posición de memoria de MUSE con tamaño suficiente para almacenar `n` bytes.
 * @param src Posición de memoria local de donde leer los `n` bytes.
 * @param n Cantidad de bytes a copiar.
 * @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
 */
int muse_cpy(uint32_t dst, void* src, int n){
	return 0;
}


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
uint32_t muse_map(char *path, size_t length, int flags){
	return 0;
}

/**
 * Descarga una cantidad `len` de bytes y lo escribe en el archivo en el FileSystem.
 * @param addr Dirección a memoria mappeada.
 * @param len Cantidad de bytes a escribir.
 * @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
 * @note Si `len` es menor que el tamaño de la página en la que se encuentre, se deberá escribir la página completa.
 */
int muse_sync(uint32_t addr, size_t len){
	return 0;
}

/**
 * Borra el mappeo a un archivo hecho por muse_map.
 * @param dir Dirección a memoria mappeada.
 * @param
 * @note Esto implicará que todas las futuras utilizaciones de direcciones basadas en `dir` serán accesos inválidos.
 * @note Solo se deberá cerrar el archivo mappeado una vez que todos los hilos hayan liberado la misma cantidad de muse_unmap que muse_map.
 * @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
 */
int muse_unmap(uint32_t dir){
	return 0;
}

void pruebita(){
	puts("pruebita uwu");
}

uint32_t conectar_socket_a(char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM,0);
	if (connect(cliente,(void*) &direccionServidor, sizeof(direccionServidor)) != 0)
	{
		printf("Error al conectar a ip %s y puerto %d\n",ip,puerto);
		return -1;
	}
	return cliente;
}
int handshake_muse(int id){
	if(socket_muse>=0){
		uint32_t com = MUSE_INIT;
		uint32_t bytes = sizeof(uint32_t)*2;
		void* magic = malloc(bytes);
		uint32_t puntero = 0;
		memcpy(magic+puntero,&com,4);
		puntero += 4;
		memcpy(magic+puntero,&id,4);
		puntero += 4;
		int res = send(socket_muse,magic,bytes,0);//mandamos nuestro pid
		free(magic);
		if(res<0){
			puts("Nada de muse :(");
			free(magic);
			return -1;
		}
		else{//recivimos nuestro char* muse_id
			uint32_t tam;
			recv(socket_muse,&com,4,0);
			recv(socket_muse,&tam,4,0);
			void* paquete = malloc(tam);
			recv(socket_muse,paquete,tam,0);
			memcpy(muse_id,paquete,tam);//guardo el muse_id
			free(paquete);
			printf("Hola muse n.n\nmuse_id: %s\n",muse_id);
			return 0;
		}
	}
	else{
		puts("Nada de muse :(");
		return -1;
	}
}

muse_alloc_t* crear_muse_alloc(uint32_t tamanio,char* id){
	muse_alloc_t* mat = malloc(sizeof(muse_alloc_t));
	mat->id = string_duplicate(id);
	mat->size_id = strlen(id)+1;
	mat->tamanio = tamanio;
	return mat;
}

muse_free_t* crear_muse_free(char* id,uint32_t direccion){
	muse_free_t* mft = malloc(sizeof(muse_free_t));
	mft -> id = string_duplicate(id);
	mft->direccion = direccion;
	mft->size_id = strlen(id)+1;
	return mft;
}

muse_get_t* crear_muse_get(uint32_t tamanio, char* id,uint32_t direccion){
	muse_get_t* mgt = malloc(sizeof(muse_get_t));
	mgt -> id= string_duplicate(id);
	mgt->direccion = direccion;
	mgt->size_id = strlen(id)+1;
	mgt->tamanio = tamanio;
	return mgt;
}


void muse_alloc_destroy(muse_alloc_t* mat){
	free(mat->id);
	free(mat);
}
void muse_free_destroy(muse_free_t* mfr){
	free(mfr->id);
	free(mfr);
}
void muse_get_destroy(muse_get_t* mgt){
	free(mgt->id);
	free(mgt);
}
void* serializar_muse_alloc(muse_alloc_t* mat){
	int bytes = sizeof(uint32_t)*2+ mat->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_ALLOC;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mat->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mat->id,mat->size_id);
	puntero += mat->size_id;
	memcpy(magic+puntero,&mat->tamanio,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	return magic;
}


void* serializar_muse_free(muse_free_t* mft){
	int bytes = sizeof(uint32_t)*2+ mft->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_FREE;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mft->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mft->id,mft->size_id);
	puntero += mft->size_id;
	memcpy(magic+puntero,&mft->direccion,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	return magic;
}

void* serializar_muse_get(muse_get_t* mgt){
	int bytes = sizeof(uint32_t)*3+ mgt->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_GET;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mgt->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mgt->id,mgt->size_id);
	puntero += mgt->size_id;
	memcpy(magic+puntero,&mgt->direccion,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mgt->tamanio,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	return magic;
}


muse_alloc_t* deserializar_muse_alloc(void* magic){
	muse_alloc_t* mat = malloc(sizeof(muse_alloc_t));
	uint32_t puntero = 0;
	memcpy(&mat->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mat->id = malloc(mat->size_id);
	memcpy(mat->id,magic+puntero,mat->size_id);
	puntero+=mat->size_id;
	memcpy(&mat->tamanio,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	return mat;
}

muse_free_t* deserializar_muse_free(void* magic){
	muse_free_t* mft = malloc(sizeof(muse_free_t));
	uint32_t puntero = 0;
	memcpy(&mft->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mft->id = malloc(mft->size_id);
	memcpy(mft->id,magic+puntero,mft->size_id);
	puntero+=mft->size_id;
	memcpy(&mft->direccion,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	return mft;
}

muse_get_t* deserializar_muse_get(void* magic){
	muse_get_t* mgt = malloc(sizeof(muse_get_t));
	uint32_t puntero = 0;
	memcpy(&mgt->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mgt->id = malloc(mgt->size_id);
	memcpy(mgt->id,magic+puntero,mgt->size_id);
	puntero+=mgt->size_id;
	memcpy(&mgt->direccion,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mgt->tamanio,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	return mgt;
}
