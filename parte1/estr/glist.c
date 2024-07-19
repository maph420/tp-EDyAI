#include "glist.h"
#include "pila.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Devuelve una lista vacía.
 */
GList glist_crear() { return NULL; }

/**
 * Destruccion de la lista.
 * destroy es una función que libera el dato almacenado.
 */
void glist_destruir(GList list, FuncionDestructora destroy) {
  GNode *nodeToDelete;
  while (list != NULL) {
    nodeToDelete = list;
    list = list->next;
    // como los datos ya NO SON necesariamente enteros, podrian ser tranquilamente estructuras etc etc... que
    // tienen que ser liberadas en caso que se haya asignado memoria en ellas
    destroy(nodeToDelete->data);
    free(nodeToDelete);
  }
}

/**
 * Agrega un elemento al inicio de la lista.
 * copy es una función que retorna una copia física del dato.
 */
GList glist_agregar_inicio(GList list, void *data, FuncionCopia copy) {
  GNode *newNode = malloc(sizeof(GNode));
  assert(newNode != NULL);
  newNode->next = list;
  newNode->data = copy(data);
  return newNode;
}

/**
 * Recorrido de la lista, utilizando la funcion pasada.
 */
void glist_recorrer(GList list, FuncionVisitante visit) {
  printf("LISTA: \n");
  for (GNode *node = list; node != NULL; node = node->next)
    visit(node->data);
}

void glist_elim_primero(GList* refLista) {
  GList tmp = *refLista;
  *refLista = tmp->next;
  free(tmp);
}

void* glist_dato_primero(GList l) {
  return l->data;
}




