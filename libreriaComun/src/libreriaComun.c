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
int recibir_op(int sock){
	int cod;
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
		perror("Error de connect");
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

uint32_t length_de_char_asterisco(char** arrays){
	uint32_t i = 0;
	while(arrays[i] != NULL){
		i++;
	}
	return i;
}

//t_readdir* crear_readdir (char* path){
//	int size_path = char_length(path);
//	t_readdir* estruc = malloc(sizeof(t_readdir));
//	estruc->path = malloc(size_path);
//	estruc->size_path = size_path;
//	memcpy(estruc->path, path, estruc->size_path);
//	return estruc;
//}

void* serializar_path(const char* path, operaciones comando){
	int size_path = char_length(path);
	int bytes = sizeof(int)*2 + size_path;
	int puntero=0;
	void* magic = malloc(bytes);

	memcpy(magic+puntero, &comando, sizeof(int));
	puntero += sizeof(int);
//	memcpy(magic+puntero, &bytes, sizeof(int));
//	puntero += sizeof(int);
	memcpy(magic+puntero, &size_path, sizeof(int));
	puntero += sizeof(int);
	memcpy(magic+puntero, path, size_path);
	puntero += size_path;
	return magic;
}

//void reddir_destroy (t_readdir* estructura){
//	free(estructura->path);
//	free(estructura);
//}

void* serializar_lista_ent_dir(t_list* lista){
	int _tam = tamanio_de_todos_las_ent_dir(lista) + 12;
	void* magic = malloc(_tam);
	int puntero = 0;
	operaciones op = READDIR;

	memcpy(magic+puntero, &op, 4);
	puntero += 4;
	memcpy(magic+puntero, &_tam, 4);
	puntero += 4;
	memcpy(magic+puntero, &lista->elements_count, 4);
	puntero += 4;
	for(int j = 0; j < lista->elements_count; j++){
		char* elemento = list_get(lista, j);
		int tam_elem = char_length(elemento);
		memcpy(magic+puntero, &tam_elem, 4);
		puntero += 4;
		memcpy(magic+puntero, elemento, tam_elem);
		puntero += tam_elem;
	}
	return magic;
}

t_list* deserializar_lista_ent_dir(void* magic, int tam_lista){
	t_list* lista = list_create();
	int tam_elem;
	char* elem;
	int puntero = 0;

	for(int i=0; i<tam_lista; i++){
		memcpy(&tam_elem, magic+puntero,4);
		puntero += 4;
		elem = malloc(tam_elem);
		memcpy(elem,magic+puntero,tam_elem);
		puntero += tam_elem;

		list_add(lista,elem);
	}
	return lista;
}

int tamanio_de_todos_las_ent_dir(t_list* lista){
	int tam = 0;
	char* elem;

	for(int i=0;i<lista->elements_count;i++){
		elem = list_get(lista,i);
		tam += char_length(elem)+4;
	}
	return tam;
}

t_getattr* crear_getattr(uint32_t size, uint64_t modif_time, uint8_t tipo){
	t_getattr* resp = malloc(sizeof(t_getattr));
	resp->size = size;
	resp->modif_time = modif_time;
	resp->tipo = tipo;
	return resp;
}

void* serializar_getattr(t_getattr* stat){
	int _tam = sizeof(int)*2 + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint8_t);
	void* magic = malloc(_tam);
	int puntero = 0;
	operaciones op = GETATTR;
	memcpy(magic+puntero, &op, 4);
	puntero += 4;
	memcpy(magic+puntero, &_tam, 4);
	puntero += 4;
	memcpy(magic+puntero, &stat->size, sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero, &stat->modif_time, sizeof(uint64_t));
	puntero += sizeof(uint64_t);
	memcpy(magic+puntero, &stat->tipo, sizeof(uint8_t));
	puntero += sizeof(uint8_t);
	return magic;
}

t_getattr* deserializar_getattr(void* magic){
	t_getattr* resp = malloc(sizeof(t_getattr));
	int puntero = 0;
	memcpy(&resp->size, magic+puntero, 4);
	puntero += 4;
	memcpy(&resp->modif_time, magic+puntero, 8);
	puntero += 8;
	memcpy(&resp->tipo, magic+puntero, 1);
	puntero += 1;
	return resp;
}

t_open* crear_open(char* path, int flags){
	t_open* pedido = malloc(sizeof(t_open));
	pedido->size_path = char_length(path);
	pedido->path = malloc(pedido->size_path);
	memcpy(pedido->path,path,pedido->size_path);

	if(flags & O_CREAT)
		pedido->crear = 1;
	else
		pedido->crear = 0;


	if(flags & O_EXCL)
			pedido->crear_ensure = 1;
	else
		pedido->crear_ensure = 0;


	if(flags & O_TRUNC)
			pedido->truncate = 1;
	else
		pedido->truncate = 0;

	return pedido;
}

void* serialiazar_open(t_open* open){
	int bytes = sizeof(int)*2 + sizeof(int)*4 + open->size_path;
	operaciones op = OPEN;
	void* magic = malloc(bytes);
	int puntero = 0;

	memcpy(magic+puntero,&bytes,4);
	puntero += 4;
	memcpy(magic+puntero,&op,4);
	puntero += 4;
	memcpy(magic+puntero,&open->size_path,4);
	puntero += 4;
	memcpy(magic+puntero,open->path,open->size_path);
	puntero += open->size_path;
	memcpy(magic+puntero,&open->crear,4);
	puntero += 4;
	memcpy(magic+puntero,&open->crear_ensure,4);
	puntero += 4;
	memcpy(magic+puntero,&open->truncate,4);
	puntero += 4;

	return magic;
}

t_open* deserializar_open(void* magic){
}
