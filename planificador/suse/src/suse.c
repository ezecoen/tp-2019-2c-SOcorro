#include "suse.h"

int main(int argc, char **argv) {
	char* path_de_config = string_duplicate(argv[1]);

	iniciar_log(path_de_config);

	leer_config(path_de_config);

	//Inicializo variables globales
	numeroDePrograma = 0;

	inicializarSemaforos();

	inicializarEstadosComunes();//Inicializo las listas de New - Block - Exit

	inicializarOtrasListas();//Inicializo la lista de  programas ejecutando/a ejecutar

	int servidor = crearServidor();

	while(1){
		escucharServidor(servidor);//Con esto suse se queda esperando conexiones
	}
//
//	pthread_t hiloDePrueba;
//	if(pthread_create(&hiloDePrueba, NULL, (void*)funcionDePrueba, NULL) != 0){
//		log_error(logg, "Erroe creando el hilo de prueba");
//	}

//	pthread_detach(hiloDePrueba);

//	while(1){};

	return 0;

}


//FUNCIONES PARA INICIALIZAR COSAS
p_config* leer_config(char* path){

	g_config = config_create(path);
	configuracion = malloc(sizeof(p_config));

	configuracion->LISTEN_PORT = config_get_int_value(g_config, "LISTEN_PORT");
	configuracion->METRICS_TIMER = config_get_int_value(g_config, "METRICS_TIMER");
	configuracion->MAX_MULTIPROG = config_get_int_value(g_config, "MAX_MULTIPROG");
	char** aux_sem_ids = config_get_array_value(g_config, "SEM_IDS");
	char** aux_sem_init = config_get_array_value(g_config, "SEM_INIT");
	char** aux_sem_max = config_get_array_value(g_config, "SEM_MAX");
	configuracion->ALPHA_SJF = config_get_double_value(g_config, "ALPHA_SJF");
	configuracion->SEM_INIT = list_create();
	configuracion->SEM_IDS = list_create();
	configuracion->SEM_MAX = list_create();

	int i=0,j=0,k=0;
	while(aux_sem_ids[k] != NULL){
		list_add(configuracion->SEM_IDS,string_duplicate(aux_sem_ids[k]));
		semaforo_t* semaforo = malloc(sizeof(semaforo_t));
		semaforo->colaDeBloqueo = list_create();
		semaforo->idNumerico = k;
		list_add(listaDeSemaforos, semaforo);
		k++;
	}

	while(aux_sem_init[i] != NULL){
		_Bool buscarSemaforoPorId(semaforo_t* _sem){
			return _sem->idNumerico == i;
		}

		semaforo_t* sem = list_find(listaDeSemaforos,(void*)buscarSemaforoPorId);
		sem->valorInicial = atoi(aux_sem_init[i]);
		list_add(configuracion->SEM_INIT,(void*)atoi(aux_sem_init[i]));
		i++;
	}

	while(aux_sem_max[j] != NULL){
		_Bool buscarSemaforoPorId(semaforo_t* _sem){
			return _sem->idNumerico == j;
		}

		semaforo_t* sem = list_find(listaDeSemaforos,(void*)buscarSemaforoPorId);
		sem->valorMaximo = atoi(aux_sem_init[j]);
		list_add(configuracion->SEM_MAX,(void*)atoi(aux_sem_max[j]));
		j++;
	}


	//Imprimo las variables
//	log_info(logg, "Puerto de escucha: %d",configuracion->LISTEN_PORT);
//	log_info(logg, "Timer para metricas: %d",configuracion->METRICS_TIMER);
//	log_info(logg, "Grado de Multiprogramacion: %d",configuracion->MAX_MULTIPROG);
//	log_info(logg, "Alpha para el algoritmo: %d",configuracion->ALPHA_SJF);

//	for(int i=0 ; i < (sizeof(configuracion->SEM_IDS / sizeof(uint32_t))); i++){
//		log_info(logg, "Sem id %d: %c", i, configuracion->SEM_IDS[i]);
//	}

	return configuracion;

}

void iniciar_log(char* path){
	char* nombre = string_new();
	string_append(&nombre,path);
	string_append(&nombre,".log");
	logg = log_create(nombre,"suse",1,LOG_LEVEL_TRACE);
	log_info(logg, "Log creado!");
}

void inicializarSemaforos(){

	sem_init(sem_mutNew, 0, 1);
	sem_init(sem_mutConfig, 0, 1);
	sem_init(mut_multiprogramacion, 0, configuracion->MAX_MULTIPROG);
	sem_init(mut_numeroDePrograma, 0, 1);
	sem_init(mut_listaDeSemaforos, 0, 1);
	sem_init(mut_blocked, 0, 1);

}

void inicializarEstadosComunes(){
	estadoNew = list_create();
	estadoBlocked = list_create();
	estadoExit = list_create();
}

void inicializarOtrasListas(){

	listaDeProgramas = list_create();
	listaDeSemaforos = list_create();

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

	log_info(logg, "Estoy escuchando en el puerto 8080");
	listen(servidor,SOMAXCONN);
	return servidor;
}

void escucharServidor(int servidor){//Esta funcion es la que va a hacer de escucha para todos los programas que se vayan levantando

	uint32_t cliente = aceptarConexion(servidor);

	pthread_t hiloDeEscucha;

	pthread_create(&hiloDeEscucha,NULL,(void*)ocupateDeEste,(void*)cliente);
	pthread_detach(hiloDeEscucha);

}

void ocupateDeEste(uint32_t cliente){//Con esta funcion identifico lo que me pida hilolay y lo ejecuto
	log_info(logg, "Tengo a este cliente %d",cliente);
	int operacion;
	int pid,tid;
	t_list* colaDeReady;
	tcb* exec;
	while(recv(cliente,&operacion,4,MSG_WAITALL)>0){
		switch(operacion){
			case INIT://recive el pid
				//se hace 1 vez por proceso
				recv(cliente,&pid,4,0);
				crearPCB(pid);
				colaDeReady = list_create();
				break;
			case CREATE:;
				recv(cliente,&tid,4,0);
				tcb* _tcb = crearTCB(pid,tid);
				if(multiprogramacion>0){
					//pongo en ready y saco de new
					list_add(colaDeReady,_tcb);
					//me fijo si no hay nadie en exec, si esta vacia, me meto
					sacarDeNew(_tcb);
					if(exec == NULL){
						exec = _tcb;
						sacarDeReady(_tcb,colaDeReady);
					}
				}

				int resultado = 0;

				send(cliente,&resultado,4,0);

				break;
			case SCHEDULE_NEXT:;
				_Bool ordenamientoSjf(tcb* tcb1,tcb* tcb2){
					return tcb1->estimacion < tcb2->estimacion;
				}
				//si no hay nadie en ready, sigue ejecutando
				if(list_is_empty(colaDeReady)){
					send(cliente,&exec->t_id,4,0);
					actualizarEstimacion(exec,timestamp());
					break;
				}
				//traigo el proximo a ejecutar
				list_sort(colaDeReady,(void*)ordenamientoSjf);
				tcb* tcbAEjecutar = list_get(colaDeReady,0);
				tcb* tcbAux = exec;
				uint64_t tiempoAux;
				//pongo al nuevo en exec y lo saco de ready
				exec = tcbAEjecutar;
				tiempoAux = timestamp();
				exec->tiempoDeInicio = tiempoAux;
				sacarDeReady(tcbAEjecutar,colaDeReady);
				send(cliente,&tcbAEjecutar->t_id,4,0);
				//el q estaba en exec lo devuelvo a ready
				list_add(colaDeReady,tcbAux);
				//se actualiza el estimador del que salio de exec
				actualizarEstimacion(tcbAux,tiempoAux);

				break;
			case JOIN:
				int tid_para_join;
				_Bool buscarTid(tcb* _tcb){
					return _tcb->t_id == tid_para_join;
				}
				recv(cliente,&tid_para_join,4,0);
				//tid actual se bloquea esperando a que termine tid exec->tid
				//hay que buscar a tid_para_join y ponerle en su lista el tid actual
				tcb* tcb_para_join = list_find(estadoNew,(void*)buscarTid);
				if(tcb_para_join == NULL){
					tcb_para_join = list_find(colaDeReady,(void*)buscarTid);
					if(tcb_para_join == NULL){
						tcb_para_join = list_find(estadoBlocked,(void*)buscarTid);
						if(tcb_para_join == NULL){
							//no hay que bloquear al tid actual
							int resultado = 0;
							send(cliente,&resultado,4,0);
							break;
						}
					}
				}
				//tengo que agregarle a tcb_para_join->lista de tid esperandolo, el tid actual
				list_add(tcb_para_join->tidsEsperando,exec->t_id);
				//bloqueo al tid actual
				list_add(estadoBlocked,exec);
				//actualizo la estimacion
				actualizarEstimacion(exec,timestamp());
				exec = NULL;

				break;
			case CLOSE:
				//lo que tenga que hacer para suse_close
				break;
			case WAIT:
				uint32_t tamanioPaquete;
				recv(cliente, &tamanioPaquete, 4, MSG_WAITALL);
				void* paquete = malloc(tamanioPaquete);
				recv(cliente, &paquete, tamanioPaquete,MSG_WAITALL);
				suse_wait_t* wait = deserializar_suse_wait(paquete);

				_Bool buscarSemaforoPorId(semaforo_t* sem){
					return strcmp(sem->id_semaforo, wait->id_semaforo) == 0;
				}

				semaforo_t* sem = list_find(listaDeSemaforos, (void*)buscarSemaforoPorId);

				if(sem->valorInicial == 0){
					sem_wait(&mut_blocked);
						list_add(estadoBlocked, excec);
					sem_post(&mut_blocked);
					list_add(sem->colaDeBloqueo, excec);
					excec = NULL;
				}else{
					sem->valorInicial--;
				}

				int resultado = 0;
				send(cliente,&resultado,4,0);
				break;

			case SIGNAL:
				uint32_t tamanioPaquete;
				recv(cliente, &tamanioPaquete, 4, MSG_WAITALL);
				void* paquete = malloc(tamanioPaquete);
				recv(cliente, &paquete, tamanioPaquete,MSG_WAITALL);
				suse_signal_t* signal = deserializar_suse_signal(paquete);

				_Bool buscarSemaforoPorId(semaforo_t* sem){
					return strcmp(sem->id_semaforo, wait->id_semaforo) == 0;
				}

				semaforo_t* sem = list_find(listaDeSemaforos, (void*)buscarSemaforoPorId);

				if(sem->valorInicial > 0){
					sem->valorInicial++;
				}else{
					sem->valorInicial++;

					if(!list_is_empty(sem->colaDeBloqueo)){

						_Bool buscar_tcb_por_pid(tcb* tcb_lista){
							return tcb_lista->p_id == pid;
						}

						tcb* ultParaMover = list_find(sem->colaDeBloqueo, (void*)buscar_tcb_por_pid);

						if(ultParaMover != NULL){

							_Bool buscar_tcb(tcb* _tcb){
								return _tcb->t_id == ultParaMover->t_id && _tcb->p_id == ultParaMover->p_id;
							}
							sem_wait(mut_blocked);
								list_remove_by_condition(estadoBlocked, buscar_tcb);
							sem_post(mut_blocked);
							list_add(colaDeReady, ultParaMover);

						}
					}
				}

				break;
		}
	}
}

int aceptarConexion(int servidor){

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);
	uint32_t cliente = accept(servidor, (void*)&direccionCliente, &tamanioDireccion);
	//perror("Error: ");
	log_info(logg, "Recibi una conexion de %d!!\n", cliente);

	send(cliente, "Hola!", 6, 0);

	return cliente;

}



//FUNCIONES PARA HACER LO QUE ME PIDA HILOLAY, O SEA, LA IMPLEMENTACION DE SUSE
pcb* crearPCB(int pid){//Esto es mas para gestion interna, no tiene mucho que ver con el planificador

	pcb* pcb = malloc(sizeof(pcb));
	pcb->ejecutando = false; //Me dice si tiene algun hilo ejecutando
	pcb->p_id = pid;
	pcb->ults = list_create();//Lista de ults de este proceso

	list_add(listaDeProgramas, pcb);
	return pcb;
}

tcb* crearTCB(uint32_t pid, uint32_t tid){//Esto tiene que devolver un int, pero como todavia nose que int lo dejo como un void

	tcb* tcb = malloc(sizeof(tcb));
	tcb->estimacion = 0; //De entrada siempre es 0
	tcb->p_id = pid;
	tcb->t_id = tid;
	tcb->estimacionAnterior = 0;
	tcb->realAnterior = 0;

	pcb* procesoPadre = buscarProcesoEnListaDeProcesos(pid);

	list_add(procesoPadre->ults, tcb);

	//pasa a cola de new
	list_add(estadoNew,tcb);

	return tcb;
}

pcb* buscarProcesoEnListaDeProcesos(int pid){
	_Bool buscar_pid(pcb* _pcb){
		return _pcb->p_id == pid;
	}
	return list_find(listaDeProgramas,(void*)buscar_pid);
}

void sacarDeNew(tcb* _tcb){
	_Bool buscar_tcb(tcb* tcb_lista){
		return tcb_lista->t_id == _tcb->t_id && tcb_lista->p_id == _tcb->p_id;
	}
	sem_wait(sem_mutNew);
	list_remove_by_condition(estadoNew,(void*)buscar_tcb);
	sem_post(sem_mutNew);
}

void sacarDeReady(tcb* _tcb,t_list* colaReady){
	_Bool buscar_tcb(tcb* tcb_lista){
		return tcb_lista->t_id == _tcb->t_id && tcb_lista->p_id == _tcb->p_id;
	}
	list_remove_by_condition(colaReady,(void*)buscar_tcb);
}

void actualizarEstimacion(tcb* _tcb,uint64_t tiempoSalida){
	//alfa*raf ant + (1-alfa)*est ant
	_tcb->realAnterior = tiempoSalida -_tcb->tiempoDeInicio;//en milisegundos
	_tcb->estimacionAnterior = _tcb->estimacion;
	_tcb->estimacion = configuracion->ALPHA_SJF * _tcb->realAnterior +
			(1-configuracion->ALPHA_SJF) * _tcb->estimacionAnterior;
}

uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long )tv.tv_sec) * 1000 + ((unsigned long) tv.tv_usec) / 1000);
	uint64_t a = result;
	return a;
}

//OTRAS FUNCIONES
suse_wait_t* crear_suse_wait(int tid, char* id_semaforo){
	suse_wait_t* swt = malloc(sizeof(suse_wait_t));
	swt->tid = tid;
	swt->size_id = strlen(id_semaforo)+1;
	swt->id_semaforo = string_duplicate(id_semaforo);
	return swt;
}

suse_wait_t* deserializar_suse_wait(void* magic){
	suse_wait_t* mmt = malloc(sizeof(suse_wait_t));
	uint32_t puntero = 0;
	memcpy(&mmt->tid,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mmt->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mmt->id_semaforo = malloc(mmt->size_id);
	memcpy(mmt->id_semaforo,magic+puntero,mmt->size_id);
	puntero+=mmt->size_id;
	return mmt;
}

void suse_wait_destroy(suse_wait_t* swt){
	free(swt->id_semaforo);
	free(swt);
}

suse_signal_t* crear_suse_signal(int tid, char* id_semaforo){
	suse_signal_t* sst = malloc(sizeof(suse_signal_t));
	sst->tid = tid;
	sst->size_id = strlen(id_semaforo)+1;
	sst->id_semaforo = string_duplicate(id_semaforo);
	return sst;
}

void* serializar_suse_signal(suse_signal_t* swt){
	int bytes = sizeof(uint32_t)*2+ swt->size_id + sizeof(uint32_t)*2;
	//2 int de adentro de swt y 2 int de comando y tamanio
	int comando = SIGNAL;
	int puntero = 0;
	void* magic = malloc(bytes);
	memcpy(magic+puntero,&comando,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&bytes,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&swt->tid,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,&swt->size_id,sizeof(uint32_t));
	puntero += sizeof(uint32_t);
	memcpy(magic+puntero,swt->id_semaforo,swt->size_id);
	puntero += swt->size_id;
	return magic;
}

suse_signal_t* deserializar_suse_signal(void* magic){
	suse_signal_t* mmt = malloc(sizeof(suse_signal_t));
	uint32_t puntero = 0;
	memcpy(&mmt->tid,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	memcpy(&mmt->size_id,magic+puntero,sizeof(uint32_t));
	puntero+=sizeof(uint32_t);
	mmt->id_semaforo = malloc(mmt->size_id);
	memcpy(mmt->id_semaforo,magic+puntero,mmt->size_id);
	puntero+=mmt->size_id;
	return mmt;
}

void suse_signal_destroy(suse_signal_t* swt){
	free(swt->id_semaforo);
	free(swt);
}

