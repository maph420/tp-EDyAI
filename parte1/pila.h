#ifndef __PILA_H__
#define __PILA_H__

#include "glist.h"

/** el compilador infiere del .c respectivo 
al .h para saber que def de pila usar**/
typedef GNode* Pila;

typedef void (*FuncionDestructora)(void *dato);
typedef void *(*FuncionCopia)(void *dato);
typedef void (*FuncionVisitante)(void *dato);

/**
 crea e inicializa una nueva pila vac ́ıa con la capacidad dada
**/
Pila pila_crear();

/**
 libera la memoria requerida para la pila
**/
void pila_destruir(Pila p, FuncionDestructora f);

/**
 retorna 1 si la pila est ́a vac ́ıa, y 0 en caso contrario.
**/
int pila_es_vacia(Pila p);

/**
 retorna el elemento que se encuentre en el tope de la pila, pero sin
eliminarlo
**/
void* pila_tope(Pila p);

/**
inserta un elemento en el tope de la pila, en caso que la pila
se encuentre llena, debera aumentar al doble la capacidad del arreglo.
**/
void pila_apilar(Pila* p, void* dato, FuncionCopia c);

/**
elimina el elemento que se encuentra en el tope de la pila
**/
void pila_desapilar(Pila* p);

/**
imprime en orden los elementos de la pila
**/
void pila_imprimir(Pila p, FuncionVisitante visit);


#endif

