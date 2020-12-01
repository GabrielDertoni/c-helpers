/**
 * Módulo para manipulação de JSON.
 * Autor: Gabriel Dertoni
 * GitHub: github.com/GabrielDertoni
 */

#ifndef __JSON_H__
#define __JSON_H__

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <parsing_utils.h>
#include <dict.h>

/**
 * Conversões entre tipos de valores JSON:
 *
 * Um determinado valor json pode tomar uma de várias formas. Todos os tipos de
 * valores suportados podem ser vistos na definição de json_type_t. Cada um
 * desses tipos é representado por um struct diferente e todos possuem em comum
 * o primeiro campo que sempre será um json_self_t. Isso garante que a conversão
 * json_X_t* -> json_self_t* é sempre possível. A conversão oposta só é garantida
 * se o jtype(valor) = X.
 *
 * Para representar um valor genérico que pode assumir qualquer tipo existe o
 * json_value_t que é definido como void, já não é possível saber seu tamanho.
 * A conversão json_value_t* <-> json_self_t* sempre é garantida. Entretanto
 * vale notar que json_self_t representa apenas parte da memória de json_value_t
 * e portanto sizeof(json_self_t) não pode ser usado para memcpy de json_value_t.
 * Para isso, deve ser usado json_sizeof().
 */

#define jtype(value) (((json_self_t *)value)->type)

#define jis_obj(v)   (jtype(v) == JSON_OBJECT   )
#define jis_arr(v)   (jtype(v) == JSON_ARRAY    )
#define jis_str(v)   (jtype(v) == JSON_STRING   )
#define jis_num(v)   (jtype(v) == JSON_NUMBER   )
#define jis_bool(v)  (jtype(v) == JSON_BOOL     )
#define jis_undef(v) (jtype(v) == JSON_UNDEFINED)
#define jis_null(v)  (jtype(v) == JSON_NULL     )

// Macros para conversão de json_value_t -> json_X_t.
#define jas_val(v) ((json_value_t *)v) // Sempre é seguro.
#define jas_obj(v)   ({ assert(jis_obj  (v)); (json_object_t    *)v; })
#define jas_arr(v)   ({ assert(jis_arr  (v)); (json_array_t     *)v; })
#define jas_str(v)   ({ assert(jis_str  (v)); (json_string_t    *)v; })
#define jas_num(v)   ({ assert(jis_num  (v)); (json_number_t    *)v; })
#define jas_bool(v)  ({ assert(jis_bool (v)); (json_bool_t      *)v; })
#define jas_undef(v) ({ assert(jis_undef(v)); (json_undefined_t *)v; })
#define jas_null(v)  ({ assert(jis_null (v)); (json_null_t      *)v; })

typedef enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_UNDEFINED,
    JSON_NULL,
} json_type_t;

typedef void json_value_t;

typedef struct {
    json_type_t type;
} json_self_t;

typedef struct {
    char *key;
    json_value_t *value;
} json_pair_t;

typedef struct {
    json_self_t self;
    dict_t *pairs;
} json_object_t;

typedef struct {
    json_self_t self;
    json_value_t **values;
    uint size;
} json_array_t;

typedef struct {
    json_self_t self;
    char *content;
} json_string_t;

typedef struct {
    json_self_t self;
    double number;
} json_number_t;

typedef struct {
    json_self_t self;
    bool value;
} json_bool_t;

typedef struct {
    json_self_t self;
} json_null_t;

typedef struct {
    json_self_t self;
} json_undefined_t;


parser_result_t json_parse_file(char *fname, json_value_t **parsed);

parser_result_t json_parse_value(char **input, json_value_t **parsed);
parser_result_t json_parse_object(char **input, json_object_t **parsed);
parser_result_t json_parse_array(char **input, json_array_t **parsed);
parser_result_t json_parse_string(char **input, json_string_t **parsed);
parser_result_t json_parse_number(char **input, json_number_t **parsed);
parser_result_t json_parse_bool(char **input, json_bool_t **parsed);
parser_result_t json_parse_null(char **input, json_null_t **parsed);
parser_result_t json_parse_undefined(char **input, json_undefined_t **parsed);

void json_value_delete(json_value_t *value);
size_t json_sizeof(json_value_t *value);

json_value_t *json_object_get(json_object_t *obj, char *key);
json_value_t *json_js_index(json_value_t *value, char *indexing);

#endif
