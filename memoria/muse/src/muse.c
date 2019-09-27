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
#include <pthread.h>

int main(int argc, char **argv) {
//	INICIANDO
	path_de_config = string_duplicate(argv[1]);
	iniciar_log(path_de_config);
	leer_config(path_de_config);
	init_estructuras();

//	prueba de dec a bin
	int x=9000;
	char arr[DIR_TAM_DIRECCION+1];
	dec_a_bin(arr, x,DIR_TAM_DIRECCION);
	printf("\nBIN: %s\n", arr);
//	prueba bin a dec
	int res = bin_a_dec(arr);
	printf("DEC: %d",res);
//	prueba creo segmento 1
	muse_alloc_t* mat = crear_muse_alloc(1000,"asdasd");
	int result = muse_alloc(mat);
	printf("\nDireccion virtual de %d|%d|%d: %d",0,0,0,result);
//	prueba creo segmento 2
	muse_alloc_t* mat1 = crear_muse_alloc(1000,"asdasd1");
	int result1 = muse_alloc(mat1);
	printf("\nDireccion virtual de %d|%d|%d: %d",1,0,0,result1);
//	prueba creo segmento 3
	muse_alloc_t* mat2 = crear_muse_alloc(500,"asdasd2");
	int result2 = muse_alloc(mat2);
	printf("\nDireccion virtual de %d|%d|%d: %d",2,0,0,result2);
//	prueba creo segmento 4
	muse_alloc_t* mat3 = crear_muse_alloc(500,"asdasd3");
	int result3 = muse_alloc(mat3);
	printf("\nDireccion virtual de %d|%d|%d: %d",3,0,0,result3);
//	prueba log_2
	int testeo = 0;
	for(int i = 1024;i<16383;i++){
		if(1024<i&&i<=2048){
			if(log_2((double)i)!=11){
				testeo++;
			}
		}
		if(2048<i&&i<=4096){
			if(log_2((double)i)!=12){
				testeo++;
			}
		}
		if(4096<i&&i<=8192){
			if(log_2((double)i)!=13){
				testeo++;
			}
		}
		if(8192<i&&i<=16384){
			if(log_2((double)i)!=14){
				testeo++;
			}
		}
	}
	printf("\nTESTEO = %d",testeo);

	goto end;//jasdjaja
//	SERVIDOR
	uint32_t servidor = crear_servidor(configuracion->puerto);
	while(1){
		esperar_conexion(servidor);
	}
	end:
	free_final();
	return EXIT_SUCCESS;
}
void init_estructuras(){
	upcm = malloc(configuracion->tam_mem);
	tabla_de_segmentos = list_create();
	lugar_disponible = configuracion->tam_mem+configuracion->tam_swap;
	DIR_TAM_DESPLAZAMIENTO = redondear_double_arriba(log_2((double)configuracion->tam_pag));
	DIR_TAM_PAGINA = redondear_double_arriba(log_2((double)(configuracion->tam_mem+configuracion->tam_swap)));
	DIR_TAM_DIRECCION = DIR_TAM_PAGINA*2+DIR_TAM_DESPLAZAMIENTO;
	printf("SEG:%d|PAG:%d|OFF:%d=%d",DIR_TAM_PAGINA,DIR_TAM_PAGINA,DIR_TAM_DESPLAZAMIENTO,DIR_TAM_DIRECCION);
//	0000000000000|0000000000000|00000
}
int log_2(double n){
     int logValue = 0;
     while (n>1) {
         logValue++;
         n /= 2;
     }
     return logValue;
 }
void free_final(){
	free(upcm);
	free(path_de_config);
	//free tabla de segmentos
}
void iniciar_log(char* path){//0 es archivo, 1 es consola
	char* nombre = string_new();
	string_append(&nombre,path);
	string_append(&nombre,".log");
	logg = log_create(nombre,"muse",1,LOG_LEVEL_TRACE);
	free(nombre);
}
s_config* leer_config(char* path){
	g_config = config_create(path);
	configuracion = malloc(sizeof(s_config));

	configuracion->puerto = config_get_int_value(g_config,"LISTEN_PORT");
	log_info(logg,"LISTEN_PORT: %d",configuracion->puerto);

	configuracion->tam_mem = config_get_int_value(g_config,"MEMORY_SIZE");
	log_info(logg,"MEMORY_SIZE: %d",configuracion->tam_mem);

	configuracion->tam_pag = config_get_int_value(g_config,"PAGE_SIZE");
	log_info(logg,"PAGE_SIZE​: %d",configuracion->tam_pag);

	configuracion->tam_swap = config_get_int_value(g_config,"SWAP_SIZE");
	log_info(logg,"SWAP_SIZE​: %d",configuracion->tam_swap);

	config_destroy(g_config);
	return configuracion;
}
void dec_a_bin(char destino[],int dec,int tam){
	if (dec >= 0 && dec <= 65535){
		for(int i = tam-1; i >= 0; i--){
			if (dec % 2 == 0){
				destino[i] = '0';
			}
			else{
				destino[i] = '1';
			}
			dec = dec / 2;
		}
	}
	destino[tam] = '\0';
}
int bin_a_dec(char* binario){
  char *p = binario;
  int   r = 0;

  while (p && *p ) {
    r <<= 1;
    r += (unsigned int)((*p++) & 0x01);
  }

  return (int)r;
}
int redondear_double_arriba(double d){
	if(d-(int)d!=0){
		return (int)d + 1;
	}
	else{
		return (int)d;
	}
}
int muse_alloc(muse_alloc_t* datos){
	//me fijo si hay lugar disponible
	if(lugar_disponible>=datos->tamanio){
		//busco si ya existe un segmento del que me pidio memoria
		segmento* segmento_buscado = buscar_segmento_por_id(datos->id);
		uint32_t direccion_return = 0;
		if(segmento_buscado==NULL){//hay q crear el segmento
			uint32_t cantidad_de_paginas = paginas_necesarias_para_tamanio(datos->tamanio);
			segmento* segmento_nuevo = malloc(sizeof(segmento));
			segmento_nuevo->compartido = false;
			segmento_nuevo->mmapeado = false;
			segmento_nuevo->nombre = string_duplicate(datos->id);
			segmento_nuevo->paginas = reservar_paginas(cantidad_de_paginas);
			if(segmento_nuevo->paginas == NULL){
				return 0;
			}
			segmento_nuevo->num_segmento = list_size(tabla_de_segmentos);
			list_add(tabla_de_segmentos,segmento_nuevo);
			direccion_return = obtener_direccion_virtual(segmento_nuevo->num_segmento,0,0);
		}
		else{//=>se encontro un segmento que pertenece a ese id...
			//primero me fijo si el muse_alloc entra en el lugar libre del segmento
			int reserva = reservar_lugar_en_segmento(segmento_buscado,datos->tamanio);
			//terminar xd!!
		}
		//retorno la direccion de memoria (virtual) que le asigne
		return direccion_return;
	}
	else{//no hay lugar
		return 0;
	}
}
segmento* buscar_segmento_por_id(char* id){
	_Bool segmento_igual(segmento* seg){
		return string_equals_ignore_case(seg->nombre,id);
	}
	return list_find(tabla_de_segmentos,(void*)segmento_igual);
}
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio){
	uint32_t pags = tamanio/configuracion->tam_pag;
	if(tamanio%configuracion->tam_pag>0){
		pags++;
	}
	return pags;
}
t_list* reservar_paginas(uint32_t cantidad_de_paginas){
	t_list* paginas = list_create();
	if(lugar_disponible>=cantidad_de_paginas*configuracion->tam_pag){
		lugar_disponible-=cantidad_de_paginas*configuracion->tam_pag;
		//solo resto el tamaño, total las paginas solo se cargan en memoria cuando se necesiten
		for(int i = 0;i<cantidad_de_paginas;i++){
			pagina* pag = malloc(sizeof(pagina));
			pag->presencia = false;
			pag->modificado = false;
			pag->num_pagina = i;
			pag->ultimo_heap_metadata_libre = 0;
			pag->datos=asignar_marco_nuevo();
			list_add(paginas,pag);
		}
		return paginas;
	}
	else{
		//se pudre to2 xd?
		return NULL;
	}
}
int reservar_lugar_en_segmento(segmento* seg,uint32_t tamanio){//hay q marcar la pag como modificada???
	int direccion_return;
	_Bool ultima_pagina(pagina* pag){
		return pag->ultimo_heap_metadata_libre!=-1;
		//si es != -1 entonces es la ultima pagina del segmento
	}
	pagina* pagina_buscada = list_find(seg->paginas,(void*)ultima_pagina);
	heap_metadata* ultimo_heap = pagina_buscada->datos+pagina_buscada->ultimo_heap_metadata_libre;
	if(ultimo_heap->size >= tamanio+sizeof(heap_metadata)){//=>entra en este segmento
		heap_metadata* heap_nuevo = malloc(sizeof(heap_metadata));
		heap_nuevo->isFree = true;
		heap_nuevo->size = ultimo_heap->size-tamanio;
		memcpy(pagina_buscada->datos + pagina_buscada->ultimo_heap_metadata_libre + tamanio,
				heap_nuevo,sizeof(heap_metadata));
		ultimo_heap->isFree = false;
		ultimo_heap->size = tamanio;
		int offset = configuracion->tam_pag-heap_nuevo->size-sizeof(heap_metadata)-tamanio;
		direccion_return = obtener_direccion_virtual(seg->num_segmento,pagina_buscada->num_pagina,offset);
	}
	else{//=>hay que agrandar el segmento
		uint32_t lugar_extra_necesario = tamanio-ultimo_heap->size-sizeof(heap_metadata);
		uint32_t paginas_necesarias = paginas_necesarias_para_tamanio(lugar_extra_necesario);
		uint32_t tamanio_paginas_necesarias = paginas_necesarias*configuracion->tam_pag;
		if(lugar_disponible >= tamanio_paginas_necesarias){//=>hay lugar, reservo las nuevas pags
			lugar_disponible -= tamanio_paginas_necesarias;
			//agrego las pagas a la lista del segmento
			int espacio_libre_ultima_pag = tamanio_paginas_necesarias-tamanio-ultimo_heap->size;
			heap_metadata* heap_nuevo = malloc(sizeof(heap_metadata));
			heap_nuevo->isFree = true;
			heap_nuevo->size = espacio_libre_ultima_pag;
			_Bool agarrar_direccion_de_primera_pag_agregada = false;
			if(ultimo_heap->size==0){
				agarrar_direccion_de_primera_pag_agregada = true;
			}
			for(int i = 0;i<paginas_necesarias;i++){
				pagina* pagina_nueva = malloc(sizeof(pagina*));
				pagina_nueva->presencia = false;
				pagina_nueva->modificado = false;
				pagina_nueva->num_pagina = pagina_buscada->num_pagina+i;
				pagina_nueva->datos = asignar_marco_nuevo();
				if(i==paginas_necesarias-1){
					//es la ultima pag
					pagina_nueva->ultimo_heap_metadata_libre =
							configuracion->tam_pag - espacio_libre_ultima_pag;
					memcpy(pagina_nueva->datos+pagina_nueva->ultimo_heap_metadata_libre,
							heap_nuevo,sizeof(heap_metadata));
				}
				else{
					pagina_nueva->ultimo_heap_metadata_libre = -1;
					//porque la pag va a estar llena
				}
				list_add(seg->paginas,pagina_nueva);
				if(agarrar_direccion_de_primera_pag_agregada){
					if(i==0){
						direccion_return = obtener_direccion_virtual(
								seg->num_segmento,pagina_nueva->num_pagina,0);
					}
				}
			}
			if(!agarrar_direccion_de_primera_pag_agregada){//=>tengo que llenar direccion_return
				direccion_return = obtener_direccion_virtual(seg->num_segmento,pagina_buscada->num_pagina,
						pagina_buscada->ultimo_heap_metadata_libre+sizeof(heap_metadata));
			}
			ultimo_heap->isFree = false;
			ultimo_heap->size = tamanio;
		}
		else{//=>no hay lugar
			return -1;
		}
	}
	return direccion_return;
}
void* asignar_marco_nuevo(){
	//hay que codear esto!!
}
int obtener_direccion_virtual(uint32_t num_segmento,uint32_t num_pagina, uint32_t offset){
	//num_seg-num_pag-offset-\0
	char resultado[DIR_TAM_DIRECCION+1];
	char char_segmento[DIR_TAM_PAGINA+1];
	char char_pagina[DIR_TAM_PAGINA+1];
	char char_offset[DIR_TAM_DESPLAZAMIENTO+1];
	dec_a_bin(char_segmento,num_segmento,DIR_TAM_PAGINA);
	dec_a_bin(char_pagina,num_pagina,DIR_TAM_PAGINA);
	dec_a_bin(char_offset,offset,DIR_TAM_DESPLAZAMIENTO);
	resultado[DIR_TAM_DIRECCION]='\0';//xq string termina con \0

	int j=1;
//	EJEMPLO:
//	  SEG     PAG   OFF
//	   2   |   5   | 0
//	0000 10|00 0101|0000|\0
//	TAM_DIR = 16, TAM_SEG=TAM_PAG=6,TAM_DESP=4
//
	for(int i =1;i<=DIR_TAM_DESPLAZAMIENTO;i++,j++){
		resultado[DIR_TAM_DIRECCION-j]=char_offset[DIR_TAM_DESPLAZAMIENTO-i];
	}
	for(int i =1;i<=DIR_TAM_PAGINA;i++,j++){
		resultado[DIR_TAM_DIRECCION-j]=char_pagina[DIR_TAM_PAGINA-i];
	}
	for(int i =1;i<=DIR_TAM_PAGINA;i++,j++){
		resultado[DIR_TAM_DIRECCION-j]=char_segmento[DIR_TAM_PAGINA-i];
	}
	int resultado_decimal = bin_a_dec(resultado);
	return resultado_decimal;
}
int muse_free(muse_free_t* datos){
	return 0;
}

int muse_get(muse_get_t* datos){
	return 0;
}
int muse_cpy(muse_cpy_t* datos){
	return 0;
}
int muse_map(muse_map_t* datos){
	return 0;
}
int muse_sync(muse_sync_t* datos){
	return 0;
}
int muse_unmap(muse_unmap_t* datos){
	return 0;
}
void esperar_conexion(uint32_t servidor){
	uint32_t socket = aceptar_cliente(servidor);
	pthread_t cliente;
	puts("creo hilo");
	pthread_create(&cliente,NULL,(void*)ocupate_de_este,(void*)socket);
	pthread_detach(cliente);
}
void ocupate_de_este(int socket){
	_Bool exit_loop = false;
	uint32_t operacion;
	while(recv(socket,&operacion,4,MSG_WAITALL)!=-1 && exit_loop==false){
		switch (operacion) {
			case MUSE_INIT:;
				//recibo int pid, crep el char* id y se lo mando
				uint32_t pid;
				recv(socket,&pid,4,0);
				char* pid_char = string_itoa(pid);
				//hay q conseguir la ip de el cliente xd
				char* id = string_new();
				struct sockaddr_in addr;
				uint32_t addrlen = sizeof(addr);
				getpeername(socket, (struct sockaddr *)&addr, &addrlen);
				char* ip = inet_ntoa(addr.sin_addr);
				string_append(&id,ip);
				string_append(&id,"-");
				string_append(&id,pid_char);
				log_info(logg,"nuevo cliente, id: %s",id);
				mandar_char(id,socket,operacion);
				//hay q liberar lo de sockaddr_in?
				free(pid_char);
				free(id);
				break;
			case MUSE_ALLOC:;
				uint32_t tam;
				recv(socket,&tam,4,0);
				void* vmat = malloc(tam);
				recv(socket,vmat,tam,0);
				muse_alloc_t* datos = deserializar_muse_alloc(vmat);
				uint32_t resultado = muse_alloc(datos);
				send(socket,&resultado,4,0);
				break;
			case MUSE_FREE:

				break;
			case MUSE_GET:

				break;
			case MUSE_CPY:

				break;
			case MUSE_MAP:

				break;
			case MUSE_SYNC:

				break;
			case MUSE_UNMAP:

				break;
			case MUSE_CLOSE:
				//somethin
				exit_loop = true;
				break;
			default:
				//xd?
				break;
		}
	}
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
