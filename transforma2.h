/** 
 * @brief Transforma un AFND en AFD
 * 
 * @file transforma.h
 * @author AUTLEN Grupo 9, José Manuel Freire and Miguel Herrera 
**/

#ifndef TRANSFORMA2_H
#define TRANSFORMA2_H

#include <stdlib.h>
#include <string.h>
#include "afnd.h"

typedef struct _Transicion Transicion;
typedef struct _AFD AFD;

AFND * quitarLambda(AFND * afndl);
AFND * AFNDTransforma(AFND * afnd);

#endif