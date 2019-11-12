/*
 * prueba.h
 *
 *  Created on: 19 sep. 2019
 *      Author: utnso
 */

#ifndef PRUEBA_H_
#define PRUEBA_H_
#include <commons/config.h>
#include <commons/log.h>
#include <sys/mman.h>
typedef struct{
	char* ip_propia;
	char* ip_muse;
	uint32_t puerto_muse;
}s_config;

s_config* configuracion;
t_config* g_config;
t_log* logg;

void iniciar_log();
s_config* leer_config();
#endif /* PRUEBA_H_ */
