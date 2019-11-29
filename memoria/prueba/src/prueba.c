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
		test_izi();
		muse_close();
	}
	return EXIT_SUCCESS;
}
void test_pulenta(){
	int alloc1 = muse_alloc(200);
	printf("Direccion de muse_alloc recibida en prueba: %d\n",alloc1);

	int alloc2 = muse_alloc(200);
	printf("Direccion de muse_alloc 2 recibida en prueba: %d\n",alloc2);

	int alloc3 = muse_alloc(200);
	printf("Direccion de muse_alloc 3 recibida en prueba: %d\n",alloc3);

	int alloc4 = muse_alloc(200);
	printf("Direccion de muse_alloc 4 recibida en prueba: %d\n",alloc4);

	int alloc5 = muse_alloc(1000);
	printf("Direccion1 de muse_alloc 5 recibida en prueba: %d\n",alloc5);

	int alloc6 = muse_alloc(80);
	printf("Direccion1 de muse_alloc 5 recibida en prueba: %d\n",alloc6);

	char* mensaje1 = string_new();
	string_append(&mensaje1,"hola este es mi mensaje");
	int cpy1 = muse_cpy(alloc2,(void*)mensaje1,25);
	printf("resultado cpy 1: %d\n",cpy1);

	int cpy2 = muse_cpy(alloc3,(void*)mensaje1,25);
	printf("resultado cpy 2: %d\n",cpy2);

	char* mensaje5 = string_new();
	char* a = string_new();
	string_append(&a,"aa");
	for(int i = 0; i < 100;i++){
		string_append(&mensaje5,a);
	}
	int cpy5 = muse_cpy(alloc5,(void*)mensaje5,200);
	printf("resultado cpy 5: %d\n",cpy5);

	char* mensaje_recibido1 = malloc(25);
	int get1 = muse_get((void*)mensaje_recibido1,alloc2,25);
	printf("resultado get 1: %d\n",get1);
	printf("contenido en get 1: %s\n",mensaje_recibido1);

	char* mensaje_recibido2 = malloc(25);
	int get2 = muse_get((void*)mensaje_recibido2,alloc3,25);
	printf("resultado get 2: %d\n",get2);
	printf("contenido en get 2: %s\n",mensaje_recibido2);

	char* mensaje_recibido5= malloc(200);
	int get5 = muse_get((void*)mensaje_recibido5,alloc5,200);
	printf("resultado get 5: %d\n",get5);
	printf("contenido en get 5: %s\n",mensaje_recibido5);

	muse_free(alloc3);

	uint32_t map1 = muse_map("/home/utnso/tp-2019-2c-SOcorro/memoria/ejemplo_map",100,MAP_SHARED);
	printf("resultado map 1: %d\n",map1);

	char* mensaje_map1 = malloc(100);
	int get_map1 = muse_get((void*)mensaje_map1,map1,100);
	printf("resultado map get 1: %d\n",get_map1);
	printf("contenido en map get 1: %s\n",mensaje_map1);

	char* mensaje_copiado_map1 = string_new(25);
	string_append(&mensaje_copiado_map1,"hola este es mi mensaje");
	int cpy_map1 = muse_cpy(map1,(void*)mensaje_copiado_map1,25);
	printf("resultado cpy map 1: %d\n",cpy_map1);

	char* mensaje_map2 = malloc(100);
	int get_map2 = muse_get((void*)mensaje_map2,map1,100);
	printf("resultado map get 2: %d\n",get_map2);
	printf("contenido en map get 2: %s\n",mensaje_map2);

	muse_sync(map1,100);

	muse_unmap(map1);
}
void test_izi(){
	int alloc1 = muse_alloc(50);
	printf("Direccion de muse_alloc recibida en prueba: %d\n",alloc1);

	int alloc2 = muse_alloc(25);
	printf("Direccion de muse_alloc 2 recibida en prueba: %d\n",alloc2);

	int alloc3 = muse_alloc(200);
	printf("Direccion de muse_alloc 3 recibida en prueba: %d\n",alloc3);

	char* mensaje1 = string_new();
	string_append(&mensaje1,"hola este es mi mensaje");
	int cpy1 = muse_cpy(alloc1,(void*)mensaje1,26);
	printf("resultado cpy 1: %d\n",cpy1);

	int cpy2 = muse_cpy(alloc2,(void*)mensaje1,25);
	printf("resultado cpy 2: %d\n",cpy2);

	char* mensaje3 = string_new();
	char* a = string_new();
	string_append(&a,"aa");
	for(int i = 0; i < 100;i++){
		string_append(&mensaje3,a);
	}

	int cpy3 = muse_cpy(alloc3,(void*)mensaje3,25);
	printf("resultado cpy 3: %d\n",cpy3);

	char* mensaje_recibido1 = malloc(50);
	int get1 = muse_get((void*)mensaje_recibido1,alloc2,50);
	printf("resultado get 1: %d\n",get1);
	printf("contenido en get 1: %s\n",mensaje_recibido1);

	char* mensaje_recibido2 = malloc(25);
	int get2 = muse_get((void*)mensaje_recibido2,alloc3,25);
	printf("resultado get 2: %d\n",get2);
	printf("contenido en get 2: %s\n",mensaje_recibido2);

	char* mensaje_recibido3= malloc(200);
	int get3 = muse_get((void*)mensaje_recibido3,alloc3,200);
	printf("resultado get 3: %d\n",get3);
	printf("contenido en get 3: %s\n",mensaje_recibido3);

	uint32_t map1 = muse_map("/home/utnso/tp-2019-2c-SOcorro/memoria/ejemplo_map",100,MAP_SHARED);
	printf("resultado map 1: %d\n",map1);

	char* mensaje_map1 = malloc(100);
	int get_map1 = muse_get((void*)mensaje_map1,map1,100);
	printf("resultado map get 1: %d\n",get_map1);
	printf("contenido en map get 1: %s\n",mensaje_map1);

	char* mensaje_copiado_map1 = string_new(25);
	string_append(&mensaje_copiado_map1,"hola este es mi mensaje");
	int cpy_map1 = muse_cpy(map1,(void*)mensaje_copiado_map1,25);
	printf("resultado cpy map 1: %d\n",cpy_map1);

	char* mensaje_map2 = malloc(100);
	int get_map2 = muse_get((void*)mensaje_map2,map1,100);
	printf("resultado map get 2: %d\n",get_map2);
	printf("contenido en map get 2: %s\n",mensaje_map2);

	muse_sync(map1,100);

	muse_unmap(map1);
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
