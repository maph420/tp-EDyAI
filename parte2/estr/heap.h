#ifndef __HEAP_H__
#define __HEAP_H__

typedef void *(*FuncionCopiadora)(void *dato);
typedef int (*FuncionComparadora)(const void *, const void *);
typedef void (*FuncionDestructora)(void *dato);
typedef void (*FuncionVisitanteExtra)(void *dato, void *extra);
typedef void (*FuncionVisitante)(const void* dato);

typedef struct _BHeap {
void **arr;
int capacidad;
int ultimo;
FuncionComparadora comp;
} *BHeap;


BHeap bheap_crear(int capacidad, FuncionComparadora cmp);

void bheap_destruir(BHeap b);

unsigned int bheap_es_vacio(BHeap b);

void bheap_recorrer(BHeap b, FuncionVisitante f);

void flotar(BHeap heap, int i);

void hundir(BHeap heap, int n, int indRaiz);

BHeap bheap_insertar(BHeap b, void* elem);

BHeap bheap_eliminar(BHeap b);

#endif