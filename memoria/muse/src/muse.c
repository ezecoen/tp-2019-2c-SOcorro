#include "muse.h"

int main(int argc, char **argv) {
//	INICIANDO

	path_de_config = string_duplicate("/home/utnso/tp-2019-2c-SOcorro/memoria/muse/muse.config");
	//path_de_config = string_duplicate(argv[1]);
	iniciar_log(path_de_config);
	leer_config(path_de_config);
	init_estructuras();

//	programa_t* programa = malloc(sizeof(programa_t));
//	programa->tabla_de_segmentos = list_create();
//	programa->id_programa = string_new();
//	string_append(&programa->id_programa,"asdasd");
//	list_add(tabla_de_programas,programa);
//
//	programa_t* programa1 = malloc(sizeof(programa_t));
//	programa1->tabla_de_segmentos = list_create();
//	programa1->id_programa = string_new();
//	string_append(&programa1->id_programa,"asdasd1");
//	list_add(tabla_de_programas,programa1);
//
//	programa_t* programa2 = malloc(sizeof(programa_t));
//	programa2->tabla_de_segmentos = list_create();
//	programa2->id_programa = string_new();
//	string_append(&programa2->id_programa,"asdasd2");
//	list_add(tabla_de_programas,programa2);
//
//	//	prueba creo segmento 1
//	muse_alloc_t* mat = crear_muse_alloc(100,"asdasd");
//	int result = muse_alloc(mat);
//	printf("\nDireccion virtual de %d|%d|%d: %d",0,0,0,result);
//	fflush(stdout);
//
//	//	prueba creo segmento 2
//	muse_alloc_t* mat1 = crear_muse_alloc(500,"asdasd1");
//	int result1 = muse_alloc(mat1);
//	printf("\nDireccion virtual de %d|%d|%d: %d",1,0,0,result1);
//	//	prueba creo segmento 3
//	muse_alloc_t* mat2 = crear_muse_alloc(250,"asdasd");
//	int result2 = muse_alloc(mat2);
//	printf("\nDireccion virtual de %d|%d|%d: %d",2,0,0,result2);
//	//	prueba uso segmento 3
//	muse_alloc_t* mat3 = crear_muse_alloc(1000,"asdasd1");
//	int result3 = muse_alloc(mat3);
//	printf("\nDireccion virtual de %d|%d|%d: %d\n",2,18,0,result3);

//	Pruebas clock modificado
	t_bit_memoria* llenar_bits(t_bit_memoria* _bit){
		_bit->ocupado = true;
		_bit->bit_modificado = true;
		_bit->bit_uso = true;
		return _bit;
	}
	list_map(bitarray->bitarray_memoria,(void*)llenar_bits);
//	t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,5);
//	_bit->bit_uso = true;
//	_bit->bit_modificado = true;
//	t_bit_memoria* _bit2 = list_get(bitarray->bitarray_memoria,7);
//	_bit2->bit_uso = false;
//	_bit2->bit_modificado = true;
	posicion_puntero_clock = 4;
	t_bit_memoria* _b = ejecutar_clock_modificado();
	printf("\n%d-numero de bit: %d.(%d,%d)\n",_b->ocupado,_b->bit_position,_b->bit_uso,_b->bit_modificado);
	return 0;
//	SERVIDOR
	uint32_t servidor = crear_servidor(configuracion->puerto);
	while(true){
		esperar_conexion(servidor);
	}

	return EXIT_SUCCESS;
}
void init_estructuras(){
	upcm = malloc(configuracion->tam_mem);
	swap = malloc(configuracion->tam_swap);//provisorio
	lugar_disponible = configuracion->tam_mem+configuracion->tam_swap;
	tabla_de_programas = list_create();
	CANT_PAGINAS_MEMORIA = configuracion->tam_mem/configuracion->tam_pag;
	CANT_PAGINAS_MEMORIA_VIRTUAL = configuracion->tam_swap/configuracion->tam_pag;

	DIR_TAM_DESPLAZAMIENTO = log_2((double)configuracion->tam_pag);
	DIR_TAM_DIRECCION = 32;
	DIR_TAM_PAGINA = DIR_TAM_DIRECCION - DIR_TAM_DESPLAZAMIENTO;

	init_bitarray();
	posicion_puntero_clock = 0;
}
int log_2(double n){
	//testea2
	//redondea el valor hacia arriba y funciona siempre bien con los bits
     int logValue = 0;
     while (n>1) {
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
void dec_a_bin(char destino[],int dec,int tam){
	//testea2
	if (dec >= 0 && dec <= 2147483647){
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
	//testea2
	char *p = binario;
	int   r = 0;

	while (p && *p ) {
		r <<= 1;
		r += (unsigned int)((*p++) & 0x01);
	}
	return (int)r;
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
	programa_t* programa_buscado = list_find(tabla_de_programas,(void*)id_programa_igual);
	return programa_buscado->tabla_de_segmentos;
}
void liberar_paginas_de_segmento(segmento* segmento) {
	//Esto solo recorre toodo el segmento
	//en realidad, la lista de heaps
	//y va calculando cuales paginas pueden liberarse
	for(int i=0;i<list_size(segmento->info_heaps);i++) {
		int dir_heap = list_get(segmento->info_heaps,i);
		heap_metadata* _heap = malloc(sizeof(heap_metadata));
		int numero_de_pagina = redondear_double_arriba((double)dir_heap/configuracion->tam_pag);
		pagina* pag = buscar_pagina_por_numero(segmento->paginas,numero_de_pagina);
		//memccpy(_heap,dir_heap+pag->,sizeof(heap_metadata));
		if(_heap->is_free) {
			//si esta free
			//y no es la primera..
			if(i!=0){
			//tengo que mirar la pag siguiente y anterior
			// y ver si puedo liberar algo

			}
		} else {
			//si no esta free ..
			//nada?
		}
	}
}
int muse_alloc(muse_alloc_t* datos){
//me fijo si hay lugar disponible
int direccion_return = -1;
if(lugar_disponible >= datos->tamanio+sizeof(heap_metadata)){
	//busco si ya tengo algun segmento
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	if(list_is_empty(tabla_de_segmentos)){
		//hay que crear el 1er segmento
		uint32_t cantidad_de_paginas = paginas_necesarias_para_tamanio(datos->tamanio+sizeof(heap_metadata)*2);
		int espacio_libre_ultima_pag = cantidad_de_paginas*configuracion->tam_pag-datos->tamanio-sizeof(heap_metadata)*2;
		if(lugar_disponible>=cantidad_de_paginas*configuracion->tam_pag){
			lugar_disponible -= cantidad_de_paginas*configuracion->tam_pag;
			segmento* segmento_nuevo = malloc(sizeof(segmento));
			segmento_nuevo->compartido = false;
			segmento_nuevo->mmapeado = false;
			segmento_nuevo->base_logica = 0;
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
				pag->bit_marco = asignar_marco_nuevo();
				if(i == 0){//si es la 1ra => hay que agregar el heap al inicio
					heap_metadata* heap_nuevo = malloc(sizeof(heap_metadata));
					heap_nuevo->is_free = false;
					heap_nuevo->size = datos->tamanio;
					void* puntero_a_marco = obtener_puntero_a_marco(pag->bit_marco);
					memcpy(puntero_a_marco,heap_nuevo,sizeof(heap_metadata));
					free(heap_nuevo);
				}
				if(i == cantidad_de_paginas-2){
					if(hay_que_entrar_en_la_anteultima_pag){
						void* puntero_a_marco = obtener_puntero_a_marco(pag->bit_marco);
						memcpy(puntero_a_marco+offset_heap,heap_al_final,heap_en_ante_ultima_pagina);
					}
				}
				if(i == cantidad_de_paginas-1){
					void* puntero_a_marco = obtener_puntero_a_marco(pag->bit_marco);
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
				void* marco_inicial = obtener_puntero_a_marco(pagina_inicial->bit_marco);
				memcpy(marco_inicial+offset_inicial,heap_inicial,tamanio_a_copiar);

				pagina* pagina_inicial2 = list_get(segmento_buscado->paginas,num_pagina_inicial+1);
				void* marco_inicial2 = obtener_puntero_a_marco(pagina_inicial2->bit_marco);
				memcpy(marco_inicial2,heap_inicial+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);

				direccion_return = segmento_buscado->base_logica + pagina_inicial2->num_pagina*configuracion->tam_pag
						+ sizeof(heap_metadata)-tamanio_a_copiar;
			}
			else{
				//no queda en la mitad :D
				pagina* pagina_inicial = list_get(segmento_buscado->paginas,num_pagina_inicial);
				void* marco_inicial = obtener_puntero_a_marco(pagina_inicial->bit_marco);
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
				void* marco_final = obtener_puntero_a_marco(pagina_final->bit_marco);
				memcpy(marco_final+offset_inicial,heap_final,tamanio_a_copiar);

				pagina* pagina_final2 = list_get(segmento_buscado->paginas,num_pagina_final+1);
				void* marco_final2 = obtener_puntero_a_marco(pagina_final2->bit_marco);
				memcpy(marco_final2,heap_final+tamanio_a_copiar,sizeof(heap_metadata)-tamanio_a_copiar);
			}
			else{
				//no queda en la mitad :D
				pagina* pagina_final = list_get(segmento_buscado->paginas,num_pagina_final);
				void* marco_final = obtener_puntero_a_marco(pagina_final->bit_marco);
				memcpy(marco_final+offset_inicial,heap_final,sizeof(heap_metadata));
			}
			heap_lista_encontrado->espacio = datos->tamanio;
			heap_lista_encontrado->is_free = false;

			heap_lista* heap_lista_nuevo = malloc(sizeof(heap_lista));
			heap_lista_nuevo->direccion_heap_metadata = num_pagina_final*configuracion->tam_pag + offset_final;
			heap_lista_nuevo->espacio = heap_final->size;
			heap_lista_nuevo->is_free = true;
			//agrego el nuevo heap_lista dsps del anterior
			list_add_in_index(segmento_buscado->info_heaps,heap_lista_encontrado->indice+1,heap_lista_nuevo);

			free(heap_inicial);
			free(heap_final);
			//HICE HASTA ACA IGUAL HAY QUE REVISAR!!!!!!!!!!!!!!!!!!!
		}
		else{
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
				void* puntero_a_marco = obtener_puntero_a_marco(ultima_pagina->bit_marco);
				uint32_t lugar_extra_necesario = datos->tamanio-lista_ultimo_heap->espacio+sizeof(heap_metadata);
				uint32_t paginas_necesarias = paginas_necesarias_para_tamanio(lugar_extra_necesario);
				uint32_t tamanio_paginas_necesarias = paginas_necesarias*configuracion->tam_pag;
				if(lugar_disponible >= tamanio_paginas_necesarias){//=>hay lugar, reservo las nuevas pags
					lugar_disponible -= tamanio_paginas_necesarias;
					ultimo_segmento->tamanio+=tamanio_paginas_necesarias;
					//agrego las pags a la lista del segmento
					int espacio_libre_ultima_pag = tamanio_paginas_necesarias-datos->tamanio
							- sizeof(heap_metadata)+lista_ultimo_heap->espacio;
					//actualizo el ex ultimo heap
					lista_ultimo_heap->espacio=datos->tamanio;
					nuevo_ultimo_heap->size = espacio_libre_ultima_pag;
					_Bool agarrar_direccion_de_primera_pag_agregada = false;
					if(nuevo_ultimo_heap->size==0){
						agarrar_direccion_de_primera_pag_agregada = true;
					}
					for(int i = 0;i<paginas_necesarias;i++){
						pagina* pagina_nueva = malloc(sizeof(pagina*));
						pagina_nueva->num_pagina = ultima_pagina->num_pagina+1+i;
						pagina_nueva->presencia = true;
						pagina_nueva->bit_marco = asignar_marco_nuevo();
						list_add(ultimo_segmento->paginas,pagina_nueva);
						if(i==0){
							if(agarrar_direccion_de_primera_pag_agregada){
								direccion_return = ultimo_segmento->base_logica+pagina_nueva->num_pagina*configuracion->tam_pag;
							}
						}
					} //termina el for
					heap_lista* heap_lista_nuevo = malloc(sizeof(heap_lista));
					heap_lista_nuevo->direccion_heap_metadata=sizeof(heap_metadata)+lista_ultimo_heap->direccion_heap_metadata+datos->tamanio;
					heap_lista_nuevo->espacio=espacio_libre_ultima_pag;
					heap_lista_nuevo->indice=ultimo_segmento->info_heaps->elements_count;
					heap_lista_nuevo->is_free=true;
					list_add_in_index(ultimo_segmento->info_heaps,heap_lista_nuevo->indice+1,heap_lista_nuevo);

					if(!agarrar_direccion_de_primera_pag_agregada){//=>tengo que llenar direccion_return
						int offset_pagina = lista_ultimo_heap->direccion_heap_metadata;
						direccion_return = ultimo_segmento->base_logica+ultimo_segmento->paginas->elements_count*
								configuracion->tam_pag+offset_pagina+sizeof(heap_metadata);
					}
					int offset_heap_al_marco = lista_ultimo_heap->direccion_heap_metadata%configuracion->tam_pag;
					memcpy(puntero_a_marco+offset_heap_al_marco,ultimo_heap,sizeof(heap_metadata));
					memcpy(puntero_a_marco+lista_ultimo_heap->direccion_heap_metadata
							+datos->tamanio,nuevo_ultimo_heap,sizeof(heap_metadata));
					free(ultimo_heap);
					free(nuevo_ultimo_heap);

				}
				else{//=>no hay lugar
					free(ultimo_heap);
					return -1;
				}
			}
			else{
				//no se puede agrandar, hay que crear un segmento nuevo
				uint32_t cantidad_de_paginas = paginas_necesarias_para_tamanio(datos->tamanio+sizeof(heap_metadata)*2);
				int espacio_libre_ultima_pag = cantidad_de_paginas*configuracion->tam_pag-datos->tamanio-sizeof(heap_metadata)*2;
				if(lugar_disponible>=cantidad_de_paginas*configuracion->tam_pag){
					lugar_disponible-=cantidad_de_paginas*configuracion->tam_pag;
					segmento* segmento_nuevo = malloc(sizeof(segmento));
					segmento_nuevo->compartido = false;
					segmento_nuevo->mmapeado = false;
					segmento_nuevo->tamanio = cantidad_de_paginas*configuracion->tam_pag;

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
						pag->bit_marco = asignar_marco_nuevo();
						if(i == 0){//si es la 1ra => hay que agregar el heap al inicio
							heap_metadata* heap_nuevo = malloc(sizeof(heap_metadata));
							heap_nuevo->is_free = false;
							heap_nuevo->size = datos->tamanio;
							void* puntero_a_marco = obtener_puntero_a_marco(pag->bit_marco);
							memcpy(puntero_a_marco,heap_nuevo,sizeof(heap_metadata));
							free(heap_nuevo);
						}
						if(i == cantidad_de_paginas-2){
							if(hay_que_entrar_en_la_anteultima_pag){
								void* puntero_a_marco = obtener_puntero_a_marco(pag->bit_marco);
								memcpy(puntero_a_marco+offset_heap,heap_al_final,heap_en_ante_ultima_pagina);
							}
						}
						if(i == cantidad_de_paginas-1){//se agrega el heap siguiente al final
							void* puntero_a_marco = obtener_puntero_a_marco(pag->bit_marco);
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
					segmento_nuevo->base_logica = base_logica_segmento_nuevo(ultimo_segmento);
					list_add(tabla_de_segmentos,segmento_nuevo);
					direccion_return = segmento_nuevo->base_logica+sizeof(heap_metadata);//es el principio del segmento nuevo
				}
				else{
					//no hay lugar en el sistema
					//free el segmento_nuevo
					return -1;
				}
			}
		}
	}

	//retorno la direccion de memoria (virtual) que le asigne
	return direccion_return;
	}
else{//no hay lugar
	return -1;
}
}

void* list_last_element(t_list* lista){
	return (list_get(lista,lista->elements_count-1));
}

uint32_t base_logica_segmento_nuevo(segmento* segmento_anterior){
	return segmento_anterior->base_logica+segmento_anterior->tamanio;
}

pagina* ultima_pagina_por_numero(segmento* seg)
{	pagina* pag=NULL;
	if(list_is_empty(seg->paginas)){
		return pag; //vacia?
		}
	pag->num_pagina=0;
	for(int i=0;i<list_size(seg->paginas)-1;i++) {
	pagina* p = list_get(seg->paginas,i);
		if(p->num_pagina > pag->num_pagina)
		{
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
		return list_any_satisfy(segmento->info_heaps,(void*)tiene_lugar_disponible);
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
//_Bool encontrar_ultima_pagina(pagina* pag){
	//return pag->ultimo_heap_metadata_libre!=-1;
	//si es != -1 entonces es la ultima pagina del segmento
//}
void* obtener_puntero_a_marco(t_bit_memoria* bit_marco){
	return upcm + bit_marco->bit_position * configuracion->tam_pag;
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
	//(uso,modificado) hay que pensar como hacer para mover el "puntero" del algoritmo
	//primero hay que buscar si hay alguna pagina de memoria en (0,0) -> en el bitarray_memoria
	//si no se encuentra hay que buscar el (0,1) y pasando los (1,x) a (0,x)
	//si no se encuentra repetir los pasos devuelta

	t_bit_memoria* bit_return = buscar_0_0();
	if(bit_return==NULL){
		bit_return = buscar_0_1();
		if(bit_return == NULL){
			bit_return = ejecutar_clock_modificado();
		}
	}
	bit_return->bit_modificado = false;//lo dejo en (0,0) listo para usar
	//falta pasar la pagina a sacar a area de swap!!
	return bit_return;
}
t_bit_memoria* buscar_0_0(){
	int puntero_al_iniciar = posicion_puntero_clock;
	t_bit_memoria* bit_nulo = NULL;
	//busco en la primera mitad
	for(int i = 0; i < bitarray->size_memoria-puntero_al_iniciar; i++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(!_bit->bit_modificado && !_bit->bit_uso){
			return _bit;
		}
		posicion_puntero_clock ++;
	}
	for(posicion_puntero_clock = 0; posicion_puntero_clock < puntero_al_iniciar;posicion_puntero_clock++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(!_bit->bit_modificado && !_bit->bit_uso){
			return _bit;
		}
	}
	posicion_puntero_clock = puntero_al_iniciar;
	return bit_nulo;
}
t_bit_memoria* buscar_0_1(){
	int puntero_al_iniciar = posicion_puntero_clock;
	t_bit_memoria* bit_nulo = NULL;
	//busco en la primera mitad
	for(int i = 0; i < bitarray->size_memoria-puntero_al_iniciar; i++){
		t_bit_memoria* _bit = list_get(bitarray->bitarray_memoria,posicion_puntero_clock);
		if(_bit->bit_modificado && !_bit->bit_uso){
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
			return _bit;
		}
		if(_bit->bit_uso){
			_bit->bit_uso = false;
		}
	}
	posicion_puntero_clock = puntero_al_iniciar;
	return bit_nulo;
}
int no_obtener_direccion_virtual(uint32_t num_segmento,uint32_t num_pagina, uint32_t offset){
	//testea2
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
//	retorna 0 si falla

	return 0;
}
void no_abrir_direccion_virtual(int direccion,uint32_t* destino_segmento,uint32_t* destino_pagina, uint32_t* destino_offset){
	//testea2
	char binario[DIR_TAM_DIRECCION+1];
	dec_a_bin(binario,direccion,DIR_TAM_DIRECCION+1);
	char char_segmento[DIR_TAM_PAGINA+1];
	char char_pagina[DIR_TAM_PAGINA+1];
	char char_offset[DIR_TAM_DESPLAZAMIENTO+1];
	char_segmento[DIR_TAM_PAGINA]='\0';
	char_pagina[DIR_TAM_PAGINA]='\0';
	char_offset[DIR_TAM_DESPLAZAMIENTO]='\0';
	int j = 1;
	for(int i = 0;i<DIR_TAM_PAGINA;i++,j++){
		char_segmento[i]=binario[j];
	}
	for(int i = 0;i<DIR_TAM_PAGINA;i++,j++){
		char_pagina[i]=binario[j];
	}
	for(int i = 0;i<DIR_TAM_DESPLAZAMIENTO;i++,j++){
		char_offset[i]=binario[j];
	}
	*destino_offset = bin_a_dec(char_offset);
	*destino_pagina = bin_a_dec(char_pagina);
	*destino_segmento = bin_a_dec(char_segmento);
}
void* muse_get(muse_get_t* datos){
	void* resultado_get;
	t_list* tabla_de_segmentos = traer_tabla_de_segmentos(datos->id);
	segmento* segmento_buscado = traer_segmento_de_direccion(tabla_de_segmentos,datos->direccion);
	if(segmento_buscado!=NULL){
		//encontro un segmento, hay que buscar la direccion ahi adentro
		//if(cayo en un heap) => se pudre to2?
		//seguro hay que ir rocorriendo to2 el segmento y analizar en que heap cae la consulta
	}
	else{
		resultado_get = NULL;
	}
	return resultado_get;
}
segmento* traer_segmento_de_direccion(t_list* tabla_de_segmentos,uint32_t direccion){
	//codear xd!!
	return NULL;
}
pagina* buscar_pagina_por_numero(t_list* lista, int numero_de_pag) {
	_Bool numero_de_pagina(pagina* pag){
		return pag->num_pagina == numero_de_pag;
	}
	pagina* pagina_buscada = list_find(lista,(void*)numero_de_pagina);
	return pagina_buscada;
}

void* traer_datos_de_memoria(segmento* segmento_buscado,uint32_t dir_pagina,uint32_t dir_offset){
//hay que buscar la pagina, buscar el heap_metadata en el que esta la direccion buscada, ver el tamanio y si no se pasa=>traerla
	_Bool numero_de_pagina(pagina* pag){
		return pag->num_pagina == dir_pagina;
	}
	pagina* pagina_buscada = list_find(segmento_buscado->paginas,(void*)numero_de_pagina);
	if(!pagina_buscada->presencia){//=> la traigo a memoria
		ejecutar_clock_modificado();//hay q codearlo
	}
	//aca ya tendria en memoria la pagina. podria necesitar mas... hay q pensarlo

	return NULL;
}
int muse_cpy(muse_cpy_t* datos){
//	devuelve 0 si falla
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
int muse_close(char* id_cliente){
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
	printf("Estoy escuchando en el puerto %d\n",puerto);
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
	uint32_t tam, operacion_respuesta, resultado, operacion;
	char* id_cliente;
	void* respuesta;
	while(recv(socket,&operacion,4,MSG_WAITALL) >0 && exit_loop==false){
		printf("Nuevo pedido de %d\n",socket);
		switch (operacion) {
			case MUSE_INIT:;
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
				list_add(tabla_de_programas,programa);
				free(pid_char);
				break;
			case MUSE_ALLOC:;
				recv(socket,&tam,4,0);
				void* vmat = malloc(tam);
				recv(socket,vmat,tam,0);
				muse_alloc_t* datos = deserializar_muse_alloc(vmat);
				resultado = muse_alloc(datos);
				if(resultado>=0){
					respuesta = malloc(8);
					operacion_respuesta = MUSE_INT;
					memcpy(respuesta,&operacion_respuesta,4);
					memcpy(respuesta+4,&resultado,4);
					send(socket,respuesta,8,0);
					printf("mando direccion virtual a %d: %d\n",socket,resultado);
				}
				else{
					respuesta = malloc(4);
					operacion_respuesta = MUSE_ERROR;
					memcpy(respuesta,&operacion_respuesta,4);
					send(socket,respuesta,4,0);
					printf("Error en muse_alloc\n");
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
				resultado = muse_free(dmft);
				if(resultado == 0){
					operacion_respuesta = MUSE_ERROR;
				}
				else{
					operacion_respuesta = MUSE_EXITOSO;
				}
				memcpy(respuesta,&operacion_respuesta,4);
				send(socket,&operacion_respuesta,4,0);
				printf("haciendo free de %d, resultado: %d\n",socket,resultado);
				muse_free_destroy(dmft);
				free(vmft);
				break;
			case MUSE_GET:;
				recv(socket,&tam,4,0);
				void* vmgt = malloc(tam);
				recv(socket,vmgt,tam,0);
				muse_get_t* dmgt = deserializar_muse_get(vmgt);
				void* resultado_get = muse_get(dmgt);
				//devuelve el void* resultado
				if(resultado_get !=NULL){
					muse_void* mv = crear_muse_void(resultado_get,dmgt->tamanio);
					respuesta = serializar_muse_void(mv);
					uint32_t tamanio_respuesta;
					memcpy(&tamanio_respuesta,respuesta+4,4);
					send(socket,respuesta,tamanio_respuesta,0);
					printf("enviando resolucion del get a: %d\n",socket);
					free(resultado_get);
					free(respuesta);
					muse_void_destroy(mv);
				}
				else{
//					hay que pensar si todos los errores son segm fault!!??
					operacion_respuesta = MUSE_ERROR;
					send(socket,&operacion_respuesta,4,0);
				}
				muse_get_destroy(dmgt);
				free(vmgt);
				break;
			case MUSE_CPY:;
				recv(socket,&tam,4,0);
				void* vmct = malloc(tam);
				recv(socket,vmct,tam,0);
				muse_cpy_t* mct = deserializar_muse_cpy(vmct);
				resultado = muse_cpy(mct);
				//recive 0 si fallo
				if(resultado == 0){
					operacion_respuesta = MUSE_ERROR;
					send(socket,&respuesta,4,0);
					free(vmct);
					muse_cpy_destroy(mct);
				}
				else{
					//funciono
					operacion_respuesta = MUSE_EXITOSO;
					send(socket,&respuesta,4,0);
					free(vmct);
					muse_cpy_destroy(mct);
					muse_cpy_destroy(mct);
					free(vmct);
				}
				printf("enviando resolucion del cpy a: %d, resultado: %d\n",socket,resultado);
				break;
			case MUSE_MAP://hay que hacerla bien
				recv(socket,&tam,4,0);
				void* vmmt = malloc(tam);
				recv(socket,vmmt,tam,0);
				muse_map_t* dmmt = deserializar_muse_map(vmmt);
				resultado = muse_map(dmmt);
				//devuelve si esta to do ok o no
				send(socket,&resultado,4,0);
				printf("enviando resolucion del map %d a: %d\n",socket,resultado);
				muse_map_destroy(dmmt);
				free(vmmt);
				break;
			case MUSE_SYNC://hay que hacerla bien
				recv(socket,&tam,4,0);
				void* vmst = malloc(tam);
				recv(socket,vmst,tam,0);
				muse_sync_t* dmst = deserializar_muse_sync(vmst);
				resultado = muse_sync(dmst);
				//devuelve si esta to do ok o no
				send(socket,&resultado,4,0);
				printf("enviando resolucion del sync %d a: %d\n",socket,resultado);
				muse_sync_destroy(dmst);
				free(vmst);
				break;
			case MUSE_UNMAP://hay que hacerla bien
				recv(socket,&tam,4,0);
				void* vmut = malloc(tam);
				recv(socket,vmut,tam,0);
				muse_unmap_t* dmut = deserializar_muse_unmap(vmut);
				resultado = muse_unmap(dmut);
				//devuelve si esta to do ok o no
				send(socket,&resultado,4,0);
				printf("enviando resolucion del unmap %d a: %d\n",socket,resultado);
				muse_unmap_destroy(dmut);
				free(vmut);
				break;
			case MUSE_CLOSE:;
//				si no se libera algun muse_alloc-> es un memory leak
			//liberar tabla de programas
				resultado = muse_close(id_cliente);
				printf("Se fue %d\n",socket);
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
		bit->bit_position = i;
		bit->bit_modificado = false;
		bit->bit_uso = 0;
		list_add(bitarray->bitarray_memoria,bit);
	}
	bitarray->size_memoria_virtual = CANT_PAGINAS_MEMORIA_VIRTUAL;
	bitarray->bitarray_memoria_virtual = list_create();
	for(uint32_t i = 0; i<CANT_PAGINAS_MEMORIA_VIRTUAL; i++){
		t_bit_swap* bit = malloc(sizeof(t_bit_swap));
		bit->bit_position = i;
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
	_Bool bit_libre(t_bit_swap* bit){
		return !bit->ocupado;
	}
	return list_find(bitarray->bitarray_memoria_virtual,(void*)bit_libre);
}
t_bit_memoria* bit_libre_memoria(){
	//si encuentro uno, ya le pongo como que esta usado
	_Bool bit_libre(t_bit_memoria* bit){
		return !bit->ocupado;
	}
	t_bit_memoria* bit_asignado = list_find(bitarray->bitarray_memoria,(void*)bit_libre);
	if(bit_asignado!=NULL){
		bit_asignado->ocupado = true;
	}
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
