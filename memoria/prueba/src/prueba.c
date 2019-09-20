#include <stdio.h>
#include <stdlib.h>
#include <../libmuse/src/libmuse.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "prueba.h"

int main(void) {
	iniciar_log();
	leer_config();
	pruebita();
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
	s_config* configuracion = malloc(sizeof(s_config));

	configuracion->ip_propia = string_duplicate(config_get_string_value(g_config,"IP_PROPIA"));
	log_info(logg,"IP_PROPIA: %s",configuracion->ip_propia);

	configuracion->ip_muse = string_duplicate(config_get_string_value(g_config,"IP_MUSE"));
	log_info(logg,"IP_MUSE: %s",configuracion->ip_muse);

	configuracion->puerto_muse = config_get_int_value(g_config,"PUERTO_MUSE");
	log_info(logg,"PUERTO_MUSE: %d",configuracion->puerto_muse);

	config_destroy(g_config);
	return configuracion;
}
