#include "hilolayExtended.h"
void hilolay_init(){
	leer_config("/home/utnso/tp-2019-2c-SOcorro/planificador/hilolay/Debug/hilolay.config");
	int sock = conectar_socket_a(configuracion->ip_suse,configuracion->puerto_suse);
	if(sock > 0){
		socket_suse = sock;
		//envio el init a suse
		void* mensaje = malloc(8);
		int op = INIT;
		int pid = getpid();
		memcpy(mensaje,&op,4);
		memcpy(mensaje+4,&pid,4);
		send(socket_suse,mensaje,8,0);
	}
	else{
		perror("error en la conexion: ");
	}
}
int suse_create(int tid){
	int op = CREATE;
	void* mensaje = malloc(8);
	memcpy(mensaje,&op,4);
	memcpy(mensaje,&tid,4);
	send(socket_suse,mensaje,8,0);
	int resultado;
	recv(socket_suse,&resultado,4,0);
	return resultado;
}

int suse_schedule_next(void){
	//Aca es donde basicamente le digo a suse que corra el algoritmo SJF
	int op = SCHEDULE_NEXT;
	send(socket_suse,&op,4,0);
	int tid_return;
	recv(socket_suse,&tid_return,4,0);
	return tid_return;
}

int suse_join(int tid){
	int tid_actual = hilolay_get_tid();
	int op = JOIN;
	void* paquete = malloc(12);
	memcpy(paquete,&op,4);
	memcpy(paquete+4,&tid_actual,4);
	memcpy(paquete+8,&tid,4);
	send(socket_suse,paquete,12,0);
	int resultado;
	recv(socket_suse,&resultado,4,0);
	return resultado;
}

int suse_close(int){

}
int suse_wait(int, char *){

}
int suse_signal(int, char *){

}


