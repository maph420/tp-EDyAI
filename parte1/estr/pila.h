#ifndef __PILA_H__
#define __PILA_H__

#include "glist.h"

typedef void (*FuncionDestructora)(void *dato);
typedef void *(*FuncionCopia)(void *dato);
// sacar
typedef void (*FuncionVisitante)(void *dato);

typedef GNode* Pila;
/**
 * Crea e inicializa una nueva pila vacia con la capacidad dada
**/
Pila pila_crear();

/**
 * Libera la memoria requerida para la pila
**/
void pila_destruir(Pila, FuncionDestructora);

/**
 * Retorna 1 si la pila esta vacia, y 0 en caso contrario.
**/
int pila_es_vacia(Pila);

/**
 * Retorna el elemento que se encuentre en el tope de la pila, pero sin
 * eliminarlo
**/
void* pila_tope(Pila);

/**
* Inserta un elemento en el tope de la pila. 
**/
void pila_apilar(Pila*, void*, FuncionCopia);

/**
* Elimina el elemento que se encuentra en el tope de la pila
**/
void pila_desapilar(Pila*, FuncionDestructora);

/**
 * Sacar
Imprime en orden los elementos de la pila
**/
void pila_imprimir(Pila, FuncionVisitante);

#endif /* __PILA_H__ */

