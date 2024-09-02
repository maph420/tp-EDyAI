#include <stdio.h>
#include <stdlib.h>
#include "bheap.h"

void bheap_recorrer(BHeap b, FuncionVisitante f) {
    
    //fprintf(stderr, "entra\n");
    if (b->ultimo == 0) {
        fprintf(stderr, "bheap ta vacio\n");
        return;
    }
    for (int i = 0; i < b->ultimo; i++)
        f(b->arr[i]);
}

BHeap bheap_crear(int capacidad, FuncionComparadora cmp, FuncionDestructora d, FuncionCopiadora c) {
    if (capacidad == 0) return NULL;
    BHeap heap = malloc(sizeof(struct _BHeap));
    heap->arr = malloc(sizeof(void*) * capacidad);
    heap->comp = cmp;
    heap->destroy = d;
    heap->copy = c;
    heap->capacidad = capacidad;  
    heap->ultimo = 0;
    return heap; 
}

// arreglar
void bheap_destruir(BHeap bheap) {
    if (bheap == NULL) return;
    
    for (int i = 0; i < bheap->ultimo; i++) {
        bheap->destroy(bheap->arr[i]);
    }
    free(bheap->arr);
    free(bheap);
}

unsigned int bheap_es_vacio(BHeap bheap) {
    return (bheap->ultimo == 0);
}

void* bheap_minimo(BHeap bheap) {
    return bheap_es_vacio(bheap) ? NULL : bheap->arr[0];
}

void flotar(BHeap bheap, int pos) {
    if (pos == 0) return; 
    int pos_padre = (pos - 1) / 2;

    // Si el hijo es menor al padre, cambiarlos, y seguir chequeando
    if (bheap->comp(bheap->arr[pos], bheap->arr[pos_padre]) < 0) {
        void* tmp = bheap->arr[pos];
        bheap->arr[pos] = bheap->arr[pos_padre];
        bheap->arr[pos_padre] = tmp;
        flotar(bheap, pos_padre);
    }
}

void hundir(BHeap bheap, int pos, int n) {
    int min = pos;
    int pos_hijo_izq = 2 * pos + 1;
    int pos_hijo_der = 2 * pos + 2;

    // Si el hijo izquierdo es más grande que la raíz
    if (pos_hijo_izq < n && bheap->comp(bheap->arr[pos_hijo_izq], bheap->arr[min]) < 0)
        min = pos_hijo_izq;

    // Si el hijo derecho es más grande que el más grande encontrado hasta ahora
    if (pos_hijo_der < n && bheap->comp(bheap->arr[pos_hijo_der], bheap->arr[min]) < 0)
        min = pos_hijo_der;

    // Hay algun nodo que incumple la propiedad de min-heap
    if (min != pos) {
        void* temp = bheap->arr[pos];
        bheap->arr[pos] = bheap->arr[min];
        bheap->arr[min] = temp;
        // Llamar recursivamente a hundir en el subárbol "afectado"
        hundir(bheap, min, n);
    }
}

BHeap bheap_insertar(BHeap bheap, void* elem) {
    // En caso de necesitar espacio, pedirlo
    if (bheap->ultimo == bheap->capacidad) {
        bheap->capacidad *= 2;
        bheap->arr = realloc(bheap->arr, bheap->capacidad * sizeof(void *));
    }
    // Insercion. El ultimo elem hasta ese momento estaba en la pos bheap->ultimo -1
    bheap->arr[bheap->ultimo] = bheap->copy(elem);
    flotar(bheap, bheap->ultimo);
    bheap->ultimo++;
    return bheap;
}

BHeap bheap_eliminar_minimo(BHeap bheap) {

    if (bheap->ultimo == 0) {
        printf("El heap está vacío. No se puede eliminar ningún elemento.\n");
        return bheap;
    }
    // Reemplazar la raíz del heap con el último elemento del arreglo
    bheap->destroy(bheap->arr[0]);
    bheap->arr[0] = bheap->arr[bheap->ultimo - 1];
    bheap->ultimo--;

    // Reordenar el heap para restaurar su propiedad de heap
    // arrancamos desde la raiz (indice 0) para chequear TODO el arbol
    hundir(bheap, 0, bheap->ultimo);

    return bheap;
}

int bheap_buscar(BHeap bheap, void* elemento) {
    int pos;
    int hallado = 0; 

    // Buscar el elemento en el arreglo
    for (pos = 0; pos < bheap->ultimo; pos++) {
        if ((bheap->comp(bheap->arr[pos], elemento)) == 2) {
            hallado = 1;
            break;  // Encontrado, salir del bucle
        }
    }
    return hallado ? pos : -1;
   
}

void bheap_eliminar(BHeap bheap, int pos) {
    // Intercambiar el elemento con el último y reducir el tamaño del heap
    // chequear
    void* temp = bheap->arr[pos];
    bheap->arr[pos] = bheap->arr[bheap->ultimo - 1];
    bheap->arr[bheap->ultimo - 1] = temp;
    bheap->destroy(bheap->arr[bheap->ultimo - 1]);
    bheap->ultimo--;

    // Restaurar la propiedad del heap solo si se eliminó algo
    if (pos < bheap->ultimo) { 
        // Comparar el elemento con su padre para decidir si flotar o hundir
        if (pos > 0 && bheap->comp(bheap->arr[pos], bheap->arr[(pos - 1) / 2]) < 0) {
            flotar(bheap, pos);
        } else {
            hundir(bheap, pos, bheap->ultimo);
        }
    }
}


// sacar
int es_bheap_r(BHeap bheap, int pos, int n) {
    if (pos >= (n - 1) /2) return 1;

    if ((bheap->comp(bheap->arr[pos], bheap->arr[2*pos +1]) <= 0) &&
    (bheap->comp(bheap->arr[pos], bheap->arr[2*pos +2]) <= 0) &&
    es_bheap_r(bheap, 2*pos +1, n) &&
    es_bheap_r(bheap, 2*pos + 2, n)) {
        return 1;
    }
    return 0;
}

int es_bheap(BHeap bheap) {
    return es_bheap_r(bheap, 0, bheap->ultimo);
}

/*
int compara_int( void* refA,  void* refB) {
    const int a = *(int*)refA;
    const int b = *(int*)refB;
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 2;
}

void imprimir_int( void* refD) {
    printf("%d, ", *(int*)refD);
}

void* no_copia(void* d) {
    return d;
}


void no_destruye(void* d) {
    return;
}



int main() {

    BHeap b = bheap_crear(15, compara_int, no_destruye, no_copia);

     int listaValores[] = 
    {340, 530, 154, 225, 440, 136, 128, 119};
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

    fprintf(stderr, "HEAP:\n");
    bheap_recorrer(b, imprimir_int);
    int r =es_bheap(b);
    printf("Es bheap: %d\n", r);
    int val = 156;
    b->arr[2] = &val;

    fprintf(stderr, "HEAP:\n");
    bheap_recorrer(b, imprimir_int);
    r =es_bheap(b);
    printf("Es bheap: %d\n", r);
}*/