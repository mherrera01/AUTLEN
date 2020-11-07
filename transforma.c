#include "transforma.h"

int addCierreLambda(AFND* afnd, int estado, List* cierre);

AFND* AFNDTransforma(AFND* afnd){
    /* Lista de estados */
    List* estados = NULL;
    List* transiciones = NULL;

    /* Lista de enteros */
    List* cierre_inicial = NULL;
    List* compone_estado = NULL;
    List* compone_transicion = NULL;

    int num_estados, num_simbolos;
    int estado_inicial;
    int *transita_estado = NULL;

    /* Contadores de loop */
    int i, j, k, l;

    /* Creamos un archivo txt con mensajes de la ejecución del transforma */
    dlog_init();
    dlog("Comienza la ejecución.");
    char Message[100];

    /* Control de errores */
    if(afnd == NULL){
        dlog("ERROR: El autómata inicial no está definido");
        return NULL;
    }

    /* Obtenemos el # de estados y símbolos del afnd a transformar */
    num_estados = AFNDNumEstados(afnd);
    sprintf(Message, "Número de Estados: %d", num_estados);
    dlog(Message);

    num_simbolos = AFNDNumSimbolos(afnd);
    sprintf(Message, "Número de Símbolos: %d", num_simbolos);
    dlog(Message);

    /* Posición en el afnd del estado inicial */
    estado_inicial = AFNDIndiceEstadoInicial(afnd);

    /* Creamos una lista para todos los estados nuevos del autómata */
    estados = list_ini(estado_destroy, estado_clone, estado_print, estado_compare);
    if (estados == NULL){
        dlog("ERROR: La lista de estados no se creó correctamente");
        return NULL;
    }

    /* Creamos una lista de enteros para el cierre del estado inicial */
    cierre_inicial = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (cierre_inicial == NULL) {
        dlog("ERROR: La lista del cierre incial no se creó correctamente");
        list_destroy(estados);
        return NULL;
    }

    /* Obtenemos el cierre transitivo del estado inicial */
    if (addCierreLambda(afnd, estado_inicial, cierre_inicial)) {
        dlog("ERROR: Cierre Lambda incial");
        list_destroy(cierre_inicial);
        list_destroy(estados);
        return NULL;
    }

    list_print(stdout, cierre_inicial);

    /* Lo añadimos a la lista de estados nuevos */
    if (list_insertLast(estados, cierre_inicial)){
        dlog("ERROR: No se añadió el cierre de un estado a la lista correctamente");
        list_destroy(cierre_inicial);
        list_destroy(estados);
        return NULL;
    }
    list_destroy(cierre_inicial);

    list_print(stdout, estados);

    /* Recorremos la lista de estados para procesar sus transiciones y crear nuevos
    estados en caso de que no existan ya */
    for (i=0; i < list_size(estados); i++){
        compone_estado = list_get(estados, i);
        if (compone_estado == NULL){
            dlog("ERROR: La lista del cierre incial no se creó correctamente");
            list_destroy(estados);
            return NULL;
        }

        /* Creamos una lista de estados para las transiciones de compone_estado con todos los símbolos */
        transiciones = list_ini(estado_destroy, estado_clone, estado_print, estado_compare);
        if (transiciones == NULL){
            dlog("ERROR: La lista de transiciones no se creó correctamente");
            list_destroy(estados);
            return NULL;
        }

        /* Incluimos las transiciones con cada símbolo del estado nuevo */
        for (j=0; j < num_simbolos; j++){

            /* Creamos una lista de enteros para las transiciones con el símbolo j */
            compone_transicion = list_ini(int_destroy, int_copy, int_print, int_compare);
            if (compone_transicion == NULL){
                sprintf(Message, "ERROR: La lista de transiciones con el símbolo %s no se creó correctamente", AFNDSimboloEn(afnd, j));
                dlog(Message);
                list_destroy(transiciones);
                list_destroy(estados);
                return NULL;
            }

            for (k=0; k < num_estados; k++){
                for (l=0; l < list_size(compone_estado); l++){

                    /* Vemos si se puede transitar del estado k a l con el símbolo j */
                    transita_estado = list_get(compone_estado, l);

                    if (AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, k, j, *transita_estado)){
                        /* Añadimos a la lista de enteros el nuevo estado al que hemos transitado (incluyendo lambda) */
                        if (addCierreLambda(afnd, l, compone_transicion)){
                            sprintf(Message, "ERROR: Al crear el cierre lambda de q%d", l);
                            dlog(Message);
                            list_destroy(compone_transicion);
                            list_destroy(transiciones);
                            list_destroy(estados);
                            return NULL;
                        }
                    }

                }
            }

            /* Comprobar si no hay transición con el símbolo j */
            if (list_isEmpty(compone_transicion)) {
                list_destroy(compone_transicion);
                continue;
            }

            /* Comprobar si el estado al que transitamos con el símbolo j ya existe */
            if (!list_contains(estados, compone_transicion)){
                /* Añadimos el estado al que transitamos como nuevo estado */
                if (list_insertLast(estados, compone_transicion)){
                    dlog("ERROR: No se añadió el cierre de un estado a la lista correctamente");
                    list_destroy(compone_transicion);
                    list_destroy(transiciones);
                    list_destroy(estados);
                    return NULL;
                }
            }

            /* Añadimos la transición al estado que estamos procesando */
            if (list_insertLast(transiciones, compone_transicion)){
                dlog("ERROR: No se añadió la transición a la lista correctamente");
                list_destroy(compone_transicion);
                list_destroy(transiciones);
                list_destroy(estados);
                return NULL;
            }
            list_destroy(compone_transicion);
        }
    }

    list_print(stdout, estados);
    list_print(stdout, transiciones);

    /* Convertimos el afd transformado de una estructura a la de la librería */

    /* Liberamos memoria */
    list_destroy(transiciones);
    list_destroy(estados);

    return NULL;
}

/* Función que añade a una lista de enteros dada los estados del cierre transitivo de un estado */
int addCierreLambda(AFND* afnd, int estado, List* cierre){
    int i;

    /* Control de errores */
    if (afnd == NULL || cierre == NULL){
        dlog("ERROR: No se obtuvo el cierre lambda de un estado correctamente");
        return 1;
    }

    /* Obtenemos y añadimos las transiciones con lambda que se pueden hacer desde el estado */
    for (i=0; i < AFNDNumEstados(afnd); i++){
        if (AFNDCierreLTransicionIJ(afnd, estado, i)) {
            /* Hay transición lambda. Añadimos a la lista del cierre en caso de que no esté ya */
            if (!list_contains(cierre, &i)){
                printf("Transición a %d", i);
                if (list_insertLast(cierre, &i)) {
                    dlog("ERROR: No se añadió un estado a la lista del cierre correctamente");
                    return 1;
                }
            }
        }
    }

    return 0;
}
