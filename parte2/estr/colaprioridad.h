#ifndef __COLAPRIORIDAD_H__
#define __COLAPRIORIDAD_H__

#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

typedef struct _ColaPrioridad {
    BHeap heap;
} *ColaPrioridad;

ColaPrioridad cola_prioridad_crear(int capacidad, FuncionComparadora cmp);

void cola_prioridad_destruir(ColaPrioridad cp);

int cola_prioridad_es_vacia(ColaPrioridad cp);

void* cola_prioridad_maximo(ColaPrioridad cp);

void cola_prioridad_eliminar_maximo(ColaPrioridad cp);

#endif