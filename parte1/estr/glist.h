#ifndef __GLIST_H__
#define __GLIST_H__

typedef void (*FuncionDestructora)(void *dato);
typedef void *(*FuncionCopia)(void *dato);
typedef void (*FuncionVisitante)(void *dato); //sacar?
typedef int (*FuncionComparadora)(void* dato1, void* dato2);

typedef struct _GNode {
  void *data;
  struct _GNode *next;
} GNode;

typedef GNode *GList;

/**
 * Devuelve una lista vacía.
 */
GList glist_crear();

/**
 * Determina si la lista es vacia
 */
int glist_vacia(GList);

/**
 * Destruccion de la lista
 */
void glist_destruir(GList, FuncionDestructora);

/**
 * Agrega un elemento al inicio de la lista.
 */
GList glist_agregar_inicio(GList, void*, FuncionCopia);

/**
 * Recorrido de la lista, utilizando la funcion pasada.
 */
void glist_recorrer(GList, FuncionVisitante);

/* Elimina el primer elemento de la lista enlazada.*/
void glist_elim_primero(GList*, FuncionDestructora);

/**
 * Retorna el primer elemento de la lista enlazada
 */
void* glist_dato_primero(GList);

/**
 * Busca elemento, si lo encuentra 1 y sino 0
 */
int glist_buscar(GList, void*, FuncionComparadora);

#endif /* __GLIST_H__ */
