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
    int max;

    /* Contadores de loops */
    int i, j, x, z, k, h, a;

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

    for (j=0; j < afd.num_estados; j++){
        print_AFD(afndl, afd);
        printf("\nNúmero de estados nuevos: %d | J:%d\n\n", afd.num_estados, j);
        printf("Compone estado: %d\n", afd.compone_estado[j]);

        /* Añadir las transiciones a los estados nuevos con cada símbolo */
        for (x=0; x < num_simbolos; x++){
            for(i=0; i < afd.compone_estado[j]; i++){
                for (z=0; z < num_estados; z++){
                    if (AFNDTransicionIndicesEstadoiSimboloEstadof(afndl, afd.estados[j][i], x, z)){
                        tran.simbolo = x;
                        tran.transita_estado = z;

                        afd.transiciones[j][afd.compone_transicion[j]] = tran;
                        afd.compone_transicion[j]++;
                    }
                }
            }

            /* Comprobar si ya existe el nuevo estado */
            int contador = 0;
            int num_tran_con_x = 0;

            for (a=0; a < afd.num_estados; a++) {
                for (k=0; k < afd.compone_estado[a]; k++){
                    for (h=0; h < afd.compone_transicion[j]; h++){
                        printf("A:%d K:%d H:%d ", a, k, h);
                        if (afd.transiciones[j][h].simbolo == x){
                            printf ("Estado: %d, Transición: %d\n", afd.estados[a][k], afd.transiciones[j][h].transita_estado);
                            if (afd.estados[a][k] == afd.transiciones[j][h].transita_estado){
                                printf("Estado y transición iguales con Símbolo: %d\n", afd.transiciones[j][h].simbolo);
                                contador++;
                            }
                        }
                    }
                }

                /* Vemos cuántas transiciones con símbolo x hay */
                for (h=0; h < afd.compone_transicion[j]; h++){
                    if (afd.transiciones[j][h].simbolo == x){
                        num_tran_con_x++;
                    }
                }

                /* El estado ya existe */
                if (afd.compone_estado[a] >= num_tran_con_x){
                    max = afd.compone_estado[a];
                } else {
                    max = num_tran_con_x;
                }

                if (contador == max){
                    contador = -1;
                    break;
                }

                contador = 0;
                num_tran_con_x = 0;
            }
            printf("FIN DEL BUCLE.\n");

            /* Vemos si hay alguna transición con ese símbolo */
            for (h=0; h < afd.compone_transicion[j]; h++){
                if (afd.transiciones[j][h].simbolo == x){
                    num_tran_con_x++;
                }
            }

            /* El estado ya existe */
            if (contador == -1 || num_tran_con_x == 0){
                continue;
            } else {
                /* No existe, incluir estado nuevo */
                printf("No existe el estado. Lo creamos\n");
                afd.compone_estado[afd.num_estados] = 0;

                for (k=0; k < afd.compone_transicion[j]; k++) {
                    if (afd.transiciones[j][k].simbolo == x){

                        for (h=0; h < num_estados; h++){
                            if(AFNDCierreLTransicionIJ(afndl, afd.transiciones[j][k].transita_estado, h)){
                                /* Insertamos los estados del cierre lambda al nuevo estado */
                                afd.estados[afd.num_estados][afd.compone_estado[afd.num_estados]] = h;
                                afd.compone_estado[afd.num_estados]++;
                            }
                        }

                        afd.compone_transicion[afd.num_estados] = 0;
                    }
                }

                afd.num_estados++;
            }
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
