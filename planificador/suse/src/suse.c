#include "suse.h"

int main(void) {

	numeroDePrograma = 0;

	configuracion = leer_config();

	inicializarSemaforos();

	inicializarEstadosComunes();//Inicializo las listas de New - Block - Exit

	iniciar_log();

	crearHiloDeServidor();//Con esto empiezo a escuchar los programas que se vayan creando y los ults que vayan creando

//	for(int i = 0; i<4; i++){
//		sem_wait(&cont_multiprogramacion);
//		crearHilo(i+1);
//		//usleep(500000);
//		sleep(5);
//	}

	for(;;);

	return 0;

}

void iniciar_log(){
	log = log_create("suse.log","suse",1,LOG_LEVEL_TRACE);
	log_info(log, "Log creado!");
}

p_config* leer_config(){

	g_config = config_create("plani.config");
	configuracion = malloc(sizeof(p_config));

	configuracion->LISTEN_PORT = config_get_int_value(g_config, "LISTEN_PORT");
	configuracion->METRICS_TIMER = config_get_int_value(g_config, "METRICS_TIMER");
	configuracion->MAX_MULTIPROG = config_get_int_value(g_config, "MAX_MULTIPROG");
//	configuracion->SEM_IDS = config_get_array_value(g_config, "SEM_IDS");
//	configuracion->SEM_INIT = config_get_array_value(g_config, "SEM_INIT");
//	configuracion->SEM_MAX = config_get_array_value(g_config, "SEM_MAX");
	configuracion->ALPHA_SJF = config_get_int_value(g_config, "ALPHA_SJF");

	//Imprimo las variables
//	log_info(log, "Puerto de escucha: %d",configuracion->LISTEN_PORT);
//	log_info(log, "Timer para metricas: %d",configuracion->METRICS_TIMER);
//	log_info(log, "Grado de Multiprogramacion: %d",configuracion->MAX_MULTIPROG);
//	log_info(log, "Alpha para el algoritmo: %d",configuracion->ALPHA_SJF);

//	for(int i=0 ; i < (sizeof(configuracion->SEM_IDS / sizeof(uint32_t))); i++){
//		log_info(log, "Sem id %d: %c", i, configuracion->SEM_IDS[i]);
//	}

	return configuracion;

}

void inicializarSemaforos(){

	sem_init(&sem_mutNew, 0, 1);
	sem_init(&sem_mutConfig, 0, 1);
	sem_init(&cont_multiprogramacion, 0, configuracion->MAX_MULTIPROG);

}

void inicializarEstadosComunes(){
	estadoNew = list_create();
	estadoBlocked = list_create();
	estadoExit = list_create();
}

void planificarUlts(){

	while(1){
		//Si esta vacia la lista
			//No hago nada
		//Si tiene algo
			//Lo mando a estado new
				//Compruebo si el grado de multiprogramacion me lo permite
					//Si puedo
						//Compruebo si no hay otros hilos del mismo programa corriendo (En execute)
						//Si no hay lo mando a execute
						//Si hay lo dejo en new
					//Si no
						//??
	}

}

void printearNumeroDeHilo(tcb* threadControlBlock){

	int i = 0;
	int cantidadLoops = randomEntre1y10(rand());

	while(cantidadLoops > i){

		log_info(log, "Soy el hilo numero %d del proceso %d\n\n", threadControlBlock->t_id, threadControlBlock->p_id);
		//usleep(400000);
		sleep(4);

		i++;

	}

	log_info(log, "Termine de printear, soy el hilo numero %d y me printee %d veces", threadControlBlock->t_id, cantidadLoops);

	_Bool compararNumeroDeTID(tcb* _threadControlBlock){
		return threadControlBlock->t_id == threadControlBlock->t_id;
	}

	list_remove_by_condition(estadoNew, (void*)compararNumeroDeTID);

	return;

}

void crearHilo(int numero){

	tcb* threadControlBlock = malloc(sizeof(tcb));

	threadControlBlock->p_id = numeroDePrograma;
	threadControlBlock->t_id = numero;
	threadControlBlock->estimacion = 0;

	sem_wait(&sem_mutNew);
		list_add(estadoNew, threadControlBlock);
	sem_post(&sem_mutNew);

	pthread_t hiloDeNumero;
	if(pthread_create(&hiloDeNumero, NULL, (void*)printearNumeroDeHilo, (void*)threadControlBlock)!=0){
		log_error(log, "Error creando el hilo");
	}

	pthread_detach(hiloDeNumero);

}

int randomEntre1y10(int numero){

	int aux = numero % 10;

	if(aux > 10){
		aux = randomEntre1y10(aux);
	}

	return aux;
}

//Cosas para el servidor
void montarServidor(){

	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8080);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	//Esto es para que cuando haga ctrl+c y vuelvo a levantar el sv, no tenga problemas con el bind
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if(bind(servidor, (void*)&direccionServidor, sizeof(direccionServidor))!= 0){

		perror("Fallo la creacion del servidor en el bind");

	}

	printf("Estoy escuchando en el puerto 8080\n");
	listen(servidor, 100);

	int cliente = aceptarConexion(servidor);

	char* buffer = malloc(1000);

	while(1){
		int bytesRecibidos = recv(cliente, buffer, 4, 0);

		if(bytesRecibidos <= 0){
			perror("Se deconecto el cliente");
			return;
		}

		buffer[bytesRecibidos] = '\0';
		log_info(log, "Me llegaron %d bytes con %s", bytesRecibidos, buffer);
	}

	free(buffer);

	return;

}

int aceptarConexion(int servidor){

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);
	int cliente = accept(servidor, (void*)&direccionCliente, &tamanioDireccion);
	//perror("Error: ");
	printf("Recibi una conexion de %d!!\n", cliente);

	send(cliente, "Hola NetCat!", 13, 0);
	send(cliente, ":)", 3, 0);

	return cliente;

}

void crearHiloPlanificador(){

	pthread_t planificador;

	if(pthread_create(&planificador, NULL, (void*)planificarUlts, NULL) != 0){
		log_error(log, "Hubo un error creando el hilo de planificacion");
	}

	pthread_detach(planificador);
}

void crearHiloDeServidor(){

	pthread_t servidor;//Con este thread escucho/recibo a los nuevos threads de programa que se creen

	if(pthread_create(&servidor, NULL, (void*)montarServidor, NULL) != 0){
		log_error(log, "Hubo un error crando el hilo del servidor");
	}

	pthread_detach(servidor);


}
