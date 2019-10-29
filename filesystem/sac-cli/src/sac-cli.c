#include "sac-cli.h"
#include "/home/utnso/tp-2019-2c-SOcorro/libreriaComun/src/libreriaComun.h"
#include "/home/utnso/tp-2019-2c-SOcorro/libreriaComun/src/libreriaComun.c"



/* Este es el contenido por defecto que va a contener
 * el unico archivo que se encuentre presente en el FS.
 * Si se modifica la cadena se podra ver reflejado cuando
 * se lea el contenido del archivo
 */
#define DEFAULT_FILE_CONTENT "Hello World!\n"

/*
 * Este es el nombre del archivo que se va a encontrar dentro de nuestro FS
 */
#define DEFAULT_FILE_NAME "hello"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME



/*
 * Esta es una estructura auxiliar utilizada para almacenar parametros
 * que nosotros le pasemos por linea de comando a la funcion principal
 * de FUSE
 */
struct t_runtime_options {
	char* welcome_msg;
} runtime_options;

/*
 * Esta Macro sirve para definir nuestros propios parametros que queremos que
 * FUSE interprete. Esta va a ser utilizada mas abajo para completar el campos
 * welcome_msg de la variable runtime_options
 */
#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }

void conectar_socket_a_server(char* ip, int puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);

	if (connect(_socket,(void*) &direccionServidor, sizeof(direccionServidor)) != 0)
	{
		perror("Error al conectar");
	}
}
struct stat *ato_stat(char *str_stat) {
    struct stat* param = (struct stat*)malloc(sizeof(struct stat)); // parameters
    memcpy(param, str_stat, sizeof(struct stat));

    return param;
}
char *statptr_to_str(struct stat *buf) {
    char *str = (char *)malloc(2000 * sizeof(char));
    memcpy(str, buf, sizeof(struct stat));

    return str;
}

/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la metadata de un archivo/directorio. Esto puede ser tamaño, tipo,
 * permisos, dueño, etc ...
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		stbuf - Esta esta estructura es la que debemos completar
 *
 * 	@RETURN
 * 		O archivo/directorio fue encontrado. -ENOENT archivo/directorio no encontrado
 */

static int sac_getattr(const char *path, struct stat *stbuf) {
	int _tam;

	memset(stbuf, 0, sizeof(struct stat));

	void* peticion = serializar_path(path, GETATTR);
	memcpy(&_tam, peticion+4, 4);
	send(_socket, peticion, _tam+8,0);
	free(peticion);

	operaciones op = recibir_op(_socket);
	void* _respuesta;
	int tam, error;
	if(op == ERROR){
//		recv(_socket,&error,4,0);
		return -ENOENT;
	}
	else{
		recv(_socket,&tam,4,MSG_WAITALL);
		tam-=8;
		_respuesta = malloc(tam);
		recv(_socket,_respuesta,tam,MSG_WAITALL);
		t_getattr* atributos = deserializar_getattr(_respuesta);
		stbuf->st_size = (long int)atributos->size;
		struct timespec time;
		time.tv_sec = (__time_t)(atributos->modif_time/1000);
		stbuf->st_mtim = time;
		if(atributos->tipo == 1){// es un archivo
			stbuf->st_mode = S_IFREG | 0777;
		}else{// es un directorio
			stbuf->st_mode = S_IFDIR | 0777;
		}
		return 0;
	}
//	st_mtime;


	//Si path es igual a "/" nos estan pidiendo los atributos del punto de montaje

//	if (strcmp(path, "/") == 0) {
//		stbuf->st_mode = S_IFDIR | 0777;
//		stbuf->st_nlink = 2;
//	} else if (strcmp(path, DEFAULT_FILE_PATH) == 0) {
//		stbuf->st_mode = S_IFREG | 0777;
//		stbuf->st_nlink = 1;
//		stbuf->st_size = strlen(DEFAULT_FILE_CONTENT);
//	} else {
//		res = -ENOENT;
//	}
//	return res;
}


/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la lista de archivos o directorios que se encuentra dentro de un directorio
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es un buffer donde se colocaran los nombres de los archivos y directorios
 * 		      que esten dentro del directorio indicado por el path
 * 		filler - Este es un puntero a una función, la cual sabe como guardar una cadena dentro
 * 		         del campo buf
 *
 * 	@RETURN
 * 		O directorio fue encontrado. -ENOENT directorio no encontrado
 */
static int sac_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	(void) offset;
	(void) fi;

	int _tam;

	void* peticion = serializar_path(path, READDIR);
	memcpy(&_tam, peticion+4, 4);
	send(_socket, peticion, _tam+8,0);
	free(peticion);

	operaciones op = recibir_op(_socket);
	void* _respuesta;
	int tam, cant, error;
	if(op == ERROR){
		recv(_socket,&error,4,0);
		return -1;
	}
	else{
		recv(_socket,&tam,4,0);
		_respuesta = malloc(tam);
		recv(_socket,_respuesta,tam,0);
		recv(_socket,&cant,4,0);
		t_list* dirents = deserializar_lista_ent_dir(_respuesta,cant);
		cargar_dirents_en_buffer(dirents, buf, filler, cant);
		return 0;
	}
}

void cargar_dirents_en_buffer(t_list* lista, void *buf, fuse_fill_dir_t filler,int cant){
	char* elem;
//	filler( buf, ".", NULL, 0 );  // Current Directory
//	filler( buf, "..", NULL, 0 ); // Parent Directory

	for(int i=0; i<cant; i++){    // All Other Directories
		elem = list_get(lista,i);
		filler(buf, elem, NULL, 0);
	}
}

/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para tratar de abrir un archivo
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		fi - es una estructura que contiene la metadata del archivo indicado en el path
 *
 * 	@RETURN
 * 		O archivo fue encontrado. -EACCES archivo no es accesible
 */
static int sac_open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}


static int sac_mknod(const char * path, mode_t mode, dev_t rdev){
	int _tam;

	void* peticion = serializar_path(path, MKNOD);
	memcpy(&_tam, peticion+4, 4);
	send(_socket, peticion, _tam+8,0);
	free(peticion);

	operaciones op = recibir_op(_socket);
	void* _respuesta;
	int tam, error;
	if(op == ERROR){
//		recv(_socket,&error,4,0);
		return -1;
	}
	else{
		return 0;
	}

//	int res;
//	res = mknod(path, mode, rdev);
//	if(res == -1)
//		return -errno;
//
//	return 0;
}


static int sac_chmod(const char *path, mode_t mode)
{
    int res;
    res = chmod(path, mode);
    if(res == -1)
        return -errno;
    return 0;
}


static int sac_unlink(const char *path)
{
    int res;

    res = unlink(path);
    if(res == -1)
        return -errno;

    return 0;
}


static int sac_mkdir(const char *path, mode_t mode)
{
	int _tam;

	void* peticion = serializar_path(path, MKDIR);
	memcpy(&_tam, peticion+4, 4);
	send(_socket, peticion, _tam+8,0);
	free(peticion);

	operaciones op = recibir_op(_socket);
	void* _respuesta;
	int tam, error;
	if(op == ERROR){
//		recv(_socket,&error,4,0);
		return -1;
	}
	else{
		return 0;
	}


//    int res;
//
//    res = mkdir(path, mode);
//    if(res == -1)
//        return -errno;
//    return 0;
}

/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener el contenido de un archivo
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es el buffer donde se va a guardar el contenido solicitado
 * 		size - Nos indica cuanto tenemos que leer
 * 		offset - A partir de que posicion del archivo tenemos que leer
 *
 * 	@RETURN
 * 		Si se usa el parametro direct_io los valores de retorno son 0 si  elarchivo fue encontrado
 * 		o -ENOENT si ocurrio un error. Si el parametro direct_io no esta presente se retorna
 * 		la cantidad de bytes leidos o -ENOENT si ocurrio un error. ( Este comportamiento es igual
 * 		para la funcion write )
 */
static int sac_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	size_t len;
	(void) fi;
	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
		return -ENOENT;

	len = strlen(DEFAULT_FILE_CONTENT);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, DEFAULT_FILE_CONTENT + offset, size);
	} else
		size = 0;

	return size;
}


/*
 * Esta es la estructura principal de FUSE con la cual nosotros le decimos a
 * biblioteca que funciones tiene que invocar segun que se le pida a FUSE.
 * Como se observa la estructura contiene punteros a funciones.
 */

static struct fuse_operations sac_oper = {
		.getattr = sac_getattr,
		.readdir = sac_readdir,
		.open = sac_open,
		.read = sac_read,
		.mknod = sac_mknod,
		.mkdir = sac_mkdir,
		.chmod = sac_chmod,
		.unlink = sac_unlink
};


/** keys for FUSE_OPT_ options */
enum {
	KEY_VERSION,
	KEY_HELP,
};


/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
static struct fuse_opt fuse_options[] = {
		// Este es un parametro definido por nosotros
		CUSTOM_FUSE_OPT_KEY("--welcome-msg %s", welcome_msg, 0),

		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};

// Dentro de los argumentos que recibe nuestro programa obligatoriamente
// debe estar el path al directorio donde vamos a montar nuestro FS
int main(int argc, char *argv[]) {
	/*==	Init Socket		==*/

//	Aca habria que hacer el handshake con el srv mandandole la operacion INIT_CLI
	_socket = conectar_socket_a("127.0.0.1", 8080);
	int cod = INIT_CLI;
	send(_socket,&cod, 4,0);

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		/** error parsing options */
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	// Si se paso el parametro --welcome-msg
	// el campo welcome_msg deberia tener el
	// valor pasado
	if( runtime_options.welcome_msg != NULL ){
		printf("%s\n", runtime_options.welcome_msg);
	}

	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar to do
	// en varios threads
	return fuse_main(args.argc, args.argv, &sac_oper, NULL);
}
