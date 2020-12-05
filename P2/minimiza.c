#include "minimiza.h"

/* Función que obtiene todos los estados accesibles de un autómata finito determinista,
descartando así los estados inaccesibles del afd */
List* obtenerEstadosAccesibles(AFND* afd);

/* Función que crea una matriz con todas las transiciones entre estados del afd */
int** matrizTransiciones(AFND* afd);
/* Función que libera la memoria de la matriz de transiciones */
void freeMatriz(AFND* afd, int** matriz);

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
    int coincidencias;
    int* estado = NULL, *estado2 = NULL;
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

    /* Obtenemos la matriz con todas las transiciones entre estados del afd */
    transiciones = matrizTransiciones(afd);
    if (transiciones == NULL){
        dlog("ERROR: ");
        list_destroy(estados);
        return NULL;
    }

    /* Recorremos todas las clases de los estados del afd para juntar los estados equivalentes */
    for (i=0; i < list_size(estados); i++){
        clase = list_get(estados, i);
        if (clase == NULL){
            sprintf(Message, "ERROR: La clase en la posición %d a procesar no se consiguió correctamente", i);
            dlog(Message);
            freeMatriz(afd, transiciones);
            list_destroy(estados);
            return NULL;
        }

        /*
        A ->
        B -> A
        C -> A B
        D -> A B C
        */

        /* Comprobamos si los estados son equivalentes, viendo que desde cada clase se puede transitar a elementos de la misma clase */
        for (k=0; k < list_size(clase); k++){
            estado = list_get(clase, k);
            if (estado == NULL){
                sprintf(Message, "ERROR: El estado de la clase %d en la posición %d a procesar no se consiguió correctamente", i, k);
                dlog(Message);
                freeMatriz(afd, transiciones);
                list_destroy(estados);
                return NULL;
            }

            for (h = k-1; h >= 0; h--){
                estado2 = list_get(clase, h);
                if (estado2 == NULL){
                    sprintf(Message, "ERROR: El estado de la clase %d en la posición %d a procesar no se consiguió correctamente", i, h);
                    dlog(Message);
                    freeMatriz(afd, transiciones);
                    list_destroy(estados);
                    return NULL;
                }

                coincidencias = 0;
                for (j=0; j < num_simbolos; j++){
                    /*
                    [A B C D] - [E, F] [G, H]

                    A --> 0 B
                    B --> 0 C
                    */
                    if (transiciones[*estado][j] == transiciones[*estado2][j]){
                        coincidencias++;                        
                    }
                }

                /* Son la misma clase */
                if(num_simbolos == coincidencias);
            }
        }
    }

    dlog("\nOK: Afd minimizado con éxito");
    dlog("Convirtiendo el afd minimizado a la estructura de la librería...");

    /* Convertimos el afd minimizado de nuestra estructura a la de la librería */

    /* Liberamos memoria */
    freeMatriz(afd, transiciones);
    list_destroy(estados);

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
                    printf("%d transita a %d con %d. Como lo oyes.\n", i, matriz[i][j], j);
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
