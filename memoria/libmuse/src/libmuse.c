#include "libmuse.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <syscall.h>
#include <commons/string.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

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
	if(iniciado < 0){
		int sock = conectar_socket_a(ip,puerto);
		if(sock == -1){
			puts("Nada de muse :(");
			return -1;
		}
		socket_muse = sock;
		muse_id = string_new();
		iniciado = handshake_muse(id);//aca se setea muse_id
	}
	return iniciado;
}

/**
 * Cierra la biblioteca de MUSE.
 */
void muse_close(){
	if(iniciado<0){//si ya esta cerrado no hace nada
		return;
	}
	int operacion = MUSE_CLOSE;
	send(socket_muse,&operacion,4,0);
	close(socket_muse);
	iniciado = -1;
	free(muse_id);
	puts("Chau muse  :´(\n");
}

/**
 * Reserva una porción de memoria contígua de tamaño `tam`.
 * @param tam La cantidad de bytes a reservar.
 * @return La dirección de la memoria reservada.
 */
uint32_t muse_alloc(uint32_t tam){
	muse_alloc_t* mat = crear_muse_alloc(tam,muse_id);
	void* magic = serializar_muse_alloc(mat); // todos estos hay que liberarlos
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	uint32_t comando, direccion;
	recv(socket_muse,&comando,4,0);
	if(comando == MUSE_INT){
		recv(socket_muse,&direccion,4,0);
		printf("Direccion recibida en libmuse: %d\n",direccion);
	}
	else{
		direccion = NULL;
		//probar si se manda el null
	}
	free(magic);
	muse_alloc_destroy(mat);
	return direccion;
}
/**
 * Libera una porción de memoria reservada.
 * @param dir La dirección de la memoria a reservar. // liberar ??
 */

void muse_free(uint32_t dir){
	muse_free_t* mft = crear_muse_free(muse_id,dir);
	void* magic = serializar_muse_free(mft);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	free(magic);
	muse_free_destroy(mft);
	uint32_t operacion;
	recv(socket_muse,&operacion,4,0);
	if(operacion == MUSE_SEG_FAULT){
		printf("error al realizar el free para: %d\n",dir);
		raise(SIGSEGV);
	}
	else{
		printf("free realizado para: %d\n",dir);
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
	muse_get_t* mgt = crear_muse_get(n,muse_id,src);
	void* magic = serializar_muse_get(mgt);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	free(magic);
	muse_get_destroy(mgt);
	uint32_t resultado;
	recv(socket_muse,&resultado,4,0);
	if(resultado==MUSE_VOID){
		uint32_t size_resultado;
		recv(socket_muse,&size_resultado,4,0);
		void* resultado = malloc(size_resultado);
		recv(socket_muse,resultado,size_resultado,0);
		muse_void* mv = deserializar_muse_void(resultado);
		memcpy(dst,mv->paquete,mv->size_paquete);
		printf("get realizado, resultado en %p\n",dst);
		printf("Buffer en libmuse: %s\n",(char*)mv->paquete);
		free(resultado);
		muse_void_destroy(mv);
		return 0;
	}
	else if(resultado == MUSE_SEG_FAULT){
		printf("La cagaste con el get en: %d\n",src);
		raise(11);
	}
	return -1;//pa q no tire warnin
}
/**
 * Copia una cantidad `n` de bytes desde una posición de memoria local a una `dst` en MUSE.
 * @param dst Posición de memoria de MUSE con tamaño suficiente para almacenar `n` bytes.
 * @param src Posición de memoria local de donde leer los `n` bytes.
 * @param n Cantidad de bytes a copiar.
 * @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
 */

int muse_cpy(uint32_t dst, void* src, int n){
	muse_cpy_t* mct = crear_muse_cpy(n,muse_id,dst,src);
	void* magic = serializar_muse_cpy(mct);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	free(magic);
	muse_cpy_destroy(mct);
	uint32_t operacion;
	recv(socket_muse,&operacion,4,0);
	if(operacion == MUSE_SEG_FAULT){
		printf("error al realizar el cpy en: %d\n",dst);
		raise(11);
	}
	else{
		//entonces operacion es MUSE_EXITOSO
		//hay q preguntar si segfault y error es lo mismo?
		printf("cpy realizado en: %d\n",dst);
	}


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
	muse_map_t* mmt = crear_muse_map(length,muse_id,flags,path);
	void* magic = serializar_muse_map(mmt);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	uint32_t posicion_memoria_mapeada;
	if(recv(socket_muse,&posicion_memoria_mapeada,4,0)==-1){
		printf("error en map para : %s\n",path);
		return posicion_memoria_mapeada = 0; // ??
	}
	printf("map hecho para : %s\n",path);
	return posicion_memoria_mapeada;
}
/**
 * Descarga una cantidad `len` de bytes y lo escribe en el archivo en el FileSystem.
 * @param addr Dirección a memoria mappeada.
 * @param len Cantidad de bytes a escribir.
 * @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
 * @note Si `len` es menor que el tamaño de la página en la que se encuentre, se deberá escribir la página completa.
 */
int muse_sync(uint32_t addr, size_t len){ // size_t ?? es un int? wtf
	muse_sync_t* mst = crear_muse_sync(len,muse_id,addr);
	void* magic = serializar_muse_sync(mst);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	uint32_t resultado;
	if(recv(socket_muse,&resultado,4,0)==-1){
		printf("error en sync\n");
		return -1;

	}
	printf("sync realizado\n");
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
	muse_unmap_t* mut = crear_muse_unmap(muse_id,dir);
	void* magic = serializar_muse_unmap(mut);
	uint32_t tamanio_magic;
	memcpy(&tamanio_magic,magic+4,4);
	send(socket_muse,magic,tamanio_magic,0);
	uint32_t resultado;
	if(recv(socket_muse,&resultado,4,0)==-1) {
		printf("error en unmap para %i\n",dir);
		return -1;
	}
	printf("unmap realizado para %i\n",dir);
	return 0;
}
muse_alloc_t* crear_muse_alloc(uint32_t tamanio,char* id){
	muse_alloc_t* mat = malloc(sizeof(muse_alloc_t));
	mat->id = string_duplicate(id);
	mat->size_id = strlen(id)+1;
	mat->tamanio = tamanio;
	return mat;
}

void muse_alloc_destroy(muse_alloc_t* mat){
	free(mat->id);
	free(mat);
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


muse_free_t* crear_muse_free(char* id,uint32_t direccion){
	muse_free_t* mft = malloc(sizeof(muse_free_t));
	mft -> id = string_duplicate(id);
	mft->direccion = direccion;
	mft->size_id = strlen(id)+1;
	return mft;
}

void muse_free_destroy(muse_free_t* mfr){
	free(mfr->id);
	free(mfr);
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



muse_get_t* crear_muse_get(uint32_t tamanio, char* id,uint32_t direccion){
	muse_get_t* mgt = malloc(sizeof(muse_get_t));
	mgt -> id= string_duplicate(id);
	mgt->direccion = direccion;
	mgt->size_id = strlen(id)+1;
	mgt->tamanio = tamanio;
	return mgt;
}

void muse_get_destroy(muse_get_t* mgt){
	free(mgt->id);
	free(mgt);
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


muse_cpy_t* crear_muse_cpy(uint32_t tamanio, char* id,uint32_t direccion, void* paquete){
	muse_cpy_t* mct = malloc(sizeof(muse_cpy_t));
	mct -> id= string_duplicate(id);
	mct->direccion = direccion;
	mct->size_id = strlen(id)+1;
	mct->size_paquete = tamanio;
	void* paquete2 = malloc(mct->size_paquete);
	memcpy(paquete2,paquete,mct->size_paquete);
	mct->paquete = paquete2;
	return mct;
}

void muse_cpy_destroy(muse_cpy_t* mct){
	free(mct->id);
	free(mct->paquete);
	free(mct);
}

void* serializar_muse_cpy(muse_cpy_t* mct){
	int bytes = sizeof(uint32_t)*3+ mct->size_id + mct->size_paquete + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_CPY;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mct->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mct->id,mct->size_id);
	puntero += mct->size_id;
	memcpy(magic+puntero,&mct->direccion,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mct->size_paquete,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mct->paquete,mct->size_paquete);
	puntero += mct->size_paquete;
	return magic;
}

muse_cpy_t* deserializar_muse_cpy(void* magic){
	muse_cpy_t* mct = malloc(sizeof(muse_cpy_t));
	uint32_t puntero = 0;
	memcpy(&mct->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mct->id = malloc(mct->size_id);
	memcpy(mct->id,magic+puntero,mct->size_id);
	puntero+=mct->size_id;
	memcpy(&mct->direccion,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mct->size_paquete,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mct->paquete = malloc(mct->size_paquete); // ?? leaks
	memcpy(mct->paquete,magic+puntero,mct->size_paquete);
	puntero+=mct->size_paquete;
	return mct;
}


muse_map_t* crear_muse_map(uint32_t tamanio, char* id, uint32_t flag, char* path){
	muse_map_t* mmt = malloc(sizeof(muse_map_t));
	mmt->id= string_duplicate(id);
	mmt->size_id = strlen(id)+1;
	mmt->path = string_duplicate(path);
	mmt->size_path = strlen(path)+1;
	mmt->tamanio = tamanio;
	mmt->flag = flag;
	return mmt;
}

void muse_map_destroy(muse_map_t* mmt){
	free(mmt->id);
	free(mmt->path);
	free(mmt);
}

void* serializar_muse_map(muse_map_t* mmt){
	int bytes = sizeof(uint32_t)*4+ mmt->size_id + mmt->size_path + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_MAP;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mmt->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mmt->id,mmt->size_id);
	puntero += mmt->size_id;
	memcpy(magic+puntero,&mmt->tamanio,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mmt->flag,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mmt->size_path,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mmt->path,mmt->size_path);
	puntero += mmt->size_path;
	return magic;
}

muse_map_t* deserializar_muse_map(void* magic){
	muse_map_t* mmt = malloc(sizeof(muse_map_t));
	uint32_t puntero = 0;
	//puntero+=sizeof(uint32_t)*2;
	memcpy(&mmt->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mmt->id = malloc(mmt->size_id);
	memcpy(mmt->id,magic+puntero,mmt->size_id);
	puntero+=mmt->size_id;
	memcpy(&mmt->tamanio,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mmt->flag,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mmt->size_path,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mmt->path = malloc(mmt->size_path);
	memcpy(mmt->path,magic+puntero,mmt->size_path);
	puntero+=mmt->size_path;
	return mmt;
}



muse_sync_t* crear_muse_sync(uint32_t tamanio, char* id, uint32_t direccion){
	muse_sync_t* mst = malloc(sizeof(muse_sync_t));
	mst -> id= string_duplicate(id);
	mst->size_id = strlen(id)+1;
	mst->direccion = direccion;
	mst->tamanio = tamanio;
	return mst;
}

void muse_sync_destroy(muse_sync_t* mst){
	free(mst->id);
	free(mst);
}

void* serializar_muse_sync(muse_sync_t* mst){
	int bytes = sizeof(uint32_t)*3+ mst->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_SYNC;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mst->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mst->id,mst->size_id);
	puntero += mst->size_id;
	memcpy(magic+puntero,&mst->tamanio,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mst->direccion,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	return magic;
}

muse_sync_t* deserializar_muse_sync(void* magic){
	muse_sync_t* mst = malloc(sizeof(muse_sync_t));
	uint32_t puntero = 0;
	memcpy(&mst->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mst->id = malloc(mst->size_id);
	memcpy(mst->id,magic+puntero,mst->size_id);
	puntero+=mst->size_id;
	memcpy(&mst->tamanio,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mst->direccion,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	return mst;
}


muse_unmap_t* crear_muse_unmap(char* id, uint32_t direccion){
	muse_unmap_t* mut = malloc(sizeof(muse_unmap_t));
	mut -> id= string_duplicate(id);
	mut->size_id = strlen(id)+1;
	mut->direccion = direccion;
	return mut;
}

void muse_unmap_destroy(muse_unmap_t* mut){
	free(mut->id);
	free(mut);
}

void* serializar_muse_unmap(muse_unmap_t* mut){
	int bytes = sizeof(uint32_t)*2 + mut->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de mat y 2 int de comando y tamaño
	int comando = MUSE_UNMAP;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&mut->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,mut->id,mut->size_id);
	puntero += mut->size_id;
	memcpy(magic+puntero,&mut->direccion,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	return magic;
}

muse_unmap_t* deserializar_muse_unmap(void* magic){
	muse_unmap_t* mut = malloc(sizeof(muse_unmap_t));
	uint32_t puntero = 0;
	memcpy(&mut->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mut->id = malloc(mut->size_id);
	memcpy(mut->id,magic+puntero,mut->size_id);
	puntero+=mut->size_id;
	memcpy(&mut->direccion,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	return mut;
}
muse_char* crear_muse_char(char* mensaje, uint32_t size_mensaje){
	muse_char* mc = malloc(sizeof(muse_char));
	mc->mensaje = string_duplicate(mensaje);
	mc->size_mensaje = size_mensaje;
	return mc;
}
void muse_char_destroy(muse_char* mc){
	free(mc->mensaje);
	free(mc);
}
void* serializar_muse_char(muse_char* mc){
	int bytes = mc->size_mensaje + sizeof(uint32_t)*3;
	int comando = MUSE_CHAR;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,4);
	puntero += 4;
	memcpy(magic+puntero,&bytes,4);
	puntero += 4;
	memcpy(magic+puntero,&mc->size_mensaje,4);
	puntero += 4;
	memcpy(magic+puntero,mc->mensaje,mc->size_mensaje);
	puntero += mc->size_mensaje;
	return magic;
}
muse_char* deserializar_muse_char(void* magic){
	muse_char* mc = malloc(sizeof(muse_char));
	uint32_t puntero = 0;
	memcpy(&mc->size_mensaje,magic+puntero,4);
	puntero += 4;
	mc->mensaje = malloc(mc->size_mensaje);
	memcpy(mc->mensaje,magic+puntero,mc->size_mensaje);
	puntero += mc->size_mensaje;
	return mc;
}
muse_void* crear_muse_void(void* paquete, uint32_t size_paquete){
	muse_void* mv = malloc(sizeof(muse_void));
	mv->size_paquete = size_paquete;
	mv->paquete = malloc(size_paquete);
	memcpy(mv->paquete,paquete,size_paquete);
	return mv;
}
void muse_void_destroy(muse_void* mv){
	free(mv->paquete);
	free(mv);
}
void* serializar_muse_void(muse_void* mv){
	int bytes = mv->size_paquete + sizeof(uint32_t)*3;
	int comando = MUSE_VOID;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,4);
	puntero += 4;
	memcpy(magic+puntero,&bytes,4);
	puntero += 4;
	memcpy(magic+puntero,&mv->size_paquete,4);
	puntero += 4;
	memcpy(magic+puntero,mv->paquete,mv->size_paquete);
	puntero += mv->size_paquete;
	return magic;
}
muse_void* deserializar_muse_void(void* magic){
	muse_void* mv = malloc(sizeof(muse_void));
	uint32_t puntero = 0;
	memcpy(&mv->size_paquete,magic+puntero,4);
	puntero += 4;
	mv->paquete = malloc(mv->size_paquete);
	memcpy(mv->paquete,magic+puntero,mv->size_paquete);
	puntero += mv->size_paquete;
	return mv;
}

void pruebita(){
	puts("pruebita uwu");
//	raise(11); -> asi se manda un segm fault
//	puts("no llegue?");
}

uint32_t conectar_socket_a(char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM,0);
	if (connect(cliente,(void*) &direccionServidor, sizeof(direccionServidor)) != 0){
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
			string_append(&muse_id,paquete);//guardo el muse_id
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

