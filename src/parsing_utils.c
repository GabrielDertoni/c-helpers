#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <parsing_utils.h>

// TODO: add char *ptr and change char **input to char **endptr

int skip_space(char **input) {
    int c;
    for (c = 0; **input && strchr(" \t\r\n", **input); c++, (*input)++);
    return c;
}

bool parse_string(char *str, char **input) {
    int len = strlen(str);
    if (strncmp(str, *input, len)) return false;
    *input += len;
    return true;
}

bool parse_char(char c, char **input) {
    if (c != **input) return false;
    (*input)++;
    return true;
}

// NOTE: There may be a problem when a string is like "anything \"
char *parse_util_unescaped(char *chars, char **input) {
    char *str = *input;
    while (**input && (!strchr(chars, **input) || **input == '\\'))
        *input += **input == '\\' ? 2 : 1;

    return str;
}

char *parse_some(char *chars, char **input) {
    char *str = *input;
    while (**input && strchr(chars, **input)) (*input)++;
    return str;
}

char *parse_until(char *stop, char **input) {
    char *str = *input;
    while(**input && !strchr(stop, **input)) (*input)++;
    return str;
}
