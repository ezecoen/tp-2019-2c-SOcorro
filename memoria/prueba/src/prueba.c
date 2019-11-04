#include <stdio.h>
#include <stdlib.h>
#include <../libmuse/src/libmuse.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "prueba.h"
#include <unistd.h>
#include <string.h>

int main(void) {
	iniciar_log();
	leer_config();
	pruebita();
	int init = muse_init((int)getpid(),configuracion->ip_muse,configuracion->puerto_muse);
	if(init==0){
		void* void1 = malloc(10);
		char* mensaje = string_new();
		char* mensaje_recibido = malloc(10);
		string_append(&mensaje,"hola :D:D");
		memcpy(void1,mensaje,10);

//		int alloc1 = muse_alloc(10);
//		printf("Direccion1 de muse_alloc recibida en prueba: %d\n",alloc1);
//		muse_free(alloc1);
//		alloc1 = muse_alloc(4096);
//		printf("Direccion1 de muse_alloc recibida en prueba: %d\n",alloc1);
//		int cpy1 = muse_cpy(alloc1,void1,10);
//		printf("resultado cpy: %d\n",cpy1);
//		int get1 = muse_get((void*)mensaje_recibido,alloc1,10);
//		printf("resultado get: %d\n",get1);
//		printf("contenido en get: %s\n",mensaje_recibido);
//		int alloc2 = muse_alloc(500);
//		printf("Direccion1 de muse_alloc recibida en prueba: %d\n",alloc2);

		char* char2 = string_new();
		for(int i = 0;i<4999;i++){
			string_append(&char2,"0");
		}
		char* char2_respuesta = malloc(5000);

		int alloc2 = muse_alloc(5000);
		printf("Direccion2 de muse_alloc recibida en prueba: %d\n",alloc2);
		int cpy2 = muse_cpy(alloc2,(void*)char2,5000);
		printf("resultado cpy: %d\n",cpy2);
		int get2 = muse_get((void*)char2_respuesta,alloc2,5000);
		printf("resultado get: %d\n",get2);
		printf("contenido en get: %s\n",char2_respuesta);


		muse_close();
	}
	return EXIT_SUCCESS;
}
void iniciar_log(){//0 es archivo, 1 es consola
	char* nombre = string_new();
	string_append(&nombre,"prueba.log");
	logg = log_create(nombre,"prueba",1,LOG_LEVEL_TRACE);
	free(nombre);
}
s_config* leer_config(){
	g_config = config_create("/home/utnso/tp-2019-2c-SOcorro/memoria/prueba/Debug/prueba.config");
	configuracion = malloc(sizeof(s_config));

	configuracion->ip_propia = string_duplicate(config_get_string_value(g_config,"IP_PROPIA"));
	log_info(logg,"IP_PROPIA: %s",configuracion->ip_propia);

	configuracion->ip_muse = string_duplicate(config_get_string_value(g_config,"IP_MUSE"));
	log_info(logg,"IP_MUSE: %s",configuracion->ip_muse);

	configuracion->puerto_muse = config_get_int_value(g_config,"PUERTO_MUSE");
	log_info(logg,"PUERTO_MUSE: %d",configuracion->puerto_muse);

	config_destroy(g_config);
	return configuracion;
}
