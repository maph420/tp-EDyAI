#include "utils.h"

/** Funcion para chequear si el
 * numero dado es primo o no.
 */
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
  if (es_primo(n)) return n;
  while (!es_primo(n)) n++;
  return n;
}

/**
 * Funcion de hash para strings propuesta por Kernighan & Ritchie en "The C
 * Programming Language (Second Ed.)".
 */
unsigned int KRHash(char *s) {
  unsigned int hashval;
  for (hashval = 0; *s != '\0'; ++s) {
    hashval = *s + 31 * hashval;
  }
  return hashval;
}