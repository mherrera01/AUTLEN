#include <stdio.h>
#include "afnd.h"
#include "minimiza.h"

int main(int argc, char ** argv) 
{

    AFND * p_afnd;
    AFND * p_afnd_min;

    p_afnd = AFNDNuevo("af1",12,2);

    AFNDInsertaSimbolo(p_afnd,"0");
    AFNDInsertaSimbolo(p_afnd,"1");

    AFNDInsertaEstado(p_afnd,"q0",INICIAL_Y_FINAL);
    AFNDInsertaEstado(p_afnd,"q1",NORMAL);
    AFNDInsertaEstado(p_afnd,"q2",NORMAL);
    AFNDInsertaEstado(p_afnd,"q3",NORMAL);
    AFNDInsertaEstado(p_afnd,"q4",FINAL);
    AFNDInsertaEstado(p_afnd,"q5",NORMAL);
    AFNDInsertaEstado(p_afnd,"q6",NORMAL);
    AFNDInsertaEstado(p_afnd,"q7",NORMAL);
    AFNDInsertaEstado(p_afnd,"q8",FINAL);
    AFNDInsertaEstado(p_afnd,"q9",NORMAL);
    AFNDInsertaEstado(p_afnd,"q10",NORMAL);
    AFNDInsertaEstado(p_afnd,"q11",NORMAL);

    AFNDInsertaTransicion(p_afnd, "q0", "0", "q1");
    AFNDInsertaTransicion(p_afnd, "q0", "1", "q1");
    AFNDInsertaTransicion(p_afnd, "q1", "0", "q2");
    AFNDInsertaTransicion(p_afnd, "q1", "1", "q2");
    AFNDInsertaTransicion(p_afnd, "q2", "0", "q3");
    AFNDInsertaTransicion(p_afnd, "q2", "1", "q3");
    AFNDInsertaTransicion(p_afnd, "q3", "0", "q4");
    AFNDInsertaTransicion(p_afnd, "q3", "1", "q4");
    AFNDInsertaTransicion(p_afnd, "q4", "0", "q5");
    AFNDInsertaTransicion(p_afnd, "q4", "1", "q5");
    AFNDInsertaTransicion(p_afnd, "q5", "0", "q6");
    AFNDInsertaTransicion(p_afnd, "q5", "1", "q6");
    AFNDInsertaTransicion(p_afnd, "q6", "0", "q7");
    AFNDInsertaTransicion(p_afnd, "q6", "1", "q7");
    AFNDInsertaTransicion(p_afnd, "q7", "0", "q8");
    AFNDInsertaTransicion(p_afnd, "q7", "1", "q8");
    AFNDInsertaTransicion(p_afnd, "q8", "0", "q9");
    AFNDInsertaTransicion(p_afnd, "q8", "1", "q9");
    AFNDInsertaTransicion(p_afnd, "q9", "0", "q10");
    AFNDInsertaTransicion(p_afnd, "q9", "1", "q10");
    AFNDInsertaTransicion(p_afnd, "q10", "0", "q11");
    AFNDInsertaTransicion(p_afnd, "q10", "1", "q11");
    AFNDInsertaTransicion(p_afnd, "q11", "0", "q0");
    AFNDInsertaTransicion(p_afnd, "q11", "1", "q0");

    /*p_afnd = AFNDNuevo("af1",8,2);

    AFNDInsertaSimbolo(p_afnd,"0");
    AFNDInsertaSimbolo(p_afnd,"1");

    AFNDInsertaEstado(p_afnd,"A",INICIAL);
    AFNDInsertaEstado(p_afnd,"B",NORMAL);
    AFNDInsertaEstado(p_afnd,"C",FINAL);
    AFNDInsertaEstado(p_afnd,"D",NORMAL);
    AFNDInsertaEstado(p_afnd,"E",NORMAL);
    AFNDInsertaEstado(p_afnd,"F",NORMAL);
    AFNDInsertaEstado(p_afnd,"G",NORMAL);
    AFNDInsertaEstado(p_afnd,"H",NORMAL);

    AFNDInsertaTransicion(p_afnd, "A", "0", "B");
    AFNDInsertaTransicion(p_afnd, "A", "1", "F");
    AFNDInsertaTransicion(p_afnd, "B", "0", "G");
    AFNDInsertaTransicion(p_afnd, "B", "1", "C");
    AFNDInsertaTransicion(p_afnd, "C", "0", "A");
    AFNDInsertaTransicion(p_afnd, "C", "1", "C");
    AFNDInsertaTransicion(p_afnd, "D", "0", "C");
    AFNDInsertaTransicion(p_afnd, "D", "1", "G");
    AFNDInsertaTransicion(p_afnd, "E", "0", "H");
    AFNDInsertaTransicion(p_afnd, "E", "1", "F");
    AFNDInsertaTransicion(p_afnd, "F", "0", "C");
    AFNDInsertaTransicion(p_afnd, "F", "1", "G");
    AFNDInsertaTransicion(p_afnd, "G", "0", "G");
    AFNDInsertaTransicion(p_afnd, "G", "1", "E");
    AFNDInsertaTransicion(p_afnd, "H", "0", "G");
    AFNDInsertaTransicion(p_afnd, "H", "1", "C");*/

    /* Función implementada en minimiza.c */
    p_afnd_min = AFNDMinimiza(p_afnd);

    AFNDADot(p_afnd_min);
    AFNDImprime(stdout,p_afnd_min);

    AFNDElimina(p_afnd);
    AFNDElimina(p_afnd_min);

    return 0;
}