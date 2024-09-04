#ifndef __BHEAP_H__
#define __BHEAP_H__

typedef void *(*FuncionCopiadora)(void *dato);
typedef int (*FuncionComparadora)(void *, void *);
typedef void (*FuncionDestructora)(void *dato);

typedef struct _BHeap {
void **arr;
int capacidad;
int ultimo;
FuncionComparadora comp;
FuncionDestructora destroy;
FuncionCopiadora copy;
} *BHeap;

/*
    Implementacion de min heap
*/

/**
 * Crea un heap de una capacidad dada
 */
BHeap bheap_crear(int, FuncionComparadora, FuncionDestructora, FuncionCopiadora);

/**
 *  Destruye el heap (funcion destructora?)
 */
void bheap_destruir(BHeap);

/**
 * Verifica si el heap es vacio
 */
unsigned int bheap_es_vacio(BHeap);

/**
 * Obtiene el primer elemento del heap, i.e. el minimo elemento.
 */
void* bheap_minimo(BHeap);

/**
 * Flota el elemento en la pos dada hasta que el heap verifique
 * la condicion de min-heap.
 */
void flotar(BHeap, int);

/**
 * Hunde el elemento en la pos dada hasta que el heap verifique
 * la condicion de min-heap. n es el tamaño del arreglo
 */
void hundir(BHeap, int, int);

/**
 * Inserta el dato en el heap
 */
void bheap_insertar(BHeap, void*);

/**
 * Elimina el primer elemento del heap, i.e. el minimo.
 * En caso de estar el heap vacio, no se hace nada
 */
void bheap_eliminar_minimo(BHeap);

/**
 * Busca un elemento en el heap y, si lo encuentra lo elimina.
 * De no encontrarlo no hace nada. 
 * Al tratarse de otro criterio de comparacion, la funcion comparadora debe
 * retornar 2 para saber que dos nodos son iguales (segun el criterio usado).
 */
void bheap_buscar_eliminar(BHeap, void*);

#endif /* __BHEAP_H__ */