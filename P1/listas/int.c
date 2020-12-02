#include "int.h"
#include <stdlib.h>
#include <stdio.h>

typedef void (*destroy_element_function_type)(void*);
typedef void* (*copy_element_function_type)(const void*);
typedef int (*print_element_function_type)(FILE *, const void*);
typedef int (*cmp_element_function_type)(const void*, const void*);

void int_destroy(void* v){
    if(v) free(v);
}

void* int_copy(const void* element){
    int* i;
    if(element == NULL) return NULL;
    
    if((i = (int*) malloc(sizeof(int)))==NULL) return NULL;
    *i = *((int*)element);
    
    return i;
}

int int_print(FILE * f, const void* pointer){
    if(f == NULL || pointer == NULL){
        return 0;
    }
    return fprintf(f, "[%d]", *((int*)pointer));
}

int int_compare(const void* i, const void* j){
    if(i == NULL || j == NULL) return 0;
    return *((int*)i) - *((int*)j);
}
