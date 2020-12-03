#include "minimiza.h"

/* Función que elimina todos los estados inaccesibles de un autómata finito determinista */
AFND* eliminarEstadosInaccesibles(AFND* afd);

AFND* AFNDMinimiza(AFND* afd){
    /* Afd minimizado */
    AFND* afd_min = NULL;

    /* Lista de estados */
    List* estados = NULL;

    int num_estados, num_simbolos;

    /* Contadores de loop */
    int i, j, k;

    /* Creamos un archivo txt con mensajes de la ejecución del minimiza */
    dlog_init();
    dlog("Comienza la ejecución...");
    char Message[150];

    /* Control de errores */
    if(afd == NULL){
        dlog("ERROR: El autómata inicial no está definido");
        return NULL;
    }

    /* Obtenemos el # de estados y símbolos del afd a minimizar */
    num_estados = AFNDNumEstados(afd);
    sprintf(Message, "Número de Estados del afd: %d", num_estados);
    dlog(Message);

    num_simbolos = AFNDNumSimbolos(afd);
    sprintf(Message, "Número de Símbolos del afd: %d", num_simbolos);
    dlog(Message);
    dlog("Minimizando el afd...");

    /* Creamos una lista para todos los estados nuevos del autómata */
    estados = list_ini(estado_destroy, estado_clone, estado_print, estado_compare);
    if (estados == NULL){
        dlog("ERROR: La lista de estados no se creó correctamente");
        return NULL;
    }

    /*
    for (i=0; i < num_estados; i++){
        for (j=0; j < num_simbolos; j++){
            for (k=0; k < num_estados; k++){
                if (AFNDTransicionIndicesEstadoiSimboloEstadof(afd, i, j, k)){
                    
                }
            }
        }
    }
    */

    /* Liberamos memoria */
    list_destroy(estados);

    dlog("Termina la ejecución correctamente");

    return afd_min;
}

AFND* eliminarEstadosInaccesibles(AFND* afd){
    return NULL;
}
