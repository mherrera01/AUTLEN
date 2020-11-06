#include "transforma.h"

int addCierreLambda(AFND* afnd, int estado, List* cierre);

AFND* AFNDTransforma(AFND* afnd){
    List* estados = NULL;
    List* cierre_inicial = NULL;
    List* compone_estado = NULL;
    int num_estados, num_simbolos;
    int estado_inicial;

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
    estados = list_ini(estado_destroy, estado_copy, estado_print, estado_compare);
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

    /* Lo añadimos a la lista de estados nuevos */
    if (list_insertLast(estados, cierre_inicial)){
        dlog("ERROR: No se añadió el cierre de un estado a la lista correctamente");
        list_destroy(cierre_inicial);
        list_destroy(estados);
        return NULL;
    }
    list_destroy(cierre_inicial);

    /* Recorremos la lista de estados para procesar sus transiciones y crear nuevos
    estados en caso de que no existan ya */
    for (i=0; i < list_size(estados); i++){
        compone_estado = list_ini(int_destroy, int_copy, int_print, int_compare);
        if (compone_estado == NULL){
            dlog("ERROR: La lista del cierre incial no se creó correctamente");
            list_destroy(estados);
        }

        /* Incluimos las transiciones con cada símbolo del estado nuevo */
        for (j=0; j < num_simbolos; j++){
            for (k=0; k < num_estados; k++){
                for (l=0; l < list_size(list_get(estados, i)); l++){

                    /* Vemos si se puede transitar del estado k a l con el símbolo j */
                    if (AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, k, j, l)){
                        /* Añadimos a la lista de enteros el nuevo estado al que hemos transitado (incluyendo lambda) */
                        if (addCierreLambda(afnd, l, compone_estado)){
                            sprintf(Message, "ERROR: Al crear el cierre lambda de q%d", l);
                            dlog(Message);
                            list_destroy(compone_estado);
                            list_destroy(estados);
                        }
                    }

                }
            }

            /* Comprobar si el estado al que transitamos con el símbolo j ya existe */
        }

        list_InsertLast(estados, compone_estado);
        list_destroy(compone_estado);
    }

    /* Convertimos el afd transformado de una estructura a la de la librería */

    return NULL;
}

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
            /* Hay transición lambda. Añadimos a la lista del cierre en caso de que no esté ya*/
            if (!list_contains(cierre, i)){
                if (list_insertLast(cierre, i)) {
                    dlog("ERROR: No se añadió un estado a la lista del cierre correctamente");
                    return 1;
                }
            }
        }
    }

    return 0;
}
