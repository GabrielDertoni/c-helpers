#ifndef __PARSING_UTILS_H__
#define __PARSING_UTILS_H__

#include <stdbool.h>

#define DIGITS "0123456789"
#define LLETTERS "abcdefghijklmnopqrstuvwxyz"
#define ULETTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define LETTERS LLETTERS ULETTERS
#define ALPHANUM LETTERS DIGITS

#define PARSER_FAIL -1
#define PARSER_SUCCESS 0

typedef short int parser_result_t;

int skip_space(char **input);
bool parse_string(char *str, char **input);
bool parse_char(char c, char **input);
char *parse_util_unescaped(char *chars, char **input);
char *parse_some(char *chars, char **input);
char *parse_until(char *stop, char **input);

#endif
