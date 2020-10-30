/** 
 * @brief Transforma un AFND en AFD
 * 
 * @file transforma.h
 * @author AUTLEN Grupo 9, José Manuel Freire and Miguel Herrera 
**/

#ifndef TRANSFORMA_H
#define TRANSFORMA_H

#include <stdlib.h>
#include "afnd.h"

AFND * quitarLambda(AFND * afndl);
AFND * AFNDTransforma(AFND * afnd);

#endif