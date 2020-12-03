#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

// Macro para alocação de memória
#define CHUNK 32

// Garante que ptr terá um espaço a mais alocado que used
void *maybeRealloc(void *ptr, const size_t used, size_t *alloc);

// Lê uma linha de um arquivo.
char *readline(FILE *fp);

#endif
