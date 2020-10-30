/** 
 * @brief Transforma un AFND en AFD
 * 
 * @file transforma.c
 * @author AUTLEN Grupo 9, José Manuel Freire and Miguel Herrera 
**/

#include "transforma.h"

#define debug 0 /* Debug is 1 for terminal messages during execution */

AFND * quitarLambda(AFND* afndl){
    int num_estados, num_simbolos;

    /* Contadores de loops */
    int i, j;

    /* Control de errores */
    if(afndl == NULL){
        return NULL;
    }

    num_estados = AFNDNumEstados(afndl);
    num_simbolos = AFNDNumSimbolos(afndl);

    /* Cierres Lambda */
    int cierres[num_estados][num_estados];

    /* Obtenemos los cierres lambda de los estados del afndl */
    for (i=0; i < num_estados; i++){
        for (j=0; j < num_estados; j++){
            
            if(AFNDCierreLTransicionIJ(afndl, i, j)){
                cierres[i][j] = 1;
            } else {
                cierres[i][j] = 0;
            }
        }
    }

    for(i=0; i<num_estados; i++){
        for(j=0; j<num_estados; j++){
            printf("%d\t", cierres[i][j]);
        }
        printf("\n");
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
    printf("%d\n", AFNDCierreLTransicionIJ(afnd, 0, 1));
    return NULL;
}
