#include "glist.h"
#include <assert.h>
#include <stdlib.h>
// sacar
#include <stdio.h>

GList glist_crear() { return NULL; }

int glist_vacia(GList l) {return l == NULL;}

void glist_destruir(GList list, FuncionDestructora destroy) {
  GNode *nodeToDelete;
  while (list != NULL) {
    nodeToDelete = list;
    list = list->next;
    destroy(nodeToDelete->data);
    free(nodeToDelete);
  }
}

GList glist_agregar_inicio(GList list, void *data, FuncionCopia copy) {
  GNode *newNode = malloc(sizeof(GNode));
  assert(newNode != NULL);
  newNode->next = list;
  newNode->data = copy(data);
  return newNode;
}

/**
 Sacar
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

int glist_buscar(GList lista, void *dato, FuncionComparadora comp) {
  if (lista == NULL) return 0;

  int encontrado = 0;

  while (lista != NULL && !encontrado) {
    if (comp(dato, lista->data) == 0) {
      encontrado = 1;
    } else lista = lista->next;
  }
  return encontrado;
}