// servidor para probar sockets mañana
#include "sac-server.h"
#include "libreriaComun/libreriaComun.h"

int main(void) {
	logger = log_create("loger","sac-server",1,LOG_LEVEL_TRACE);
	int _servidor = crear_servidor(8080);
	while(1){
		esperar_conexion(_servidor);
	}
	return 0;
}


uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long )tv.tv_sec) * 1000 + ((unsigned long) tv.tv_usec) / 1000);
	uint64_t a = result;
	return a;
}
void esperar_conexion(int servidor){
	int cliente = aceptar_cliente(servidor);
	log_info(logger,"Se conecto un cliente con el socket numero %d",cliente);
//	falta la implementacion de la funcion que va hacer el hilo al conectarse sac-cli
//	pthread_t hilo_retardo_mem;
//		if(pthread_create(&hilo_retardo_mem,NULL,(void*)hace_algo,NULL)!=0){
//			log_error(logger,"Error al crear el hilo de journal");
//		}
//		pthread_detach(hilo_retardo_mem);
	//close(cliente);

}


