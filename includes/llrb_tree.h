#ifndef __LLRB_TREE_H__
#define __LLRB_TREE_H__

#include <stdlib.h>
#include <stdbool.h>

// Tipo parcial para TAD da LLRB tree.
typedef struct _llrb_tree llrb_tree_t;

// Função que compara dois elementos.
typedef int (*comp_fn_t)(void *, void *);

// Função que libera um elemento.
typedef void (*free_fn_t)(void *);

// Função que recebe um elemento e algum outro argumento opicional e performa
// alguma ação com o elemento (geralmente IO).
typedef void (*foreach_fn_t)(void *, void *);

/**
 * Cria uma LLRB tree com elementos de algum tamanho arbitrário. Tempo O(1).
 *
 * @param elsize - tamanho de cada elemento que será armazenado na árvore.
 * @param comp - função de comparação usada para comparar elementos.
 * @return uma nova LLRB tree. [ownership]
 */
llrb_tree_t *llrb_create(size_t elsize, comp_fn_t comp);

/**
 * Libera a memória alocada para a LLRB tree.
 *
 * @param tree - a árvore a ser liberada. [ownership]
 * @param free_fn - uma função que libera um ponteiro de um elemento.
 */
void llrb_delete(llrb_tree_t *tree, free_fn_t free_fn);

/**
 * Procura por algum elemento dentro da LLRB tree. Busca binária, tempo O(log n).
 * NOTE: A função de comparação `comp` será sempre executada com `element` de
 *       primeiro argumento.
 *
 * @param tree - a árvore no qual performar a busca. [ref]
 * @param element - o elemento pelo qual procurar.
 * @return um ponteiro ao elemento desejado caso encontrado. NULL caso contrário. [ref]
 */
void *llrb_search(llrb_tree_t *tree, void *element);

/**
 * Insere um novo elemento na LLRB tree. Aloca o espaço necessário para `element`
 * e o copia para dentro da árvore.
 *
 * @param tree - a árvore no qual inserir o elemento. [mut ref]
 * @param element - o elemento a ser inserido. [ref]
 * @return true se o elemento foi inserido corretamente, false se for duplicado.
 */
bool llrb_insert(llrb_tree_t *tree, void *element);
// bool llrb_remove(llrb_tree_t *tree, void *element, free_fn_t free);

/**
 * Retorna o elemento sucessor em-ordem na árvore.
 *
 * @param tree - a árvore do qual pegar o sucessor. [ref]
 * @param element - o elemento contido na árvore utilizado para encontrar o sucessor. [ref]
 * @return NULL se `element` é NULL ou não está contido na árvore ou o sucessor. [ref]
 */
void *llrb_successor(llrb_tree_t *tree, void *element);

/**
 * Retorna o elemento predecessor em-ordem na árvore.
 *
 * @param tree - a árvore do qual pegar o predecessor. [ref]
 * @param element - o elemento contido na árvore utilizado para encontrar o predecessor. [ref]
 * @return NULL se `element` é NULL ou não está contido na árvore ou o predecessor. [ref]
 */
void *llrb_predecessor(llrb_tree_t *tree, void *element);

/**
 * Retorna o elemento máximo da árvore.
 *
 * @param tree - a árvore do qual pegar o elemento máximo. [ref]
 * @return NULL se a árvore estiver vazia, ou o elemento máximo. [ref]
 */
void *llrb_max(llrb_tree_t *tree);

/**
 * Retorna o elemento minimo da árvore.
 *
 * @param tree - a árvore do qual pegar o elemento minimo. [ref]
 * @return NULL se a árvore estiver vazia, ou o element minimo. [ref]
 */
void *llrb_min(llrb_tree_t *tree);

/**
 * Atravessa a árvore pre-ordem e executa `func` para cada elemento.
 *
 * @param tree - a árvore a ser atravessada. [ref]
 * @param func - a função a ser executada para cada elemento. [ref]
 * @param args - argumento adicional a ser repassado para `func`. [mut ref]
 */
void llrb_preorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args);

/**
 * Atravessa a árvore pos-ordem e executa `func` para cada elemento.
 *
 * @param tree - a árvore a ser atravessada. [ref]
 * @param func - a função a ser executada para cada elemento. [ref]
 * @param args - argumento adicional a ser repassado para `func`. [mut ref]
 */
void llrb_postorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args);

/**
 * Atravessa a árvore em-ordem e executa `func` para cada elemento.
 *
 * @param tree - a árvore a ser atravessada. [ref]
 * @param func - a função a ser executada para cada elemento. [ref]
 * @param args - argumento adicional a ser repassado para `func`. [mut ref]
 */
void llrb_inorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args);

#endif
