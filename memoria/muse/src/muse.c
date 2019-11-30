#include "muse.h"

int main(int argc, char **argv) {
//	INICIANDO
	char* path_de_config = string_duplicate(argv[1]);
	path_swap = string_duplicate(argv[0]);
	iniciar_log(path_de_config);
	leer_config(path_de_config);
	init_estructuras(path_swap);
	log_info(log_metricas,"<<<<<<<<<<<<NUEVA EJECUCION>>>>>>>>>>>");

//	programa_t* programa2 = malloc(sizeof(programa_t));
//	programa2->tabla_de_segmentos = list_create();
//	programa2->id_programa = string_new();
//	string_append(&programa2->id_programa,"prog2");
//	list_add(tabla_de_programas,programa2);

//	programa_t* programa = malloc(sizeof(programa_t));
//	programa->tabla_de_segmentos = list_create();
//	programa->id_programa = string_new();
//	string_append(&programa->id_programa,"prog");
//	list_add(tabla_de_programas,programa);

//	muse_cpy_t* mcp = crear_muse_cpy(15,"prog2",10,"uertyehdejroi2u");
//	muse_cpy(mcp);
//
//	muse_alloc_t* mat = crear_muse_alloc(100,"prog2");
//	int resu = muse_alloc(mat);
//	log_info(logg,"Direccion virtual del mat: %d",resu);
//
//	char* blah = "habia una vez 11111111111111111111112222222222222222333333";
//	muse_cpy_t* mcp1 = crear_muse_cpy(string_length(blah),"prog2",10,blah);
//	muse_cpy(mcp1);
//
//	muse_get_t* mgt = crear_muse_get(string_length(blah),"prog2",25);
//	void* algo = muse_get(mgt);
//	log_info(logg,"%s",(char*)algo);
//
//	muse_cpy_t* mcp2 = crear_muse_cpy(15,"prog2",25,"heystrregdtahsye");
//	muse_cpy(mcp2);
//
//	muse_get_t* mgt2 = crear_muse_get(string_length(blah),"prog2",10);
//	algo = muse_get(mgt2);
//	//log_info(logg,"%s",(char*)algo);
//
//	muse_alloc_t* mat2 = crear_muse_alloc(100,"prog2");
//	int resu2 = muse_alloc(mat2);
//	//log_info(logg,"Direccion virtual del mat: %d",resu2);
//
//	muse_alloc_t* mat3 = crear_muse_alloc(100,"prog2");
//	int resu3 = muse_alloc(mat3);
//	log_info(logg,"Direccion virtual del mat: %d",resu3);
//
//	muse_map_t* mmt = crear_muse_map(300,"prog2",MAP_SHARED,"/home/utnso/tp-2019-2c-SOcorro/memoria/ejemplo_map");
//	int puntero_map = muse_map(mmt);
//	log_info(logg,"Direccion virtual del map: %d",puntero_map);
//
//	muse_get_t* mgt3 = crear_muse_get(305,"prog2",340);
//	algo = muse_get(mgt3);
//	//log_info(logg,"%s",(char*)algo);
//
//	muse_map_t* mmt1 = crear_muse_map(100,"prog",MAP_SHARED,"/home/utnso/tp-2019-2c-SOcorro/memoria/ejemplo_map");
//	int puntero_map1 = muse_map(mmt1);
//	log_info(logg,"Direccion virtual del map: %d",puntero_map1);
//
//	muse_unmap_t* mut1 = crear_muse_unmap("prog",puntero_map1);
//	int puntero_unmap1 = muse_unmap(mut1);
//	log_info(logg,"Unmap %d",puntero_unmap1);
//
//	muse_alloc_t* mat4 = crear_muse_alloc(100,"prog2");
//	int resu4 = muse_alloc(mat4);
//	log_info(logg,"Direccion virtual del mat: %d",resu4);
//
//	muse_free_t* mft = crear_muse_free("prog2",resu3);
//	muse_free(mft);
//
//	muse_free_t* mft1 = crear_muse_free("prog",resu);
//	muse_free(mft1);
//
//	muse_free_t* mft2 = crear_muse_free("prog2",resu2);
//	muse_free(mft2);
//
//	muse_alloc_t* mat5 = crear_muse_alloc(311,"prog2");
//	int resu5 = muse_alloc(mat5);
//	log_info(logg,"Direccion virtual del mat: %d",resu5);
//
//
//	muse_close("prog2");
//	muse_close("prog");

//	metricas();
////
//return 0;
//	printear_memoria();
//	SERVIDOR
	uint32_t servidor = crear_servidor(configuracion->puerto);
	while(true){
		esperar_conexion(servidor);
	}

	return EXIT_SUCCESS;
}
void init_estructuras(char* path){
	upcm = malloc(configuracion->tam_mem);
//	swap = malloc(configuracion->tam_swap);//provisorio
	iniciar_memoria_virtual(path);
	lugar_disponible = configuracion->tam_mem+configuracion->tam_swap;
	tabla_de_programas = list_create();
	tabla_de_mapeo = list_create();
	tabla_de_memoria_liberada=list_create();
	tabla_de_memoria_pedida=list_create();
	CANT_PAGINAS_MEMORIA = configuracion->tam_mem/configuracion->tam_pag;
	CANT_PAGINAS_MEMORIA_VIRTUAL = configuracion->tam_swap/configuracion->tam_pag;
	init_semaforos();
	init_bitarray();
	posicion_puntero_clock = 0;
}
void printear_memoria(){
	for(int i = 0;i<configuracion->tam_mem;i++){
		if(*(char*)(upcm+i)!='\0'){
			printf("%c",*(char*)(upcm+i));
		}
	}
	printf("\n");
	fflush(stdout);
}
void iniciar_memoria_virtual(char* path_swap){
	int i =strlen(path_swap);
	for(;i>=0;i--)
	{
		if (path_swap[i]=='/')
		{
			break;
		}
	}
	char* aux = string_substring_until(path_swap,i);
	free(path_swap);
	path_swap = string_new();
	string_append(&path_swap,aux);
	string_append(&path_swap,"/SwappingArea");
	log_info(logg,"path swap: %s",path_swap);
	free(aux);


	fd = open(path_swap,O_RDWR|O_CREAT,0777);
	if(fd<0){
		log_info(logg,"no se pudo abrir el archivo de swap");
	}
	ftruncate(fd,0);
	ftruncate(fd,configuracion->tam_swap);
	//creo que si funciona!!
	swap = mmap(NULL, configuracion->tam_swap, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
	if(swap == MAP_FAILED || swap == NULL){
		perror("error: ");
	}
	
}
void init_semaforos(){
	pthread_mutex_init(&mutex_lugar_disponible,NULL);
	pthread_mutex_init(&mutex_tabla_de_programas,NULL);
	pthread_mutex_init(&mutex_tabla_de_mapeo,NULL);
	pthread_mutex_init(&mutex_bitarray,NULL);
}
int log_2(double n){
	//testea2
	//redondea el valor hacia arriba y funciona siempre bien con los bits
     int logValue = 0;
     while (n>1){
         logValue++;
         n /= 2;
     }
     return logValue;
}
void iniciar_log(char* path){//0 es archivo, 1 es consola
	char* nombre = string_new();
	string_append(&nombre,path);
	string_append(&nombre,".log");
	logg = log_create(nombre,"muse",1,LOG_LEVEL_TRACE);
	free(nombre);

	nombre = string_new();
	string_append(&nombre,"metricas.log");
	log_metricas = log_create(nombre,"muse",0,LOG_LEVEL_TRACE);
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

	configuracion->ip = string_duplicate(config_get_string_value(g_config,"IP"));
	log_info(logg,"IP: %s",configuracion->ip);

	config_destroy(g_config);
	return configuracion;
}
int redondear_double_arriba(double d){
	//testea2
	if(d-(int)d!=0){
		return (int)d + 1;
	}
	else{
		return (int)d;
	}
}
t_list* traer_tabla_de_segmentos(char* id_programa){
	_Bool id_programa_igual(programa_t* programa){
		return string_equals_ignore_case(programa->id_programa,id_programa);
	}
	pthread_mutex_lock(&mutex_tabla_de_programas);
	programa_t* programa_buscado = list_find(tabla_de_programas,(void*)id_programa_igual);
	pthread_mutex_unlock(&mutex_tabla_de_programas);
	if(programa_buscado==NULL)
	{
		return NULL;
	}
	return programa_buscado->tabla_de_segmentos;
}
int muse_alloc(muse_alloc_t* datos){
//me fijo si hay lugar disponible
int direccion_return = -1;
pthread_mutex_lock(&mutex_lugar_disponible);
if(lugar_disponible >= datos->tamanio+sizeof(heap_metadata)){
	pthread_mutex_unlock(&mutex_lugar_disponible);
	//busco si ya tengo algun segmento
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(tabla_de_segmentos==NULL){
		log_info(logg,"%s no tiene tabla de segmentos",datos->id);
		return -1;
	}

	if(list_is_empty(tabla_de_segmentos)){
		//hay que crear el 1er segmento
		uint32_t cantidad_de_paginas = paginas_necesarias_para_tamanio(datos->tamanio+sizeof(heap_metadata)*2);
		int espacio_libre_ultima_pag = cantidad_de_paginas*configuracion->tam_pag-datos->tamanio-sizeof(heap_metadata)*2;
		pthread_mutex_lock(&mutex_lugar_disponible);
		if(lugar_disponible>=cantidad_de_paginas*configuracion->tam_pag){
			lugar_disponible -= cantidad_de_paginas*configuracion->tam_pag;
			pthread_mutex_unlock(&mutex_lugar_disponible);
			segmento* segmento_nuevo = malloc(sizeof(segmento));
			segmento_nuevo->compartido = false;
			segmento_nuevo->mmapeado = false;
			segmento_nuevo->base_logica = 0;
			segmento_nuevo->paginas_liberadas=0;
			segmento_nuevo->tamanio = cantidad_de_paginas*configuracion->tam_pag;
			segmento_nuevo->info_heaps = list_create();
			heap_lista* heap_inicial_lista = malloc(sizeof(heap_lista));
			heap_inicial_lista->direccion_heap_metadata = 0;
			heap_inicial_lista->espacio = datos->tamanio;
			heap_inicial_lista->is_free = false;
			heap_inicial_lista->indice = 0;
			heap_lista* heap_final_lista = malloc(sizeof(heap_lista));
			heap_final_lista->direccion_heap_metadata = datos->tamanio+sizeof(heap_metadata);
			heap_final_lista->is_free = true;
			heap_final_lista->espacio = espacio_libre_ultima_pag;
			heap_final_lista->indice = 1;
			list_add(segmento_nuevo->info_heaps,heap_inicial_lista);
			list_add(segmento_nuevo->info_heaps,heap_final_lista);//agrego heap de inicio y fin
			t_list* paginas = list_create();
			_Bool hay_que_entrar_en_la_anteultima_pag = false;
			int heap_en_ultima_pagina,heap_en_ante_ultima_pagina,offset_heap;
			heap_metadata* heap_al_final = malloc(sizeof(heap_metadata));
			heap_al_final->is_free = true; //es el ultimo
			heap_al_final->size = espacio_libre_ultima_pag;
			if(espacio_libre_ultima_pag>configuracion->tam_pag-sizeof(heap_metadata)) {
				// significa que el heap esta entre las ultimas dos paginas,
				// hay que agregarselas en el for
				heap_en_ultima_pagina = configuracion->tam_pag-espacio_libre_ultima_pag;
				heap_en_ante_ultima_pagina = sizeof(heap_metadata)-heap_en_ultima_pagina;
				offset_heap = configuracion->tam_pag-heap_en_ante_ultima_pagina;
				hay_que_entrar_en_la_anteultima_pag = true;
			}
			else{
				//el heap_m entra completo en la ultima pag
				offset_heap = configuracion->tam_pag-espacio_libre_ultima_pag-sizeof(heap_metadata);
			}
			for(int i = 0;i<cantidad_de_paginas;i++){
				pagina* pag = malloc(sizeof(pagina));
				pag->num_pagina = i;
				pag->presencia = true;
				if(i <= configuracion->tam_mem / configuracion->tam_pag){
					pag->bit_marco = asignar_marco_nuevo();
					pag->bit_marco->bit_uso = true;
					pag->bit_marco->bit_modificado = true;
					pag->bit_swap = NULL;
				}
				else{
					pag->bit_marco = NULL;
					pag->bit_swap = bit_libre_memoria_virtual();
					pag->presencia = false;
				}
				if(i == 0){//si es la 1ra => hay que agregar el heap al inicio
					heap_metadata* heap_nuevo = malloc(sizeof(heap_metadata));
					heap_nuevo->is_free = false;
					heap_nuevo->size = datos->tamanio;
					void* puntero_a_marco = obtener_puntero_a_marco(pag);
					memcpy(puntero_a_marco,heap_nuevo,sizeof(heap_metadata));
					free(heap_nuevo);
				}
				if(i == cantidad_de_paginas-2){
					if(hay_que_entrar_en_la_anteultima_pag){
						void* puntero_a_marco = obtener_puntero_a_marco(pag);
						memcpy(puntero_a_marco+offset_heap,heap_al_final,heap_en_ante_ultima_pagina);
					}
				}
				if(i == cantidad_de_paginas-1){
					void* puntero_a_marco = obtener_puntero_a_marco(pag);
					if(hay_que_entrar_en_la_anteultima_pag){
						memcpy(puntero_a_marco,heap_al_final+heap_en_ante_ultima_pagina,heap_en_ultima_pagina);
					}
					else{
						memcpy(puntero_a_marco+offset_heap,heap_al_final,sizeof(heap_metadata));
					}
				}
				list_add(paginas,pag);
			}
			free(heap_al_final);
			segmento_nuevo->paginas = paginas;
			segmento_nuevo->num_segmento = list_size(tabla_de_segmentos);
			list_add(tabla_de_segmentos,segmento_nuevo);
			direccion_return = sizeof(heap_metadata);//xq es el primer muse_alloc del programa
		}
		else{
			//no hay lugar en el sistema
			pthread_mutex_unlock(&mutex_lugar_disponible);
			return -1;
		}
	}
	else{
		//tabla de segmentos no esta vacia => hay que buscar si entra en algun segmento ya existente
		//tenes la lista de heaps con espacios para recorrer un segmento y ver donde tiene espacio libre

		segmento* segmento_buscado = buscar_segmento_con_espacio(tabla_de_segmentos,datos->tamanio+sizeof(heap_metadata));
		if(segmento_buscado != NULL){
			//hay lugar en segmento_buscado, pero tengo que buscar esa direccion donde poner los datos
			_Bool tiene_lugar_disponible(heap_lista* heap){//por cada heap de un seg se fija si tiene espacio
				if(heap->is_free && heap->espacio >= datos->tamanio){
					return true;
				}
				return false;
			}
			heap_lista* heap_lista_encontrado = list_find(segmento_buscado->info_heaps,(void*)tiene_lugar_disponible);

			//aca creo el heap que va a actualizar el heap anterior
			heap_metadata* heap_inicial = malloc(sizeof(heap_metadata));
			heap_inicial->is_free = false;
			heap_inicial->size = datos->tamanio;
			int num_pagina_inicial = heap_lista_encontrado->direccion_heap_metadata / configuracion->tam_pag;
			int offset_inicial = heap_lista_encontrado->direccion_heap_metadata % configuracion->tam_pag;

			if(offset_inicial > configuracion->tam_pag-sizeof(heap_metadata)){//me fijo si va a quedar en la mitad
				//queda en la mitad >:(
				int tamanio_a_copiar = configuracion->tam_pag-offset_inicial;

				pagina* pagina_inicial = list_get(segmento_buscado->paginas,num_pagina_inicial);
				void* marco_inicial = obtener_puntero_a_marco(pagina_inicial);
				memcpy(marco_inicial+offset_inicial,heap_inicial,tamanio_a_copiar);

				pagina* pagina_inicial2 = list_get(segmento_buscado->paginas,num_pagina_inicial+1);
				void* marco_inicial2 = obtener_puntero_a_marco(pagina_inicial2);
				memcpy(marco_inicial2,heap_inicial+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);

				direccion_return = segmento_buscado->base_logica + pagina_inicial2->num_pagina*configuracion->tam_pag
						+ sizeof(heap_metadata)-tamanio_a_copiar;
			}
			else{
				//no queda en la mitad :D
				pagina* pagina_inicial = list_get(segmento_buscado->paginas,num_pagina_inicial);
				void* marco_inicial = obtener_puntero_a_marco(pagina_inicial);
				memcpy(marco_inicial+offset_inicial,heap_inicial,sizeof(heap_metadata));
				direccion_return = segmento_buscado->base_logica + pagina_inicial->num_pagina*configuracion->tam_pag
						+ offset_inicial+sizeof(heap_metadata);
			}

			//aca creo el heap que va a ir despues de los datos
			heap_metadata* heap_final = malloc(sizeof(heap_metadata));
			heap_final->is_free = true;
			heap_final->size = heap_lista_encontrado->espacio-datos->tamanio-sizeof(heap_metadata);
			int num_pagina_final = (heap_lista_encontrado->direccion_heap_metadata+datos->tamanio +
					sizeof(heap_metadata)) / configuracion->tam_pag;
			int offset_final = (heap_lista_encontrado->direccion_heap_metadata+datos->tamanio +
					sizeof(heap_metadata)) % configuracion->tam_pag;

			if(offset_final > configuracion->tam_pag-sizeof(heap_metadata)){//me fijo si va a quedar en la mitad
				//queda en la mitad >:(
				int tamanio_a_copiar = configuracion->tam_pag-offset_final;

				pagina* pagina_final = list_get(segmento_buscado->paginas,num_pagina_final);
				void* marco_final = obtener_puntero_a_marco(pagina_final);
				memcpy(marco_final+offset_final,heap_final,tamanio_a_copiar);

				pagina* pagina_final2 = list_get(segmento_buscado->paginas,num_pagina_final+1);
				void* marco_final2 = obtener_puntero_a_marco(pagina_final2);
				memcpy(marco_final2,heap_final+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);
			}
			else{
				//no queda en la mitad :D
				pagina* pagina_final = list_get(segmento_buscado->paginas,num_pagina_final);
				void* marco_final = obtener_puntero_a_marco(pagina_final);
				memcpy(marco_final+offset_inicial,heap_final,sizeof(heap_metadata));
			}
			heap_lista_encontrado->espacio = datos->tamanio;
			heap_lista_encontrado->is_free = false;

			heap_lista* heap_lista_nuevo = malloc(sizeof(heap_lista));
			heap_lista_nuevo->direccion_heap_metadata = num_pagina_final*configuracion->tam_pag + offset_final;
			heap_lista_nuevo->espacio = heap_final->size;
			heap_lista_nuevo->is_free = true;
			heap_lista_nuevo->indice=heap_lista_encontrado->indice+1;
			//agrego el nuevo heap_lista dsps del anterior
			list_add_in_index(segmento_buscado->info_heaps,heap_lista_encontrado->indice+1,heap_lista_nuevo);

			free(heap_inicial);
			free(heap_final);
		}
		else{
			//hay que ver si hay algun segmento con paginas liberadas tal que entre lo que
			//quiero guardar
			segmento* seg = buscar_segmento_con_paginas_liberadas(datos->tamanio,tabla_de_segmentos);
			if(seg!=NULL){
				heap_lista* heap_lista_ultimo = list_last_element(seg->info_heaps);
				uint32_t paginas_necesarias = paginas_necesarias_para_tamanio(datos->tamanio+sizeof(heap_metadata)-heap_lista_ultimo->espacio);
				pthread_mutex_lock(&mutex_lugar_disponible);
				if(lugar_disponible>=paginas_necesarias*configuracion->tam_pag){
					lugar_disponible-=paginas_necesarias*configuracion->tam_pag;
					pthread_mutex_unlock(&mutex_lugar_disponible);
					uint32_t numero_proxima_pagina = seg->paginas->elements_count;
					for(int i = 0;i<paginas_necesarias;i++){
						pagina* pagina_nueva = malloc(sizeof(pagina));
						pagina_nueva->num_pagina = numero_proxima_pagina+i;
						pagina_nueva->presencia = true;
						if(i <= configuracion->tam_mem / configuracion->tam_pag){
							pagina_nueva->bit_marco = asignar_marco_nuevo();
							pagina_nueva->bit_marco->bit_uso = true;
							pagina_nueva->bit_marco->bit_modificado = true;
							pagina_nueva->bit_swap = NULL;
						}
						else{
							pagina_nueva->bit_marco = NULL;
							pagina_nueva->bit_swap = bit_libre_memoria_virtual();
							pagina_nueva->presencia = false;
						}
						list_add(seg->paginas,pagina_nueva);
					}
					seg->paginas_liberadas-=paginas_necesarias;
					seg->tamanio=seg->paginas->elements_count*configuracion->tam_pag;

					heap_lista_ultimo->espacio=datos->tamanio;
					heap_lista_ultimo->is_free=false;

					heap_lista* heap_lista_nuevo = malloc(sizeof(heap_lista));
					heap_lista_nuevo->direccion_heap_metadata=heap_lista_ultimo->direccion_heap_metadata+sizeof(heap_metadata)+datos->tamanio;
					heap_lista_nuevo->is_free=true;
					heap_lista_nuevo->indice=heap_lista_ultimo->indice+1;
					int espacio_nuevo = (heap_lista_nuevo->direccion_heap_metadata+sizeof(heap_metadata))%configuracion->tam_pag;
					heap_lista_nuevo->espacio=configuracion->tam_pag-espacio_nuevo;
					list_add(seg->info_heaps,heap_lista_nuevo);

					heap_metadata* heap_metadata_ultimo = malloc(sizeof(heap_metadata));
					heap_metadata_ultimo->is_free=false;
					heap_metadata_ultimo->size=heap_lista_ultimo->espacio;
					reemplazar_heap_en_memoria(heap_lista_ultimo,seg,heap_metadata_ultimo);

					heap_metadata* heap_metadata_nuevo = malloc(sizeof(heap_metadata));
					heap_metadata_nuevo->is_free=true;
					heap_metadata_nuevo->size=heap_lista_nuevo->espacio;
					reemplazar_heap_en_memoria(heap_lista_nuevo,seg,heap_metadata_nuevo);


					direccion_return = seg->base_logica+sizeof(heap_metadata)+
							heap_lista_ultimo->direccion_heap_metadata;

				}
				else{
					pthread_mutex_unlock(&mutex_lugar_disponible);
					//no hay espacio disponible
					return -1;
				}

			}else{
			//no entra en ningun segmento existente
				segmento* ultimo_segmento = list_get(tabla_de_segmentos,list_size(tabla_de_segmentos)-1);
				if(!ultimo_segmento->mmapeado){
					//hay que agrandar el segmento
					heap_lista* lista_ultimo_heap = list_last_element(ultimo_segmento->info_heaps);
					lista_ultimo_heap->is_free=false;
					pagina* ultima_pagina = list_last_element(ultimo_segmento->paginas);
					heap_metadata* nuevo_ultimo_heap = malloc(sizeof(heap_metadata));
					nuevo_ultimo_heap->is_free=true;
					heap_metadata* ultimo_heap = malloc(sizeof(heap_metadata));
					ultimo_heap->is_free=false;
					ultimo_heap->size =datos->tamanio;
					uint32_t lugar_extra_necesario = datos->tamanio-lista_ultimo_heap->espacio+sizeof(heap_metadata);
					uint32_t paginas_necesarias = paginas_necesarias_para_tamanio(lugar_extra_necesario);
					uint32_t tamanio_paginas_necesarias = paginas_necesarias*configuracion->tam_pag;
					pthread_mutex_lock(&mutex_lugar_disponible);
					if(lugar_disponible >= tamanio_paginas_necesarias){//=>hay lugar, reservo las nuevas pags
						lugar_disponible -= tamanio_paginas_necesarias;
						pthread_mutex_unlock(&mutex_lugar_disponible);
						ultimo_segmento->tamanio+=tamanio_paginas_necesarias;
						//agrego las pags a la lista del segmento
						int espacio_libre_ultima_pag = tamanio_paginas_necesarias-datos->tamanio
								- sizeof(heap_metadata)+lista_ultimo_heap->espacio;
						//actualizo el ex ultimo heap
						lista_ultimo_heap->espacio=datos->tamanio;
						nuevo_ultimo_heap->size = espacio_libre_ultima_pag;

						for(int i = 0;i<paginas_necesarias;i++){
							pagina* pagina_nueva = malloc(sizeof(pagina));
							pagina_nueva->num_pagina = ultima_pagina->num_pagina+1+i;
							pagina_nueva->presencia = true;
							if(i <= configuracion->tam_mem / configuracion->tam_pag){
								pagina_nueva->bit_marco = asignar_marco_nuevo();
								pagina_nueva->bit_marco->bit_uso = true;
								pagina_nueva->bit_marco->bit_modificado = true;
								pagina_nueva->bit_swap = NULL;
							}
							else{
								pagina_nueva->bit_marco = NULL;
								pagina_nueva->bit_swap = bit_libre_memoria_virtual();
								pagina_nueva->presencia = false;
							}
							list_add(ultimo_segmento->paginas,pagina_nueva);

						} //termina el for
						//Agrego el heap_lista nuevo a info_heaps
						heap_lista* heap_lista_nuevo = malloc(sizeof(heap_lista));
						heap_lista_nuevo->direccion_heap_metadata=lista_ultimo_heap->direccion_heap_metadata+datos->tamanio+sizeof(heap_metadata);
						heap_lista_nuevo->espacio=espacio_libre_ultima_pag;
						heap_lista_nuevo->indice=lista_ultimo_heap->indice+1;
						heap_lista_nuevo->is_free=true;
						list_add_in_index(ultimo_segmento->info_heaps,heap_lista_nuevo->indice,heap_lista_nuevo);

						//verificar donde queda el heap nuevo
						int nuevo_offset_heap_al_marco = heap_lista_nuevo->direccion_heap_metadata%configuracion->tam_pag;
						int num_pagina_final = heap_lista_nuevo->direccion_heap_metadata / configuracion->tam_pag;
						if(nuevo_offset_heap_al_marco > configuracion->tam_pag-sizeof(heap_metadata)){
							//significa que quedo en el medio
							int tamanio_a_copiar = configuracion->tam_pag-nuevo_offset_heap_al_marco;
							pagina* pagina_final = list_get(ultimo_segmento->paginas,num_pagina_final);
							void* marco_final = obtener_puntero_a_marco(pagina_final);
							memcpy(marco_final+nuevo_offset_heap_al_marco,nuevo_ultimo_heap,tamanio_a_copiar);

							pagina* pagina_final2 = list_get(ultimo_segmento->paginas,num_pagina_final+1);
							void* marco_final2 = obtener_puntero_a_marco(pagina_final2);
							memcpy(marco_final2,nuevo_ultimo_heap+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);
						}
						else{
								//no quedo en el medio
							pagina* pagina_final = list_get(ultimo_segmento->paginas,num_pagina_final);
							void* puntero_a_marco_nueva_ultima_pagina = obtener_puntero_a_marco(pagina_final);
							memcpy(puntero_a_marco_nueva_ultima_pagina+nuevo_offset_heap_al_marco,nuevo_ultimo_heap,sizeof(heap_metadata));
						}

						//verificar donde queda el heap viejo
						int viejo_offset_heap_al_marco = lista_ultimo_heap->direccion_heap_metadata%configuracion->tam_pag;
						int num_pagina_final_vieja = redondear_double_arriba((double)lista_ultimo_heap->direccion_heap_metadata / configuracion->tam_pag);
						if(viejo_offset_heap_al_marco > configuracion->tam_pag-sizeof(heap_metadata)){
							//queda en el medio
							int tamanio_a_copiar = configuracion->tam_pag-viejo_offset_heap_al_marco;
							pagina* pagina_final = list_get(ultimo_segmento->paginas,num_pagina_final_vieja);
							void* marco_final = obtener_puntero_a_marco(pagina_final);
							memcpy(marco_final+viejo_offset_heap_al_marco,ultimo_heap,tamanio_a_copiar);

							pagina* pagina_final2 = list_get(ultimo_segmento->paginas,num_pagina_final_vieja+1);
							void* marco_final2 = obtener_puntero_a_marco(pagina_final2);
							memcpy(marco_final2,ultimo_heap+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);
						}
						else{
							//no queda en el medio
							pagina* pagina_final = list_get(ultimo_segmento->paginas,num_pagina_final_vieja);
							void* puntero_a_marco_vieja_ultima_pagina = obtener_puntero_a_marco(pagina_final);
							memcpy(puntero_a_marco_vieja_ultima_pagina+viejo_offset_heap_al_marco,ultimo_heap,sizeof(heap_metadata));
						}
						direccion_return = ultimo_segmento->base_logica+sizeof(heap_metadata)+lista_ultimo_heap->direccion_heap_metadata;
						free(ultimo_heap);
						free(nuevo_ultimo_heap);
					}
					else{//=>no hay lugar
						pthread_mutex_unlock(&mutex_lugar_disponible);
						free(ultimo_heap);
						return -1;
					}
				}
				else{
					//no se puede agrandar, hay que crear un segmento nuevo
					uint32_t cantidad_de_paginas = paginas_necesarias_para_tamanio(datos->tamanio+sizeof(heap_metadata)*2);
					int espacio_libre_ultima_pag = cantidad_de_paginas*configuracion->tam_pag-datos->tamanio-sizeof(heap_metadata)*2;
					pthread_mutex_lock(&mutex_lugar_disponible);
					if(lugar_disponible>=cantidad_de_paginas*configuracion->tam_pag){
						lugar_disponible-=cantidad_de_paginas*configuracion->tam_pag;
						pthread_mutex_unlock(&mutex_lugar_disponible);
						segmento* segmento_nuevo = malloc(sizeof(segmento));
						segmento_nuevo->compartido = false;
						segmento_nuevo->mmapeado = false;
						segmento_nuevo->paginas_liberadas=0;
						segmento_nuevo->tamanio = cantidad_de_paginas*configuracion->tam_pag;
						segmento_nuevo->info_heaps = list_create();

						heap_lista* heap_inicial_lista = malloc(sizeof(heap_lista));
						heap_inicial_lista->direccion_heap_metadata = 0;
						heap_inicial_lista->espacio = datos->tamanio;
						heap_inicial_lista->is_free = false;
						heap_inicial_lista->indice = 0;
						heap_lista* heap_final_lista = malloc(sizeof(heap_lista));
						heap_final_lista->direccion_heap_metadata = datos->tamanio+sizeof(heap_metadata);
						heap_final_lista->is_free = true;
						heap_final_lista->espacio = espacio_libre_ultima_pag;
						heap_final_lista->indice = 1;
						list_add(segmento_nuevo->info_heaps,heap_inicial_lista);
						list_add(segmento_nuevo->info_heaps,heap_final_lista);//agrego heap de inicio y fin
						t_list* paginas = list_create();
						_Bool hay_que_entrar_en_la_anteultima_pag = false;
						int heap_en_ultima_pagina,heap_en_ante_ultima_pagina,offset_heap;
						heap_metadata* heap_al_final = malloc(sizeof(heap_metadata));
						heap_al_final->is_free = true; //es el ultimo
						heap_al_final->size = espacio_libre_ultima_pag;
						if(espacio_libre_ultima_pag>configuracion->tam_pag-sizeof(heap_metadata)) {
							// significa que el heap esta entre las ultimas dos paginas,
							// hay que agregarselas en el for
							heap_en_ultima_pagina = configuracion->tam_pag-espacio_libre_ultima_pag;
							heap_en_ante_ultima_pagina = sizeof(heap_metadata)-heap_en_ultima_pagina;
							offset_heap = configuracion->tam_pag-heap_en_ante_ultima_pagina;
							hay_que_entrar_en_la_anteultima_pag = true;
						}
						else{
							offset_heap = configuracion->tam_pag-espacio_libre_ultima_pag-sizeof(heap_metadata);
						}

						for(int i = 0;i<cantidad_de_paginas;i++){
							pagina* pag = malloc(sizeof(pagina));
							pag->num_pagina = i;
							pag->presencia = true;
							if(i <= configuracion->tam_mem / configuracion->tam_pag){
								pag->bit_marco = asignar_marco_nuevo();
								pag->bit_marco->bit_uso = true;
								pag->bit_marco->bit_modificado = true;
								pag->bit_swap = NULL;
							}
							else{
								pag->bit_marco = NULL;
								pag->bit_swap = bit_libre_memoria_virtual();
								pag->presencia = false;
							}
							if(i == 0){//si es la 1ra => hay que agregar el heap al inicio
								heap_metadata* heap_nuevo = malloc(sizeof(heap_metadata));
								heap_nuevo->is_free = false;
								heap_nuevo->size = datos->tamanio;
								void* puntero_a_marco = obtener_puntero_a_marco(pag);
								memcpy(puntero_a_marco,heap_nuevo,sizeof(heap_metadata));
								free(heap_nuevo);
							}
							if(i == cantidad_de_paginas-2){
								if(hay_que_entrar_en_la_anteultima_pag){
									void* puntero_a_marco = obtener_puntero_a_marco(pag);
									memcpy(puntero_a_marco+offset_heap,heap_al_final,heap_en_ante_ultima_pagina);
								}
							}
							if(i == cantidad_de_paginas-1){//se agrega el heap siguiente al final
								void* puntero_a_marco = obtener_puntero_a_marco(pag);
								if(hay_que_entrar_en_la_anteultima_pag){
									memcpy(puntero_a_marco,heap_al_final+heap_en_ante_ultima_pagina,heap_en_ultima_pagina);
								}
								else{
									memcpy(puntero_a_marco+offset_heap,heap_al_final,sizeof(heap_metadata));
								}
							}
							list_add(paginas,pag);
						}
						free(heap_al_final);
						segmento_nuevo->paginas = paginas;
						segmento_nuevo->num_segmento = list_size(tabla_de_segmentos);
						segmento_nuevo->base_logica = base_logica_segmento_nuevo(tabla_de_segmentos);
						list_add(tabla_de_segmentos,segmento_nuevo);
						direccion_return = segmento_nuevo->base_logica+sizeof(heap_metadata);//es el principio del segmento nuevo
					}
					else{
						pthread_mutex_unlock(&mutex_lugar_disponible);
						//no hay lugar en el sistema
						//free el segmento_nuevo
						return -1;
					}
			}	}
		}
	}

	//retorno la direccion de memoria (virtual) que le asigne
	acumular_espacio_pedido(datos->id,datos->tamanio);
	metricas("-1"); //para no muestre leaks
	return direccion_return;
	}
else{//no hay lugar
	pthread_mutex_unlock(&mutex_lugar_disponible);
	return -1;
}
}

segmento* buscar_segmento_con_paginas_liberadas(int tamanio, t_list* tabla_segmentos)
{
	//tengo que mirar las paginas liberadas de cada segmento y fijarme si
	//tamanio entra ahi
	_Bool buscar_segmento_con_tamanio(segmento* seg){
		if(seg->mmapeado){
			return false;
		}
		//cambio
		if(list_is_empty(seg->info_heaps)){
			// si esta vacio, como que tengo que armarlo de vuelta ...
			pthread_mutex_lock(&mutex_lugar_disponible);
			if(lugar_disponible>=configuracion->tam_pag){
				lugar_disponible -= configuracion->tam_pag;
				pthread_mutex_unlock(&mutex_lugar_disponible);
				heap_lista* heap_lista_inicial =  malloc(sizeof(heap_lista));
				heap_lista_inicial->direccion_heap_metadata=0;
				heap_lista_inicial->is_free=true;
				heap_lista_inicial->espacio=configuracion->tam_pag-sizeof(heap_metadata);
				heap_lista_inicial->indice=0;
				//y deberia agregar al menos una pagina
				pagina* pag_inicial = malloc(sizeof(pagina));
				pag_inicial->num_pagina=0;
				pag_inicial->presencia = true;
				pag_inicial->bit_marco = asignar_marco_nuevo();
				pag_inicial->bit_swap = NULL;
				list_add(seg->paginas,pag_inicial);
				list_add(seg->info_heaps,heap_lista_inicial);
				heap_metadata* heap_inicial = malloc(sizeof(heap_metadata));
				heap_inicial->is_free=true;
				heap_inicial->size=configuracion->tam_pag-sizeof(heap_metadata);
				reemplazar_heap_en_memoria(heap_lista_inicial,seg,heap_inicial);
			}
			else{
				pthread_mutex_unlock(&mutex_lugar_disponible);
				return false;
			}
		}
		heap_lista* heap_lista_ultimo = list_last_element(seg->info_heaps);
		uint32_t paginas_necesarias = paginas_necesarias_para_tamanio(tamanio+sizeof(heap_metadata)-heap_lista_ultimo->espacio);
		return seg->paginas_liberadas>=paginas_necesarias;
	}
	return list_find(tabla_segmentos,(void*)buscar_segmento_con_tamanio);

}

void* list_last_element(t_list* lista){
	return (list_get(lista,lista->elements_count-1));
}

uint32_t base_logica_segmento_nuevo(t_list* tabla_de_segmentos){
	if(list_is_empty(tabla_de_segmentos)){
		return 0;
	}
	segmento* segmento_anterior = list_last_element(tabla_de_segmentos);
	return segmento_anterior->base_logica+
			(segmento_anterior->paginas->elements_count+segmento_anterior->paginas_liberadas)
			*configuracion->tam_pag;
}

pagina* ultima_pagina_por_numero(segmento* seg){
	pagina* pag=NULL;
	if(list_is_empty(seg->paginas)){
		return pag;
	}
	pag->num_pagina=0;
	for(int i=0;i<list_size(seg->paginas)-1;i++) {
		pagina* p = list_get(seg->paginas,i);
		if(p->num_pagina > pag->num_pagina){
			pag = p;
		}
	}
	return pag;
}

segmento* buscar_segmento_con_espacio(t_list* tabla_de_segmentos,uint32_t tamanio){
	_Bool tiene_lugar_disponible(heap_lista* heap){//por cada heap de un seg se fija si tiene espacio
		if(heap->is_free && heap->espacio >= tamanio){
			return true;
		}
		return false;
	}
	_Bool encontrar_segmento_con_espacio(segmento* segmento){//se fija en cada seg si hay espacio
		if(!segmento->mmapeado){
			return list_any_satisfy(segmento->info_heaps,(void*)tiene_lugar_disponible);
		}
		else return false;
	}
	segmento* segmento_encontrado = list_find(tabla_de_segmentos,(void*)encontrar_segmento_con_espacio);
	return segmento_encontrado;
	//testear!!
}
uint32_t paginas_necesarias_para_tamanio(uint32_t tamanio){
	uint32_t pags = tamanio/configuracion->tam_pag;
	if(tamanio%configuracion->tam_pag>0){
		pags++;
	}
	return pags;
}
void* obtener_puntero_a_marco(pagina* pag){
	//fijarse si esta en memoria o en swap
	if(pag->presencia == false){
		if(pag->bit_swap!=NULL){
			void* pagina_a_sacar = malloc(configuracion->tam_pag);
			memcpy(pagina_a_sacar,swap+pag->bit_swap->posicion*configuracion->tam_pag,configuracion->tam_pag);
			//copio lo que esta en swap a un puntero temporal
			pag->bit_swap->ocupado = false;//saco el bit del bitmap_memoria_virutal
			pag->bit_swap = NULL;
			t_bit_memoria* _bit = asignar_marco_nuevo();
			pag->bit_marco = _bit;
			pag->presencia = true;
			//ahora tengo q pegar la pagina en memoria
			memcpy(upcm+_bit->posicion*configuracion->tam_pag,pagina_a_sacar,configuracion->tam_pag);
		}
		else{
			log_info(logg,"Aca hay un mapeo que se quiere acceder pero no esta en memoria >:(");
			//si llega aca estamos en las malas
		}
	}
	pag->bit_marco->bit_uso = true;
	return upcm + pag->bit_marco->posicion * configuracion->tam_pag;
}

t_bit_memoria* asignar_marco_nuevo(){
//	retorno un marco nuevo en memoria
//	esto va a tener q estar mutexeado
	t_bit_memoria* bit_libre = bit_libre_memoria();
	if(bit_libre == NULL){
		//no se encontro=>ejecutar algoritmo clock
		bit_libre = ejecutar_clock_modificado();
	}
	return bit_libre;
}
t_bit_memoria* ejecutar_clock_modificado(){
	//si lo que quiero es poner en memoria un bit que esta en swap, primero tengo que
	//sacar el bit de swap asi cuando se ejecuta el clock tengo lugar para pasarlo a swap
	//(uso,modificado)
	//primero hay que buscar si hay alguna pagina de memoria en (0,0) -> en el bitarray_memoria
	//si no se encuentra hay que buscar el (0,1) y pasando los (1,x) a (0,x)
	//si no se encuentra repetir los pasos devuelta
	t_bit_memoria* bit_return = buscar_0_0();
	if(bit_return==NULL){
		bit_return = buscar_0_1();
		if(bit_return == NULL){
			bit_return = ejecutar_clock_modificado_2vuelta();
		}
	}

	pagina* pagina_a_sacar = buscar_pagina_por_bit(bit_return);
	if(pagina_a_sacar != NULL){
		pagina_a_sacar->bit_swap = pasar_marco_a_swap(pagina_a_sacar->bit_marco);
		pagina_a_sacar->bit_marco = NULL;
		pagina_a_sacar->presencia = false;
		bit_return->bit_modificado = false;//lo dejo en (1,0) listo para usar
		bit_return->bit_uso = true;
	}
	else{
		log_error(logg,"Se quiere sacar una pag q no se encuentra en la tabla de pags");
	}

	return bit_return;
}
pagina* buscar_pagina_por_bit(t_bit_memoria* bit){
	pagina* pagina_return = NULL;
	void buscar_bit(pagina* pag){
		if(pag->presencia){
			if(pag->bit_marco->posicion == bit->posicion){
				pagina_return = pag;
			}
		}
	}
	void iteracion2(segmento* seg){
		list_iterate(seg->paginas,(void*)buscar_bit);
	}
	void iteracion(programa_t* programa){
		list_iterate(programa->tabla_de_segmentos,(void*)iteracion2);
	}
	pthread_mutex_lock(&mutex_tabla_de_programas);
	list_iterate(tabla_de_programas,(void*)iteracion);
	pthread_mutex_unlock(&mutex_tabla_de_programas);
	return pagina_return;
}
t_bit_memoria* ejecutar_clock_modificado_2vuelta(){
	t_bit_memoria* bit_return = buscar_0_0();
	if(bit_return==NULL){
		bit_return = buscar_0_1();
	}
	return bit_return;
}
t_bit_memoria* buscar_0_0(){
	pthread_mutex_lock(&mutex_bitarray);
	int puntero_al_iniciar = posicion_puntero_clock;
	t_bit_memoria* bit_nulo = NULL;
	//busco en la primera mitad
	for(int i = 0; i < bitarray->size_memoria-puntero_al_iniciar; i++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(!_bit->bit_modificado && !_bit->bit_uso){
			posicion_puntero_clock ++;
			pthread_mutex_unlock(&mutex_bitarray);
			return _bit;
		}
		posicion_puntero_clock ++;
	}
	for(posicion_puntero_clock = 0; posicion_puntero_clock < puntero_al_iniciar;posicion_puntero_clock++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(!_bit->bit_modificado && !_bit->bit_uso){
			pthread_mutex_unlock(&mutex_bitarray);
			return _bit;
		}
	}
	posicion_puntero_clock = puntero_al_iniciar;
	pthread_mutex_unlock(&mutex_bitarray);
	return bit_nulo;
}
t_bit_memoria* buscar_0_1(){
	pthread_mutex_lock(&mutex_bitarray);
	int puntero_al_iniciar = posicion_puntero_clock;
	t_bit_memoria* bit_nulo = NULL;
	//busco en la primera mitad
	for(int i = 0; i < bitarray->size_memoria-puntero_al_iniciar; i++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(_bit->bit_modificado && !_bit->bit_uso){
			posicion_puntero_clock ++;
			pthread_mutex_unlock(&mutex_bitarray);
			return _bit;
		}
		if(_bit->bit_uso){
			_bit->bit_uso = false;
		}
		posicion_puntero_clock++;
	}
	for(posicion_puntero_clock = 0; posicion_puntero_clock < puntero_al_iniciar;posicion_puntero_clock++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(_bit->bit_modificado && !_bit->bit_uso){
			pthread_mutex_unlock(&mutex_bitarray);
			return _bit;
		}
		if(_bit->bit_uso){
			_bit->bit_uso = false;
		}
	}
	posicion_puntero_clock = puntero_al_iniciar;
	pthread_mutex_unlock(&mutex_bitarray);
	return bit_nulo;
}
t_bit_swap* pasar_marco_a_swap(t_bit_memoria* bit){
	char* puntero_a_marco = upcm + bit->posicion * configuracion->tam_pag;
	t_bit_swap* bit_swap = bit_libre_memoria_virtual();
	char* puntero_a_swap = swap + bit_swap->posicion * configuracion->tam_pag;
//	memcpy(puntero_a_swap,puntero_a_marco,configuracion->tam_pag);
	for(int i=0;i<configuracion->tam_pag;i++){
		puntero_a_swap[i] = puntero_a_marco[i];
	}
	msync(swap,configuracion->tam_swap,MS_SYNC);
	sleep(1);
	printf(">>>>>>>>>>>>mando algo a swap");
	fflush(stdout);
	bit_swap->ocupado = true;
	//close(fd);
	//fd = open(path_swap,O_RDWR|O_CREAT,0777);
	return bit_swap;
}
void reemplazar_heap_en_memoria(heap_lista* heap_de_lista,segmento* seg,heap_metadata* nuevo_heap_metadata){
	//verificar donde queda el heap nuevo
	int nuevo_offset_heap_al_marco = heap_de_lista->direccion_heap_metadata%configuracion->tam_pag;
	int num_pagina_del_heap = heap_de_lista->direccion_heap_metadata / configuracion->tam_pag;
	if(nuevo_offset_heap_al_marco > configuracion->tam_pag-sizeof(heap_metadata)){
		//significa que quedo en el medio
		int tamanio_a_copiar = configuracion->tam_pag-nuevo_offset_heap_al_marco;
		pagina* pagina_de_heap = list_get(seg->paginas,num_pagina_del_heap);
		void* marco1 = obtener_puntero_a_marco(pagina_de_heap);
		memcpy(marco1+nuevo_offset_heap_al_marco,nuevo_heap_metadata,tamanio_a_copiar);

		pagina* pagina_de_heap2 = list_get(seg->paginas,num_pagina_del_heap+1);
		void* marco2 = obtener_puntero_a_marco(pagina_de_heap2);
		memcpy(marco2,nuevo_heap_metadata+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);
	}
	else{
			//no quedo en el medio
		pagina* pagina_del_heap = list_get(seg->paginas,num_pagina_del_heap);
		void* puntero_a_marco = obtener_puntero_a_marco(pagina_del_heap);
		memcpy(puntero_a_marco+nuevo_offset_heap_al_marco,nuevo_heap_metadata,sizeof(heap_metadata));
	}
	free(nuevo_heap_metadata);
}

// inicializar tam _ mapeos !!
int muse_free(muse_free_t* datos){
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(tabla_de_segmentos==NULL){
		log_info(logg,"no se encontro la tabla de segmentos de %s",datos->id);
		return -1;
	}
	segmento* segmento_buscado = traer_segmento_de_direccion(tabla_de_segmentos,datos->direccion);
	if(segmento_buscado==NULL){
		//no existe el segmento buscado o se paso del segmento
		log_info(logg,"no se encontro el segmento de %s",datos->id);
		return -1;
	}
	if(segmento_buscado->mmapeado){
			//si esta mmpapeado, no se puede liberar
			log_info(logg,"El segmento de %s esta mappeado entonces no se puede liberar",datos->id);
			return -1;
	}

	heap_lista* heap_lista_encontrado = NULL;
	int direccion_al_segmento = datos->direccion-segmento_buscado->base_logica;
	for(int i =0; i < list_size(segmento_buscado->info_heaps); i++) {
		//por cada heap_lista, ver si es el heap que busco
		heap_lista* heap_lista_aux = list_get(segmento_buscado->info_heaps,i);
		if(heap_lista_aux->direccion_heap_metadata+sizeof(heap_metadata) == direccion_al_segmento) {
			heap_lista_encontrado = heap_lista_aux;
			heap_lista_encontrado->is_free = true;
			break;
		}
	}
	if(heap_lista_encontrado == NULL){
		log_info(logg,"No se pudo liberar la direccion %d de %s",datos->direccion,datos->id);
		return -1;
	}
	//hay que reemplazar el heap posta,
	heap_metadata* heap_metadata_nuevo = malloc(sizeof(heap_metadata));
	heap_metadata_nuevo->is_free = true;
	heap_metadata_nuevo->size = heap_lista_encontrado->espacio;
	acumular_espacio_liberado(datos->id, heap_metadata_nuevo->size);
	reemplazar_heap_en_memoria(heap_lista_encontrado,segmento_buscado,heap_metadata_nuevo);

	//recorro todos los heaps_lista viendo cuales tengo que juntar
	t_list* heaps_lista =segmento_buscado->info_heaps;

	int contador_index = heaps_lista->elements_count-1;
	while(contador_index >= 0){
		//agarro el ultimo
		heap_lista* heap_de_lista = list_get(heaps_lista,contador_index);
		if(heap_de_lista->is_free && heaps_lista->elements_count==1)
		{
			int cuantas_paginas_son_liberadas=0;
			//cambio
			for(int i = 0;i<segmento_buscado->paginas->elements_count;){
				//No dejo paginas
				list_remove_and_destroy_element(segmento_buscado->paginas,i,(void*)destroy_pagina);
				cuantas_paginas_son_liberadas++;
			}
			//saco el heap_lista y listo
			segmento_buscado->paginas_liberadas+=cuantas_paginas_son_liberadas;
			segmento_buscado->tamanio=segmento_buscado->paginas->elements_count*configuracion->tam_pag;
			pthread_mutex_lock(&mutex_lugar_disponible);
			lugar_disponible+=cuantas_paginas_son_liberadas*configuracion->tam_pag;
			pthread_mutex_unlock(&mutex_lugar_disponible);
			list_remove_and_destroy_element(segmento_buscado->info_heaps,0/*porque es el unico*/,(void*)free);

		}
		else if(heap_de_lista->is_free && contador_index==heaps_lista->elements_count-1){
			//es el ultimo
			// si parte del ultimo y el anterior esta vacio
			heap_lista* heap_de_lista_anterior=list_get(heaps_lista,contador_index-1);
			if(heap_de_lista_anterior->is_free){
				//a partir de esta pagina hay que liberar
				int pagina_heap_anterior = (heap_de_lista_anterior->direccion_heap_metadata+sizeof(heap_metadata))
						/ configuracion->tam_pag;
				int cuantas_paginas_son_liberadas=0;
				for(int i = pagina_heap_anterior+1;i<segmento_buscado->paginas->elements_count;){
					//fijarse si hay otros lugares que libere pagina
					//destroy_pagina
					list_remove_and_destroy_element(segmento_buscado->paginas,i,(void*)destroy_pagina);
					cuantas_paginas_son_liberadas++;
				}
				//cambio el tamano del segmento !!
				segmento_buscado->paginas_liberadas+=cuantas_paginas_son_liberadas;
				segmento_buscado->tamanio=segmento_buscado->paginas->elements_count*configuracion->tam_pag;
				pthread_mutex_lock(&mutex_lugar_disponible);
				lugar_disponible+=cuantas_paginas_son_liberadas*configuracion->tam_pag;
				pthread_mutex_unlock(&mutex_lugar_disponible);
				int offset_heap_anterior=heap_de_lista_anterior->direccion_heap_metadata%configuracion->tam_pag;
				heap_de_lista_anterior->espacio=configuracion->tam_pag-offset_heap_anterior-sizeof(heap_metadata);

				heap_metadata* heap_metadata_nuevo = malloc(sizeof(heap_metadata));
				heap_metadata_nuevo->is_free=true;
				heap_metadata_nuevo->size = heap_de_lista_anterior->espacio;
				reemplazar_heap_en_memoria(heap_de_lista_anterior,
						segmento_buscado,heap_metadata_nuevo);
				if(contador_index>=1){
					list_remove_and_destroy_element(segmento_buscado->info_heaps,heap_de_lista->indice,(void*)free);
				}
			}

		}
		else if(heap_de_lista->is_free && contador_index>0){//si no es el ultimo
			//miro el de indice anterior
			//si agarra dos seguidos que estan vacios
			heap_lista* heap_de_lista_anterior=list_get(heaps_lista,contador_index-1);
			if(heap_de_lista_anterior->is_free) {//hay que mergearlos
				//no estoy sacando las paginas //las paginas siguen asociadas al segmento
				heap_de_lista_anterior->espacio+=sizeof(heap_metadata)+heap_de_lista->espacio;
				heap_metadata* heap_metadata_nuevo = malloc(sizeof(heap_metadata));
				heap_metadata_nuevo->is_free=true;
				heap_metadata_nuevo->size = heap_de_lista_anterior->espacio;
				acumular_espacio_liberado(datos->id, heap_metadata_nuevo->size);
				reemplazar_heap_en_memoria(heap_de_lista_anterior,segmento_buscado,heap_metadata_nuevo);
				list_remove_and_destroy_element(segmento_buscado->info_heaps,heap_de_lista->indice,(void*)free);
			}
		}
		contador_index--;
	}//termina el while
	//cambiar todos los indices
	for(int i=0;i<list_size(segmento_buscado->info_heaps);i++){
		heap_lista* heap_de_lista = list_get(segmento_buscado->info_heaps,i);
		heap_de_lista->indice=i;
	}

//	retorna -1 si falla
	log_info(logg,"Direccion %i del segmento %s liberada correctamente",datos->direccion,datos->id);
	return 0;
}

void* muse_get(muse_get_t* datos){
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(tabla_de_segmentos==NULL){
		//no existe el segmento buscado o se paso del segmento
		log_info(logg,"no se encontro tabla de segmentos de %s",datos->id);
		return NULL;
	}
	segmento* segmento_buscado = traer_segmento_de_direccion(tabla_de_segmentos,datos->direccion);
	if(segmento_buscado==NULL){
		//no existe el segmento buscado o se paso del segmento
		log_info(logg,"no se encontro segmento de %s",datos->id);
		return NULL;
	}
	int direccion_al_segmento = datos->direccion-segmento_buscado->base_logica;
	void* resultado_get = NULL;
	//si copia demas tira segm fault en libmuse
	int pagina_inicial = direccion_al_segmento / configuracion->tam_pag;
	int offset_inicial = direccion_al_segmento % configuracion->tam_pag;
	if(segmento_buscado != NULL && segmento_buscado->tamanio-direccion_al_segmento >= datos->tamanio
			&& pagina_inicial*configuracion->tam_pag+offset_inicial+datos->tamanio
			<= segmento_buscado->tamanio){
		//encontro un segmento, hay que buscar la direccion ahi adentro
		int direccion_final = direccion_al_segmento+datos->tamanio;
		int offset_final = direccion_final % configuracion->tam_pag;
		int tamanio_de_todas_las_paginas = datos->tamanio + offset_inicial + configuracion->tam_pag-offset_final;
		int cantidad_de_paginas = tamanio_de_todas_las_paginas / configuracion->tam_pag;
		resultado_get = malloc(datos->tamanio);
		void* super_void = malloc(tamanio_de_todas_las_paginas);
		int puntero = 0;

		if(segmento_buscado->mmapeado){
			//puede haber pags q nunca se levantaron (no estan en memoria)
			paginas_de_map_en_memoria(direccion_al_segmento,datos->tamanio,segmento_buscado);
		}
		for(int i = 0;i<cantidad_de_paginas;i++,puntero+=configuracion->tam_pag){
			pagina* pag = list_get(segmento_buscado->paginas,pagina_inicial+i);
			void* puntero_a_marco = obtener_puntero_a_marco(pag);
			pag->bit_marco->bit_uso = true;
			pag->presencia = true;
			memcpy(super_void+puntero,puntero_a_marco,configuracion->tam_pag);
		}
		memcpy(resultado_get,super_void+offset_inicial,datos->tamanio);
		free(super_void);
	}

	log_info(logg,"%s",(char*)resultado_get);
	return resultado_get;
}
segmento* traer_segmento_de_direccion(t_list* tabla_de_segmentos,uint32_t direccion){
	_Bool buscar_segmento_por_direccion(segmento* seg){
		if(direccion - seg->base_logica < seg->tamanio){
			return true;
		}
		return false;
	}

	return list_find(tabla_de_segmentos,(void*)buscar_segmento_por_direccion);
}
pagina* buscar_pagina_por_numero(t_list* lista, int numero_de_pag) {
	_Bool numero_de_pagina(pagina* pag){
		return pag->num_pagina == numero_de_pag;
	}
	pagina* pagina_buscada = list_find(lista,(void*)numero_de_pagina);
	return pagina_buscada;
}
void paginas_de_map_en_memoria(int direccion,int tamanio,segmento* segmento_buscado){
	int pagina_inicial = direccion / configuracion->tam_pag;
	int offset_inicial = direccion % configuracion->tam_pag;
	int pagina_final = (direccion+tamanio) / configuracion->tam_pag;

	_Bool ver_si_hay_pags_no_cargadas(pagina* _pag){
		if(_pag->num_pagina >= pagina_inicial && _pag->num_pagina <= pagina_final){
			if(_pag->bit_marco == NULL && _pag->bit_swap == NULL){
				return true;
			}
		}
		return false;
	}
	if(pagina_inicial*configuracion->tam_pag+offset_inicial+tamanio > segmento_buscado->tamanio){
		//me esta pidiendo mas que lo que tiene el segmento
		return;
	}
	if(list_any_satisfy(segmento_buscado->paginas,(void*)ver_si_hay_pags_no_cargadas)){
		//si se cumple, hay pags q no estan cargadas en memoria
		int cantidad_de_paginas = pagina_final - pagina_inicial + 1;//xq empieza en 0
		int bytes_a_leer = cantidad_de_paginas * configuracion->tam_pag;
		int padding = bytes_a_leer-offset_inicial-tamanio;
		void* void_padding = generar_padding(padding);
		int ultima_pagina_de_la_lista = segmento_buscado->paginas->elements_count-1;
		//levanto el archivo
		void* buffer = malloc(bytes_a_leer);
		FILE* archivo = fopen(segmento_buscado->path_mapeo,"rb");
		fread(buffer,configuracion->tam_pag,cantidad_de_paginas,archivo);
		fclose(archivo);
		int puntero = pagina_inicial*configuracion->tam_pag;
		//voy copiando las pags necesarias a los marcos en memoria
		for(int i = pagina_inicial;i<=pagina_final;i++,puntero += configuracion->tam_pag, bytes_a_leer-= configuracion->tam_pag){
			pagina* pag = list_get(segmento_buscado->paginas,i);
			if(pag->bit_marco==NULL && pag->bit_swap == NULL){
				//hay q traerla a memoria
				pag->bit_marco = asignar_marco_nuevo();
				pag->bit_marco->bit_uso = true;
				pag->bit_marco->bit_modificado = true;
				pag->bit_swap = NULL;
				pag->presencia = true;
				void* puntero_a_marco = obtener_puntero_a_marco(pag);

				if(pag->num_pagina == ultima_pagina_de_la_lista){
					//si es la ultima hay que agregar el padding
					memcpy(puntero_a_marco,buffer+puntero,configuracion->tam_pag-padding);
					memcpy(puntero_a_marco+configuracion->tam_pag-padding,void_padding,padding);
					//padding => se llena de \0 al final

				}
				else{
					memcpy(puntero_a_marco,buffer+puntero,configuracion->tam_pag);
				}
			}
		}
		free(buffer);
		free(void_padding);
	}
}
int muse_cpy(muse_cpy_t* datos){ //datos->direccion es destino, datos->src void* es lo que hay que pegar
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(tabla_de_segmentos==NULL){
		//no existe el segmento buscado o se paso del segmento
		log_info(logg,"no se encontro la tabla de segmentos de %s",datos->id);
		return -1;
	}
	segmento* segmento_buscado = traer_segmento_de_direccion(tabla_de_segmentos,datos->direccion);
	if(segmento_buscado==NULL){
		//no existe el segmento buscado o se paso del segmento
		log_info(logg,"no se encontro el segmento de %s",datos->id);
		return -1;
	}
	heap_lista* heap_dst = NULL;
	//la dire es abs
	int direccion_al_segmento = datos->direccion-segmento_buscado->base_logica;
	if(!segmento_buscado->mmapeado){
		if(list_size(segmento_buscado->info_heaps)>0){
			for(int i =0; i < list_size(segmento_buscado->info_heaps); i++) {
				// por cada heap_lista, veo si la direccion que me mandaron
				// pertenece a ese ->datos
				heap_lista* heap_aux=  list_get(segmento_buscado->info_heaps,i);
				if(heap_aux->direccion_heap_metadata+sizeof(heap_metadata) <= direccion_al_segmento &&
						heap_aux->direccion_heap_metadata+sizeof(heap_metadata)+heap_aux->espacio
						> direccion_al_segmento && !heap_aux->is_free) {
					//significa que la dire nueva esta entre un heap y el siguiente
					heap_dst=heap_aux;
					break;
				}
				//no esta en esta
			}
		}
		if(heap_dst == NULL){
		//no se encontro el heap de destino- segm fault
			log_info(logg,"No se encontro direccion %i en el segmento de %s",datos->direccion,datos->id);
			return -1;
		}
		if(heap_dst->espacio<datos->size_paquete) {
			//no entra en el espacio de ese heap_dst-segm fault
			log_info(logg,"Los datos que desea copiar no entran en la direccion %i para el segmento %s",datos->direccion,datos->id);
			return -1;
		}
		int offset_al_muse_alloc = direccion_al_segmento-heap_dst->direccion_heap_metadata-sizeof(heap_metadata);
		if(offset_al_muse_alloc + datos->size_paquete > heap_dst->espacio) {
			//va a pisar el heap_lista_siguiente
			log_info(logg,"Hacer un cpy en la direccion %i genera un conflicto en el segmento %s",datos->direccion,datos->id);
			return -1;
		}

		//pego la inicial
		int nro_pagina =  direccion_al_segmento/configuracion->tam_pag;
		pagina* pag = list_get(segmento_buscado->paginas,nro_pagina);
		void* marco = obtener_puntero_a_marco(pag);
		pag->bit_marco->bit_uso = true;
		pag->bit_marco->bit_modificado = true;
		int cuanto_puedo_pegar = configuracion->tam_pag-(direccion_al_segmento%configuracion->tam_pag);
		//aca datos->direccion no tendria que ser direccion_al_segmento?!?!
		if(cuanto_puedo_pegar <= datos->size_paquete){
			//pego lo que pueda
			memcpy(marco+(direccion_al_segmento%configuracion->tam_pag),datos->paquete,cuanto_puedo_pegar);

		}
		else{
			//pego el paquete completo
			memcpy(marco+(direccion_al_segmento%configuracion->tam_pag),datos->paquete,datos->size_paquete);
			log_info(logg,"Datos en segmento %d direccion %d fueron copiados con exito"
					,segmento_buscado->num_segmento,datos->direccion);
			return 0;
		}
		//voy pegando las que me faltan
		nro_pagina++; //siguiente pagina
		do{
			//agarro la pag siguiente y su marco
			int cuanto_me_falta = datos->size_paquete-cuanto_puedo_pegar;

			if(cuanto_me_falta>=configuracion->tam_pag){
				//si me falta mas que una pagina, puedo pegar la pag entera
				pagina* pag = list_get(segmento_buscado->paginas,nro_pagina);
				void* marco = obtener_puntero_a_marco(pag);
				pag->bit_marco->bit_uso = true;
				pag->bit_marco->bit_modificado = true;
				//voy acumulando cuantas paginas voy pegando
				memcpy(marco,datos->paquete+cuanto_puedo_pegar,configuracion->tam_pag);
				cuanto_puedo_pegar+=configuracion->tam_pag;
				nro_pagina++;
			}
			else if(cuanto_me_falta>0){
				//si me falta menos de una pagina, pego lo que tengo
				pagina* pag = list_get(segmento_buscado->paginas,nro_pagina);
				void* marco = obtener_puntero_a_marco(pag);
				pag->bit_marco->bit_uso = true;
				pag->bit_marco->bit_modificado = true;
				memcpy(marco,datos->paquete+cuanto_puedo_pegar,cuanto_me_falta);
				break;
			}
			else{
				break;
			}
		}while (1);
		//HAY QUE PROBAR MMCOPY

		log_info(logg,"Datos en segmento %d direccion %d fueron copiados con exito"
						,segmento_buscado->num_segmento,datos->direccion);
		return 0;
	}
	else{
		//es mmapeado
		//puede haber pags q nunca se levantaron (no estan en memoria)
		paginas_de_map_en_memoria(direccion_al_segmento,datos->size_paquete,segmento_buscado);

		int numero_pagina_inicial = direccion_al_segmento / configuracion->tam_pag;
		int offset_pagina_inicial = direccion_al_segmento % configuracion->tam_pag;
		int numero_pagina_final = (direccion_al_segmento+datos->size_paquete) / configuracion->tam_pag;
		if(numero_pagina_inicial*configuracion->tam_pag+offset_pagina_inicial+datos->size_paquete
				> segmento_buscado->tamanio){
			return -1;
		}
		int puntero = 0;
		for(int i = numero_pagina_inicial;i<=numero_pagina_final;i++){
			pagina* pag = list_get(segmento_buscado->paginas,i);
			void* puntero_a_marco = obtener_puntero_a_marco(pag);
			pag->bit_marco->bit_modificado = true;
			pag->bit_marco->bit_uso = true;
			if(pag->num_pagina == numero_pagina_inicial){//si es la primera pego desde el off
				if(pag->num_pagina == numero_pagina_final){//es la primera y ulitma
					memcpy(puntero_a_marco+offset_pagina_inicial,datos->paquete,
							datos->size_paquete);
				}
				else{
					memcpy(puntero_a_marco+offset_pagina_inicial,datos->paquete,
							configuracion->tam_pag-offset_pagina_inicial);
					puntero += configuracion->tam_pag-offset_pagina_inicial;
				}
			}
			else if(pag->num_pagina == numero_pagina_final){
				memcpy(puntero_a_marco,datos->paquete+puntero,
						datos->size_paquete-puntero);
			}
			else{
				memcpy(puntero_a_marco,datos->paquete+puntero,configuracion->tam_pag);
				puntero += configuracion->tam_pag;
			}
		}
		return 0;
	}
}
/**
* Devuelve un puntero a una posición mappeada de páginas por una cantidad `length` de bytes
* el archivo del `path` dado.
* @param path Path a un archivo en el FileSystem de MUSE a mappear.
* @param length Cantidad de bytes de memoria a usar para mappear el archivo.
* @param flags
*          MAP_PRIVATE     Solo un proceso/hilo puede mappear el archivo.
*          MAP_SHARED      El segmento asociado al archivo es compartido.
* @return Retorna la posición de memoria de MUSE mappeada.
* @note: Si `length` sobrepasa el tamaño del archivo, toda extensión deberá estar llena de "\0".
* @note: muse_free no libera la memoria mappeada. @see muse_unmap
*/
int muse_map(muse_map_t* datos){
	int tamanio_paginas = redondear_double_arriba((double)datos->tamanio/configuracion->tam_pag)*configuracion->tam_pag;

	pthread_mutex_lock(&mutex_lugar_disponible);
	if(lugar_disponible >= tamanio_paginas){
		lugar_disponible-= tamanio_paginas;
		pthread_mutex_unlock(&mutex_lugar_disponible);
		t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
		if(tabla_de_segmentos==NULL){
			log_info(logg,"no se encontro tabla de segmentos para %s",datos->id);
			return -1;
		}
		segmento* segmento_nuevo = malloc(sizeof(segmento));
		segmento_nuevo->num_segmento = tabla_de_segmentos->elements_count;
		segmento_nuevo->mmapeado = true;
		segmento_nuevo->tamanio_mapeo = datos->tamanio;
		segmento_nuevo->base_logica = base_logica_segmento_nuevo(tabla_de_segmentos);
		segmento_nuevo->info_heaps = NULL;
		segmento_nuevo->paginas_liberadas=0;
		segmento_nuevo->path_mapeo = string_duplicate(datos->path);
		segmento_nuevo->tamanio = tamanio_paginas;
		if(datos->flag != MAP_PRIVATE){
			segmento_nuevo->compartido = true;
			t_list* tabla_de_paginas = buscar_mapeo_existente(datos->path,datos->tamanio);
			if(tabla_de_paginas!=NULL){
				//comparto la tabla de paginas :D
				segmento_nuevo->paginas = tabla_de_paginas;
				list_add(tabla_de_segmentos,segmento_nuevo);
				return segmento_nuevo->base_logica;
			}
		}
		else{
			segmento_nuevo->compartido = false;
		}
		//tengo que crear el el mapeo de cero
		//y llenar tabla de mapeo
		t_list* tabla_de_paginas = list_create();
		int cantidad_de_paginas = redondear_double_arriba((double)datos->tamanio/configuracion->tam_pag);

		for(int i = 0; i < cantidad_de_paginas;i++){
			pagina* pag = malloc(sizeof(pagina));
			pag->num_pagina = i;
			pag->bit_marco = NULL;
			pag->presencia = false;
			pag->bit_swap = NULL;
			list_add(tabla_de_paginas,pag);
		}
		segmento_nuevo->paginas = tabla_de_paginas;
		list_add(tabla_de_segmentos,segmento_nuevo);

		mapeo_t* mapeo_tabla = malloc(sizeof(mapeo_t));
		mapeo_tabla->contador = 1;
		mapeo_tabla->paginas = tabla_de_paginas;
		mapeo_tabla->path = string_duplicate(datos->path);
		mapeo_tabla->tamanio = datos->tamanio;
		mapeo_tabla->tamanio_de_pags = segmento_nuevo->tamanio;
		pthread_mutex_lock(&mutex_tabla_de_mapeo);
		list_add(tabla_de_mapeo,mapeo_tabla);
		pthread_mutex_unlock(&mutex_tabla_de_mapeo);
		acumular_espacio_pedido(datos->id,datos->tamanio);
		return segmento_nuevo->base_logica;
	}
	else{
		pthread_mutex_unlock(&mutex_lugar_disponible);
		//no hay lugar
		return -1;
	}
}
t_list* buscar_mapeo_existente(char* path,int tamanio){
	//me fijo si ya existe ese mapeo, si ya existe y es MAP_SHARED,
	//me traigo un puntero a su tabla de pags y aumento el contador en el segmento
	t_list* tabla_de_paginas = NULL;
	void iteracion(mapeo_t* mapeo){
		if(string_equals_ignore_case(mapeo->path,path) && mapeo->tamanio == tamanio){
			tabla_de_paginas = mapeo->paginas;
			mapeo->contador++;
		}
	}
	pthread_mutex_lock(&mutex_tabla_de_mapeo);
	list_iterate(tabla_de_mapeo,(void*)iteracion);
	pthread_mutex_unlock(&mutex_tabla_de_mapeo);
	return tabla_de_paginas;
}
void* generar_padding(int padding){
	void* void_return = malloc(padding);
	char* barra0 = malloc(1);
	char xd = '\0';
	memcpy(barra0,&xd,1);
	int puntero = 0;
	for(int i = 0;i<padding;i++,puntero++){
		memcpy(void_return+puntero,(void*)barra0,1);
	}
	free(barra0);
	return void_return;
}
/**
* Descarga una cantidad `len` de bytes y lo escribe en el archivo en el FileSystem.
* @param addr Dirección a memoria mappeada.
* @param len Cantidad de bytes a escribir.
* @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
* @note Si `len` es menor que el tamaño de la página en la que se encuentre, se deberá escribir la página completa.
*/
int muse_sync(muse_sync_t* datos){
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(tabla_de_segmentos == NULL){
		log_info(logg,"No hay tabla de segmentos para el id: &s ", datos->id);
		return -1;
	}
	if(list_is_empty(tabla_de_segmentos)){
		log_info(logg,"La tabla de segmentos para el id &s esta vacia", datos->id);
		return -1;
	}

	segmento* segmento_buscado = traer_segmento_de_direccion(tabla_de_segmentos,datos->direccion);
	if(segmento_buscado != NULL){
		int direccion_al_segmento = datos->direccion - segmento_buscado->base_logica;
		if(segmento_buscado->mmapeado){
			int numero_pagina_inicial = direccion_al_segmento / configuracion->tam_pag;
			int numero_pagina_final = (direccion_al_segmento+datos->tamanio)/configuracion->tam_pag;
			int cantidad_de_paginas = numero_pagina_final - numero_pagina_inicial + 1;
			int tam_a_copiar = cantidad_de_paginas * configuracion->tam_pag;
			void* super_void_con_datos = malloc(tam_a_copiar);
			int puntero = 0;
			paginas_de_map_en_memoria(direccion_al_segmento,datos->tamanio,segmento_buscado);
			for(int i = numero_pagina_inicial;i<=numero_pagina_final;i++,puntero+=configuracion->tam_pag,tam_a_copiar-=configuracion->tam_pag){
				pagina* pag = list_get(segmento_buscado->paginas,i);
				void* puntero_a_marco = obtener_puntero_a_marco(pag);
				memcpy(super_void_con_datos+puntero,puntero_a_marco,configuracion->tam_pag);
			}
			int posicion_inicial = numero_pagina_inicial*configuracion->tam_pag;
			FILE* file = fopen(segmento_buscado->path_mapeo,"r+");
			if(file != NULL){
				if(!fseek(file,posicion_inicial,SEEK_SET)){
					//revisar esto!!
					fwrite(super_void_con_datos,cantidad_de_paginas*configuracion->tam_pag,1,file);
					free(super_void_con_datos);
					fclose(file);
					return 0;
				}
			}
			free(super_void_con_datos);
		}
	}
	//si llega aca es xq fallo
	perror("Error en muse_sync: ");
	return -1;
}
/**
* Borra el mappeo a un archivo hecho por muse_map.
* @param dir Dirección a memoria mappeada.
* @param
* @note Esto implicará que todas las futuras utilizaciones de direcciones basadas en `dir` serán accesos inválidos.
* @note Solo se deberá cerrar el archivo mappeado una vez que todos los hilos hayan liberado la misma cantidad de muse_unmap que muse_map.
* @return Si pasa un error, retorna -1. Si la operación se realizó correctamente, retorna 0.
*/
int muse_unmap(muse_unmap_t* datos){
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(tabla_de_segmentos==NULL){
		log_info(logg,"no se encontro tabla de segmentos de %s", datos->id);
		return -1;
	}
	segmento* segmento_buscado = traer_segmento_de_direccion(tabla_de_segmentos,datos->direccion);
	if(segmento_buscado!=NULL){
		if(segmento_buscado->mmapeado){
			bajar_mapeo(segmento_buscado->path_mapeo,segmento_buscado->tamanio_mapeo,datos->id);
			return 0;
		}
	}
	perror("error en unmap: ");
	return -1;
}
void bajar_mapeo(char* path_mapeo, int tam_mapeo, char* id_programa){

	_Bool encontrar_mapeo(mapeo_t* _mapeo){
		return _mapeo->tamanio == tam_mapeo && string_equals_ignore_case(_mapeo->path,path_mapeo);
	}

	_Bool esPrograma(programa_t* prog){
		return string_equals_ignore_case(prog->id_programa,id_programa);
	}
	_Bool esSegmento(segmento* seg){
		if(seg->mmapeado){
			return string_equals_ignore_case(seg->path_mapeo,path_mapeo) && seg->tamanio_mapeo == tam_mapeo;
		}
		return false;
	}
	//busco el mapeo
	pthread_mutex_lock(&mutex_tabla_de_mapeo);
	mapeo_t* mapeo_encontrado = list_find(tabla_de_mapeo,(void*)encontrar_mapeo);
	if(mapeo_encontrado != NULL){
		mapeo_encontrado->contador--;
		//quiero sacar el segmento de la lista del programa
		pthread_mutex_lock(&mutex_tabla_de_programas);
		programa_t* prog = list_find(tabla_de_programas,(void*)esPrograma);
		pthread_mutex_unlock(&mutex_tabla_de_programas);
		list_remove_by_condition(prog->tabla_de_segmentos,(void*)esSegmento);

		if(mapeo_encontrado->contador==0){//si no quedan mas referenciandolo se elimina
			//libero los bits de memoria
			//list_iterate(mapeo_encontrado->paginas,(void*)liberar_pags);
			//elimino el mapeo de la lista
			list_remove_and_destroy_by_condition(tabla_de_mapeo,(void*)encontrar_mapeo,(void*)mapeo_destroy);
		}
	}
	pthread_mutex_unlock(&mutex_tabla_de_mapeo);
}
void mapeo_destroy(mapeo_t* _mapeo){
	free(_mapeo->path);
	list_destroy_and_destroy_elements(_mapeo->paginas,(void*)destroy_pagina);
	free(_mapeo);
}
int muse_close(char* id_cliente){
	//sacarlo de la tabla de programas
	//con el id del cliente tengo que sacar el programa?
	_Bool esPrograma(programa_t* programa){
			return string_equals_ignore_case(programa->id_programa,id_cliente);
	}
	pthread_mutex_lock(&mutex_tabla_de_programas);
	programa_t* prog = list_find(tabla_de_programas,(void*)esPrograma);
	pthread_mutex_unlock(&mutex_tabla_de_programas);
	if(prog!=NULL)
	{
		//encontro el programa del cliente
		//me conviene tirar antes las metricas, asi me fijo los leaks y eso
		metricas(prog->id_programa);
		//hay que sacarlo de la lista
		log_info(logg,"muse_close para %s",prog->id_programa);
		pthread_mutex_lock(&mutex_tabla_de_programas);
		list_remove_and_destroy_by_condition(tabla_de_programas,
				(void*)esPrograma,(void*)destroy_programa);
		pthread_mutex_unlock(&mutex_tabla_de_programas);
	}
	return 0;
}
uint32_t crear_servidor(uint32_t puerto){
	/*== creamos el socket ==*/
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puerto);
	servidor = socket(AF_INET,SOCK_STREAM,0);
	/*== socket reusable multiples conexiones==*/
	uint32_t flag = 1;
	setsockopt(servidor, SOL_SOCKET,SO_REUSEPORT,&flag,sizeof(flag));
	/*== inicializamos el socket ==*/
	if(bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		perror("Fallo el binde0 del servidor");
		return 1;
	}
	log_info(logg,"Estoy escuchando en el puerto %d",puerto);
	listen(servidor,SOMAXCONN);
	return servidor;
}
uint32_t conectar_socket_a(char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM,0);
	if (connect(cliente,(void*) &direccionServidor, sizeof(direccionServidor)) != 0){
		return -1;
	}
	return cliente;
}
void mandar_char(char* _char, uint32_t _socket,uint32_t com){
	uint32_t tam = strlen(_char)+1;
	uint32_t bytes = tam + sizeof(uint32_t)*2;
	void* magic = malloc(bytes);
	uint32_t puntero = 0;
	memcpy(magic+puntero,&com,4);
	puntero += 4;
	memcpy(magic+puntero,&tam,4);
	puntero += 4;
	memcpy(magic+puntero,_char,tam);
	puntero += tam;
	send(_socket,magic,bytes,0);
	free(magic);
}
uint32_t aceptar_cliente(uint32_t servidor){
	struct sockaddr_in direccion_cliente;
	uint32_t tamanio_direccion = sizeof(struct sockaddr_in);
	uint32_t cliente;
	cliente = accept(servidor,(void*) &direccion_cliente,&tamanio_direccion);
	puts("conexion recibida!");
	return cliente;
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
	int tam, operacion_respuesta, resultado, operacion;
	char* id_cliente;
	void* respuesta;
	while(recv(socket,&operacion,4,MSG_WAITALL) >0 && exit_loop==false){
		printf("\n");
		log_info(logg,"-Nuevo pedido de %d",socket);
		switch (operacion) {
			case MUSE_INIT:;
				log_info(logg,"-INIT");
				//recibo int pid, crep el char* id y se lo mando
				uint32_t pid;
				recv(socket,&pid,4,0);
				char* pid_char = string_itoa(pid);
				//hay q conseguir la ip de el cliente xd
				id_cliente = string_new();
				struct sockaddr_in addr;
				uint32_t addrlen = sizeof(addr);
				getpeername(socket, (struct sockaddr *)&addr, &addrlen);
				char* ip = inet_ntoa(addr.sin_addr);
				string_append(&id_cliente,ip);
				string_append(&id_cliente,"-");
				string_append(&id_cliente,pid_char);
				log_info(logg,"nuevo cliente, id: %s",id_cliente);
				mandar_char(id_cliente,socket,operacion);
				programa_t* programa = malloc(sizeof(programa_t));
				programa->tabla_de_segmentos = list_create();
				programa->id_programa = id_cliente;
				pthread_mutex_lock(&mutex_tabla_de_programas);
				list_add(tabla_de_programas,programa);
				pthread_mutex_unlock(&mutex_tabla_de_programas);
				free(pid_char);
				break;
			case MUSE_ALLOC:;
				recv(socket,&tam,4,0);
				void* vmat = malloc(tam);
				recv(socket,vmat,tam,0);
				muse_alloc_t* datos = deserializar_muse_alloc(vmat);
				log_info(logg,"-ALLOC de %d bytes",datos->tamanio);
				if(datos->tamanio > configuracion->tam_mem){
					respuesta = malloc(4);
					operacion_respuesta = MUSE_ERROR;
					memcpy(respuesta,&operacion_respuesta,4);
					send(socket,respuesta,4,0);
					log_info(logg,"No se puede pedir de una mas del tamaño de la memoria");
				}
				resultado = muse_alloc(datos);
				if(resultado>=0){
					respuesta = malloc(8);
					operacion_respuesta = MUSE_INT;
					memcpy(respuesta,&operacion_respuesta,4);
					memcpy(respuesta+4,&resultado,4);
					send(socket,respuesta,8,0);
					log_info(logg,"mando direccion virtual a %d: %d",socket,resultado);
				}
				else{
					respuesta = malloc(4);
					operacion_respuesta = MUSE_ERROR;
					memcpy(respuesta,&operacion_respuesta,4);
					send(socket,respuesta,4,0);
					log_info(logg,"Error en muse_alloc");
				}
				free(respuesta);
				muse_alloc_destroy(datos);
				free(vmat);
				break;
			case MUSE_FREE:;
				recv(socket,&tam,4,0);
				void* vmft = malloc(tam);
				recv(socket,vmft,tam,0);
				muse_free_t* dmft = deserializar_muse_free(vmft);
				log_info(logg,"-FREE de direccion %d",dmft->direccion);
				resultado = muse_free(dmft);
				if(resultado == -1){
					operacion_respuesta = MUSE_SEG_FAULT;
				}
				else{
					operacion_respuesta = MUSE_EXITOSO;
				}
				send(socket,&operacion_respuesta,4,0);
				log_info(logg,"haciendo free de %d, resultado: %d",socket,resultado);
				muse_free_destroy(dmft);
				free(vmft);
				break;
			case MUSE_GET:;
				log_info(logg,"-GET");
				recv(socket,&tam,4,0);
				void* vmgt = malloc(tam);
				recv(socket,vmgt,tam,0);
				muse_get_t* mgt = deserializar_muse_get(vmgt);
				void* resultado_get = muse_get(mgt);
				//devuelve el void* resultado
				if(resultado_get != NULL){
					muse_void* mv = crear_muse_void(resultado_get,mgt->tamanio);
					respuesta = serializar_muse_void(mv);
					uint32_t tamanio_respuesta;
					memcpy(&tamanio_respuesta,respuesta+4,4);
					send(socket,respuesta,tamanio_respuesta,0);
					// ^^error de valgrind q no pude arreglar??
					log_info(logg,"enviando resolucion del get a: %d",socket);
					free(resultado_get);
					free(respuesta);
					muse_void_destroy(mv);
				}
				else{
//					todos los errores son segm fault??
					operacion_respuesta = MUSE_SEG_FAULT;
					send(socket,&operacion_respuesta,4,0);
				}
				muse_get_destroy(mgt);
				free(vmgt);
				break;
			case MUSE_CPY:;
				log_info(logg,"-CPY");
				recv(socket,&tam,4,0);
				void* vmct = malloc(tam);
				recv(socket,vmct,tam,0);
				muse_cpy_t* mct = deserializar_muse_cpy(vmct);
				resultado = muse_cpy(mct);
				if(resultado == -1){//fallo
					operacion_respuesta = MUSE_SEG_FAULT;
					send(socket,&respuesta,4,0);
					free(vmct);
					muse_cpy_destroy(mct);
				}
				else{
					//funciono
					operacion_respuesta = MUSE_EXITOSO;
					send(socket,&respuesta,4,0);
					muse_cpy_destroy(mct);
					free(vmct);
				}
				log_info(logg,"enviando resolucion del cpy a: %d, resultado: %d",socket,resultado);
				break;
			case MUSE_MAP:
				log_info(logg,"-MAP");
				recv(socket,&tam,4,0);
				void* vmmt = malloc(tam);
				recv(socket,vmmt,tam,0);
				muse_map_t* dmmt = deserializar_muse_map(vmmt);
				resultado = muse_map(dmmt);
				//devuelve la direccion de memoria en muse
				void* void_respuesta;
				if(resultado <0){
					//error
					void_respuesta = malloc(4);
					int _op = MUSE_ERROR;
					memcpy(void_respuesta,&_op,4);
					send(socket,void_respuesta,4,0);
					log_info(logg, "error en el map de %d",socket);
				}
				else{
					int _op = MUSE_INT;
					void_respuesta = malloc(8);
					memcpy(void_respuesta,&_op,4);
					memcpy(void_respuesta+4,&resultado,4);
					send(socket,void_respuesta,8,0);
					log_info(logg,"enviando resolucion del map de%d: %d",socket,resultado);
				}
				muse_map_destroy(dmmt);
				free(vmmt);
				free(void_respuesta);
				break;
			case MUSE_SYNC://hay que hacerla bien
				log_info(logg,"-SYNC");
				recv(socket,&tam,4,0);
				void* vmst = malloc(tam);
				recv(socket,vmst,tam,0);
				muse_sync_t* dmst = deserializar_muse_sync(vmst);
				resultado = muse_sync(dmst);
				//devuelve si esta to do ok o no
				send(socket,&resultado,4,0);
				log_info(logg,"enviando resolucion del sync %d a: %d",socket,resultado);
				muse_sync_destroy(dmst);
				free(vmst);
				break;
			case MUSE_UNMAP://hay que hacerla bien
				log_info(logg,"-UNMAP");
				recv(socket,&tam,4,0);
				void* vmut = malloc(tam);
				recv(socket,vmut,tam,0);
				muse_unmap_t* dmut = deserializar_muse_unmap(vmut);
				resultado = muse_unmap(dmut);
				//devuelve si esta to do ok o no
				send(socket,&resultado,4,0);
				log_info(logg,"enviando resolucion del unmap %d a: %d",socket,resultado);
				muse_unmap_destroy(dmut);
				free(vmut);
				break;
			case MUSE_CLOSE:;
				log_info(logg,"-CLOSE");
//				si no se libera algun muse_alloc-> es un memory leak
//				liberar tabla de programas

				resultado = muse_close(id_cliente);
				log_info(logg,"Se fue %d",socket);
				exit_loop = true;
				break;
			default:
				//xd
				break;
		}
	}
	close(socket);
}
void init_bitarray(){
	bitarray = malloc(sizeof(bitarray_nuestro));
	bitarray->size_memoria = CANT_PAGINAS_MEMORIA;
	bitarray->bitarray_memoria = list_create();
	for(uint32_t i = 0; i<CANT_PAGINAS_MEMORIA; i++){
		t_bit_memoria* bit = malloc(sizeof(t_bit_memoria));
		bit->ocupado = false;
		bit->posicion = i;
		bit->bit_modificado = false;
		bit->bit_uso = false;
		list_add(bitarray->bitarray_memoria,bit);
	}
	bitarray->size_memoria_virtual = CANT_PAGINAS_MEMORIA_VIRTUAL;
	bitarray->bitarray_memoria_virtual = list_create();
	for(uint32_t i = 0; i<CANT_PAGINAS_MEMORIA_VIRTUAL; i++){
		t_bit_swap* bit = malloc(sizeof(t_bit_swap));
		bit->posicion = i;
		bit->ocupado = false;
		list_add(bitarray->bitarray_memoria_virtual,bit);
	}
}
void destroy_bitarray(){
	list_destroy_and_destroy_elements(bitarray->bitarray_memoria,free);
	list_destroy_and_destroy_elements(bitarray->bitarray_memoria_virtual,free);
	free(bitarray);
}
t_bit_swap* bit_libre_memoria_virtual(){
	pthread_mutex_lock(&mutex_bitarray);
	_Bool bit_libre(t_bit_swap* bit){
		return !bit->ocupado;
	}
	pthread_mutex_unlock(&mutex_bitarray);
	return list_find(bitarray->bitarray_memoria_virtual,(void*)bit_libre);
}
t_bit_memoria* bit_libre_memoria(){
	pthread_mutex_lock(&mutex_bitarray);
	//si encuentro uno, ya le pongo como que esta usado
	_Bool bit_libre(t_bit_memoria* bit){
		return !bit->ocupado;
	}
	t_bit_memoria* bit_asignado = list_find(bitarray->bitarray_memoria,(void*)bit_libre);
	if(bit_asignado!=NULL){
		bit_asignado->ocupado = true;
		bit_asignado->bit_modificado = true;
		bit_asignado->bit_uso = true;
	}
	pthread_mutex_unlock(&mutex_bitarray);
	return bit_asignado;
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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
	mct->paquete = malloc(mct->size_paquete);
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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
	//2 int de adentro de mat y 2 int de comando y tamanio
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

void destroy_programa(programa_t* prog){
	_Bool esPrograma(programa_t* programa){
		return string_equals_ignore_case(programa->id_programa,prog->id_programa);
	}
	// !! hay algo en el destroy que rompe el tema de pag -> bit
	// como que igual antes removed el programa de la lista ??
	list_destroy_and_destroy_elements(prog->tabla_de_segmentos,(void*)destroy_segmento);
	list_remove_by_condition(tabla_de_programas,(void*)esPrograma);
}
void destroy_segmento(segmento* seg)
{
	if(!seg->mmapeado){
		list_destroy_and_destroy_elements(seg->info_heaps,(void*)free);
		list_destroy_and_destroy_elements(seg->paginas,(void*)destroy_pagina);
	}else{
		destroy_mapeo(seg);
	}
	free(seg);
}

void destroy_pagina(pagina* pag)
{
	//libero antes los marcos
	if(pag->bit_marco != NULL){
		pag->bit_marco->ocupado = false;
		pag->bit_marco = NULL;
	}
	if(pag->bit_swap != NULL){
		pag->bit_swap->ocupado = false;
	}
	free(pag);
}

void metricas(char* id_cliente)
{
	//Cuando un programa finaliza, tanto correctamente como al generar un segmentation fault
	//Cuando se realiza una nueva petición de memoria muse_alloc
	log_info(log_metricas,"----> METRICAS <----");
	log_info(log_metricas,"-DEL SISTEMA");
	metrica_del_sistema();
	log_info(log_metricas,"-POR SOCKET CONECTADO");
	metrica_por_socket_conectado();
	log_info(log_metricas,"-POR PROGRAMA");
	metrica_por_programa(id_cliente);
}
void metrica_por_socket_conectado()
{
	//Cantidad de memoria disponible (en bytes) en el último segmento
	//de memoria dinámica pedido

	int total_de_segmentos=0;
	pthread_mutex_lock(&mutex_tabla_de_programas);
	for (int i=0;i<tabla_de_programas->elements_count;i++){
		programa_t* prog = list_get(tabla_de_programas,i);
		total_de_segmentos+=prog->tabla_de_segmentos->elements_count;
	}
	for (int i=0;i<tabla_de_programas->elements_count;i++){
		//agarro cada programa
		programa_t* prog = list_get(tabla_de_programas,i);
		log_info(log_metricas,"PROGRAMA %s",prog->id_programa);
		if(prog->tabla_de_segmentos->elements_count==0)
		{
			log_info(log_metricas,"No tiene segmentos");
			pthread_mutex_unlock(&mutex_tabla_de_programas);
			return;
		}
		segmento* seg = list_last_element(prog->tabla_de_segmentos);
		if(!seg->mmapeado){
			//cantidad de memoria disponible al final de cada programa
			int espacio_libre= 0;
			for(int i=0;i<seg->info_heaps->elements_count;i++){
				heap_lista* heap_lista = list_get(seg->info_heaps,i);
				if(heap_lista->is_free){
					espacio_libre+=heap_lista->espacio;
				}
			}
			log_info(log_metricas,"el espacio en el ultimo segmento %i",espacio_libre);
		}
		else{
			log_info(log_metricas,"el ultimo segmento esta mmapeado");
		}

		if(total_de_segmentos>0){
			//agarro cada programa
			//porcentaje de todos los segmentos
			log_info(log_metricas,"%s tiene el %i porciento de los segmentos pedidos",prog->id_programa,prog->tabla_de_segmentos->elements_count*100/total_de_segmentos);
		}
		else{
			log_info(log_metricas,"No hay segmentos");
		}
	}
	pthread_mutex_unlock(&mutex_tabla_de_programas);
}

void metrica_por_programa(char* id_cliente){
	pthread_mutex_lock(&mutex_tabla_de_programas);
	pthread_mutex_lock(&mutex_tabla_de_memoria);
	for (int i=0;i<tabla_de_programas->elements_count;i++){
		//agarro cada programa
		programa_t* prog = list_get(tabla_de_programas,i);
		log_info(log_metricas,"PROGRAMA %s",prog->id_programa);
		//memoria liberada por programa
		_Bool esMemoriaDelPrograma(memoria_liberada* mem_liberada){
			return string_equals_ignore_case(mem_liberada->programa_id,prog->id_programa);
		}
		memoria_liberada* mem_liberada = list_find(tabla_de_memoria_liberada,(void*)esMemoriaDelPrograma);
		if(mem_liberada!=NULL){
			int memoria_liberada_por_programa = mem_liberada->memoria_liberada_acumulada;
			log_info(log_metricas,"Liberado acumulado: %i",memoria_liberada_por_programa);
		}
		else{
			log_info(log_metricas,"Aun no ha liberado memoria");
		}
		memoria_pedida* mem_pedida = list_find(tabla_de_memoria_pedida,(void*)esMemoriaDelPrograma);
		if(mem_pedida!=NULL){
			int memoria_pedida_por_programa = mem_pedida->memoria_pedida_acumulada;
			log_info(log_metricas,"Pedido acumulado: %i",memoria_pedida_por_programa);
		}
		else{
			log_info(log_metricas,"Aun no ha pedido memoria");
		}

		int memoria_ocupada=0;
		int memoria_leaks=0;
		for (int j=0;j<prog->tabla_de_segmentos->elements_count;j++){
			//agarro cada segmento del programa
			segmento* seg = list_get(prog->tabla_de_segmentos,j);

			if(!seg->mmapeado){
				memoria_ocupada+=seg->tamanio; //todas las paginas reservadas del segmento
				for(int i = 0;i<seg->info_heaps->elements_count;i++){
					heap_lista* heap_lista = list_get(seg->info_heaps,i);
					if(!heap_lista->is_free){
						memoria_leaks+=heap_lista->espacio+sizeof(heap_metadata);
					}
				}
			}
			else{
				memoria_ocupada+=seg->tamanio_mapeo;
				memoria_leaks+=seg->tamanio_mapeo;
			}
		}
		log_info(log_metricas,"Espacio reservado: %i",memoria_ocupada);
		if(string_equals_ignore_case(id_cliente,prog->id_programa)){
			//es un socket que esta cerrandose, miro los leaks
			//leaks deberia ser solo si el programa hizo muse_close
			log_info(log_metricas,"Cerrandose con");
			log_info(log_metricas,"Leaks: %i",memoria_leaks);
		}
	}
	pthread_mutex_unlock(&mutex_tabla_de_memoria);
	pthread_mutex_unlock(&mutex_tabla_de_programas);
}

void metrica_del_sistema(){
//Del sistema:
//Cantidad de memoria disponible (en bytes)
	pthread_mutex_lock(&mutex_lugar_disponible);
	log_info(log_metricas,"el espacio disponible en la memoria es %i",lugar_disponible);
	pthread_mutex_unlock(&mutex_lugar_disponible);
}

void acumular_espacio_liberado(char* programa, int cuanto){
	_Bool esMemoriaDelPrograma(memoria_liberada* mem_liberada){
				return string_equals_ignore_case(mem_liberada->programa_id,programa);
	}
	pthread_mutex_lock(&mutex_tabla_de_memoria);
	memoria_liberada* mem_liberada = list_find(tabla_de_memoria_liberada,(void*)esMemoriaDelPrograma);

	if(mem_liberada==NULL){
		//si no hay, creo uno
		memoria_liberada* mem_liberada = malloc(sizeof(memoria_liberada));
		mem_liberada->programa_id = string_new();
		string_append(&mem_liberada->programa_id,programa);
		mem_liberada->memoria_liberada_acumulada=cuanto;
		list_add(tabla_de_memoria_liberada,mem_liberada);

	}else {
		mem_liberada->memoria_liberada_acumulada+=cuanto;
	}
	pthread_mutex_unlock(&mutex_tabla_de_memoria);
}


void acumular_espacio_pedido(char* programa, int cuanto)
{
	_Bool esMemoriaDelPrograma(memoria_pedida* mem_pedida){
				return string_equals_ignore_case(mem_pedida->programa_id,programa);
	}
	pthread_mutex_lock(&mutex_tabla_de_memoria);
	memoria_pedida* mem_pedida = list_find(tabla_de_memoria_pedida,(void*)esMemoriaDelPrograma);

	if(mem_pedida==NULL)
	{ //si no hay, creo uno
		memoria_pedida* mem_pedida = malloc(sizeof(memoria_pedida));
		mem_pedida->programa_id = string_new();
		string_append(&mem_pedida->programa_id,programa);
		mem_pedida->memoria_pedida_acumulada=cuanto;
		list_add(tabla_de_memoria_pedida,mem_pedida);

	}else {
		mem_pedida->memoria_pedida_acumulada+=cuanto;
	}
	pthread_mutex_unlock(&mutex_tabla_de_memoria);
}

void destroy_mapeo(segmento* seg)
{
	_Bool esSegmentoEnMapeo (mapeo_t* mpt)
	{
		return seg->tamanio_mapeo == mpt->tamanio && string_equals_ignore_case(mpt->path,seg->path_mapeo);

	}
	pthread_mutex_lock(&mutex_tabla_de_mapeo);
	mapeo_t* mpt = list_find(tabla_de_mapeo,(void*)esSegmentoEnMapeo);
	pthread_mutex_unlock(&mutex_tabla_de_mapeo);
	mpt->contador--;

	if(mpt->contador==0)
	{
		list_destroy_and_destroy_elements(mpt->paginas,(void*)destroy_pagina);
		pthread_mutex_lock(&mutex_tabla_de_mapeo);
		list_remove_by_condition(tabla_de_mapeo,(void*)esSegmentoEnMapeo);
		pthread_mutex_unlock(&mutex_tabla_de_mapeo);
		free(mpt->path);
		free(mpt);
	}
}

