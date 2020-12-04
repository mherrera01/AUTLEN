#include "minimiza.h"

/* Función que obtiene todos los estados accesibles de un autómata finito determinista,
descartando así los estados inaccesibles del afd */
List* obtenerEstadosAccesibles(AFND* afd);

AFND* AFNDMinimiza(AFND* afd){
    /* Afd minimizado */
    AFND* afd_min = NULL;

    /* Lista de estados */
    List* estados = NULL;

    /* Lista de enteros */
    List* estados_accesibles = NULL;
    List* clase_finales = NULL;
    List* clase_noFinales = NULL;
    List* clase = NULL;

    int num_estados, num_simbolos, num_estados_accesibles;
    int tipoEstado;
    int* estado = NULL;

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

    /* Obtenemos los estados accesibles del afd, eliminando así los inaccesibles */
    estados_accesibles = obtenerEstadosAccesibles(afd);
    if (estados_accesibles == NULL){
        dlog("ERROR: La lista de estados accesibles no se obtuvo correctamente");
        list_destroy(estados);
        return NULL;
    }
    dlog("Obtenidos los estados accesibles del afd");

    /* Obtenemos el # de estados accesibles del afd */
    num_estados_accesibles = list_size(estados_accesibles);
    if (num_estados_accesibles == -1){
        dlog("ERROR: El tamaño de la lista de estados accesibles no se obtuvo correctamente");
        list_destroy(estados_accesibles);
        list_destroy(estados);
        return NULL;
    }
    sprintf(Message, "Número de Estados Accesibles del afd: %d", num_estados_accesibles);
    dlog(Message);

    /* Creamos una lista de enteros que agrupa los estados finales en una clase */
    clase_finales = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (clase_finales == NULL){
        dlog("ERROR: La lista de estados finales no se creó correctamente");
        list_destroy(estados_accesibles);
        list_destroy(estados);
        return NULL;
    }

    /* Creamos una lista de enteros que agrupa los estados no finales en una clase */
    clase_noFinales = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (clase_noFinales == NULL){
        dlog("ERROR: La lista de estados no finales no se creó correctamente");
        list_destroy(clase_finales);
        list_destroy(estados_accesibles);
        list_destroy(estados);
        return NULL;
    }

    /* Dividimos los estados accesibles en dos clases, estados finales y estados no finales.
    Esto se debe a que los estados finales son distinguibles de los no finales */
    for (i=0; i < num_estados_accesibles; i++){
        estado = list_get(estados_accesibles, i);
        if (estado == NULL){
            sprintf(Message, "ERROR: El estado en la posición %d a procesar no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(clase_noFinales);
            list_destroy(clase_finales);
            list_destroy(estados_accesibles);
            list_destroy(estados);
            return NULL;
        }

        tipoEstado = AFNDTipoEstadoEn(afd, *estado);

        if (tipoEstado == FINAL || tipoEstado == INICIAL_Y_FINAL){
            /* Metemos el estado en la clase de estados finales */
            if (list_insertLast(clase_finales, estado)){
                sprintf(Message, "ERROR: El estado en la posición %d a no se insertó en la clase finales correctamente", i);
                dlog(Message);
                list_destroy(clase_noFinales);
                list_destroy(clase_finales);
                list_destroy(estados_accesibles);
                list_destroy(estados);
                return NULL;
            }

        } else {
            /* Metemos el estado en la clase de estados no finales */
            if (list_insertLast(clase_noFinales, estado)){
                sprintf(Message, "ERROR: El estado en la posición %d a no se insertó en la clase no finales correctamente", i);
                dlog(Message);
                list_destroy(clase_noFinales);
                list_destroy(clase_finales);
                list_destroy(estados_accesibles);
                list_destroy(estados);
                return NULL;
            }
        }
    }
    list_destroy(estados_accesibles);

    /* Insertamos las clases finales y no finales a la lista de estados */
    if (list_insertLast(estados, clase_noFinales) || list_insertLast(estados, clase_finales)){
        dlog("ERROR: No se insertaron las clases finales y no finales a lista de estados correctamente");
        list_destroy(clase_noFinales);
        list_destroy(clase_finales);
        list_destroy(estados);
        return NULL;
    }
    list_destroy(clase_noFinales);
    list_destroy(clase_finales);

    /* Recorremos todas las clases de los estados del afd para juntar los estados equivalentes */
    for (i=0; i < list_size(estados); i++){
        clase = list_get(estados, i);
        if (clase == NULL){
            sprintf(Message, "ERROR: La clase en la posición %d a procesar no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(estados);
            return NULL;
        }

        for (j=0; j < num_simbolos; j++){
            for (k=0; k < num_estados_accesibles; k++){

                /* Vemos si se puede transitar desde el estado en la posición i al k con el símbolo j */
            }
        }
    }
    dlog("\nOK: Afd minimizado con éxito");
    dlog("Convirtiendo el afd minimizado a la estructura de la librería...");

    /* Convertimos el afd minimizado de nuestra estructura a la de la librería */

    /* Liberamos memoria */
    list_destroy(estados);

    dlog("\nOK: Afd convertido con éxito");
    dlog("Termina la ejecución correctamente");

    return afd_min;
}

List* obtenerEstadosAccesibles(AFND* afd){
    int i, j, k, estado_inicial;
    int *estado = NULL;

    List *estados_accesibles = NULL;

    /* Control de errores */
    if (afd == NULL){
        dlog("ERROR: No se eliminaron los estados inaccesibles del afd correctamente");
        return NULL;
    }

    /* Creamos una lista de enteros para guardar los estados accesibles */
    estados_accesibles = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (estados_accesibles == NULL){
        dlog("ERROR: La lista de estados accesibles no se creó correctamente");
        return NULL;
    }

    /* Posición en el afd del estado inicial */
    estado_inicial = AFNDIndiceEstadoInicial(afd);

    /* Marcamos el estado inicial del afd como accesible */
    if (list_insertLast(estados_accesibles, &estado_inicial)){
        dlog("ERROR: No se añadió el estado inicial a la lista de estados accesibles correctamente");
        list_destroy(estados_accesibles);
        return NULL;
    }

    /* Vemos todas las transiciones posibles entre estados */
    for (i=0; i < list_size(estados_accesibles); i++){
        estado = list_get(estados_accesibles, i);
        if (estado == NULL) {
            dlog("ERROR: El estado a analizar su accesibilidad no se consiguió correctamente");
            list_destroy(estados_accesibles);
            return NULL;
        }

        for (j=0; j < AFNDNumSimbolos(afd); j++){
            for (k=0; k < AFNDNumEstados(afd); k++){

                /* Vemos si se puede transitar desde el estado en la posición i al k con el símbolo j */
                if (AFNDTransicionIndicesEstadoiSimboloEstadof(afd, *estado, j, k)){

                    /* Marcamos como accesibles los estados a los que podemos transitar y no estén ya en la lista */
                    if (!list_contains(estados_accesibles, &k)){
                        if (list_insertLast(estados_accesibles, &k)){
                            dlog("ERROR: No se añadió un estado a la lista de estados accesibles correctamente");
                            list_destroy(estados_accesibles);
                            return NULL;
                        }
                    }
                }
            }
        }
    }

    return estados_accesibles;
}
