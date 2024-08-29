#include <stdio.h>
#include <stdlib.h>
#include "pila.h"

Pila pila_crear() {
    Pila p = (Pila)glist_crear();
    return p;
}

void pila_destruir(Pila p, FuncionDestructora destroy) {
    glist_destruir((GList)p, destroy);
}

int pila_es_vacia(Pila p) {
    return p == NULL;
}

// (se asume la pila NO vacia)
void* pila_tope(Pila p) {
    void* refDato = glist_dato_primero((GList)p);
    return refDato;
}

void pila_apilar(Pila* pilaRef, void* dato, FuncionCopia copiar) {
    *pilaRef = (Pila)glist_agregar_inicio((GList)*pilaRef, dato, copiar);
}

void pila_desapilar(Pila* p, FuncionDestructora destroy) {
    glist_elim_primero((GList*)p, destroy);
}

void pila_imprimir(Pila p, FuncionVisitante visit) {
    printf("----------\n");
    glist_recorrer((GList)p, visit);
}