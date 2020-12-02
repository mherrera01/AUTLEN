#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "int.h"
#include "estado.h"

int main(int argc, char *argv[]){
    List *l1, *l2, *l3;
    int i, x;
    int p2=2;/*, p8=8, p50=50*/
    if(argc < 2){
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }
    
    l1 = list_ini(int_destroy, int_copy, int_print, int_compare);
    l2 = list_ini(int_destroy, int_copy, int_print, int_compare);
    l3 = list_ini(estado_destroy, estado_clone, estado_print, estado_compare);

    /* x = your input number */
    x = atoi(argv[1]);

    /* Adding the elements to the lists */
    for(i=x;i>0;i--){
        if(i%2==0) list_insertFirst(l1, &i);
        else list_insertLast(l1, &i);
        list_insertInOrder(l2, &i);
    }

    list_insertLast(l3, l1);
    list_insertLast(l3, l2);

    /*Pruebas de contains*/
    /*
    printf("Boolean A (1): %d\n", list_contains(l1, &p2));
    printf("Boolean B (0): %d\n", list_contains(l2, &p50));
    printf("Boolean C (0): %d\n", list_contains(l1, &p8));
    printf("Boolean D (1): %d\n", list_contains(l2, &p2));
    printf("Boolean E (1): %d\n", list_contains(l3, l2));
    */

   /*Pruebas list_isEmpty*/
   printf("Boolean A (0): %d\n", list_isEmpty(l1));
   for(i=x;i>0;i--){
        list_extractFirst(l1);
    }
   printf("Boolean B (1): %d\n", list_isEmpty(l1));
   list_insertFirst(l1, &p2);
   printf("Boolean C (0): %d\n", list_isEmpty(l1));
   list_extractFirst(l1);
   printf("Boolean D (1): %d\n", list_isEmpty(l1));


    /* Printing the lists */
    /*list_print(stdout, l1);
    list_print(stdout, l2);
    list_print(stdout, l3);*/

    /* Destroying lists and files */
    list_destroy(l1);
    list_destroy(l2);
    list_destroy(l3);

    return 0;
}