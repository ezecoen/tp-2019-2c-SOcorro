/*
 * sac-server.h
 *
 *  Created on: 2 oct. 2019
 *      Author: utnso
 */

#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#include <time.h>
#include <pthread.h>
#include <readline/readline.h>
#include <stdio.h>
#include <unistd.h>
#include <commons/config.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>

struct sockaddr_in  direccionServidor;
struct sockaddr_in  direccionCliente;

/*==	Variables Globales		==*/
int servidor;
t_log* logger;

/*==	Firmas de Funciones		==*/
void esperar_conexion(int servidor);
int aceptar_cliente(int servidor);
int crear_servidor(int puerto);
uint64_t timestamp();


#endif /* SAC_SERVER_H_ */
