#include <stdio.h>
#include <stdlib.h>
#include "colaprioridad.h"

int compara_int(void* refA, void* refB) {
    const int a = *(int*)refA;
    const int b = *(int*)refB;
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 0;
}

 void imprimir_int(const void* refD) {
    printf("%d, ", *(int*)refD);
}

 void* no_copia(void* d) {
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
    bheap_eliminar(cp->heap);
   
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


void cola_prioridad_buscar_eliminar(ColaPrioridad cp, void* elem) {
    bheap_buscar_eliminar(cp->heap, elem);
}

int main() {
    ColaPrioridad cp = cola_prioridad_crear(10, compara_int);

    int valores[8] = {10, 20, 15, 25, 30, 16, 18, 19};

    for (int i = 0; i < 8; i++) {
        cola_prioridad_insertar(cp, &(valores[i]));
    }

    printf("Máximo elemento: %d\n", *(int*)cola_prioridad_maximo(cp));

    cola_prioridad_eliminar_maximo(cp);
    printf("Máximo elemento después de eliminar: %d\n", *(int*)cola_prioridad_maximo(cp));

    bheap_recorrer(cp->heap, imprimir_int);

    printf("%d\n", valores[7]);

    bheap_buscar_eliminar(cp->heap, &(valores[7]));
    printf("sdkljfñjslkd\n");

    cola_prioridad_eliminar_maximo(cp);
    printf("Máximo elemento después de eliminar: %d\n", *(int*)cola_prioridad_maximo(cp));

  cola_prioridad_eliminar_maximo(cp);
    printf("Máximo elemento después de eliminar: %d\n", *(int*)cola_prioridad_maximo(cp));

  cola_prioridad_eliminar_maximo(cp);
    printf("Máximo elemento después de eliminar: %d\n", *(int*)cola_prioridad_maximo(cp));

cola_prioridad_eliminar_maximo(cp);
    printf("Máximo elemento después de eliminar: %d\n", *(int*)cola_prioridad_maximo(cp));


    cola_prioridad_destruir(cp);
    return 0;
}