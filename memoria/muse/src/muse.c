#include <stdio.h>
#include <stdlib.h>
#include "muse.h"
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

int main(void) {
	tam_mem = 1000;
	init_estructuras();

	//	//aver creamo un segmento xd
//	segmento* seg = malloc(sizeof(segmento));
//	seg->compartido = 0;
//	seg->mmapeado = 0;
//	seg->nombre = string_new();
//	string_append(&seg->nombre,"segmento 1");
//	t_list* paginas = list_create();
//	pagina* pag = malloc(sizeof(pagina));
//	pag->modificado = 0;
//	pag->num_pag = 0;
//	pag->presencia = 1;
//	pag->tamanio_en_uso = 0;
//	pag->pedacito_de_memoria = upcm;
//	list_add(paginas,pag);
//	t_list* heap = list_create();
//	heap_metadata* heap_m = malloc(sizeof(heap_metadata));
//	heap_m->isFree = true;
//	heap_m->size = 100;
//	list_add(heap,heap_m);
//	seg->heap_metadatas = heap;
//	seg->paginas = paginas;

	free_final();
	return EXIT_SUCCESS;
}
void init_estructuras(){
	upcm = malloc(tam_mem);
	tabla_de_segmentos = list_create();
}
void free_final(){
	free(upcm);
	//free tabla de segmentos
}
void muse_alloc(uint32_t tamanio){
	//me fijo si hay lugar disponible
	//busco si ya existe un segmento del que me pidio memoria
	//si no existe creo el segmento
	//agrego las paginas necesarias
}

// Sobre asignacion de memoria
// crear un segmento:
// Leer metadata -> saber si esta vacio o cuanto tamano tiene
// verificar si puede agrandar su tamano
// cambiar el tamano de un segmento a una cantidad de paginas determinada
//o achicarse si libero memoria
// si no pudiera extenderse mas, deberia crear un nuevo segmento
// definir disposicion de segmentos en memoria
//
// Memoria virtual
// El algoritmo de reemplazo será de asignación variable,
// alcance global, utilizando clock modificado
// ver si la pagina esta cargada en memoria
// si no esta, page fault y buscar la pagina
// area de swap -> archivo de swap ,
// muse debe saber siempre cuales pags estan en memoria y cuales en swap
// swap va a estar dividida en paginas para ser indexada
//
// Memoria compartida
// Al tener porciones de memoria compartida mapeada a un archivo diferente
//al de swap, es necesario poder distinguirlas a nivel metadatos.
//Es por eso, que existirán 2 tipos de segmentos en la tabla de segmentos,
//los comunes (o dinámicos) y los compartidos (o “mappeados”):
// segmentos que comparten las mismas paginas IPC
// separar logicamente el arcihvo en paginas
// cambiar bit de presencia
//
