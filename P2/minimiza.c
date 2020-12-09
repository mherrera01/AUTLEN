#include "minimiza.h"

/* Función que obtiene todos los estados accesibles de un autómata finito determinista,
descartando así los estados inaccesibles del afd */
List* obtenerEstadosAccesibles(AFND* afd);

/* Función que crea una matriz con todas las transiciones entre estados del afd */
int** matrizTransiciones(AFND* afd);
/* Función que libera la memoria de la matriz de transiciones */
void freeMatriz(AFND* afd, int** matriz);

/* Función que devuelve el índice de la clase a la que pertenece el estado dado */
int getClaseDeEstado(List* clases, int estado);

/* Función que compara dos listas de clases y devuelve 0 en caso de que sean iguales */
int compararListaClases(List* c1, List* c2);
/* Función que devuelve una copia de una lista de clases */
List* copiarListaClases(List* c1);

AFND* AFNDMinimiza(AFND* afd){
    /* Afd minimizado */
    AFND* afd_min = NULL;
    int afd_min_num_estados;
    char afd_nombre_estado[50];
    char afd_anhadir_nombre[50];
    char *afd_nombre_origen, *afd_nombre_destino, *afd_nombre_simbolo;
    int *afd_origen, *afd_destino, *afd_simbolo;

    /* Lista de estados */
    List* clases = NULL;
    List* clases_anteriores = NULL;

    /* Lista de enteros */
    List* estados_accesibles = NULL;
    List* clase_finales = NULL;
    List* clase_noFinales = NULL;
    List* clase = NULL;
    List* nueva_clase = NULL;
    List* antigua_clase = NULL;

    /* Lista de enteros correspondiente al manejo de las transiciones */
    List* transiciones_origen = NULL;
    List* transiciones_destino = NULL;
    List* transiciones_simbolo = NULL;

    int num_estados, num_simbolos, num_estados_accesibles;
    int tipoEstado, origen_pos, destino_pos;
    int* estado = NULL, *aux_estado = NULL;
    int* simbolosDeClase = NULL;
    int** transiciones = NULL;

    /* Contadores de loop */
    int i, j, h;

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

    /* Obtenemos los estados accesibles del afd, eliminando así los inaccesibles */
    estados_accesibles = obtenerEstadosAccesibles(afd);
    if (estados_accesibles == NULL){
        dlog("ERROR: La lista de estados accesibles no se obtuvo correctamente");
        return NULL;
    }
    dlog("Obtenidos los estados accesibles del afd");

    /* Obtenemos el # de estados accesibles del afd */
    num_estados_accesibles = list_size(estados_accesibles);
    if (num_estados_accesibles == -1){
        dlog("ERROR: El tamaño de la lista de estados accesibles no se obtuvo correctamente");
        list_destroy(estados_accesibles);
        return NULL;
    }
    sprintf(Message, "Número de Estados Accesibles del afd: %d", num_estados_accesibles);
    dlog(Message);

    /* Creamos una lista de clases para distinguir los estados del afd */
    clases = list_ini(estado_destroy, estado_clone, estado_print, estado_compare);
    if (clases == NULL){
        dlog("ERROR: La lista de clases no se creó correctamente");
        return NULL;
    }

    /* Creamos una lista de enteros que agrupa los estados finales en una clase */
    clase_finales = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (clase_finales == NULL){
        dlog("ERROR: La lista de estados finales no se creó correctamente");
        list_destroy(estados_accesibles);
        list_destroy(clases);
        return NULL;
    }

    /* Creamos una lista de enteros que agrupa los estados no finales en una clase */
    clase_noFinales = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (clase_noFinales == NULL){
        dlog("ERROR: La lista de estados no finales no se creó correctamente");
        list_destroy(clase_finales);
        list_destroy(estados_accesibles);
        list_destroy(clases);
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
            list_destroy(clases);
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
                list_destroy(clases);
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
                list_destroy(clases);
                return NULL;
            }
        }
    }
    list_destroy(estados_accesibles);

    /* Insertamos las clases finales y no finales a la lista de clases */
    if (list_insertLast(clases, clase_noFinales) || list_insertLast(clases, clase_finales)){
        dlog("ERROR: No se insertaron las clases finales y no finales a lista de clases correctamente");
        list_destroy(clase_noFinales);
        list_destroy(clase_finales);
        list_destroy(clases);
        return NULL;
    }
    list_destroy(clase_noFinales);
    list_destroy(clase_finales);

    /* Obtenemos la matriz con todas las transiciones entre estados del afd */
    transiciones = matrizTransiciones(afd);
    if (transiciones == NULL){
        dlog("ERROR: ");
        list_destroy(clases);
        return NULL;
    }

    /* Creamos un array de enteros con las clases a las que se puede transitar con los distintos símbolos */
    simbolosDeClase = (int*) malloc (num_simbolos*sizeof(int));
    if (simbolosDeClase == NULL){
        freeMatriz(afd, transiciones);
        list_destroy(clases);
        return NULL;
    }

    /* Repite el algoritmo hasta que las clases sean iguales y guardamos en una lista los nuevos estados */
    while(compararListaClases(clases, clases_anteriores) != 0){
        list_destroy(clases_anteriores);

        /* Guardamos el estado de la lista de clases para saber si cambia en esta iteración */
        clases_anteriores = copiarListaClases(clases);
        if (clases_anteriores == NULL){
            dlog("ERROR: La lista clases_anteriores para los estados diferenciables no se copió correctamente");
            free(simbolosDeClase);
            freeMatriz(afd, transiciones);
            list_destroy(clases);
            return NULL;
        }

        /* Creamos una nueva clase para los estados diferenciables */
        nueva_clase = list_ini(int_destroy, int_copy, int_print, int_compare);
        if (nueva_clase == NULL){
            dlog("ERROR: La nueva clase para los estados diferenciables no se creó correctamente");
            list_destroy(clases_anteriores);
            free(simbolosDeClase);
            freeMatriz(afd, transiciones);
            list_destroy(clases);
            return NULL;
        }

        /* Buscamos estados equivalentes y creamos clases con ellos */
        for(h = 0; h < list_size(clases); h++){
            /* Obtenemos la clase a procesar por el algoritmo */
            clase = list_get(clases, h);
            if (clase == NULL){
                sprintf(Message, "ERROR: La clase en la posición %d a procesar no se consiguió correctamente", h);
                dlog(Message);
                list_destroy(clases_anteriores);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            /* Obtenemos el primer estado de la clase, considerándolo el dueño de la misma */
            estado = list_get(clase, 0);
            if (estado == NULL){
                sprintf(Message, "ERROR: El primer estado a procesar en la clase %d no se consiguió correctamente", h);
                dlog(Message);
                list_destroy(clases_anteriores);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            /* Obtenemos las clases a las que puede transitar el primer estado de la clase */
            for(i = 0; i < num_simbolos; i++){
                simbolosDeClase[i] = getClaseDeEstado(clases, transiciones[*estado][i]);
            }

            for(j = 1; j < list_size(clase); j++){

                /* Obtenemos los estados de la clase */
                estado = list_get(clase, j);
                if (estado == NULL){
                    sprintf(Message, "ERROR: El estado en posición %d a procesar en la clase %d no se consiguió correctamente", j, h);
                    dlog(Message);
                    list_destroy(clases_anteriores);
                    free(simbolosDeClase);
                    freeMatriz(afd, transiciones);
                    list_destroy(clases);
                    return NULL;
                }

                for(i = 0; i < num_simbolos; i++){
                    if(simbolosDeClase[i] == getClaseDeEstado(clases, transiciones[*estado][i])){
                        /* Este elemento está bien en esta clase. No hacemos nada */
                        continue;
                    } else {
                        /* Añadimos el estado a la nueva clase */
                        if (list_insertLast(nueva_clase, estado)){
                            sprintf(Message, "ERROR: El estado %d no se insertó a la nueva clase correctamente", *estado);
                            dlog(Message);
                            list_destroy(clases_anteriores);
                            free(simbolosDeClase);
                            freeMatriz(afd, transiciones);
                            list_destroy(clases);
                            return NULL;
                        }

                        break;
                    }
                }
            }
        }

        /* Borramos los estados que ya han sido movidos a la nueva clase de la clase en la que estaban */
        for(j = 0; j<list_size(nueva_clase); j++){
            /* Obtenemos los estados que han sido movidos a la nueva clase */
            estado = list_get(nueva_clase, j);
            if (estado == NULL){
                sprintf(Message, "ERROR: El estado en posición %d a procesar en la nueva clase no se consiguió correctamente", j);
                dlog(Message);
                list_destroy(clases_anteriores);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            /* Obtenemos la clase de la que vamos a borrar cada estado que se ha cambiado de clase */
            antigua_clase = list_get(clases, getClaseDeEstado(clases, *estado));
            if (antigua_clase == NULL){
                sprintf(Message, "ERROR: La antigua clase del estado %d a procesar en la nueva clase no se consiguió correctamente", *estado);
                dlog(Message);
                list_destroy(clases_anteriores);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            /* Borramos el estado de la clase en la que estaba */
            aux_estado = list_extractElement(antigua_clase, list_getIndex(antigua_clase, estado));
            if (aux_estado == NULL){
                sprintf(Message, "ERROR: El estado %d no se borró de su antigua clase correctamente", *estado);
                dlog(Message);
                list_destroy(clases_anteriores);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }
            int_destroy(aux_estado);
        }

        /* Añadimos la nueva clase a lista de clases */
        if(list_size(nueva_clase) != 0){
            if (list_insertLast(clases, nueva_clase)){
                dlog("ERROR: La nueva clase no se insertó a la lista de clases correctamente");
                list_destroy(clases_anteriores);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }
        }

        list_destroy(nueva_clase);

    }

    /* Liberamos memoria */
    free(simbolosDeClase);
    list_destroy(clases_anteriores);

    /* Con los estados equivalentes obtenidos, hallamos sus transiciones */
    dlog("Estados equivalentes del afd obtenidos y agrupados");

    /* Creamos una lista de enteros para las transiciones origen */
    transiciones_origen = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (transiciones_origen == NULL){
        dlog("ERROR: La lista de las transiciones origen no se creó correctamente");
        list_destroy(clases);
        freeMatriz(afd, transiciones);
        return NULL;
    }

    /* Creamos una lista de enteros para las transiciones destino */
    transiciones_destino = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (transiciones_destino == NULL){
        dlog("ERROR: La lista de las transiciones destino no se creó correctamente");
        list_destroy(transiciones_origen);
        list_destroy(clases);
        freeMatriz(afd, transiciones);
        return NULL;
    }

    /* Creamos una lista de enteros para los símbolos de las transiciones */
    transiciones_simbolo = list_ini(int_destroy, int_copy, int_print, int_compare);
    if (transiciones_simbolo == NULL){
        dlog("ERROR: La lista de los símbolos de las transiciones no se creó correctamente");
        list_destroy(transiciones_origen);
        list_destroy(transiciones_destino);
        freeMatriz(afd, transiciones);
        list_destroy(clases);
        return NULL;
    }
    dlog("Listas creadas para las transiciones");

    /* Recorremos la lista de clases para procesar sus transiciones */
    for (i=0; i < list_size(clases); i++){
        clase = list_get(clases, i);
        if (clase == NULL){
            sprintf(Message, "ERROR: La clase en la posición %d a procesar no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(transiciones_origen);
            list_destroy(transiciones_destino);
            list_destroy(transiciones_simbolo);
            freeMatriz(afd, transiciones);
            list_destroy(clases);
            return NULL;
        }
        sprintf(Message, "\nProcesando transiciones de la clase en posición %d...", i);
        dlog(Message);

        /* Obtenemos el primer estado de la clase, considerándolo el dueño de la misma */
        estado = list_get(clase, 0);
        if (estado == NULL){
            sprintf(Message, "ERROR: El primer estado a procesar en la clase %d no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(transiciones_origen);
            list_destroy(transiciones_destino);
            list_destroy(transiciones_simbolo);
            freeMatriz(afd, transiciones);
            list_destroy(clases);
            return NULL;
        }

        /* Incluimos las transiciones con cada símbolo de la clase */
        for (j=0; j < num_simbolos; j++){

            /* Añadimos la transición a la clase que estamos procesando */

            /* Añadimos la transición origen que corresponde a la clase en la que estamos ahora */
            origen_pos = list_getIndex(clases, clase);
            if (list_insertLast(transiciones_origen, &origen_pos)){
                sprintf(Message, "ERROR: No se añadió la transición origen en el índice %d a la lista correctamente", origen_pos);
                dlog(Message);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            /* Añadimos la transición destino que corresponde a la clase a la que llegamos con el símbolo j */
            destino_pos = getClaseDeEstado(clases, transiciones[*estado][j]);
            if (list_insertLast(transiciones_destino, &destino_pos)){
                sprintf(Message, "ERROR: No se añadió la transición destino en el índice %d a la lista correctamente", destino_pos);
                dlog(Message);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            /* Añadimos el símbolo de la transición que corresponde al símbolo j */
            if (list_insertLast(transiciones_simbolo, &j)){
                sprintf(Message, "ERROR: No se añadió el símbolo %s de la transición a la lista correctamente", AFNDSimboloEn(afd, j));
                dlog(Message);
                list_destroy(transiciones_origen);
                list_destroy(transiciones_destino);
                list_destroy(transiciones_simbolo);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }
            sprintf(Message, "Transición con el símbolo %s añadida a la lista de transiciones", AFNDSimboloEn(afd, j));
            dlog(Message);
        }
        dlog("OK");
    }
    freeMatriz(afd, transiciones);

    dlog("\nOK: Afd minimizado con éxito");
    dlog("Convirtiendo el afd minimizado a la estructura de la librería...");

    /* Convertimos el afd minimizado de nuestra estructura a la de la librería */
    afd_min_num_estados = list_size(clases);
    if (afd_min_num_estados == -1){
        dlog("ERROR: No se obtuvo el número de estados del afd minimizado correctamente");
        list_destroy(transiciones_origen);
        list_destroy(transiciones_destino);
        list_destroy(transiciones_simbolo);
        list_destroy(clases);
        return NULL;
    }
    sprintf(Message, "Número de Estados del afd minimizado: %d", afd_min_num_estados);
    dlog(Message);

    /* Creamos el afd minimizado */
    afd_min = AFNDNuevo("afd_minimizado", afd_min_num_estados, num_simbolos);
    dlog("Afd minimizado creado");

    /* Insertamos los símbolos al afd minimizado (que son los mismos que los del afd sin minimizar) */
    for (i=0; i < num_simbolos; i++){
        AFNDInsertaSimbolo(afd_min, AFNDSimboloEn(afd, i));
    }
    dlog("Símbolos insertados al afd minimizado");
    dlog("Insertando los estados al afd minimizado...\n");

    /* Insertamos los estados al afd minimizado */
    for (i=0; i < afd_min_num_estados; i++){
        /* Obtenemos la clase por la que está compuesto el estado del afd minimizado en la posición i */
        clase = list_get(clases, i);
        if (clase == NULL){
            sprintf(Message, "ERROR: La clase en la posición %d no se consiguió correctamente", i);
            dlog(Message);
            list_destroy(transiciones_origen);
            list_destroy(transiciones_destino);
            list_destroy(transiciones_simbolo);
            list_destroy(clases);
            return NULL;
        }
        sprintf(Message, "Insertando la clase en posición %d...", i);
        dlog(Message);

        /* Obtenemos el tipo de estado y el nombre a insertar en el afd minimizado */
        afd_nombre_estado[0] = '\0';
        tipoEstado = NORMAL;
        for (j=0; j < list_size(clase); j++){
            estado = list_get(clase, j);

            /* Concatenación del nombre de los estados que componen la clase del afd minimizado */
            strcpy(afd_anhadir_nombre, AFNDNombreEstadoEn(afd, *estado));
            strcat(afd_nombre_estado, afd_anhadir_nombre);

            /* Obtenemos el tipo de estado */
            if (AFNDTipoEstadoEn(afd, *estado) == INICIAL_Y_FINAL){
                tipoEstado = INICIAL_Y_FINAL;
            } else if (AFNDTipoEstadoEn(afd, *estado) == FINAL){
                if (tipoEstado == INICIAL_Y_FINAL) continue;
                tipoEstado = FINAL;
            } else if (AFNDTipoEstadoEn(afd, *estado) == INICIAL){
                tipoEstado = INICIAL;
            }
        }
        sprintf(Message, "Nombre concatenado: %s", afd_nombre_estado);
        dlog(Message);
        sprintf(Message, "Tipo de estado obtenido: %d", tipoEstado);
        dlog(Message);

        /* Insertamos el estado al afd minimizado */
        AFNDInsertaEstado(afd_min, afd_nombre_estado, tipoEstado);
        dlog("OK\n");
    }
    dlog("OK: Estados insertados al afd minimizado con éxito");
    dlog("Insertando las transiciones al afd minimizado...\n");

    /* Insertamos las transiciones al afd minimizado */
    for(i=0; i < list_size(transiciones_origen); i++){
        sprintf(Message, "Insertando la transición en posición %d...", i);
        dlog(Message);

        /* Transición origen en la posición i */
        afd_origen = list_get(transiciones_origen, i);
        afd_nombre_origen = AFNDNombreEstadoEn(afd_min, *afd_origen);

        /* Transición destino en la posición i */
        afd_destino = list_get(transiciones_destino, i);
        afd_nombre_destino = AFNDNombreEstadoEn(afd_min, *afd_destino);

        /* Símbolo de la transición en la posición i */
        afd_simbolo = list_get(transiciones_simbolo, i);
        afd_nombre_simbolo = AFNDSimboloEn(afd_min, *afd_simbolo);

        sprintf(Message, "Transición origen: %s", afd_nombre_origen);
        dlog(Message);
        sprintf(Message, "Transición destino: %s", afd_nombre_destino);
        dlog(Message);
        sprintf(Message, "Símbolo de la transición: %s", afd_nombre_simbolo);
        dlog(Message);

        /* Insertamos la transición al afd minimizado */
        AFNDInsertaTransicion(afd_min, afd_nombre_origen, afd_nombre_simbolo, afd_nombre_destino);
        dlog("OK\n");
    }
    dlog("OK: Transiciones insertadas al afd con éxito");

    /* Liberamos memoria */
    list_destroy(transiciones_origen);
    list_destroy(transiciones_destino);
    list_destroy(transiciones_simbolo);
    list_destroy(clases);

    dlog("\nOK: Afd convertido con éxito");
    dlog("Termina la ejecución correctamente");

    return afd_min;
}

int** matrizTransiciones(AFND* afd){
    int **matriz = NULL;
    int i, j, k;

    /* Creamos una matriz de enteros para las transiciones del afd */
    matriz = (int**) malloc (AFNDNumEstados(afd) * sizeof(int*));
    if (matriz == NULL){
        dlog("ERROR: La matriz de transiciones del afd no se creó correctamente");
        return NULL;
    }

    /* Reservamos memoria para cada fila de la matriz */
    for(i = 0; i < AFNDNumEstados(afd); i++){
        matriz[i] = (int*) malloc (AFNDNumSimbolos(afd) * sizeof(int));
        if (matriz[i] == NULL){
            dlog("ERROR: Una transición del afd no se añadió correctamente a la matriz de transiciones");
            freeMatriz(afd, matriz);
            return NULL;
        }
    }

    /* Guardamos en cada casilla de la matriz a qué estado transitaría desde el estado i con el símbolo j */
    for (i = 0; i < AFNDNumEstados(afd); i++){
        for (j = 0; j < AFNDNumSimbolos(afd); j++){
            matriz[i][j] = -1;
            for (k = 0; k < AFNDNumEstados(afd); k++){
                if (AFNDTransicionIndicesEstadoiSimboloEstadof(afd, i, j, k)){
                    matriz[i][j] = k;
                }
            }
        }
    }

    return matriz;
}

void freeMatriz(AFND *afd, int **matriz){
    int i;

    if (matriz != NULL){
        for(i = 0; i < AFNDNumEstados(afd); i++){
            if (matriz[i] != NULL){
                /* Liberamos la memoria de la fila int* de la matriz */
                free(matriz[i]);
            }
        }
        
        /* Liberamos la memoria del int** matriz */
        free(matriz);
    }
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

int getClaseDeEstado(List* clases, int estado){
    int i;

    /* Itera por las clases buscando a qué clase pertenece el estado */
    for(i = 0; i<list_size(clases); i++){
        if(list_contains(list_get(clases, i), &estado)){
            return i;
        }
    }
    return -1;
}

int compararListaClases(List* c1, List* c2){
    int i;

    if(!c1 || !c2){
        return 2;
    }

    /* Comprobamos que el tamaño de las listas son iguales */
    if(list_size(c1) != list_size(c2)){
        return 1;
    }

    for (i=0; i < list_size(c1); i++){
        if(estado_compare(list_get(c1, i), list_get(c2, i)) != 0){
            return 1;
        }
    }

    return 0;
}

List* copiarListaClases(List* c1){
    List* nueva = NULL;
    int i;

    if(!c1){
        return NULL;
    }

    /* Creamos una nueva lista donde guardaremos la copia */
    nueva = list_ini(estado_destroy, estado_clone, estado_print, estado_compare);
    if (nueva == NULL){
        return NULL;
    }

    /* Copiamos la lista de clases dada */
    for (i=0; i < list_size(c1); i++){
        if(list_insertLast(nueva, list_get(c1, i))){
            return NULL;
        }
    }

    return nueva;
}
