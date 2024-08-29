#include "tablahash.h"
#include "glist.h"
#include <assert.h>
#include <stdlib.h>

// sacar
#include <stdio.h>

/**
 * Casillas en la que almacenaremos los datos de la tabla hash.
 */
typedef struct {
  GList datos;
} CasillaHash;

/**
 * Estructura principal que representa la tabla hash.
 */
struct _TablaHash {
  CasillaHash *elems;
  unsigned int numElems;
  unsigned int capacidad;
  FuncionCopiadora copia;
  FuncionComparadora comp;
  FuncionDestructora destr;
  FuncionHash hash;
};

// Funciones auxiliares

unsigned int es_primo(int n) {
  if (n < 2) return 0;
  if (n == 2) return 1;
  int flag = 1;
  // va hasta n/2 ya que no existen valores en (n/2, n) que puedan dividir a n
  for (int k = 2 ; k < n / 2 && flag; k++) {
    if (n % k == 0) {
      flag = 0;
    }
  }
  return flag;
}

/** Funcion para hallar el numero primo
 * mas cercano por arriba al argumento
 * dado. Complejidad: sus
*/
unsigned int primo_mas_cercano(int n) {
  int candidato = (n % 2 == 0) ? n + 1 : n;
  while (!es_primo(candidato)) candidato += 2 ;
  return candidato;
}

// Funciones de TablaHash

/**
 * Crea una nueva tabla hash vacia, con la capacidad dada.
 */
TablaHash tablahash_crear(unsigned int capacidad, FuncionCopiadora copia,
                          FuncionComparadora comp, FuncionDestructora destr,
                          FuncionHash hash) {

  // Pedimos memoria para la estructura principal y las casillas.
  TablaHash tabla = malloc(sizeof(struct _TablaHash));
  assert(tabla != NULL);
  tabla->elems = malloc(sizeof(CasillaHash) * capacidad);
  assert(tabla->elems != NULL);

  tabla->numElems = 0;
  tabla->capacidad = capacidad;
  tabla->copia = copia;
  tabla->comp = comp;
  tabla->destr = destr;
  tabla->hash = hash;

  // Inicializamos las casillas con datos nulos.
  for (unsigned int k = 0; k < capacidad; ++k)
    tabla->elems[k].datos = glist_crear();

  return tabla;
}

/**
 * Retorna el numero de elementos de la tabla.
 */
int tablahash_nelems(TablaHash tabla) { return tabla->numElems; }

/**
 * Retorna la capacidad de la tabla.
 */
int tablahash_capacidad(TablaHash tabla) { return tabla->capacidad; }

/**
 * Destruye la tabla.
 */
void tablahash_destruir(TablaHash tabla) {

  // Destruir cada uno de los casilleros.
  for (unsigned int idx = 0; idx < tabla->capacidad; ++idx)
    if (!glist_vacia(tabla->elems[idx].datos)) {
      //printf("liberado casillero %d\n", idx);
      glist_destruir(tabla->elems[idx].datos, tabla->destr);
      tabla->elems[idx].datos = NULL;
    }
  // Liberar el arreglo de casillas y la tabla.
  free(tabla->elems);
  free(tabla);
  return;
}

/**
 * Calcula el factor de carga alpha de la tabla
 */
float tablahash_factor_carga(TablaHash t) {
  return t->capacidad ? ((float)t->numElems / (float)t->capacidad) : 0.1;
}

/**
 * Inserta un dato en la tabla, o lo reemplaza si ya se encontraba.
 */
void tablahash_insertar(TablaHash tabla, void *dato, float umbralMax) {

  // Calculamos la posicion del dato dado, de acuerdo a la funcion hash.
  unsigned int idx = tabla->hash(dato) % tabla->capacidad;
  //printf("Casillero: %d\n", idx);

  // si era un casillero aun sin usar, aumentar
  if (glist_vacia(tabla->elems[idx].datos)) {
     tabla->numElems++;
  } 

  tabla->elems[idx].datos = glist_agregar_inicio(tabla->elems[idx].datos, dato, tabla->copia);

  float fc = tablahash_factor_carga(tabla);
  printf("Factor: %.2f\n", fc);

  if (fc > umbralMax) {
    printf("hay que redimensionar, el factor supero al umbral (%.2f)\n", umbralMax);
    tablahash_redimensionar(tabla);
  }

}

/**
 * Retorna el dato de la tabla que coincida con el dato dado, o NULL si el dato
 * buscado no se encuentra en la tabla.
 */
int tablahash_buscar(TablaHash tabla, void *dato) {
  // Calculamos la posicion del dato dado, de acuerdo a la funcion hash.
  unsigned idx = tabla->hash(dato) % tabla->capacidad;

  int hallado = glist_buscar(tabla->elems[idx].datos, dato, tabla->comp);

  return hallado ;
}

void tablahash_visitar_casillero(TablaHash t, unsigned int k, FuncionVisitante f) {
  //printf("Recorre casillero %d\n", k);
  glist_recorrer(t->elems[k].datos, f);
}

/**
 * Redimensiona la tabla hash duplicando su capacidad y
 * reposiciona todos los elementos.
 */
void tablahash_redimensionar(TablaHash tabla) {
    // nueva capacidad como el primo mas cercano para evitar clusters
    unsigned int nuevaCapacidad = primo_mas_cercano(tabla->capacidad * 5);
    
    printf("nuevo tam: %d\n", nuevaCapacidad);

    // Crear una nueva tabla con la nueva capacidad.
    CasillaHash *nuevosElems = malloc(sizeof(CasillaHash) * nuevaCapacidad);
    assert(nuevosElems != NULL);
    
    // Inicializar las casillas de la nueva tabla con datos nulos.
    for (unsigned int i = 0; i < nuevaCapacidad; ++i) {
        nuevosElems[i].datos = glist_crear();
    }
    
    // Recalcular la posición de cada elemento y moverlo a la nueva tabla.
    for (unsigned int i = 0; i < tabla->capacidad; ++i) {
        if (! glist_vacia(tabla->elems[i].datos)) {
            GList tmp = tabla->elems[i].datos;
            // esta faltando liberar los elems de la lista anterior
            unsigned int idx;
            while (tmp != NULL) {
              idx = tabla->hash(tmp->data) % nuevaCapacidad;
              nuevosElems[idx].datos = glist_agregar_inicio(nuevosElems[idx].datos, tmp->data, tabla->copia);
              tmp = tmp->next;
            }
          glist_destruir(tabla->elems[i].datos, tabla->destr);
        }
    }
    
    // liberar tabla antigua
    free(tabla->elems);
    
    // actualizar valores
    tabla->elems = nuevosElems;
    tabla->capacidad = nuevaCapacidad;
}
