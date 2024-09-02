#include "glist.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Devuelve una lista vacía.
 */
GList glist_crear() { return NULL; }


int glist_vacia(GList l) {return l == NULL;}

/**
 * Destruccion de la lista.
 * destroy es una función que libera el dato almacenado.
 */
void glist_destruir(GList list, FuncionDestructora destroy) {
  GNode *nodeToDelete;
  while (list != NULL) {
    nodeToDelete = list;
    list = list->next;
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

void glist_elim_primero(GList* refLista, FuncionDestructora destroy) {
  GList tmp = *refLista;
  *refLista = tmp->next;
  destroy(tmp->data);
  free(tmp);
}

void* glist_dato_primero(GList l) {
  return l->data;
}

/**
 * Busca elemento, si lo encuentra 1 y sino 0
 */
int glist_buscar(GList lista, void *dato, FuncionComparadora comp) {
  if (lista == NULL) return 0;

  int encontrado = 0, comparacion;
  // cuando lo que busquemos contra lo que hay en la lista nos de 1, ya va a ser porque
  // estamos buscando algo mas chico que lo que hay en la lista (ORDENADA), entonces claramente
  // no va a estar

  while (lista != NULL && !encontrado) {
    comparacion = comp(dato, lista->data);
    if (comparacion == 0) {
      encontrado = 1;
    } else {
      lista = lista->next;
    }
  }
  return encontrado;
}



