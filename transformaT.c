
#include "transformaT.h"
#include "int_list.h"
#include "state_list.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void clausuraLambda(AFND *afnd, IntList *subestados);
IntList *transicionarConSimbolo(AFND *afnd, IntList *subestados, int simbolo);
char *obtenerNombreAutomata(AFND *afnd);
char *obtenerNombreEstado(AFND *afnd, IntList *subestados);
int obtenerTipoEstado(AFND *afnd, IntList *subestados, int indice);
int esEstadoFinal(AFND *afnd, IntList *subestados);

AFND *AFNDTransforma(AFND *afnd, int debug) {
    AFND *afd;
    IntList *subestados_iniciales, *subestados, *subestados_siguientes;
    IntList *destinos_transiciones, *simbolos_transiciones;
    StateList *estados;
    int n_estados_procesados, indice, tipo_estado, i, j;
    char *nombre_automata, *nombre_estado, *nombre_estado_origen, *nombre_estado_destino, *simbolo;

    if (afnd == NULL) return NULL;

    estados = StateListCreate();
    if (estados == NULL) return NULL;

    subestados_iniciales = IntListCreate();
    if (subestados_iniciales == NULL) {
        StateListFree(estados);
        return NULL;
    }

    /* Añadimos los estados iniciales que tienen lambda a la lista de estados */
    indice = AFNDIndiceEstadoInicial(afnd);
    IntListAdd(subestados_iniciales, indice);
    clausuraLambda(afnd, subestados_iniciales);

    StateListAdd(estados, subestados_iniciales);
    IntListFree(subestados_iniciales);

    /* A partir de los estados iniciales obtenidos, procesamos cada transición */
    /* Nota: Cuando no se añaden más estados, salimos del bucle ya que no podemos procesar más transiciones */
    for (n_estados_procesados = 0; n_estados_procesados < StateListSize(estados); n_estados_procesados++) {
        /* Obtenemos la lista de subestados de la lista estados en el indice n_estados_procesados */
        subestados = StateListGetSubstates(estados, n_estados_procesados);
        /* Obtenemos la lista de destinos */
        destinos_transiciones = StateListGetTransitionsTargets(estados, n_estados_procesados);
        /* Obtenemos la lista de símbolos */
        simbolos_transiciones = StateListGetTransitionsSymbols(estados, n_estados_procesados);

        if (debug) IntListPrint(subestados);

        /* Por cada símbolo del afnd */
        for (i = 0; i < AFNDNumSimbolos(afnd); i++) {
            /* Obtenemos los estados a los que transicionar con el simbolo i */
            subestados_siguientes = transicionarConSimbolo(afnd, subestados, i);
            if (IntListSize(subestados_siguientes) == 0) {
                IntListFree(subestados_siguientes);
                continue;
            }

            /* Si no están los estados a los que podemos transicionar con el simbolo i, los añadimos a estados*/
            if (!StateListContainsSubstates(estados, subestados_siguientes)) {
                StateListAdd(estados, subestados_siguientes);
            }

            /* Añadimos a la lista transitions_targets de estados, el indice de los estados siguientes */
            indice = StateListGetSubstatesIndex(estados, subestados_siguientes);
            IntListAdd(destinos_transiciones, indice);
            /* Añadimos a la lista transtions_symbols de estados, el indice del simbolo i */
            IntListAdd(simbolos_transiciones, i);

            IntListFree(subestados_siguientes);
        }
    }

    /* Creamos el autómata determinista */
    nombre_automata = obtenerNombreAutomata(afnd);
    afd = AFNDNuevo(nombre_automata, StateListSize(estados), AFNDNumSimbolos(afnd));
    free(nombre_automata);
    if (afd == NULL) {
        StateListFree(estados);
        return NULL;
    }
    
    for (i = 0; i < AFNDNumSimbolos(afnd); i++) {
        AFNDInsertaSimbolo(afd, AFNDSimboloEn(afnd, i));
    }

    /* Por cada estado en estados, vamos insertando cada estado en el afd con su nombre y tipo*/
    for (i = 0; i < StateListSize(estados); i++) {
        nombre_estado = obtenerNombreEstado(afnd, StateListGetSubstates(estados, i));
        tipo_estado = obtenerTipoEstado(afnd, StateListGetSubstates(estados, i), i);
        AFNDInsertaEstado(afd, nombre_estado, tipo_estado);
        free(nombre_estado);
    }

    /* Por cada estado en el afd, insertamos sus correspondientes transiciones */
    for (i = 0; i < StateListSize(estados); i++) {
        destinos_transiciones = StateListGetTransitionsTargets(estados, i);
        simbolos_transiciones = StateListGetTransitionsSymbols(estados, i);
        for (j = 0; j < IntListSize(StateListGetTransitionsTargets(estados, i)); j++) {
            nombre_estado_origen = AFNDNombreEstadoEn(afd, i);
            simbolo = AFNDSimboloEn(afd, IntListGet(simbolos_transiciones, j));
            nombre_estado_destino = AFNDNombreEstadoEn(afd, IntListGet(destinos_transiciones, j));
            AFNDInsertaTransicion(afd, nombre_estado_origen, simbolo, nombre_estado_destino);
        }
    }

    StateListFree(estados);

    return afd;
}

void clausuraLambda(AFND *afnd, IntList *subestados) {
    int hay_lambda;
    int e, i, j;

    /* Por cada estado en afnd */
    for (i = 0; i < AFNDNumEstados(afnd); i++) {
        /* Si ya está pasamos */
        if (IntListContains(subestados, i)) continue;
        hay_lambda = 0;
        /* Por cada estado en subestados */
        for (j = 0; j < IntListSize(subestados); j++) {
            e = IntListGet(subestados, j);
            /* Miramos las transiciones lambda que hay */
            if (AFNDCierreLTransicionIJ(afnd, e, i)) {
                hay_lambda = 1;
                break;
            }
        }
        if (hay_lambda) IntListAdd(subestados, i);
    }

    IntListSort(subestados);
}

IntList *transicionarConSimbolo(AFND *afnd, IntList *subestados, int simbolo) {
    IntList *siguientes_subestados;
    int estado, i, j;

    /* Creamos la lista de subestados siguientes */
    siguientes_subestados = IntListCreate();
    if (siguientes_subestados == NULL) return NULL;

    /* Por cada estado en afnd */
    for (i = 0; i < AFNDNumEstados(afnd); i++) {
        if (IntListContains(siguientes_subestados, i)) continue;
        for (j = 0; j < IntListSize(subestados); j++) {
            estado = IntListGet(subestados, j);
            /* Miramos si hay transicion con el simbolo */
            if (AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, estado, simbolo, i)) {
                IntListAdd(siguientes_subestados, i);
                break;
            }
        }
    }

    /* Obtenemos las transiciones lambda de los subestados siguientes */
    clausuraLambda(afnd, siguientes_subestados);
    IntListSort(siguientes_subestados);
    return siguientes_subestados;
}

char *obtenerNombreAutomata(AFND *afnd) {
    FILE *file;
    char nombre_afnd[256], *nombre_afd;
    int tam;

    file = fopen("nombre_aux.txt", "w");
	AFNDImprime(file, afnd);
	fclose(file);

    file = fopen("nombre_aux.txt", "r");
	fscanf(file, "%s", nombre_afnd);
	fclose(file);

    tam = strlen(nombre_afnd);
    nombre_afnd[tam-2] = '\0';
    nombre_afd = (char *) malloc(sizeof(char)*tam+14);
    if (nombre_afd == NULL) return NULL;

    strcpy(nombre_afd, nombre_afnd);
    strcat(nombre_afd, "_determinista");

	return nombre_afd;
}

char *obtenerNombreEstado(AFND *afnd, IntList *subestados) {
    char *nombre_estado;
    int tam, i;

    tam = 0;
    for (i = 0; i < IntListSize(subestados); i++) {
        tam += strlen(AFNDNombreEstadoEn(afnd, IntListGet(subestados, i)));
    }

    nombre_estado = (char *) malloc(sizeof(char)*(tam+1));
    if (nombre_estado == NULL) return NULL;

    nombre_estado[0] = '\0';
    for (i = 0; i < IntListSize(subestados); i++) {
        strcat(nombre_estado, AFNDNombreEstadoEn(afnd, IntListGet(subestados, i)));
    }

    return nombre_estado;
}

int obtenerTipoEstado(AFND *afnd, IntList *subestados, int indice) {
    if (esEstadoFinal(afnd, subestados)) {
        if (indice == 0) return 2; /* Inicial y final */
        else return 1; /* Final */
    } else {
        if (indice == 0) return 0; /* Inicial */
        else return 3; /* Normal */
    }
}

int esEstadoFinal(AFND *afnd, IntList *subestados) {
    int tipo_estado, i;

    for (i = 0; i < IntListSize(subestados); i++) {
        tipo_estado = AFNDTipoEstadoEn(afnd, IntListGet(subestados, i));
        if (tipo_estado == 1 || tipo_estado == 2) return 1;
    }

    return 0;
}