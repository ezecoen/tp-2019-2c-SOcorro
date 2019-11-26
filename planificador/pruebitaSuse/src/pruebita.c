#include "pruebita.h"

//FUNCIONES QUE SE UTILIZAN ADENTRO DE LOS TESTS

void recursiva(int cant) {
    if(cant > 0) recursiva(cant - 1);
}

void *test1(void *arg) {
    int i, tid;

    for (i = 0; i < 5; i++) {
        tid = hilolay_get_tid();
        printf("Soy el ult %d mostrando el numero %d \n", tid, i);
        usleep(5000 * i * tid); /* Randomizes the sleep, so it gets larger after a few iterations */

        recursiva(i);

        // Round Robin will yield the CPU
        hilolay_yield();
    }

    return 0;
}

void *test2(void *arg) {
    int i, tid;

    for (i = 0; i < 3; i++) {
        tid = hilolay_get_tid();
        printf("Soy el ult %d mostrando el numero %d \n", tid, i);
        usleep(2000 * i * tid); /* Randomizes the sleep, so it gets larger after a few iterations */
        recursiva(i);
        hilolay_yield();
    }

    return 0;
}

void *test3(){
	int i, tid;

	for (i = 0; i < 3; i++) {
		tid = hilolay_get_tid();
		printf("Soy el ult %d mostrando el numero %d \n", tid, i);
		usleep(2000 * i * tid); /* Randomizes the sleep, so it gets larger after a few iterations */
		recursiva(i);
		hilolay_yield();
	}

	return 0;
}





//TESTS PREARMADOS, ESTOS SON LOS QUE SE CORREN

/*
 * Nombre: correrPrueba1
 * Descripcion: Con esto pruebo que el funcionamiento basico (sin semaforos) funcione bien y haga lo que tenga que hacer
 */
int correrPrueba1(){
	hilolay_init();
	struct hilolay_t th1;
	struct hilolay_t th2;

	hilolay_create(&th1, NULL, &test1, NULL);
	hilolay_create(&th2, NULL, &test2, NULL);

	hilolay_join(&th2);
	hilolay_join(&th1);

	printf("Termino la ejecucion de todos los hilos\n\n");

	return hilolay_return(0);

}

/*
 * Nombre: correrPrueba2
 * Descripcion: Con este test pruebo que lo del grado de multiprogramacion (3) funcione bien. Cuando se haga el create del thread 3 me tiene que decir que
 * se quedo en new, recien cuando termine algun thread tiene que ejecutar.
 */
int correrPrueba2(){

	hilolay_init();
	struct hilolay_t th1;
	struct hilolay_t th2;
	struct hilolay_t th3;

	hilolay_create(&th1, NULL, &test1, NULL);
	hilolay_create(&th2, NULL, &test2, NULL);
	hilolay_create(&th3, NULL, &test3, NULL);

	hilolay_join(&th2);
	hilolay_join(&th1);
	hilolay_join(&th3);

	printf("Termino la ejecucion de todos los hilos\n");

	return hilolay_return(0);

}

/*
 * Nombre: correrPrueba3
 * Descripcion: Con este test pruebo el funcionamiento de los semaforos y la posibilidad de que se produzca un Deadlock entre distintos procesos
 * Instruccion para que la prueba termine: correr pruebita en una sola ventana de la terminal (debuggeando o no suse)
 * Instruccion para que haya Deadlock:
 * 1 - Correr suse en modo debug
 * 2 - Abrir 2 ventanas en la terminal, en una poner a correr pruebita y en la otra todavia no
 * 3 - Apenas se printee esto :
	 * Recibi un wait del cliente [NRO_DE_CLIENTE]
	 * Le resto 1 al semaforo B
 * poner a correr pruebita en la 2da ventana de la terminal
 * 4 - Seguir debuggeando como siempre
 * 5 - En un momento va a aparecer el mensaje
	 *Le resto 1 al semaforo B y bloqueo el thread 0 del proceso [NRO_DE_PROCESO]
 * La ventana 1 va a seguir ejecutando mientras que la 2da ventana queda bloqueada en el wait con el mensaje
	 * Pido un wait para el tid 0
 * 6 - Seguir debuggeando, la primera ventana va a terminar, la 2da va a seguir con su ejecucion hasta que se haga el ultimo wait, va a aparece el mensaje:
 * Le resto 1 al semaforo A y bloqueo el thread 0 del proceso [NRO_DE_PROCESO]
 * 7 - La ventana 2 queda en Deadlock porque la ventana 1 hizo el wait de A antes, y como A es un mutex, cuando la 1ra ventana termino dejo su valor en 0
 */
int correrPrueba3(){

	hilolay_init();

	hilolay_sem_t* sem1 = hilolay_sem_open("A");
	hilolay_sem_t* sem2 = hilolay_sem_open("B");
	hilolay_sem_t* semNE = hilolay_sem_open("C");

	hilolay_signal(sem1);
	hilolay_wait(sem2);
	hilolay_wait(semNE);

	printf("Puedo printear esto\n");

	hilolay_signal(sem2);
	hilolay_wait(sem1);

	return hilolay_return(0);

}



//MAIN
/*
 * Para correr algun test solo descomentalo, se pueden correr N pruebas
 */
int main() {

//	correrPrueba1();

//	correrPrueba2();

	correrPrueba3();

	return 0;

}
