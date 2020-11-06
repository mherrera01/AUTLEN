#ifndef estado_h
#define estado_h
#include <stdio.h>

void estado_destroy(void* v);

void* estado_copy(const void* element);

int estado_print(FILE * f, const void* pointer);

int estado_compare(const void* i, const void* j);

#endif