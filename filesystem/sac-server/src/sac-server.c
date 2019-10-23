// servidor para probar sockets mañana
#include "sac-server.h"
#include "libreriaComun/libreriaComun.h"

int main(int argc,char* argv[]) {
	if(argc != 2){
		perror("falta el archivo del fileSystem");
		return -1;
	}
	logger = log_create("loger","sac-server",1,LOG_LEVEL_TRACE);
	if(el_fs_esta_formateado(argv[1])){
		load_fs(argv[1]);
		log_info(logger,"El fileSystem esta cargado");
	}else{
		init_fs(argv[1]);
		log_info(logger,"El fileSystem fue creado");
	}
	uint32_t a1 = tabla_de_nodos[0]->punteros_indirectos[0].punteros[0];
	log_info(logger,"%d",a1);
	uint32_t a2 = tabla_de_nodos[0]->punteros_indirectos[0].punteros[1];
	log_info(logger,"%d",a2);
	uint32_t a3 = tabla_de_nodos[0]->punteros_indirectos[1].punteros[0];
	log_info(logger,"%d",a3);
	uint32_t a4 = tabla_de_nodos[0]->punteros_indirectos[1].punteros[1];
	log_info(logger,"%d",a4);
	uint32_t a5 = tabla_de_nodos[1]->punteros_indirectos[0].punteros[0];
	log_info(logger,"%d",a5);
	uint32_t a6 = tabla_de_nodos[1]->punteros_indirectos[0].punteros[1];
	log_info(logger,"%d",a6);





//	int _servidor = crear_servidor(8080);
//	while(1){
//		esperar_conexion(_servidor);
//	}

	return 0;
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
	int disco_fd = open(fs,O_RDWR|O_CREAT,0777);
	primer_bloque_de_disco = mmap(NULL,tam_del_fs,PROT_READ | PROT_WRITE,MAP_FILE | MAP_SHARED,disco_fd,0);
	_header = levantar_header(primer_bloque_de_disco);
	log_info(logger,"Cargando el header");
	bitarray = levantar_bit_array(primer_bloque_de_disco+1);
	log_info(logger,"Cargando el bitmap");
	levantar_tabla_de_nodo(primer_bloque_de_disco+1+tam_de_bitmap);
	log_info(logger,"Cargando la tabla de nodos");
}
void levantar_tabla_de_nodo(bloque* bloque){
	tabla_de_nodos = (nodo**)malloc(sizeof(nodo*)*1024);
	for(int i = 0;i<1024;i++){
		tabla_de_nodos[i] = (nodo*) bloque;
		bloque++;
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
	char* a = string_repeat(0,tam_de_bitmap*4096);
	t_bitarray* bitarray = bitarray_create_with_mode(a,tam_de_bitmap*4096,MSB_FIRST);
	for(int i=0;i<tam_de_bitmap+1+1024;i++){//sumo 1 por el header y 1024 por la tabla de nodos
		bitarray_set_bit(bitarray,i);
	}
	for(int j = tam_de_bitmap+1+1024;j<bitarray->size;j++){
		bitarray_test_bit(bitarray,j);
	}
	return bitarray;
}
void escribir_tabla_de_nodos(bloque* _bloque){//disco+1+tam_de_bitmap){
	for(int i = 0;i<1024;i++){
		nodo* nodo_vacio = (nodo*) _bloque;
		nodo_vacio->bloque_padre=0;
		nodo_vacio->estado=0;
		nodo_vacio->fecha_de_creacion=0;
		nodo_vacio->fecha_de_modificacion=0;
		for(int i = 0;i<71;i++){
			nodo_vacio->nombre_de_archivo[i] = '-';
		}
		for(int i=0;i<1000;i++){
			for(int j=0;i<1024;i++){
				if(j==0){
					nodo_vacio->punteros_indirectos[i].punteros[j]= 1;
				}else{
					nodo_vacio->punteros_indirectos[i].punteros[j]= 0;
				}
			}
		}
		nodo_vacio->tamanio_de_archivo=0;
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
		//printf("Bloque %d \n",i);
		for(int j=0;j<4096;j++){
			disco->bytes[j]=bitarray->bitarray[sub_array];
			//printf("%d",disco->bytes[j]);
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

uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long )tv.tv_sec) * 1000 + ((unsigned long) tv.tv_usec) / 1000);
	uint64_t a = result;
	return a;
}
//void ocupate_de_esta(int cliente){
//	int cli;
//	//whilear el hilo
//	recv(cliente,&cli,4,MSG_WAITALL);
//	if(cli != INIT_CLI){
//		perror("Se conecto un rancio");
//		close(cliente);
//		return;
//	}
//	operacion* op = recibir_instruccion(cliente);
//
//	switch(op->op){
//	case READDIR:
//		log_info(logger,"Llego la instruccion READDIR");
//		break;
//	case OPEN:
//		log_info(logger,"Llego la instruccion OPEN");
//		break;
//	case READ:
//		log_info(logger,"Llego la instruccion READ");
//		break;
//	case MKNOD:
//		log_info(logger,"Llego la instruccion MKNOD");
//		break;
//	case MKDIR:
//		log_info(logger,"Llego la instruccion MKDIR");
//		break;
//	case CHMOD:
//		log_info(logger,"Llego la instruccion CHMOD");
//		break;
//	case UNLINK:
//		log_info(logger,"Llego la instruccion UNLINK");
//		break;
//	default:
//		log_error(logger, "Llego una instruccion no habilitada");
//		break;
//	}
//}

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

void atender_cliente(int cliente){
	//Esperar con recv los pedidos de instrucciones que llegan del sac-cli


	operaciones operacion;

	while(recv(cliente,&operacion,4,MSG_WAITALL)>0){


		switch(operacion){
		case READDIR:
			sac_readdir();
			break;
		case OPEN:
			break;
		case READ:
			break;
		case MKNOD:
			break;
		case MKDIR:
			break;
		case CHMOD:
			break;
		case UNLINK:
			break;
		}
	}
}

void sac_readdir(){

}


