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

	printf("Termino la ejecucion de todos los hilos\n\n");

	return hilolay_return(0);

}





//MAIN
/*
 * Para correr algun test solo descomentalo, se pueden correr N pruebas
 */
int main() {

	correrPrueba1();

//	correrPrueba2();

	return 0;

}
