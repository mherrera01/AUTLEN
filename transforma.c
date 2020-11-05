/** 
 * @brief Transforma un AFND en AFD
 * 
 * @file transforma.c
 * @author AUTLEN Grupo 9, José Manuel Freire and Miguel Herrera 
**/

#include "transforma.h"

#define debug 0 /* Debug is 1 for terminal messages during execution */

struct _Transicion {
    int simbolo;
    int transita_estado; // Estado al que transita con un simbolo de entrada
};

struct _AFD {
    int estados[10][20];
    Transicion transiciones[10][20];
    int compone_estado[10]; // # Estados del afnd que componen un nuevo estado
    int compone_transicion[10]; // # Transiciones del afndl que componen un nuevo estado
    int num_estados; // Estados nuevos del afd
};

void print_AFD(AFND* afnd, AFD afd){
    Transicion tran;
    int i, j;

    /* Printeamos información relacionada con los estados */
    printf("Número de estados nuevos: %d\n", afd.num_estados);
    for(i = 0; i < afd.num_estados; i++){
        printf("Número de estados que componen A%d: %d\n", i, afd.compone_estado[i]);
        for(j = 0; j < afd.compone_estado[i]; j++){
            printf("\tEstado: A%d está compuesto por %s\n", i, AFNDNombreEstadoEn(afnd, afd.estados[i][j]));
        }
    }

    /* Printeamos información relacionada con las transiciones */
    for(i = 0; i < afd.num_estados; i++){
        printf("\nNúmero de transiciones que componen A%d: %d\n", i, afd.compone_transicion[i]);
        for(j = 0; j < afd.compone_transicion[i]; j++){
            tran = afd.transiciones[i][j];
            printf("\tTransición de A%d: Con %s llegamos a %s\n", i,
                   AFNDSimboloEn(afnd, tran.simbolo), AFNDNombreEstadoEn(afnd, tran.transita_estado));
        }
    }
    
    return;
}

AFND * quitarLambda(AFND* afndl){
    AFD afd;
    Transicion tran;
    int num_estados, num_simbolos;
    int estado_inicial;

    /* Contadores de loops */
    int i, j, x, z, k, h;

    /* Control de errores */
    if(afndl == NULL){
        return NULL;
    }

    num_estados = AFNDNumEstados(afndl);
    num_simbolos = AFNDNumSimbolos(afndl);

    /* Cierres Lambda */
    int cierres[num_estados][num_estados];

    /* Posición en el afndl del estado inicial */
    estado_inicial = AFNDIndiceEstadoInicial(afndl);

    /* Calculamos el cierre del estado inicial */
    afd.num_estados = 0;
    afd.compone_estado[0] = 0;
    for (i=0; i < num_estados; i++){
        if(AFNDCierreLTransicionIJ(afndl, estado_inicial, i)){
            /* Insertamos los estados del cierre lambda al nuevo estado */
            afd.estados[afd.num_estados][afd.compone_estado[0]] = i;
            afd.compone_estado[0]++;
        }
    }
    afd.compone_transicion[0] = 0;

    /* Hemos insertado el cierre del estado inicial (nuevo estado) */
    afd.num_estados++;

    afd.compone_transicion[0] = 0;
    for (x=0; x < num_simbolos; x++){
        for(i=0; i < afd.compone_estado[0]; i++){
            for (z=0; z < num_estados; z++){
                if (AFNDTransicionIndicesEstadoiSimboloEstadof(afndl, afd.estados[0][i], x, z)){
                    tran.simbolo = x;
                    tran.transita_estado = z;

                    afd.transiciones[0][afd.compone_transicion[0]] = tran;
                    afd.compone_transicion[0]++;
                }
            }
        }

        /* Comprobar si ya existe el nuevo estado */
        int contador = 0;

        for (k=0; k < afd.compone_estado[0]; k++){
            for (h=0; h < afd.compone_transicion[0]; h++){
                printf("K:%d H:%d ", k, h);
                if (afd.transiciones[0][h].simbolo == x){
                    printf ("Estado: %d, Transición: %d\n", afd.estados[0][k], afd.transiciones[0][h].transita_estado);
                    if (afd.estados[0][k] == afd.transiciones[0][h].transita_estado){
                        printf("Estado y transición iguales con Símbolo: %d\n", afd.transiciones[0][h].simbolo);
                        contador++;
                    }
                }

            }
        }
        printf("FIN DEL BUCLE.\n");

        /* El estado ya existe */
        if (contador == afd.compone_estado[0]){
            break;
        } else {
            /* No existe, incluir estado nuevo */
            printf("No existe el estado. Lo creamos\n");
            afd.compone_estado[afd.num_estados] = 0;

            for (k=0; k < afd.compone_transicion[0]; k++) {
                if (afd.transiciones[0][k].simbolo == x){
                    afd.estados[afd.num_estados][afd.compone_estado[afd.num_estados]] = afd.transiciones[0][k].transita_estado;
                    afd.compone_transicion[afd.num_estados] = 0;
                    afd.compone_estado[afd.num_estados]++;
                }
            }

            afd.num_estados++;
        }
    }

    print_AFD(afndl, afd);

    /* Obtenemos los cierres lambda de los estados del afndl */
    for (i=0; i < num_estados; i++){
        for (j=0; j < num_estados; j++){
            
            /* Comprobamos si se puede transitar con lambda entre los estados i y j */
            if(AFNDCierreLTransicionIJ(afndl, i, j)){
                cierres[i][j] = 1;

                /* Vemos si se puede transitar con los símbolos de entrada desde el cierre 
                a otros estados */
                if (debug) {
                    for (z=0; z < num_estados; z++){
                        for (x=0; x < num_simbolos; x++){
                            if (AFNDTransicionIndicesEstadoiSimboloEstadof(afndl, j, x, z)){
                                printf("Con %s llegamos de q%d a q%d\n", AFNDSimboloEn(afndl, x), j, z);
                            }
                        }
                    }
                }

            } else {
                cierres[i][j] = 0;
            }
        }
    }

    /*
    AFND* afnd = AFNDNuevo("AFND sin Lambdas", num_estados, num_simbolos);
    */
    return NULL;
}

AFND * AFNDTransforma(AFND * afnd){
    /* Control de errores */
    if (afnd == NULL){
        printf("Error al transformar el afnd dado.\n");
        return NULL;
    }
    
    /* Quitamos las lambdas al afnd dado */
    if (quitarLambda(afnd) == NULL){
        printf("No se pudo quitar las lambdas al afnd.\n");
        return NULL;
    }

    printf("%d\n", AFNDTransicionIndicesEstadoiSimboloEstadof(afnd, 0, 0, 1));
    return NULL;
}
