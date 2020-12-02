#ifndef int_h
#define int_h
#include <stdio.h>

void int_destroy(void* v);

void* int_copy(const void* element);

int int_print(FILE * f, const void* pointer);

int int_compare(const void* i, const void* j);

#endif
