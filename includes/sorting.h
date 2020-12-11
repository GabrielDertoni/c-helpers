/**
 * Módulo Sorting.
 *
 * Esse módulo define funções de ordenação, de busca binária e a readline. As
 * funções de ordenação e busca são genéricas para qualquer tipo de vetor.
 */

#ifndef __SORTING_H__
#define __SORTING_H__

#include <stdio.h>
#include <stdlib.h>

// Ponteiro de função que, a partir de um elemento do vetor, é capaz de recuperar
// a chave que o representa. Além disso a função recebe um segundo valor opcional
// que pode conter informações adicionais necessárias.
typedef unsigned int (*get_key_t)(void *, void *);

// Ponteiro de função que comapra dois elementos de um vetor. Deve retornar > 0
// se o primeiro for maior que o segundo, < 0 se o segundo for maior e = 0 se os
// dois forem iguais.
typedef int (*comp_t)(void *, void *);

// Função que ordena um vetor de valores arbitrários usando radix sort de base 256.
void radix256_sort_with(void *vec,         // Vetor a ser ordenado.
                        size_t nmemb,      // Número de elementos (membros) do vetor.
                        size_t elsize,     // Tamanho de cada elemento (em bytes).
                        get_key_t get_key, // Função que, recupera a chave de um elemento.
                        void *arg          // Argumento adicional a função get_key.
                        );

// Função que ordena um vetor de valores arbitrários usando quick sort com primeiro
// elemento de pivot.
void quick_sort_with(void *vec,     // Vetor a ser ordenado.
                     size_t nmemb,  // Número de elementos (membros) do vetor.
                     size_t elsize, // Tamanho de cada elemento (em bytes).
                     comp_t comp    // Função que compara dois elementos.
                     );

void heap_sort_with(void *vec,
                    size_t nmemb,
                    size_t elsize,
                    comp_t comp
                    );

void merge_sort_with(void *vec,
                     size_t nmemb,
                     size_t elsize,
                     comp_t comp
                     );

void insertion_sort_with(void *vec,
                         size_t nmemb,
                         size_t elsize,
                         comp_t comp
                         );

void binary_insertion_sort_with(void *vec,
                                size_t nmemb,
                                size_t elsize,
                                comp_t comp
                                );

void tim_sort_with(void *vec,
                   size_t nmemb,
                   size_t elsize,
                   comp_t comp
                   );

void *lower_bound(void *search,
                  void *vec,
                  size_t nmemb,
                  size_t elsize,
                  comp_t comp
                  );

void *upper_bound(void *search,
                  void *vec,
                  size_t nmemb,
                  size_t elsize,
                  comp_t comp
                  );

// Função que performa busca binária num vetor de valores com tipos arbitrários.
// Se a busca for bem sucedida, um ponteiro ao elemento encontrado será retornado,
// caso contrário, NULL será retornado.
void *binary_search(void *search,  // O elemento a se procurar.
                    void *vec,     // O vetor ordenado no qual procurar.
                    size_t nmemb,  // O número de elementos do vetor.
                    size_t elsize, // O tamanho de cada elemento (em bytes).
                    comp_t comp    // A função de comparação. O primeiro argumento
                    );             // sempre será search e o segundo um elemento.

#endif
