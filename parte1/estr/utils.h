#ifndef __UTILS_H__
#define __UTILS_H__

unsigned int es_primo(int n);


unsigned int primo_mas_cercano(int n);

/**
 * Funcion de hash para strings propuesta por Kernighan & Ritchie en "The C
 * Programming Language (Second Ed.)".
 */
unsigned KRHash(char *s);

#endif /** __UTILS_H__ */