#include "estado.h"
#include "int.h"
#include "list.h"

typedef void (*destroy_element_function_type)(void*);
typedef void* (*copy_element_function_type)(const void*);
typedef int (*print_element_function_type)(FILE *, const void*);
typedef int (*cmp_element_function_type)(const void*, const void*);

void estado_destroy(void* v){
    if(v) list_destroy(v);
}

void* estado_clone(const void* element){
    int i;
    List* l;

    if(element == NULL) return NULL;
    l = list_ini(int_destroy, int_copy, int_print, int_compare);
    if(l == NULL) return NULL;
    
    for(i = 0; i<list_size((List*)element); i++){
        list_insertLast(l, list_get((List*)element, i));
    }

    return l;
}

int estado_print(FILE * f, const void* pointer){
    if(f == NULL || pointer == NULL){
        return 0;
    }

    fprintf(f, "{\n");
    if(list_print(f, ((List*)pointer)) == -1) return -1;
    fprintf(f, "}\n");
    
    return 0;
}

int estado_compare(const void* i, const void* j){
    List* li;
    List* lj;
    int x;

    if(i == NULL || j == NULL) return 0;

    li = (List*)i;
    lj = (List*)j;

    if(list_size(li) != list_size(lj)) return 2;

    for(x = 0; x < list_size(li); x++){
        if(list_get(li, x) != list_get(lj, x)){
            return 1;
        }
    }

    return 0;
}