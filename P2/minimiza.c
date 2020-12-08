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

AFND* AFNDMinimiza(AFND* afd){
    /* Afd minimizado */
    AFND* afd_min = NULL;

    /* Lista de estados */
    List* clases = NULL;

    /* Lista de enteros */
    List* estados_accesibles = NULL;
    List* clase_finales = NULL;
    List* clase_noFinales = NULL;
    List* clase = NULL;
    List* nueva_clase = NULL;

    int num_estados, num_simbolos, num_estados_accesibles;
    int tipoEstado;
    int* estado = NULL, *aux_estado = NULL;
    int* simbolosDeClase = NULL;
    int** transiciones = NULL;

    /* Contadores de loop */
    int i, j, k, h;

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

    for(k = 0; k < 100; k++){

        /* Creamos una nueva clase para los estados diferenciables. */
        nueva_clase = list_ini(int_destroy, int_copy, int_print, int_compare);
        if (nueva_clase == NULL){
            sprintf(Message, "ERROR: La nueva clase para el estado %d no se creó correctamente", *estado);
            dlog(Message);
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
                            free(simbolosDeClase);
                            freeMatriz(afd, transiciones);
                            list_destroy(clases);
                            return NULL;
                        }

                        /* Borramos el estado de la clase en la que estaba */
                        aux_estado = list_extractElement(clase, j);
                        
                        if (aux_estado == NULL){
                            sprintf(Message, "ERROR: El estado %d no se borró de su antigua clase correctamente", *estado);
                            dlog(Message);
                            free(simbolosDeClase);
                            freeMatriz(afd, transiciones);
                            list_destroy(clases);
                            return NULL;
                        }

                        int_destroy(aux_estado);
                        /* j--; Esto no creo que sea correcto. ASK MIGUEL */

                        break;
                    }
                }
            }

            /* Añadimos la nueva clase a lista de clases */
            if (list_insertLast(clases, nueva_clase)){
                sprintf(Message, "ERROR: La nueva clase del estado %d no se insertó a la lista de clases correctamente", *estado);
                dlog(Message);
                free(simbolosDeClase);
                freeMatriz(afd, transiciones);
                list_destroy(clases);
                return NULL;
            }

            list_destroy(nueva_clase);
        }

    free(simbolosDeClase);

    dlog("\nOK: Afd minimizado con éxito");
    dlog("Convirtiendo el afd minimizado a la estructura de la librería...");

    /* Convertimos el afd minimizado de nuestra estructura a la de la librería */

    /* Liberamos memoria */
    freeMatriz(afd, transiciones);
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
