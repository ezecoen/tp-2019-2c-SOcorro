#include "suse.h"

int main(int argc, char **argv) {
	char* path_de_config = string_duplicate(argv[1]);

	iniciar_log(path_de_config);

	inicializarOtrasListas();//Inicializo la lista de  programas ejecutando/a ejecutar

	leer_config(path_de_config);

	inicializarSemaforos();

	inicializarEstadosComunes();//Inicializo las listas de New - Block - Exit

	int servidor = crearServidor();

	while(1){
		escucharServidor(servidor);//Con esto suse se queda esperando conexiones
	}

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
		semaforo->id_semaforo = string_duplicate(aux_sem_ids[k]);
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
		sem->valorMaximo = atoi(aux_sem_max[j]);
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

	sem_init(&mut_new,0,1);
	sem_init(&sem_mutConfig,0,1);
	sem_init(&mut_multiprogramacion,0,configuracion->MAX_MULTIPROG);
	sem_init(&mut_listaDeSemaforos,0,1);
	sem_init(&mut_blocked,0,1);
	sem_init(&mut_exit,0,1);

}

void inicializarEstadosComunes(){
	estadoNew = list_create();
	estadoBlocked = list_create();
	estadoExit = list_create();
	multiprogramacion = configuracion->MAX_MULTIPROG;
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
	direccionServidor.sin_port = htons(configuracion->LISTEN_PORT);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);
	//Esto es para que cuando haga ctrl+c y vuelvo a levantar el sv, no tenga problemas con el bind
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEPORT, &activado, sizeof(activado));

	if(bind(servidor, (void*)&direccionServidor, sizeof(direccionServidor))!= 0){

		perror("Fallo la creacion del servidor en el bind");
		return 1;
	}

	log_info(logg, "Estoy escuchando en el puerto %d",configuracion->LISTEN_PORT);
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
	tcb* exec = NULL;
	int resultado;
	uint32_t tamanioPaquete;
	while(recv(cliente,&operacion,4,MSG_WAITALL)>0){
		switch(operacion){
			case INIT:;//recive el pid
				//se hace 1 vez por proceso
				recv(cliente,&pid,4,0);
				crearPCB(pid);
				colaDeReady = list_create();
				break;
			case CREATE:;
				recv(cliente,&tid,4,0);
				log_info(logg, "Recibi un create del cliente %d", cliente);
				tcb* _tcb = crearTCB(pid,tid);
				if(multiprogramacion>0){
					multiprogramacion--;
					//pongo en ready y saco de new
					list_add(colaDeReady,_tcb);
					//me fijo si no hay nadie en exec, si esta vacia, me meto
					sacarDeNew(_tcb);
					if(exec == NULL){
						exec = _tcb;
						sacarDeReady(_tcb,colaDeReady);
					}
				}

				resultado = 0;

				send(cliente,&resultado,4,0);

				break;
			case SCHEDULE_NEXT:;
				_Bool ordenamientoSjf(tcb* tcb1,tcb* tcb2){
					return tcb1->estimacion < tcb2->estimacion;
				}
				log_info(logg, "Recibi un schedule_next del cliente %d", cliente);
				//si no hay nadie en ready, sigue ejecutando
				if(list_is_empty(colaDeReady)){
					if(exec != NULL){
						send(cliente,&exec->t_id,4,0);
						actualizarEstimacion(exec,timestamp());
						break;
					}else{
						int mensaje = -1;
						send(cliente,&mensaje,4,0);
						break;
					}
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
				int i = tcbAEjecutar->t_id;
				send(cliente,&i,4,0);
				//el q estaba en exec lo devuelvo a ready
				if(tcbAux != NULL){
					list_add(colaDeReady,tcbAux);
					actualizarEstimacion(tcbAux,tiempoAux);
				}
				//se actualiza el estimador del que salio de exec

				break;
			case JOIN:;
				int tid_para_join;
				_Bool buscarTid(tcb* _tcb){
					return _tcb->t_id == tid_para_join;
				}

				log_info(logg, "Recibi un join del cliente %d", cliente);

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
				list_add(tcb_para_join->tidsEsperando,(void*)exec->t_id);
				//bloqueo al tid actual
				list_add(estadoBlocked,exec);
				//actualizo la estimacion
				actualizarEstimacion(exec,timestamp());
				exec = NULL;
				int resultado = 0;
				send(cliente,&resultado,4,0);

				break;
			case CLOSE:;
				int tid_para_close;
				log_info(logg, "Recibi un close del cliente %d", cliente);
				recv(cliente,&tid_para_close,4,0);

				_Bool buscarTCB(tcb* _tcb){
					return _tcb->p_id == pid && _tcb->t_id == tid_para_close;
				}

				if(exec->t_id == tid_para_close){
					sem_wait(&mut_exit);
						list_add(estadoExit, exec);
					sem_post(&mut_exit);

					if(!list_is_empty(exec->tidsEsperando)){
						void moverTcbAReady(int tid){

							_Bool buscarTCBporTID(tcb* __tcb){
								return __tcb->p_id == pid && __tcb->t_id == tid;
							}
							sem_wait(&mut_blocked);
								tcb* tcbAReady = list_remove_by_condition(estadoBlocked, (void*)buscarTCBporTID);
							sem_post(&mut_blocked);
							list_add(colaDeReady, tcbAReady);
						}
						list_iterate(exec->tidsEsperando, (void*)moverTcbAReady);
						list_clean(exec->tidsEsperando);
					}
					exec = NULL;
					multiprogramacion++;
				}

				resultado = 0;

				send(cliente,&resultado,4,0);

				break;
			case WAIT:;

				log_info(logg, "Recibi un wait del cliente %d", cliente);

				recv(cliente, &tamanioPaquete, 4, MSG_WAITALL);
				tamanioPaquete -= 8;
				void* paquete = malloc(tamanioPaquete);
				recv(cliente, paquete, tamanioPaquete,MSG_WAITALL);
				suse_wait_t* wait = deserializar_suse_wait(paquete);
				log_info(logg,"Semaforo %s",wait->id_semaforo);
				_Bool buscarSemaforoPorId(semaforo_t* sem){
					return strcmp(sem->id_semaforo, wait->id_semaforo) == 0;
				}

				semaforo_t* semWait = list_find(listaDeSemaforos, (void*)buscarSemaforoPorId);

				if(semWait->valorInicial <= 0){
					sem_wait(&mut_blocked);
						list_add(estadoBlocked, exec);
					sem_post(&mut_blocked);
					list_add(semWait->colaDeBloqueo, exec);
					semWait->valorInicial--;
					//aca tengo que llamar a actualizarEstimacion(tcb, timestamp);
					exec = NULL;
				}else{
					semWait->valorInicial--;
				}

				resultado = 0;
				send(cliente,&resultado,4,0);
				break;

			case SIGNAL:

				log_info(logg, "Recibi un signal del cliente %d", cliente);

				recv(cliente, &tamanioPaquete, 4, MSG_WAITALL);
				tamanioPaquete -= 8;
				void* paqueteSignal = malloc(tamanioPaquete);
				recv(cliente, paqueteSignal, tamanioPaquete,MSG_WAITALL);
				suse_signal_t* signal = deserializar_suse_signal(paqueteSignal);

				_Bool buscarSemaforoPorIdSignal(semaforo_t* sem){
					return strcmp(sem->id_semaforo, signal->id_semaforo) == 0;
				}

				semaforo_t* semSignal = list_find(listaDeSemaforos, (void*)buscarSemaforoPorIdSignal);

				if(semSignal->valorInicial > 0){
					if(semSignal->valorInicial < semSignal->valorMaximo){
						semSignal->valorInicial++;
					}
				}else{
					if(semSignal->valorInicial < semSignal->valorMaximo){
						semSignal->valorInicial++;
					}
					if(!list_is_empty(semSignal->colaDeBloqueo)){

						_Bool buscar_tcb_por_pid(tcb* tcb_lista){
							return tcb_lista->p_id == pid;
						};

						tcb* ultParaMover = list_find(semSignal->colaDeBloqueo, (void*)buscar_tcb_por_pid);

						if(ultParaMover != NULL){

							_Bool buscar_tcb(tcb* _tcb){
								return _tcb->t_id == ultParaMover->t_id && _tcb->p_id == ultParaMover->p_id;
							}
							sem_wait(&mut_blocked);
								list_remove_by_condition(estadoBlocked,(void*)buscar_tcb);
							sem_post(&mut_blocked);
							list_add(colaDeReady, ultParaMover);

						}
					}
				}
				int res = 0;
				send(cliente,&res,4,0);
				break;
		}
	}
	close(cliente);
}

int aceptarConexion(int servidor){

	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);
	uint32_t cliente = accept(servidor, (void*)&direccionCliente, &tamanioDireccion);
	//perror("Error: ");
	log_info(logg, "Recibi una conexion de %d!!\n", cliente);

	//send(cliente, "Hola!", 6, 0);

	return cliente;

}



//FUNCIONES PARA HACER LO QUE ME PIDA HILOLAY, O SEA, LA IMPLEMENTACION DE SUSE
pcb* crearPCB(int pid){//Esto es mas para gestion interna, no tiene mucho que ver con el planificador

	pcb* _pcb = malloc(sizeof(pcb));
	_pcb->ejecutando = false; //Me dice si tiene algun hilo ejecutando
	_pcb->p_id = pid;
	_pcb->ults = list_create();//Lista de ults de este proceso

	list_add(listaDeProgramas, _pcb);
	return _pcb;
}

tcb* crearTCB(uint32_t pid, uint32_t tid){//Esto tiene que devolver un int, pero como todavia nose que int lo dejo como un void

	tcb* _tcb = malloc(sizeof(tcb));
	_tcb->estimacion = 0; //De entrada siempre es 0
	_tcb->p_id = pid;
	_tcb->t_id = tid;
	_tcb->estimacionAnterior = 0;
	_tcb->realAnterior = 0;
	_tcb->tidsEsperando = list_create();

	pcb* procesoPadre = buscarProcesoEnListaDeProcesos(pid);

	list_add(procesoPadre->ults, _tcb);

	//pasa a cola de new
	list_add(estadoNew,_tcb);

	return _tcb;
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
	sem_wait(&mut_new);
		list_remove_by_condition(estadoNew,(void*)buscar_tcb);
	sem_post(&mut_new);
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

