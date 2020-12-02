#ifndef TRANSFORMA_H
#define TRANSFORMA_H

#include <stdio.h>
#include <string.h>

#include "afnd.h"
#include "debugger.h"
#include "list.h"
#include "int.h"
#include "estado.h"

/* Transforma un autómata no determinista a uno determinista */
AFND *AFNDTransforma(AFND *afnd);

#endif
