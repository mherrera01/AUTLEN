#include "transforma.h"

/* Función que añade a una lista de enteros dada los estados del cierre transitivo de un estado */
int addCierreLambda(AFND* afnd, int estado, List* cierre);

AFND* AFNDTransforma(AFND* afnd){
    /* Afnd transformado (afd) */
    AFND* afnd_transformado = NULL;
    int afd_num_estados;
    int *estado = NULL;
    char afd_nombre_estado[50];
    char afd_anhadir_nombre[50];
    int afd_tipo_estado;
    char *afd_nombre_origen, *afd_nombre_destino, *afd_nombre_simbolo;
    int *afd_origen, *afd_destino, *afd_simbolo;

    /* Lista de estados */
    List* estados = NULL;

    /* Lista de enteros */
    List* cierre_inicial = NULL;
    List* compone_estado = NULL;
    List* compone_transicion = NULL;

    /* Lista de enteros correspondiente al manejo de las transiciones */
    List* transiciones_origen = NULL;
    List* transiciones_destino = NULL;
    List* transiciones_simbolo = NULL;

    int num_estados, num_simbolos;
    int estado_inicial, origen_pos, destino_pos;
    int *transita_estado = NULL;

    /* Contadores de loop */
    int i, j, k, l;

    /* Creamos un archivo txt con mensajes de la ejecución del transforma */
    dlog_init();
    dlog("Comienza la ejecución...");
    char Message[150];

    /* Control de errores */
    if(afnd == NULL){
        dlog("ERROR: El autómata inicial no está definido");
        return NULL;
    }

    /* Obtenemos el # de estados y símbolos del afnd a transformar */
    num_estados = AFNDNumEstados(afnd);
    sprintf(Message, "Número de Estados del afnd: %d", num_estados);
    dlog(Message);

    num_simbolos = AFNDNumSimbolos(afnd);
    sprintf(Message, "Número de Símbolos del afnd: %d", num_simbolos);
    dlog(Message);
    dlog("Transformando el afnd...");

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
    dlog("Obtenido cierre transitivo del estado inicial");

    /* Lo añadimos a la lista de estados nuevos */
    if (list_insertLast(estados, cierre_inicial)){
        dlog("ERROR: No se añadió el cierre de un estado a la lista correctamente");
        list_destroy(cierre_inicial);
        list_destroy(estados);
        return NULL;
    }
    list_destroy(cierre_inicial);
    dlog("Añadido como estado nuevo el cierre inicial");

    /* Creamos una lista de enteros para las transiciones origen */
    transiciones_origen = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (transiciones_origen == NULL){
        dlog("ERROR: La lista de las transiciones origen no se creó correctamente");
        list_destroy(estados);
        return NULL;
    }

    /* Creamos una lista de enteros para las transiciones destino */
    transiciones_destino = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (transiciones_destino == NULL){
        dlog("ERROR: La lista de las transiciones destino no se creó correctamente");
        list_destroy(transiciones_origen);
        list_destroy(estados);
        return NULL;
    }

    /* Creamos una lista de enteros para los símbolos de las transiciones */
    transiciones_simbolo = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (transiciones_simbolo == NULL){
        dlog("ERROR: La lista de los símbolos de las transiciones no se creó correctamente");
        list_destroy(transiciones_origen);
        list_destroy(transiciones_destino);
        list_destroy(estados);
        return NULL;
    }
    dlog("Listas creadas para las transiciones");

    /* Recorremos la lista de estados para procesar sus transiciones y crear nuevos
    estados en caso de que no existan ya */
    for (i=0; i < list_size(estados); i++){
        compone_estado = list_get(estados, i);
        if (compone_estado == NULL){
            sprintf(Message, "ERROR: El estado en la posición %d a procesar no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(transiciones_origen);
            list_destroy(transiciones_destino);
            list_destroy(transiciones_simbolo);
            list_destroy(estados);
            return NULL;
        }
        sprintf(Message, "\nProcesando transiciones del estado en posición %d...", i);
        dlog(Message);

        /* Incluimos las transiciones con cada símbolo del estado nuevo */
        for (j=0; j < num_simbolos; j++){

            /* Creamos una lista de enteros para las transiciones con el símbolo j */
            compone_transicion = list_ini(int_destroy, int_copy, int_print, int_compare);
            if (compone_transicion == NULL){
                sprintf(Message, "ERROR: La lista de transiciones con el símbolo %s no se creó correctamente", AFNDSimboloEn(afnd, j));
                dlog(Message);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                list_destroy(estados);
                return NULL;
            }

            for (k=0; k < num_estados; k++){
                for (l=0; l < list_size(compone_estado); l++){

                    /* Vemos si se puede transitar del estado k a l con el símbolo j */
                    transita_estado = list_get(compone_estado, l);

                    if (AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, *transita_estado, j, k)){
                        /* Añadimos a la lista de enteros el nuevo estado al que hemos transitado (incluyendo lambda) */
                        if (addCierreLambda(afnd, k, compone_transicion)){
                            sprintf(Message, "ERROR: Al crear el cierre lambda de %s", AFNDNombreEstadoEn(afnd, k));
                            dlog(Message);
                            list_destroy(compone_transicion);
                            list_destroy(transiciones_origen);
                            list_destroy(transiciones_destino);
                            list_destroy(transiciones_simbolo);
                            list_destroy(estados);
                            return NULL;
                        }
                    }
                }
            }
            sprintf(Message, "Transición con el símbolo %s procesada", AFNDSimboloEn(afnd, j));
            dlog(Message);

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
                    list_destroy(transiciones_origen);
                    list_destroy(transiciones_destino);
                    list_destroy(transiciones_simbolo);
                    list_destroy(estados);
                    return NULL;
                }
                sprintf(Message, "Añadida como estado nuevo la transición con el símbolo %s", AFNDSimboloEn(afnd, j));
                dlog(Message);
            }

            /* Añadimos la transición al estado que estamos procesando */

            /* Añadimos la transición origen que corresponde al estado en el que estamos ahora */
            origen_pos = list_getIndex(estados, compone_estado);
            if (list_insertLast(transiciones_origen, &origen_pos)){
                sprintf(Message, "ERROR: No se añadió la transición origen en el índice %d a la lista correctamente", origen_pos);
                dlog(Message);
                list_destroy(compone_transicion);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                list_destroy(estados);
                return NULL;
            }

            /* Añadimos la transición destino que corresponde al estado al que llegamos con el símbolo j */
            destino_pos = list_getIndex(estados, compone_transicion);
            if (list_insertLast(transiciones_destino, &destino_pos)){
                sprintf(Message, "ERROR: No se añadió la transición destino en el índice %d a la lista correctamente", destino_pos);
                dlog(Message);
                list_destroy(compone_transicion);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                list_destroy(estados);
                return NULL;
            }

            /* Añadimos el símbolo de la transición que corresponde al símbolo j */
            if (list_insertLast(transiciones_simbolo, &j)){
                sprintf(Message, "ERROR: No se añadió el símbolo %s de la transición a la lista correctamente", AFNDSimboloEn(afnd, j));
                dlog(Message);
                list_destroy(compone_transicion);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                list_destroy(estados);
                return NULL;
            }
            list_destroy(compone_transicion);
            sprintf(Message, "Transición con el símbolo %s añadida a la lista de transiciones", AFNDSimboloEn(afnd, j));
            dlog(Message);
        }
        dlog("OK");
    }
    dlog("\nOK: Afnd transformado con éxito");
    dlog("Convirtiendo el afnd transformado a la estructura de la librería...");

    /* Convertimos el afnd transformado (afd) de nuestra estructura a la de la librería */
    afd_num_estados = list_size(estados);
    if (afd_num_estados == -1){
        dlog("ERROR: No se obtuvo el número de estados del afd correctamente");
        list_destroy(transiciones_origen);
        list_destroy(transiciones_destino);
        list_destroy(transiciones_simbolo);
        list_destroy(estados);
        return NULL;
    }
    sprintf(Message, "Número de Estados del afd: %d", afd_num_estados);
    dlog(Message);

    /* Creamos el afd */
    afnd_transformado = AFNDNuevo("afnd_transformado", afd_num_estados, num_simbolos);
    dlog("Afd creado");

    /* Insertamos los símbolos al afd (que son los mismos que los del afnd) */
    for (i=0; i < num_simbolos; i++){
        AFNDInsertaSimbolo(afnd_transformado, AFNDSimboloEn(afnd, i));
    }
    dlog("Símbolos insertados al afd");
    dlog("Insertando los estados al afd...\n");

    /* Insertamos los estados al afd */
    for (i=0; i < afd_num_estados; i++){
        /* Obtenemos los estados por los que está compuesto el estado del afd en la posición i */
        compone_estado = list_get(estados, i);
        if (compone_estado == NULL){
            sprintf(Message, "ERROR: El estado en la posición %d no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(transiciones_origen);
            list_destroy(transiciones_destino);
            list_destroy(transiciones_simbolo);
            list_destroy(estados);
            return NULL;
        }
        sprintf(Message, "Insertando el estado en posición %d...", i);
        dlog(Message);

        /* Obtenemos el tipo de estado y el nombre a insertar en el afd */
        afd_nombre_estado[0] = '\0';
        afd_tipo_estado = NORMAL;
        for (j=0; j < list_size(compone_estado); j++){
            estado = list_get(compone_estado, j);

            /* Concatenación del nombre de los estados que componen el estado nuevo del afd */
            strcpy(afd_anhadir_nombre, AFNDNombreEstadoEn(afnd, *estado));
            strcat(afd_nombre_estado, afd_anhadir_nombre);

            /* Obtenemos el tipo de estado */
            if (AFNDTipoEstadoEn(afnd, *estado) == FINAL || AFNDTipoEstadoEn(afnd, *estado) == INICIAL_Y_FINAL){
                if (i == 0){
                    afd_tipo_estado = INICIAL_Y_FINAL;
                } else{
                    afd_tipo_estado = FINAL;
                }
            } else if (AFNDTipoEstadoEn(afnd, *estado) == INICIAL){
                if (i == 0){
                    afd_tipo_estado = INICIAL;
                }
            }
        }
        sprintf(Message, "Nombre concatenado: %s", afd_nombre_estado);
        dlog(Message);
        sprintf(Message, "Tipo de estado obtenido: %d", afd_tipo_estado);
        dlog(Message);

        /* Insertamos el estado al afd */
        AFNDInsertaEstado(afnd_transformado, afd_nombre_estado, afd_tipo_estado);
        dlog("OK\n");
    }
    dlog("OK: Estados insertados al afd con éxito");
    dlog("Insertando las transiciones al afd...\n");

    /* Insertamos las transiciones al afd */
    for(i=0; i < list_size(transiciones_origen); i++){
        sprintf(Message, "Insertando la transición en posición %d...", i);
        dlog(Message);

        /* Transición origen en la posición i */
        afd_origen = list_get(transiciones_origen, i);
        afd_nombre_origen = AFNDNombreEstadoEn(afnd_transformado, *afd_origen);

        /* Transición destino en la posición i */
        afd_destino = list_get(transiciones_destino, i);
        afd_nombre_destino = AFNDNombreEstadoEn(afnd_transformado, *afd_destino);

        /* Símbolo de la transición en la posición i */
        afd_simbolo = list_get(transiciones_simbolo, i);
        afd_nombre_simbolo = AFNDSimboloEn(afnd_transformado, *afd_simbolo);

        sprintf(Message, "Transición origen: %s", afd_nombre_origen);
        dlog(Message);
        sprintf(Message, "Transición destino: %s", afd_nombre_destino);
        dlog(Message);
        sprintf(Message, "Símbolo de la transición: %s", afd_nombre_simbolo);
        dlog(Message);

        /* Insertamos la transición al afd */
        AFNDInsertaTransicion(afnd_transformado, afd_nombre_origen, afd_nombre_simbolo, afd_nombre_destino);
        dlog("OK\n");
    }
    dlog("OK: Transiciones insertadas al afd con éxito");

    /* Liberamos memoria */
    list_destroy(transiciones_origen);
    list_destroy(transiciones_destino);
    list_destroy(transiciones_simbolo);
    list_destroy(estados);

    dlog("\nOK: Afd convertido con éxito");
    dlog("Termina la ejecución correctamente");

    return afnd_transformado;
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
            /* Hay transición lambda. Añadimos a la lista del cierre en caso de que no esté ya */
            if (!list_contains(cierre, &i)){
                if (list_insertLast(cierre, &i)) {
                    dlog("ERROR: No se añadió un estado a la lista del cierre correctamente");
                    return 1;
                }
            }
        }
    }

    return 0;
}
