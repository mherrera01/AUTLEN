#include "debugger.h"

void dlog_init(){
    FILE* log = NULL;

    /* Creamos el archivo txt */
    log = fopen("DEBUG.txt", "w");
    fclose(log);

    return;
}

void dlog(char* message){
    FILE* log = NULL;

    /* Abrimos para escribir el mensaje en el archivo txt */
    log = fopen("DEBUG.txt", "a");
    fprintf(log, "%s", message);
    fprintf(log, "\n");

    /* Cerramos el archivo txt */
    fclose(log);
    return;
}
