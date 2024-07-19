#include "avl.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Estructura del nodo del arbol AVL.
 * Tiene un puntero al dato (dato),
 * un puntero al nodo raiz del subarbol izquierdo (izq),
 * un puntero al nodo raiz del subarbol derecho (der), y
 * un entero para representar la altura del arbol (altura)
 */
typedef struct _AVL_Nodo {
  void* dato;
  struct _AVL_Nodo* izq, * der;
  int altura;
} AVL_Nodo;

struct _AVL {
  AVL_Nodo* raiz;
  FuncionCopiadora copia;
  FuncionComparadora comp;
  FuncionDestructora destr;
};

/**
 * avl_crear: Retorna un arbol AVL vacio
 */
AVL avl_crear(FuncionCopiadora copia, FuncionComparadora comp,
  FuncionDestructora destr) {
  AVL arbol = malloc(sizeof(struct _AVL));
  assert(arbol != NULL);
  arbol->raiz = NULL;
  arbol->copia = copia;
  arbol->comp = comp;
  arbol->destr = destr;
  return arbol;
}

/**
 * avl_destruir: Destruye el arbol y sus datos.
 */
static void avl_nodo_destruir(AVL_Nodo* raiz, FuncionDestructora destr) {
  if (raiz != NULL) {
    // destruir los nodos en postorder
    avl_nodo_destruir(raiz->izq, destr);
    avl_nodo_destruir(raiz->der, destr);
    destr(raiz->dato);
    // libera AVL_Nodo
    free(raiz);
  }
}
void avl_destruir(AVL arbol) {
  avl_nodo_destruir(arbol->raiz, arbol->destr);
  // libera _AVL
  free(arbol);
}

/**
 * avl_buscar: Retorna 1 si el dato se encuentra y 0 en caso contrario
 */

// es lo mismo que para ABB
static int avl_nodo_buscar(AVL_Nodo* raiz, FuncionComparadora comp,
  void* dato) {
  if (raiz == NULL)
    return 0;
  else if (comp(dato, raiz->dato) == 0) // raiz->dato == dato
    return 1;
  else if (comp(dato, raiz->dato) < 0) // dato < raiz->dato
    return avl_nodo_buscar(raiz->izq, comp, dato);
  else // raiz->dato < dato
    return avl_nodo_buscar(raiz->der, comp, dato);
}
int avl_buscar(AVL arbol, void* dato) {
  return avl_nodo_buscar(arbol->raiz, arbol->comp, dato);
}

/**
 * avl_nodo_altura: Funcion interna que retorna la altura del arbol.
 * La altura del arbol vacio se define como -1.
 */
static int avl_nodo_altura(AVL_Nodo* raiz) {
  return (raiz == NULL ? -1 : raiz->altura);
}

/**
 * avl_nodo_max_altura_hijos: Funcion interna que retorna la maxima altura de
 * los hijos.
 */
static unsigned int avl_nodo_max_altura_hijos(AVL_Nodo* raiz) {
  assert(raiz != NULL);
  int alturaIzq = avl_nodo_altura(raiz->izq);
  int alturaDer = avl_nodo_altura(raiz->der);
  return (alturaIzq < alturaDer ? alturaDer : alturaIzq);
}

/**
 * avl_nodo_factor_balance: Funcion interna que retorna el factor de balance de
 * un nodo.
 */

// posibles retornos de la funcion: {-1, 0, 1}
static int avl_nodo_factor_balance(AVL_Nodo* raiz) {
  assert(raiz != NULL);
  int factor = avl_nodo_altura(raiz->der) - avl_nodo_altura(raiz->izq);
  assert(-2 <= factor || factor <= 2);
  return factor;
}

/**
 * avl_nodo_rotacion_simple_izq: Funcion interna que realiza una rotacion simple
 * a izquierda y retorna la nueva raiz.
 */
static AVL_Nodo* avl_nodo_rotacion_simple_izq(AVL_Nodo* raiz) {
  // hijoDer es el candidato a ser la nueva raiz
  AVL_Nodo* hijoDer = raiz->der;
  assert(hijoDer != NULL);
  // actualizar punteros
  raiz->der = hijoDer->izq;
  hijoDer->izq = raiz;
  // actualizar alturas
  raiz->altura = 1 + avl_nodo_max_altura_hijos(raiz);
  hijoDer->altura = 1 + avl_nodo_max_altura_hijos(hijoDer);
  return hijoDer;
}

/**
 * avl_nodo_rotacion_simple_der: Funcion interna que realiza una rotacion simple
 * a derecha y retorna la nueva raiz.
 */ 
static AVL_Nodo* avl_nodo_rotacion_simple_der(AVL_Nodo* raiz) {
  AVL_Nodo* hijoIzq = raiz->izq;
  // intercambiamos punteros
  raiz->izq = hijoIzq->der;
  hijoIzq->der = raiz;

  // actualizar alturas
  raiz->altura = 1 + avl_nodo_max_altura_hijos(raiz);
  hijoIzq->altura = 1 + avl_nodo_max_altura_hijos(hijoIzq);

  return hijoIzq;
}

/**
 * avl_nodo_crear: Funcion interna que crea un nuevo nodo y lo retorna.
 * La altura de un nodo hoja es 0.
 */
static AVL_Nodo* avl_nodo_crear(void* dato, FuncionCopiadora copia) {
  AVL_Nodo* nuevoNodo = malloc(sizeof(AVL_Nodo));
  assert(nuevoNodo != NULL);
  nuevoNodo->dato = copia(dato);
  nuevoNodo->izq = nuevoNodo->der = NULL;
  nuevoNodo->altura = 0;
  return nuevoNodo;
}

/**
 * avl_insertar: Inserta un dato no repetido en el arbol, manteniendo la
 * propiedad de los arboles AVL.
 */
static AVL_Nodo* avl_nodo_insertar(AVL_Nodo* raiz, void* dato,
  FuncionCopiadora copia, FuncionComparadora comp) {
  if (raiz == NULL) // insertamos el nuevo elemento
    return avl_nodo_crear(dato, copia);

  int comparacion = comp(dato, raiz->dato);
  if (comparacion < 0) { // el dato debe ir en el subarbol izq
    raiz->izq = avl_nodo_insertar(raiz->izq, dato, copia, comp);
    // chequear balance
    int factor_balance = avl_nodo_factor_balance(raiz);
    if (factor_balance == -2) {
      // casos 1 o 2
      int factor_balance_izq = avl_nodo_factor_balance(raiz->izq);
      if (factor_balance_izq == 1) { // caso 2
        raiz->izq = avl_nodo_rotacion_simple_izq(raiz->izq);
      }
      raiz = avl_nodo_rotacion_simple_der(raiz); // caso 1
    }
  } else if (comparacion > 0) { // el dato debe ir en el subarbol der
    raiz->der = avl_nodo_insertar(raiz->der, dato, copia, comp);
    // chequear balance
    int factor_balance = avl_nodo_factor_balance(raiz);
    if (factor_balance == 2) {
      // casos 3 o 4
      int factor_balance_der = avl_nodo_factor_balance(raiz->der);
      if (factor_balance_der == -1) { // caso 3
        raiz->der = avl_nodo_rotacion_simple_der(raiz->der);
      }
      raiz = avl_nodo_rotacion_simple_izq(raiz); // caso 4
    }
  } else { // no agregar elementos repetidos
    return raiz;
  }
  
  // Actualizar altura
  raiz->altura = 1 + avl_nodo_max_altura_hijos(raiz);
  return raiz;
}
void avl_insertar(AVL arbol, void* dato) {
  arbol->raiz = avl_nodo_insertar(arbol->raiz, dato, arbol->copia, arbol->comp);
}

/**
 * avl_recorrer: Recorrido DSF del arbol
 */
static void avl_nodo_recorrer(AVL_Nodo* raiz, AVLRecorrido orden,
  FuncionVisitante visita) {
  if (raiz != NULL) {
    if (orden == AVL_RECORRIDO_PRE)
      visita(raiz->dato);
    avl_nodo_recorrer(raiz->izq, orden, visita);
    if (orden == AVL_RECORRIDO_IN)
      visita(raiz->dato);
    avl_nodo_recorrer(raiz->der, orden, visita);
    if (orden == AVL_RECORRIDO_POST)
      visita(raiz->dato);
  }
}
void avl_recorrer(AVL arbol, AVLRecorrido orden, FuncionVisitante visita) {
  avl_nodo_recorrer(arbol->raiz, orden, visita);
}


