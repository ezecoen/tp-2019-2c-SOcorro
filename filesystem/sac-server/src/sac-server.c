// servidor para probar sockets mañana
#include "sac-server.h"

int main(int argc,char* argv[]) {
	diccionario_de_path = dictionary_create();
	es_virgen = 0;
	if(argc != 2){
		perror("falta el archivo del fileSystem");
		return -1;
	}
	logger = log_create("loger","sac-server",1,LOG_LEVEL_TRACE);
	if(!el_fs_esta_formateado(argv[1])){
		init_fs(argv[1]);
		log_info(logger,"El fileSystem fue cargado");
	}
	load_fs(argv[1]);
	log_info(logger,"El fileSystem fue cargado");
	int _servidor = crear_servidor(8080);
//	while(1){
//		esperar_conexion(_servidor);
		int cliente = aceptar_cliente(servidor);
		atender_cliente(cliente);
//	}


	return 0;
}


int _mknod(char* nombre){//no hace falta actualizar el bitarray porque los bits de la tabla de nodo ya estan en 1
	nodo* nodo = dame_el_primer_nodo_libre(nombre);
	if(nodo == -1){//no hay mas nodos
		return -1;
	}else if(nodo == -2){
		return -2;
	}
	nodo->estado=1;
	nodo->fecha_de_creacion = timestamp();
	nodo->fecha_de_modificacion = timestamp();
	char** list = string_split(nombre,"/");
	char* path_padre = dame_path_padre(nombre);
	int padre = dictionary_get(diccionario_de_path,path_padre);
	char* nom_mio = dame_el_nombre(list,1);
	strcpy(nodo->nombre_de_archivo,nom_mio);
	if(padre!=0){
		nodo->bloque_padre = padre+1+tam_de_bitmap;
	}else{
		nodo->bloque_padre = padre;
	}
	return 1;
}
int _mkdir(char* nombre){//no hace falta actualizar el bitarray porque los bits de la tabla de nodo ya estan en 1
	nodo* nodo = dame_el_primer_nodo_libre(nombre);
	if(nodo == -1){//no hay mas nodos
		return -1;
	}else if(nodo == -2){ //el nodo ya existe
		return -2;
	}
	nodo->estado=2;
	nodo->fecha_de_creacion = timestamp();
	nodo->fecha_de_modificacion = timestamp();
	char** list = string_split(nombre,"/");
	char* path_padre = dame_path_padre(nombre);
	int padre = dictionary_get(diccionario_de_path,path_padre);//BLOQUE PADRE
	char* nom_mio = dame_el_nombre(list,1);
	strcpy(nodo->nombre_de_archivo,nom_mio);
	if(padre!=0){
		nodo->bloque_padre = padre+1+tam_de_bitmap;
	}else{
		nodo->bloque_padre = padre;
	}
	return 1;
}
char* dame_path_padre(char* nombre){
	char* path_padre;
	char* hijo_a_sacar = string_new();
	char** split = string_split(nombre,"/");
	int i;
	for(i=0;split[i]!=NULL;i++){

	}
	string_append(&hijo_a_sacar,"/");
	string_append(&hijo_a_sacar,split[i-1]);
	int len = strlen(nombre)+1-(strlen(hijo_a_sacar)+1);
	path_padre = string_substring_until(nombre,len);
	return path_padre;
}
char* dame_el_nombre(char** nombres,int quien){
	int i;
	for(i = 0;nombres[i]!=NULL;i++){

	}
	if(quien == 1){// me piden mi nombre
		if(i==0){
			return "/";
		}
		return nombres[i-1];
	}else{// me piden mi padre
		if(i<2){
			return "/";
		}
		return nombres[i-2];
	}

}
t_getattr* _getattr(char* nombre){
	nodo* _nodo = dame_el_nodo_de(nombre);
	if(_nodo == -1){//no hay nodos
		return -1;
	}
	t_getattr* a = crear_getattr(_nodo->tamanio_de_archivo,_nodo->fecha_de_modificacion,_nodo->estado);
	return a;

}
bool el_fs_esta_formateado(char* fs){
	int disco_fd = open(fs,O_RDWR|O_CREAT,0777);
	int tam = fileSize(fs);
	bloque* bloq = mmap(NULL,tam,PROT_READ | PROT_WRITE,MAP_FILE | MAP_SHARED,disco_fd,0);
	header* __header = levantar_header(bloq);
	bool rta = ((__header->bloque_inicio_bitmap==2)&&(strcmp(__header->id,"SAC")==0)&&(__header->version==1));
	close(disco_fd);
	return rta;
}
void init_fs(char* fs){
	es_virgen=1;
	int disco_fd = open(fs,O_RDWR|O_CREAT,0777);
	tam_del_fs = fileSize(fs);
	tam_de_bitmap = tam_del_fs/4096/8/4096;
	primer_bloque_de_disco = mmap(NULL,tam_del_fs,PROT_READ | PROT_WRITE,MAP_FILE | MAP_SHARED,disco_fd,0);
	bitarray = init_set_bitmap();
	escribir_header(primer_bloque_de_disco);
	log_info(logger,"Escribiendo el header");
	escribir_bitmap_inicio(primer_bloque_de_disco+1,bitarray);
	log_info(logger,"Escribiendo el bitmap");
	escribir_tabla_de_nodos(primer_bloque_de_disco+1+tam_de_bitmap);
	log_info(logger,"Escribiendo la tabla de nodos");
}
void load_fs(char* fs){
	tam_del_fs = fileSize(fs);
	tam_de_bitmap = tam_del_fs/4096/8/4096;
	if(tam_de_bitmap==0){
		tam_de_bitmap++;
	}
	int disco_fd = open(fs,O_RDWR|O_CREAT,0777);
	if(!es_virgen){
		primer_bloque_de_disco = mmap(NULL,tam_del_fs,PROT_READ | PROT_WRITE,MAP_FILE | MAP_SHARED,disco_fd,0);
	}
	_header = levantar_header(primer_bloque_de_disco);
	log_info(logger,"Cargando el header");
	bitarray = levantar_bit_array(primer_bloque_de_disco+1);
	log_info(logger,"Cargando el bitmap");
	levantar_tabla_de_nodo(primer_bloque_de_disco+1+tam_de_bitmap);
	log_info(logger,"Cargando la tabla de nodos");
	levantar_diccionario();

}
void levantar_diccionario(){
	for(int i = 0;i<1024;i++){
		for(int j = 0;j<1024;j++){
			if(tabla_de_nodos[j]->estado == 0){
				continue;
			}
			if(tabla_de_nodos[j]->bloque_padre==i){
				path_intermedio = string_new();
				path_generator = string_new();
				dame_mi_path_entero(j);
				dictionary_put(diccionario_de_path,path_generator,j);
				free(path_generator);
				free(path_intermedio);
			}
		}
	}
}
void levantar_tabla_de_nodo(bloque* bloque){
	tabla_de_nodos = (nodo**)malloc(sizeof(nodo*)*1024);
	for(int i = 0;i<1024;i++){
		tabla_de_nodos[i] = (nodo*) bloque;
		bloque++;
	}
}
void acomodamelo_papi(char* path_al_reves){
	char** a = string_split(path_al_reves,"/");
	int i;
	for(i=0;a[i]!=NULL;i++);
	string_append(&path_generator,"/");
	for(int j=i;j!=0;j--){
		string_append(&path_generator,a[j-1]);
		if(j-1 != 0){
			string_append(&path_generator,"/");
		}
	}

}
void dame_mi_path_entero(int numero_de_nodo){
	if(numero_de_nodo == 0){
		string_append(&path_intermedio,"/");
		//  ARE/pepo/hola//
		acomodamelo_papi(path_intermedio);
		return;
	}else{
		string_append(&path_intermedio,tabla_de_nodos[numero_de_nodo]->nombre_de_archivo);
		string_append(&path_intermedio,"/");
		int padre = tabla_de_nodos[numero_de_nodo]->bloque_padre;
		if(padre -1-tam_de_bitmap < 0){
			padre = 0;
		}else{
			padre = padre - 1 - tam_de_bitmap;
		}
		dame_mi_path_entero(padre);
	}
}
t_bitarray* levantar_bit_array(bloque* bloque){
	char* a = string_repeat('0',tam_de_bitmap*4096);
	int sub_array = 0;
	for(int i=0;i<tam_de_bitmap;i++){
		for(int j=0;j<4096;j++){
			a[sub_array] = (unsigned char)bloque->bytes[j];
			sub_array++;
		}
	}
	t_bitarray* bitarray = bitarray_create_with_mode(a,tam_de_bitmap*4096,MSB_FIRST);
	return bitarray;
}
t_bitarray* init_set_bitmap(){
	if(tam_de_bitmap==0){
		tam_de_bitmap++;
	}
	char* a = string_repeat(0,tam_de_bitmap*4096);
	t_bitarray* bitarray = bitarray_create_with_mode(a,tam_de_bitmap*4096,MSB_FIRST);
	for(int i=0;i<tam_de_bitmap+1+1024;i++){//sumo 1 por el header y 1024 por la tabla de nodos
		bitarray_set_bit(bitarray,i);
	}
	for(int j = tam_de_bitmap+1+1024;j<bitarray->size;j++){
		bitarray_clean_bit(bitarray,j);
	}
	return bitarray;
}
void escribir_tabla_de_nodos(bloque* _bloque){//disco+1+tam_de_bitmap){
	for(int i = 0;i<1024;i++){
		if(i == 0){
			nodo* nodo_vacio = (nodo*) _bloque;
			nodo_vacio->bloque_padre=0;
			nodo_vacio->estado=2;
			nodo_vacio->fecha_de_creacion=timestamp();
			nodo_vacio->fecha_de_modificacion=timestamp();
			strcpy(nodo_vacio->nombre_de_archivo,"/");
			nodo_vacio->punteros_indirectos[i].punteros = 0; //descomentar cuando este testeado
			nodo_vacio->tamanio_de_archivo=0;
		}else{
			nodo* nodo_vacio = (nodo*) _bloque;
			nodo_vacio->bloque_padre=-1;
			nodo_vacio->estado=0;
			nodo_vacio->fecha_de_creacion=0;
			nodo_vacio->fecha_de_modificacion=0;
			for(int i = 0;i<71;i++){
				nodo_vacio->nombre_de_archivo[i] = '\0';
			}
			nodo_vacio->punteros_indirectos[i].punteros = 0; //descomentar cuando este testeado
			nodo_vacio->tamanio_de_archivo=0;
			}
		_bloque++;
	}
}
header* levantar_header(bloque* _bloque){
	header* _header = (header*) _bloque;
	return _header;
}
void escribir_bitmap_inicio(bloque* disco,t_bitarray* bitarray){
	int _bits = bitarray->size;
	int bloques = _bits/4096;
	int sub_array = 0;
	for(int i=0;i<bloques;i++){
		for(int j=0;j<4096;j++){
			disco->bytes[j]=bitarray->bitarray[sub_array];
			sub_array++;
		}
		disco++;
	}
}
int fileSize(char* filename){
	FILE* _f = fopen(filename,"r");
	fseek(_f, 0L, SEEK_END);
	int i = ftell(_f);
	fclose(_f);
	return i;
}
void escribir_header(bloque* disco){
	header* _header = (header*) disco;
	memcpy(_header->id,"SAC",3);
	_header->version = (uint32_t)1;
	_header->bloque_inicio_bitmap = (uint32_t)2;
	_header->tamanio_bitmap = (uint32_t)tam_de_bitmap;
	init_relleno(_header->relleno);
}
void init_relleno(char* _relleno){
	for(int i = 0;i<4081;i++){
		_relleno[i] = '-';
	}
}
void printear(bloque* bloq){
	for(int i=0;i<4095;i++){
		printf("%c",bloq->bytes[i]);
	}
}
nodo* dame_el_primer_nodo_libre(char* nombre){
	if(dictionary_has_key(diccionario_de_path,nombre)){
		return -2;
	}
	for(int i = 0;i<1024;i++){
		if(tabla_de_nodos[i]->estado == 0){
			// el indice de la tabla de nodos es el numero de nodo
			nodo* bloque_del_nodo = (nodo*) primer_bloque_de_disco+1+tam_de_bitmap+i;
			dictionary_put(diccionario_de_path,nombre,(void*)i);
			return bloque_del_nodo;
		}
	}
	//no hay nodos libres
	return -1;

}
nodo* dame_el_nodo_de(const char* _nombre){
	int numero_nodo;
	if(dictionary_has_key(diccionario_de_path,_nombre)){
		numero_nodo = (int)dictionary_get(diccionario_de_path,_nombre);
		return (nodo*)primer_bloque_de_disco+1+tam_de_bitmap+numero_nodo;
	}
	return -1;
}
bloque* bloque_de_nodo(int nodo){
	bloque* ret = primer_bloque_de_disco+1+tam_de_bitmap+nodo;
	return ret;
}
operacion* recibir_instruccion(int cliente){
	int op;
	int tamanio;
	recv(cliente,&op,4,MSG_WAITALL);
	recv(cliente,&tamanio,4,MSG_WAITALL);
	void* argumentos = malloc(tamanio);
	recv(cliente,argumentos,tamanio,MSG_WAITALL);
	operacion* _operacion = malloc(sizeof(operacion));
	_operacion->op = op;
	_operacion->argumentos = malloc(tamanio);
	memcpy(_operacion->argumentos,argumentos,tamanio);
	free(argumentos);
	return _operacion;
}

void esperar_conexion(int servidor){
	int cliente = aceptar_cliente(servidor);
	log_info(logger,"Se conecto un cliente con el socket numero %d",cliente);
//	falta la implementacion de la funcion que va hacer el hilo al conectarse sac-cli
	pthread_t hilo_nuevo_cliente;
	if(pthread_create(&hilo_nuevo_cliente,NULL,(void*)atender_cliente,(void*)cliente)!=0){
		log_error(logger,"Error al crear el hilo de journal");
	}
	pthread_detach(hilo_nuevo_cliente);
	close(cliente);
}
void* armar_error(int error_code){
	void* err = malloc(sizeof(int)*2);
	operaciones op = ERROR;
	memcpy(err,&op,4);
	memcpy(err+4,&error_code,4);
	return err;
}
void atender_cliente(int cliente){
	//Esperar con recv los pedidos de instrucciones que llegan del sac-cli
	uint32_t _tam;
	int res;
	void* magic;
	char* path_pedido;
	int cli;
	recv(cliente,&cli,4,MSG_WAITALL);
	log_info(logger,"%d",cli);
	if(cli != INIT_CLI){
		perror("Se conecto un rancio");
		close(cliente);
		return;
	}
	log_info(logger,"Se conecto un sac-clie");
	operaciones operacion;
	while(recv(cliente,&operacion,sizeof(int),MSG_WAITALL)>0){
		switch(operacion){
		case GETATTR:
			recv(cliente,&_tam,sizeof(int),MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam,MSG_WAITALL);
			path_pedido = string_new();
			string_append(&path_pedido,magic);
			log_info(logger,"Llego la instruccion GETATTR %s",path_pedido);
			free(magic);
			t_getattr* attr = _getattr(path_pedido);
			free(path_pedido);
			if(attr == -1){
				int err = ERROR;
				send(cliente,&err,4,0);
			}else{
				magic = serializar_getattr(attr);
				int tam;
				memcpy(&tam,magic+4,sizeof(int));
				send(cliente,magic,tam,0);
			}
			break;
		case READDIR:
//			recibo el path
			recv(cliente,&_tam,sizeof(int),MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam,MSG_WAITALL);
			path_pedido = string_new();
			string_append(&path_pedido,magic);
			log_info(logger,"Llego la instruccion READDIR de %s",path_pedido);
			free(magic);
//			busco las entradas y las pongo en una lista
			t_list* entradas = list_create(); //hay que liberar esta lista?
			res = encontrame_las_entradas_de(entradas,path_pedido);
			if(res == -1){
				int err = ERROR;
				send(cliente,&err,4,0);
			}else{
				void* magic = serializar_lista_ent_dir(entradas);
				int tam;
				memcpy(&tam,magic+8,sizeof(int));
				send(cliente,magic,tam,0);
				free(magic);
			}
			break;
		case OPEN:
			recv(cliente, &_tam,4,MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam-8,MSG_WAITALL);
			t_open* pedido = deserializar_open(magic);
			log_info(logger,"Llego la instruccion OPEN de %s",pedido->path);
			res = _open(pedido);
			if(res == 1){
				int a = OPEN;
				send(cliente,&a,4,0);
			}
			else{
				void* error = armar_error(res);
				send(cliente,error,8,0);
				free(error);
			}
			open_destroy(pedido);
			free(magic);
			break;
		case READ:
			log_info(logger,"Llego la instruccion READ");
			break;
		case MKNOD:
			recv(cliente,&_tam,sizeof(int),MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam,MSG_WAITALL);
			path_pedido = string_new();
			string_append(&path_pedido,magic);
			log_info(logger,"Llego la instruccion MKNOD de %s",path_pedido);
			free(magic);
			res = _mknod(path_pedido);
			free(path_pedido);
			if(res == -1){ // no hay mas nodos
//				mandar error
				int err = ERROR;
				send(cliente,&err,4,0);
			}else if(res == -2){//el nodo con ese path ya existe
				int err = ERROR;
				send(cliente,&err,4,0);
			}else{
				int a = MKNOD;
				send(cliente,&a,4,0);

			}
			break;
//			aca habria que delvolver lo que paso (devolver ENOSPC si hubo error de espacio, devolver 0 si
//			estuvo ok) <-- eso habria que hacerlo adentro de la funcion _mknod, no aca, ademas hay que ver
//			todos los errores porque hay como mil y habria que contemplar la mayor cantidad posible (los
//			que esten a nuestro alcance por lo menos)

//			mandar socket respuesta con el valor de la respuesta
		case UNLINK:
			recv(cliente,&_tam,sizeof(int),MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam,MSG_WAITALL);
			path_pedido = string_new();
			string_append(&path_pedido,magic);
			free(magic);
			log_info(logger,"Llego la instruccion UNLINKde %s",path_pedido);
			res = _unlink(path_pedido);
			if(res != 0){
				int err = ERROR;
				send(cliente,&err,4,0);
			}else{
				int a = UNLINK;
				send(cliente,&a,4,0);
			}
			break;
		case MKDIR:
			recv(cliente,&_tam,sizeof(int),MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam,MSG_WAITALL);
			path_pedido = string_new();
			string_append(&path_pedido,magic);
			log_info(logger,"Llego la instruccion MKDIR de %s",path_pedido);
			free(magic);
			int resultado = _mkdir(path_pedido);
			free(path_pedido);
			if(resultado == -1){
				int err = ERROR;
				send(cliente,&err,4,0);
			}else{
				int a = MKDIR;
				send(cliente,&a,4,0);
			}
//			aca habria que delvolver lo que paso (devolver ENOSPC si hubo error de espacio, devolver 0 si
//			estuvo ok) <-- eso habria que hacerlo adentro de la funcion _mknod, no aca, ademas hay que ver
//			todos los errores porque hay como mil y habria que contemplar la mayor cantidad posible (los
//			que esten a nuestro alcance por lo menos)
			break;
		case CHMOD:
			log_info(logger,"Llego la instruccion CHMOD");
			break;
		case WRITE:
			log_info(logger,"Llego la instruccion WRITE");
			recv(cliente,&_tam,sizeof(int),MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam-8,MSG_WAITALL);
			t_write* wwrite = deserializar_write(magic);
//			len = strlen(DEFAULT_FILE_CONTENT);
//			if (offset < len) {
//				if (offset + size > len)
//					size = len - offset;
//			memcpy(buf, DEFAULT_FILE_CONTENT + offset, size);
			res = _write(wwrite);
			break;
		case UTIMES:
			recv(cliente,&_tam,4,MSG_WAITALL);
			magic = malloc(_tam);
			recv(cliente,magic,_tam-8,MSG_WAITALL);
			t_utime* modif = deserializar_utime(magic);
			nodo* _nodo = dame_el_nodo_de(modif->path);
			_nodo->fecha_de_modificacion = (uint64_t) modif->utime;
			resultado = UTIMES;
			send(cliente,&resultado,4,0);
			break;
		default:
			log_error(logger, "Llego una instruccion no habilitada");
			break;
		}
	}
}
int _write(t_write* wwrite){
	nodo* _nodo = dame_el_nodo_de(wwrite->path);
	int base = pow(2,22);
	int indice_de_PIS = wwrite->offset/base;
	if(_nodo->punteros_indirectos[indice_de_PIS].punteros == 0){// no tiene ningun bloque asignado
		uint32_t bloque_a_usar = dame_un_bloque_libre();
		if(bloque_a_usar == -1){
			return -1;
		}

		_nodo->punteros_indirectos[indice_de_PIS].punteros = bloque_a_usar;
	}else{ //ya tiene el bloque inicializado
	return 0;
	}
	return 0;
}
uint32_t dame_un_bloque_libre(){
	for(int i = 0;i<tam_de_bitmap*4096;i++){
		if(bitarray_test_bit(bitarray,i) == 0){

			return i;
		}
	}
	return -1;
}
int _open(t_open* pedido){
	int res_mknod;
	if (!dictionary_has_key(diccionario_de_path,pedido->path)){
		if (pedido->crear || (pedido->crear && pedido->crear_ensure)){
			res_mknod = _mknod(pedido->path);
			if (res_mknod == -1)
				return EDQUOT;
			return 1;
		}
		else
			return ENOENT;
	}else{
		if(pedido->crear && pedido->crear_ensure)
			return EEXIST;
		return 1;
	}
}

int _unlink (char* path){
	int indice_de_nodo;
	if (!dictionary_has_key(diccionario_de_path,path)){
		return ENOENT;
	}
	else{
		indice_de_nodo = dictionary_get(diccionario_de_path,path);
		nodo* nodox = (nodo*)bloque_de_nodo(indice_de_nodo);
		limpiar_nodo(nodox);
		return 0;
	}
}

void limpiar_nodo(nodo* nodox){
	int i = 0, j;
	t_punteros_a_bloques_de_datos* BPD; //Bloque de Punteros a Datos
	while(i<1000 && nodox->punteros_indirectos[i].punteros != 0){
		j = 0;
		BPD = (t_punteros_a_bloques_de_datos*)(primer_bloque_de_disco+nodox->punteros_indirectos[i].punteros);
		while(j<1024 && BPD->punteros_a_bloques_de_datos[j] != 0){
			bitarray_clean_bit(bitarray,BPD->punteros_a_bloques_de_datos[j]);
			BPD->punteros_a_bloques_de_datos[j] = 0;
			j++;
		}
		bitarray_clean_bit(bitarray,nodox->punteros_indirectos[i].punteros);
		nodox->punteros_indirectos[i].punteros = 0;
		i++;
	}
}


//int escribime_en(t_write* wwrite, nodo* nodo){
//	if()//todo
//}
//bloque* dame_el_bloque_para_escribir_de(nodo* mi_nodo){
//	if(no_tengo_bloques_asignados(mi_nodo) || el_ultimo){
//		asignar_bloque(mi_nodo);
//		return dame_el_bloque_para_escribir_de(mi_nodo);
//	}else{
//		return (bloque*)primer_bloque_de_disco+1+tam_de_bitmap+bloque_para_escribir(mi_nodo->punteros_indirectos);
//	}
//}

int encontrame_las_entradas_de(t_list* entradas,char* path_pedido){
	if(!dictionary_has_key(diccionario_de_path,path_pedido)){
		return -1;
	}
	int nodo = dictionary_get(diccionario_de_path,path_pedido);
	int bloque_padre;
	if(nodo == 0){
		bloque_padre=0;
	}else{
		bloque_padre = nodo+1+tam_de_bitmap;
	}
	for(int i = 0;i<1024;i++){
		if(string_contains(tabla_de_nodos[i]->nombre_de_archivo,"/")){
			continue;
		}
		if(tabla_de_nodos[i]->bloque_padre == bloque_padre){
			list_add(entradas,tabla_de_nodos[i]->nombre_de_archivo);
		}
	}
	return 0;
}
