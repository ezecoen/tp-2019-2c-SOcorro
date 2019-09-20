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
