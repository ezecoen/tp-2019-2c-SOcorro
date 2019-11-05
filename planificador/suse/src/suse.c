#include "suse.h"

int main(void) {

	//Inicializo variables globales
	numeroDePrograma = 0;

	iniciar_log();

	configuracion = leer_config();

	inicializarSemaforos();

	inicializarEstadosComunes();//Inicializo las listas de New - Block - Exit

	inicializarOtrasListas();//Inicializo la lista de  programas ejecutando/a ejecutar

	int servidor = crearServidor();

//	while(1){
//		escucharServidor(servidor);//Con esto suse se queda esperando conexiones
//	}
//
//	pthread_t hiloDePrueba;
//	if(pthread_create(&hiloDePrueba, NULL, (void*)funcionDePrueba, NULL) != 0){
//		log_error(log, "Erroe creando el hilo de prueba");
//	}

//	pthread_detach(hiloDePrueba);

	while(1){};

	return 0;

}



//FUNCIONES PARA PRUEBAS LOCALES
void funcionDePrueba(){
	for(int i = 0; i<4; i++){
		sem_wait(&mut_multiprogramacion);
			crearHilo(i+1);
			usleep(5000000);
	}
}

void printearNumeroDeHilo(tcb* threadControlBlock){

	int i = 0;
	srand(time(NULL));
	int numeroRandom = rand();
	int cantidadLoops = randomEntre1y10(numeroRandom);

	while(cantidadLoops > i){

		log_info(log, "Soy el hilo numero %d del proceso %d\n\n", threadControlBlock->t_id, threadControlBlock->p_id);
		usleep(4000000);

		i++;

	}

	sem_post(&mut_multiprogramacion);

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

	pthread_t hiloDeNumero;
	if(pthread_create(&hiloDeNumero, NULL, (void*)printearNumeroDeHilo, (void*)threadControlBlock)!=0){
		log_error(log, "Error creando el hilo");
	}

	pthread_detach(hiloDeNumero);

	sem_wait(&sem_mutNew);
		list_add(estadoNew, threadControlBlock);
	sem_post(&sem_mutNew);
}

int randomEntre1y10(int numero){

	int aux = numero % 10;

	if(aux > 10){
		aux = randomEntre1y10(aux);
	}

	return aux;
}



//FUNCIONES PARA INICIALIZAR COSAS
p_config* leer_config(){//TODO: tengo que ver como levantar los arrays de la config

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

void iniciar_log(){
	log = log_create("suse.log","suse",1,LOG_LEVEL_TRACE);
	log_info(log, "Log creado!");
}

void inicializarSemaforos(){

	sem_init(&sem_mutNew, 0, 1);
	sem_init(&sem_mutConfig, 0, 1);
	sem_init(&mut_multiprogramacion, 0, configuracion->MAX_MULTIPROG);
	sem_init(&mut_numeroDePrograma, 0, 1);

}

void inicializarEstadosComunes(){
	estadoNew = list_create();
	estadoBlocked = list_create();
	estadoExit = list_create();
}

void inicializarOtrasListas(){

	listaDeProgramas = list_create();

}



//COSAS PARA EL SERVIDOR
int crearServidor(){

	struct sockaddr_in direccionServidor;

	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8080);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	//Esto es para que cuando haga ctrl+c y vuelvo a levantar el sv, no tenga problemas con el bind
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEPORT, &activado, sizeof(activado));

	if(bind(servidor, (void*)&direccionServidor, sizeof(direccionServidor))!= 0){

		perror("Fallo la creacion del servidor en el bind");
		return 1;
	}

	log_info(log, "Estoy escuchando en el puerto 8080");
	listen(servidor,SOMAXCONN);
	return servidor;
}

void escucharServidor(int servidor){//Esta funcion es la que va a hacer de escucha para todos los programas que se vayan levantando

	uint32_t cliente = aceptarConexion(servidor);

	pthread_t hiloDeEscucha;

	pthread_create(&hiloDeEscucha,NULL,(void*)ocupateDeEste,(void*)cliente);
	pthread_detach(hiloDeEscucha);

}

void ocupateDeEste(uint32_t cliente, uint32_t operacion){//Con esta funcion identifico lo que me pida hilolay y lo ejecuto

	log_info(log, "Tengo a este cliente %d",cliente);

	switch(operacion){
		case 0:
			//planificarNuevoProceso(pid, tid);
			break;
		case 1:
			//lo que tenga que hacer para suse_schedule_next
			break;
		case 2:
			//lo que tenga que hacer para suse_join
			break;
		case 3:
			//lo que tenga que hacer para suse_close
			break;
		case 4:
			//lo que tenga que hacer para suse_wait
			break;
		case 5:
			//lo que tenga que hacer para suse_signal
			break;
	}
}

int aceptarConexion(int servidor){

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);
	uint32_t cliente = accept(servidor, (void*)&direccionCliente, &tamanioDireccion);
	//perror("Error: ");
	log_info(log, "Recibi una conexion de %d!!\n", cliente);

	send(cliente, "Hola!", 6, 0);

	return cliente;

}



//FUNCIONES PARA HACER LO QUE ME PIDA HILOLAY, O SEA, LA IMPLEMENTACION DE SUSE
pcb* crearPCB(){//Esto es mas para gestion interna, no tiene mucho que ver con el planificador

	pcb* pcb = malloc(sizeof(pcb));

	pcb->ejecutando = false; //Me dice si tiene algun hilo ejecutando
	sem_wait(&mut_numeroDePrograma);//Clavo este semaforo por si me levantan dos programa al mismo tiempo
		pcb->p_id = numeroDePrograma;
		numeroDePrograma++;
	sem_post(&mut_numeroDePrograma);
	pcb->ults = list_create();//Lista de ults de este proceso

	list_add(listaDeProgramas, pcb);

}

void crearTCB(uint32_t pid, uint32_t tid){//Esto tiene que devolver un int, pero como todavia nose que int lo dejo como un void

	tcb* tcb = malloc(sizeof(tcb));

	tcb->estimacion = 0; //De entrada siempre es 0
	tcb->p_id = pid;
	tcb->t_id = tid;

	pcb* procesoPadre = buscarProcesoEnListaDeProcesos(pid);

	list_add(procesoPadre->ults, tcb);

}

void planificarNuevoProceso(uint32_t pid, uint32_t tid){//Con esto creo un nuevo PCB y lo meto a la cola de New

	pcb* pcb;
	pcb  = crearPCB();

	list_add(estadoNew, pcb);

}

//OTRAS FUNCIONES

