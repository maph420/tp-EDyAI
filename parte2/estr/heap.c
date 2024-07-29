#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include <math.h>

BHeap bheap_crear(int capacidad, FuncionComparadora cmp) {
    if (capacidad == 0) return NULL;
    BHeap heap = malloc(sizeof(struct _BHeap));
    heap->arr = malloc(sizeof(void*) * capacidad);
    heap->comp = cmp;
    heap->capacidad = capacidad;  
    heap->ultimo = 0;
    printf("heap creado\n");
    return heap; 
}

void bheap_destruir(BHeap b) {
    if (b == NULL) return;
    free(b->arr);
    free(b);
}

unsigned int bheap_es_vacio(BHeap b) {
    return b->capacidad == 0;
}

/* ya no se necesita una cola, basta con
recorrer el arreglo iterativamente*/
void bheap_recorrer(BHeap b, FuncionVisitante f) {
    for (int i = 0; i < b->ultimo; i++)
        f(b->arr[i]);
}


// ir corrigiendo arbol, de acuerdo a la rel de cada nodo con su padre
// toma el heap e i que es el nodo hijo a partir de donde empiezan los chequeos
void flotar(BHeap heap, int i) {
    int parent = (i - 1) / 2;

    // si el padre es menor al hijo, cambiarlos, y seguir chequeando
    if (parent >= 0 && heap->comp(heap->arr[i], heap->arr[parent]) > 0) {
        void* tmp = heap->arr[parent];
        heap->arr[parent] = heap->arr[i];
        heap->arr[i] = tmp;
        flotar(heap, parent);
    }
}
// ir corrigiendo arbol de acuerdo a la rel de cada nodo con sus 2 hijos
// n es la longitud del array, indRaiz es a partir de donde empiezan los chequeos
void hundir(BHeap heap, int n, int indRaiz) {
    int mayor = indRaiz; // Inicializar el índice del nodo más grande como la raíz
    int indiceIzq = 2 * indRaiz + 1; // Índice del hijo izquierdo
    int indiceDer = 2 * indRaiz + 2; // Índice del hijo derecho


    // Si el hijo izquierdo es más grande que la raíz
    if (indiceIzq < n && heap->comp(heap->arr[indiceIzq], heap->arr[mayor]) > 0)
        mayor = indiceIzq;

    // Si el hijo derecho es más grande que el más grande encontrado hasta ahora
    if (indiceDer < n && heap->comp(heap->arr[indiceDer], heap->arr[mayor]) > 0)
        mayor = indiceDer;

    // si el nodo más grande no es la raíz (hay algun nodo que incumple heap)
    if (mayor != indRaiz) {
        // Intercambiar el valor del hallado por la raiz
        void* temp = heap->arr[indRaiz];
        heap->arr[indRaiz] = heap->arr[mayor];
        heap->arr[mayor] = temp;

        // Llamar recursivamente a heapify en el subárbol "afectado"
        // seguir chequeando, si hay mas problemas son por ahi
        hundir(heap, n, mayor);
    }
}

BHeap bheap_insertar(BHeap h, void* elem) {
    // en caso de necesitar espacio, pedirlo
    if (h->ultimo == h->capacidad) {
        h->capacidad *= 2;
        h->arr = realloc(h->arr, h->capacidad * sizeof(void *));
    }
    // insercion
    h->arr[h->ultimo] = elem;
    flotar(h, h->ultimo);
    h->ultimo++;
    return h;
}


BHeap bheap_eliminar(BHeap b) {
    // Verificar si el heap está vacío
    if (b->ultimo == 0) {
        printf("El heap está vacío. No se puede eliminar ningún elemento.\n");
        return b;
    }

    // Reemplazar la raíz del heap con el último elemento del arreglo
    b->arr[0] = b->arr[b->ultimo - 1];
    // Reducir el tam del arreglo
    b->ultimo--;

    // Reordenar el heap para restaurar su propiedad de heap
    // arrancamos desde la raiz (indice 0) para chequear TODO el arbol
    hundir(b, b->ultimo, 0);

    return b;
}

BHeap bheap_crear_desde_arr(void **arr, int largo, FuncionCopiadora copy, FuncionComparadora comp) {
    // Crear el heap
    BHeap heap = malloc(sizeof(struct _BHeap));
    
    // Asignar memoria para el arreglo interno
    heap->arr = malloc(largo * sizeof(void *));
    
    // Copiar los elementos del arreglo dado al arreglo interno del heap
    for (int i = 0; i < largo; i++) {
        heap->arr[i] = copy(arr[i]);
    }
    
    // settear
    heap->capacidad = largo;
    heap->ultimo = largo;
    heap->comp = comp;

    // heapifeamos todo
    // chequeamos con heapify2 el correcto enlace de cada nodo con cada hijo,
    // arrancando del padre del ultimo, y yendo "para arriba"
    int i = (largo - 1) / 2;
    for (i ; i >= 0; i--) {
        hundir(heap, largo, i);
    }

    return heap;
}

// Average: O(nlogn)
void heap_sort(void** arr, int tamArr, FuncionCopiadora copy, FuncionComparadora cmp) {
    // 1- arr -> heap
    BHeap heap = bheap_crear_desde_arr(arr, tamArr, copy, cmp);

    printf("hasta aca joya\n");
    // 2- ir extrayendo el ultimo
    for (int i = tamArr - 1; i >= 0; i--) {
        void* max = heap->arr[0];
        arr[i] = max;
        heap = bheap_eliminar(heap);
    }
    // 3- borramos espacio auxiliar usado
    bheap_destruir(heap);
}

//////

/*
// por cuestion de tiempos, hice el main aca nomas
int main() {
    // 2
    BHeap b = bheap_crear(14, compara_int);
    int listaValores[] = 
    {10, 20, 15, 25, 30, 16, 18, 19};
    int** dirValores = malloc(sizeof(int*)*8);
    for (int i=0; i < 9; i++)
        dirValores[i] = listaValores+i;
    
    b = bheap_insertar(b, dirValores[0]);
    b = bheap_insertar(b, dirValores[1]);
    b = bheap_insertar(b, dirValores[2]);
    b = bheap_insertar(b, dirValores[3]);
    b = bheap_insertar(b, dirValores[4]);
    b = bheap_insertar(b, dirValores[5]);
    b = bheap_insertar(b, dirValores[6]);
    b = bheap_insertar(b, dirValores[7]);

    puts("HEAP:");
    bheap_recorrer(b, imprimir_int);

    b = bheap_eliminar(b);
    
    puts("\nHEAP:");
    bheap_recorrer(b, imprimir_int);
    puts("");

    // 3
    BHeap bArr = bheap_crear_desde_arr((void**)dirValores, 8, no_copia, compara_int);

    bheap_recorrer(bArr, imprimir_int);

    // 4
    int otralistaValores[] = 
    {10, 20, 15, 25, 30, 16, 18, 19};
    int** otradirValores = malloc(sizeof(int*)*8);
    for (int i=0; i < 9; i++)
        otradirValores[i] = otralistaValores+i;

    // 4
    heap_sort((void**)otradirValores, 8, no_copia, compara_int);

    printf("Lista ordenada\n");
    for(int i = 0; i < 8; i++) {
        printf("%d, ", *(int*)otradirValores[i]);
    }
}
*/