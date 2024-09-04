#include "tablahash.h"
#include "glist.h"
#include <assert.h>
#include <stdlib.h>

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

unsigned int es_primo(int n) {
  if (n < 2) return 0;
  if (n == 2) return 1;
  int primo = 1;
  // Va hasta n/2 ya que no existen valores en (n/2, n) que puedan dividir a n
  for (int k = 2 ; k < n / 2 && primo; k++) {
    if (n % k == 0) {
      primo = 0;
    }
  }
  return primo;
}

unsigned int primo_mas_cercano(int n) {
  int candidato = (n % 2 == 0) ? n + 1 : n;
  while (!es_primo(candidato)) candidato += 2 ;
  return candidato;
}

/**
 * Crea una nueva tabla hash vacia, con la capacidad dada.
 */
TablaHash tablahash_crear(unsigned int capacidad, FuncionCopiadora copia,
                          FuncionComparadora comp, FuncionDestructora destr,
                          FuncionHash hash) {

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
 * Retorna la capacidad de la tabla.
 */
int tablahash_capacidad(TablaHash tabla) { return tabla->capacidad; }

/**
 * Destruye la tabla.
 */
void tablahash_destruir(TablaHash tabla) {

  for (unsigned int idx = 0; idx < tabla->capacidad; ++idx)
    if (!glist_vacia(tabla->elems[idx].datos)) {
      glist_destruir(tabla->elems[idx].datos, tabla->destr);
      tabla->elems[idx].datos = NULL;
    }
  free(tabla->elems);
  free(tabla);
  return;
}

float tablahash_factor_carga(TablaHash t) {
  return ((float)t->numElems / (float)t->capacidad);
}

void tablahash_insertar(TablaHash tabla, void *dato, float umbralMax) {

  // Calculamos la posicion del dato dado, de acuerdo a la funcion hash.
  unsigned int idx = tabla->hash(dato) % tabla->capacidad;

  // Si era un casillero aun sin usar, aumentar
  if (glist_vacia(tabla->elems[idx].datos)) {
     tabla->numElems++;
  } 

  tabla->elems[idx].datos = glist_agregar_inicio(tabla->elems[idx].datos, dato, tabla->copia);

  float fc = tablahash_factor_carga(tabla);
  if (fc > umbralMax) {
    tablahash_redimensionar(tabla);
  }

}

int tablahash_buscar(TablaHash tabla, void *dato) {
  // Calculamos la posicion del dato dado, de acuerdo a la funcion hash.
  unsigned idx = tabla->hash(dato) % tabla->capacidad;
  return glist_buscar(tabla->elems[idx].datos, dato, tabla->comp) ;
}

void tablahash_redimensionar(TablaHash tabla) {
    // Nueva capacidad como el primo mas cercano del doble de la capacidad anterior,
    // para evitar posibles formaciones de clusters
    unsigned int nuevaCapacidad = primo_mas_cercano(tabla->capacidad * 2);

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
            unsigned int idx;
            while (tmp != NULL) {
              idx = tabla->hash(tmp->data) % nuevaCapacidad;
              nuevosElems[idx].datos = glist_agregar_inicio(nuevosElems[idx].datos, tmp->data, tabla->copia);
              assert(nuevosElems[idx].datos != NULL);
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
