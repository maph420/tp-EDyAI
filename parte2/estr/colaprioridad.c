#include <stdio.h>
#include <stdlib.h>
#include "colaprioridad.h"

static int compara_int(const void* refA, const void* refB) {
    const int a = *(int*)refA;
    const int b = *(int*)refB;
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 0;
}

static void imprimir_int(const void* refD) {
    printf("%d, ", *(int*)refD);
}

static void* no_copia(void* d) {
    return d;
}

ColaPrioridad cola_prioridad_crear(int capacidad, FuncionComparadora cmp) {
    ColaPrioridad cp = malloc(sizeof(struct _ColaPrioridad));
    cp->heap = bheap_crear(capacidad, cmp);
    return cp;
}

void cola_prioridad_destruir(ColaPrioridad cp) {
    bheap_destruir(cp->heap);
    free(cp);
}

int cola_prioridad_es_vacia(ColaPrioridad cp) {
    return cp->heap->ultimo == 0;
}

void* cola_prioridad_maximo(ColaPrioridad cp) {
    if (cola_prioridad_es_vacia(cp)) {
        return NULL;
    }
    return cp->heap->arr[0];
}

void cola_prioridad_eliminar_maximo(ColaPrioridad cp) {
    if (cola_prioridad_es_vacia(cp)) {
        return;
    }

    cp->heap->arr[0] = cp->heap->arr[cp->heap->ultimo - 1];
    cp->heap->ultimo--;
    hundir(cp->heap, cp->heap->ultimo, 0);
}

void cola_prioridad_insertar(ColaPrioridad cp, void* elem) {
    if (cp->heap->ultimo == cp->heap->capacidad) {
        cp->heap->capacidad *= 2;
        cp->heap->arr = realloc(cp->heap->arr, cp->heap->capacidad * sizeof(void*));
    }
    cp->heap->arr[cp->heap->ultimo] = elem;
    cp->heap->ultimo++;
    flotar(cp->heap, cp->heap->ultimo - 1);
}

int main() {
    ColaPrioridad cp = cola_prioridad_crear(10, compara_int);

    int valores[] = {10, 20, 15, 25, 30, 16, 18, 19};

    for (int i = 0; i < 8; i++) {
        cola_prioridad_insertar(cp, &valores[i]);
    }

    printf("Máximo elemento: %d\n", *(int*)cola_prioridad_maximo(cp));

    cola_prioridad_eliminar_maximo(cp);
    printf("Máximo elemento después de eliminar: %d\n", *(int*)cola_prioridad_maximo(cp));

    cola_prioridad_destruir(cp);
    return 0;
}