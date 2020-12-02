#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdio.h>
#include <stdlib.h>

/* Crea un archivo txt donde se va a escribir los mensajes producidos por el minimiza.c */
void dlog_init();
/* Mensaje que se escribe en al archivo txt */
void dlog(char* message);

#endif
