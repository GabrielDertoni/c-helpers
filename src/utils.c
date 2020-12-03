#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

void *maybeRealloc(void *ptr, const size_t used, size_t *alloc) {
    if (*alloc == 0)
        return realloc(ptr, *alloc = CHUNK);

    if (used >= *alloc)
        return realloc(ptr, *alloc *= 2);

    if (used < *alloc / 4 && *alloc > CHUNK)
        return realloc(ptr, *alloc /= 2);
    
    return ptr;
}

char *readline(FILE *fp) {
    char *str = NULL;
    int i, c;
    size_t alloc;
    while (strchr("\r\n", c = getc(fp)) && !feof(fp));
    for (i = 0, alloc = 0; !strchr("\r\n", c) && !feof(fp); i++, c = getc(fp)) {
        str = maybeRealloc(str, (i + 1) * sizeof(char), &alloc);
        str[i] = c;
    }

    if (c != EOF) ungetc(c, fp);
    if (alloc == 0) return NULL;
    str[i] = '\0';
    return realloc(str, (i + 1) * sizeof(char));
}

