#ifndef MINIMIZA_H
#define MINIMIZA_H

#include <stdio.h>
#include <string.h>

#include "afnd.h"
#include "debugger.h"
#include "list.h"
#include "int.h"
#include "estado.h"

/* Minimiza un autómata finito determinista */
AFND *AFNDMinimiza(AFND *afd);

#endif
