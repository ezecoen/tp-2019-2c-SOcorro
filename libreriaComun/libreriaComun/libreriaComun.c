#include "libreriaComun.h"

//	PAQUETE	:	COD_COM	|	TAM_PAQUETE	|	PAQUETE
//	Para recibir los paquetes hacer
//	uint32_t comando = recibir_op(socket)
//	uint32_t pack_size;
//	recv(socket,&pack_size,4,0);
//	void* buffer = malloc(pack_size);
//	recv(socket,buffer,pack_size,0);

int crear_servidor(int puerto){
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
	log_info(logger,"Estoy escuchando en el puerto %d\n",puerto);
	listen(servidor,SOMAXCONN);
	return servidor;
}
void mandar_char(char* _char, uint32_t _socket,uint32_t com){
	uint32_t tam = char_length(_char);
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
char* recibir_char(uint32_t _socket){
	uint32_t tam;
	recv(_socket,&tam,4,MSG_WAITALL);
	char* _char = malloc(tam);
	void* magic = malloc(tam);
	recv(_socket,magic,tam,0);
	memcpy(_char,magic,tam);
	free(magic);
	return _char;
}
void mandar(uint32_t tipo,void* algo,uint32_t _socket){
	uint32_t _tam;
	switch(tipo){
	case EXITOSO:
		memcpy(&_tam,algo+4,4);
		send(_socket,algo,_tam,0);
		break;
	case ERROR:
		memcpy(&_tam,algo+4,4);
		send(_socket,algo,_tam,0);
		break;
	default:
		//puts("Error de tipo de comando");
		break;
	}

}
uint32_t char_length(char* string){
	return strlen(string)+1;
}
uint32_t recibir_op(uint32_t sock){
	uint32_t cod;
	recv(sock,&cod,4,MSG_WAITALL);
	return cod;
}
t_error* crear_error(char* descripcion){
	uint32_t size_descripcion = char_length(descripcion);
	t_error* error = malloc(sizeof(t_error));
	error->descripcion = malloc(size_descripcion);
	error->size_descripcion = size_descripcion;
	memcpy(error->descripcion,descripcion,error->size_descripcion);
	return error;
}
void error_destroy(t_error* error){
	free(error->descripcion);
	free(error);
}
void* serializar_paquete_error(t_error* error){
	uint32_t bytes = sizeof(uint32_t)+ char_length(error->descripcion) + sizeof(uint32_t)*2;
	uint32_t comando = ERROR;
	uint32_t puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,4);
	puntero += 4;
	memcpy(magic+puntero,&bytes,4);
	puntero += 4;
	memcpy(magic+puntero,&error->size_descripcion,4);
	puntero += 4;
	memcpy(magic+puntero,error->descripcion,error->size_descripcion);
	puntero += error->size_descripcion;
	return magic;
}
t_error* deserializar_paquete_error(void* magic){
	t_error* error = malloc(sizeof(t_error));
	uint32_t puntero = 0;
	memcpy(&error->size_descripcion,magic+puntero,4);
	puntero+=4;
	error->descripcion = malloc(error->size_descripcion);
	memcpy(error->descripcion,magic+puntero,error->size_descripcion);
	puntero+=error->size_descripcion;
	return error;
}
t_exitoso* crear_exitoso(char* descripcion){
	int size_descripcion = char_length(descripcion);
	t_exitoso* exitoso = malloc(sizeof(t_exitoso));
	exitoso->descripcion = malloc(size_descripcion);

	exitoso->size_descripcion = size_descripcion;
	memcpy(exitoso->descripcion,descripcion,exitoso->size_descripcion);
	return exitoso;
}
void exitoso_destroy(t_exitoso* exitoso){
	free(exitoso->descripcion);
	free(exitoso);
}
void* serializar_paquete_exitoso(t_exitoso* exitoso){
	int bytes = sizeof(int)+ char_length(exitoso->descripcion) + sizeof(int)*2;
	int comando = EXITOSO;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(int));
	puntero += sizeof(int);
	memcpy(magic+puntero,&bytes,sizeof(int));
	puntero += sizeof(int);
	memcpy(magic+puntero,&exitoso->size_descripcion,sizeof(int));
	puntero += sizeof(int);
	memcpy(magic+puntero,exitoso->descripcion,exitoso->size_descripcion);
	puntero += exitoso->size_descripcion;
	return magic;
}
t_exitoso* deserializar_paquete_exitoso(void* magic){
	t_exitoso* exitoso = malloc(sizeof(t_exitoso));
	uint32_t puntero = 0;
	memcpy(&exitoso->size_descripcion,magic+puntero,4);
	puntero+=4;
	exitoso->descripcion = malloc(exitoso->size_descripcion);
	memcpy(exitoso->descripcion,magic+puntero,exitoso->size_descripcion);
	puntero+=exitoso->size_descripcion;
	return exitoso;
}
uint32_t conectar_socket_a(char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	uint32_t cliente = socket(AF_INET, SOCK_STREAM,0);
	if (connect(cliente,(void*) &direccionServidor, sizeof(direccionServidor)) != 0)
	{
		//puts("Error al conectar");
//		log_error(logg,"Error al conectar a ip %s y puerto %d",ip,puerto);
		return -1;
	}
	return cliente;
}
int aceptar_cliente(int servidor){
	struct sockaddr_in direccion_cliente;
	uint32_t tamanio_direccion = sizeof(struct sockaddr_in);
	uint32_t cliente;
	cliente = accept(servidor,(void*) &direccion_cliente,&tamanio_direccion);
	return cliente;
}
uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long )tv.tv_sec) * 1000 + ((unsigned long) tv.tv_usec) / 1000);
	uint64_t a = result;
	return a;
}


