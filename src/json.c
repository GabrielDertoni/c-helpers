#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <file.h>
#include <parsing_utils.h>
#include <dict.h>
#include <generic.h>
#include <json.h>

#define PARSER_ASSERT(expr) ({ if (!(expr)) return PARSER_FAIL; })
#define PARSER_ALLOC(var) ({ (__typeof__(var))malloc(sizeof(*var)); })
#define PARSER_FREE(var) ({ free(*var); *var = NULL; })

static void json_value_delete_wrapper(void *value) {
    json_value_delete((json_value_t *)value);
}

parser_result_t json_parse_file(char *fname, json_value_t **parsed) {
    char *contents = fread_contents(fname);
    char *ptr = contents;
    parser_result_t result = json_parse_value(&ptr, parsed);

    skip_space(&ptr);
    if (*ptr != '\0') {
        json_value_delete(*parsed);
        free(contents);
        fprintf(stderr, "File is not valid json\n");
        return PARSER_FAIL;
    }
    free(contents);
    return result;
}

parser_result_t json_parse_value(char **input, json_value_t **parsed) {
    parser_result_t result;
    if ((result = json_parse_null     (input, (     json_null_t **)parsed)) == PARSER_SUCCESS ||
        (result = json_parse_undefined(input, (json_undefined_t **)parsed)) == PARSER_SUCCESS ||
        (result = json_parse_bool     (input, (     json_bool_t **)parsed)) == PARSER_SUCCESS ||
        (result = json_parse_number   (input, (   json_number_t **)parsed)) == PARSER_SUCCESS ||
        (result = json_parse_string   (input, (   json_string_t **)parsed)) == PARSER_SUCCESS ||
        (result = json_parse_array    (input, (    json_array_t **)parsed)) == PARSER_SUCCESS ||
        (result = json_parse_object   (input, (   json_object_t **)parsed)) == PARSER_SUCCESS )
        return PARSER_SUCCESS;

    return result;
}

parser_result_t json_parse_pair(char **input, json_pair_t *parsed) {
    char *ptr = *input;
    char *str;
    json_value_t *value;

    skip_space(&ptr);
    if (!parse_char('"', &ptr)) return PARSER_FAIL;

    str = parse_util_unescaped("\"", &ptr);
    size_t nchars = ptr - str;

    if (!parse_char('"', &ptr)) return PARSER_FAIL;
    skip_space(&ptr);

    if (!parse_char(':', &ptr)) return PARSER_FAIL;
    skip_space(&ptr);

    if (json_parse_value(&ptr, &value) != PARSER_SUCCESS) return PARSER_FAIL;

    parsed->key = strndup(str, nchars);
    parsed->value = value;

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_object(char **input, json_object_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;

    skip_space(&ptr);
    PARSER_ASSERT(parse_char('{', &ptr));

    dict_t *pairs = dict_create();

    json_pair_t pair;
    skip_space(&ptr);
    while (json_parse_pair(&ptr, &pair) == PARSER_SUCCESS) {
        skip_space(&ptr);
        dict_insert(pairs, pair.key, pair.value);
        free(pair.key);
        if (!parse_char(',', &ptr))
            break;

        skip_space(&ptr);
    }

    if (!parse_char('}', &ptr)) {
        dict_delete(pairs, json_value_delete_wrapper);
        return PARSER_FAIL;
    }
    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_OBJECT;
    (*parsed)->pairs = pairs;

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_array(char **input, json_array_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;

    json_value_t **vec = NULL;
    size_t alloc = 0;
    int i = 0;

    skip_space(&ptr);
    PARSER_ASSERT(parse_char('[', &ptr));

    json_value_t *item;
    skip_space(&ptr);
    while (json_parse_value(&ptr, &item) == PARSER_SUCCESS) {
        skip_space(&ptr);
        if (i >= alloc) {
            if ((alloc *= 2) == 0) alloc = 8 * sizeof(json_value_t *);
            vec = (json_value_t **)realloc(vec, alloc);
        }
        vec[i++] = item;
        if (!parse_char(',', &ptr))
            break;

        skip_space(&ptr);
    }

    if (!parse_char(']', &ptr)) {
        for (int j = 0; j < i; j++)
            json_value_delete(vec[j]);

        free(vec);
        return PARSER_FAIL;
    }
    
    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_ARRAY;
    (*parsed)->values = vec;
    (*parsed)->size = i;

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_string(char **input, json_string_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;
    char *str;

    skip_space(&ptr);
    PARSER_ASSERT(parse_char('"', &ptr));

    str = parse_util_unescaped("\"", &ptr);

    PARSER_ASSERT(parse_char('"', &ptr));

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_STRING;
    (*parsed)->content = strndup(str, ptr - str - 1);

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_number(char **input, json_number_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;
    double number = 0;

    skip_space(&ptr);
    char *endptr;
    number = strtod(ptr, &endptr);

    if (ptr == endptr) return PARSER_FAIL;
    ptr = endptr;

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_NUMBER;
    (*parsed)->number = number;

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_bool(char **input, json_bool_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;
    bool value;

    skip_space(&ptr);
    if (parse_string("true", &ptr))
        value = true;
    else if (parse_string("false", &ptr))
        value = false;
    else
        return PARSER_FAIL;

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_BOOL;
    (*parsed)->value = value;

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_null(char **input, json_null_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;

    skip_space(&ptr);
    PARSER_ASSERT(parse_string("null", &ptr));

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_NULL;

    *input = ptr;
    return PARSER_SUCCESS;
}

parser_result_t json_parse_undefined(char **input, json_undefined_t **parsed) {
    *parsed = NULL;
    char *ptr = *input;

    skip_space(&ptr);
    PARSER_ASSERT(parse_string("undefined", &ptr));

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->self.type = JSON_UNDEFINED;

    *input = ptr;
    return PARSER_SUCCESS;
}

size_t json_sizeof(json_value_t *value) {
    switch(jtype(value)) {
        case JSON_OBJECT:
            return sizeof(json_object_t);

        case JSON_ARRAY:
            return sizeof(json_array_t);

        case JSON_STRING:
            return sizeof(json_string_t);

        case JSON_NUMBER:
            return sizeof(json_number_t);

        case JSON_BOOL:
            return sizeof(json_bool_t);

        case JSON_UNDEFINED:
            return sizeof(json_undefined_t);

        case JSON_NULL:
            return sizeof(json_null_t);
    }
    // This should never happen.
    return 0;
}

void json_value_delete(json_value_t *value) {
    if (!value) return;
    switch (jtype(value)) {
        case JSON_OBJECT:
            dict_delete(jas_obj(value)->pairs, json_value_delete_wrapper);
            break;

        case JSON_ARRAY:
        {
            json_array_t *array = (json_array_t *)value;
            for (int i = 0; i < array->size; i++)
                json_value_delete(array->values[i]);

            free(array->values);
            break;
        }

        case JSON_STRING:
            free(((json_string_t *)value)->content);
            break;

        case JSON_NUMBER:
        case JSON_BOOL:
        case JSON_UNDEFINED:
        case JSON_NULL:
        default:
            break;
    }
    free(value);
}

json_value_t *json_object_get(json_object_t *obj, char *key) {
    return (json_value_t *)dict_get(obj->pairs, key);
}

enum idx_type {
    IDX_KEY,
    IDX_NUMBER,
};

union idx_value {
    char *key;
    int number;
};

typedef struct {
    enum idx_type type;
    union idx_value value;
} indexing_t;

// Tenta ler sintaxe obj[1234]
static parser_result_t parse_js_index_idx_num(char **input, indexing_t **parsed) {
    char *ptr = *input;
    char *endptr;
    long int num;

    PARSER_ASSERT(parse_char('[', &ptr));
    skip_space(&ptr);

    num = strtol(ptr, &endptr, 10);

    PARSER_ASSERT(endptr > ptr);
    ptr = endptr;
    skip_space(&ptr);
    PARSER_ASSERT(parse_char(']', &ptr));

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->type = IDX_NUMBER;
    (*parsed)->value.number = num;

    *input = ptr;
    return PARSER_SUCCESS;
}

// Tenta ler sintaxe obj["chave aqui"] (sintaxe obj["1234"] != obj[1234] nesse contexto)
static parser_result_t parse_js_index_idx_quo(char **input, indexing_t **parsed) {
    char *ptr = *input;
    char *str, *endptr;

    PARSER_ASSERT(parse_char('[', &ptr));
    skip_space(&ptr);
    PARSER_ASSERT(parse_char('"', &ptr));

    endptr = ptr;
    str = parse_util_unescaped("\"", &endptr);

    PARSER_ASSERT(endptr > ptr);
    ptr = endptr;
    PARSER_ASSERT(parse_char('"', &ptr));
    skip_space(&ptr);
    PARSER_ASSERT(parse_char(']', &ptr));

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->type = IDX_KEY;
    (*parsed)->value.key = strndup(str, endptr - str);

    *input = ptr;
    return PARSER_SUCCESS;
}

// Tenta ler sintaxe obj.chave
static parser_result_t parse_js_index_dot(char **input, indexing_t **parsed) {
    char *ptr = *input;
    char *str, *endptr;

    PARSER_ASSERT(parse_char('.', &ptr));

    endptr = ptr;
    str = parse_until(".[\r\n", &endptr);

    PARSER_ASSERT(endptr > ptr);
    ptr = endptr;

    *parsed = PARSER_ALLOC(*parsed);
    (*parsed)->type = IDX_KEY;
    (*parsed)->value.key = strndup(str, endptr - str);

    *input = ptr;
    return PARSER_SUCCESS;
}

// Tenta ler sintaxe obj.chave, obj[1234] ou obj["chave"].
static parser_result_t parse_js_index(char **input, indexing_t **parsed) {
    parser_result_t result;
    if ((result = parse_js_index_idx_quo(input, parsed)) == PARSER_SUCCESS ||
        (result = parse_js_index_idx_num(input, parsed)) == PARSER_SUCCESS ||
        (result = parse_js_index_dot    (input, parsed)) == PARSER_SUCCESS )
        return PARSER_SUCCESS;

    return result;
}

static void js_index_delete(indexing_t *indexing) {
    switch(indexing->type) {
        case IDX_KEY:
            free(indexing->value.key);
            break;

        case IDX_NUMBER:
        default:
            break;
    }
    free(indexing);
}

json_value_t *json_js_index(json_value_t *value, char *indexing) {
    char *ptr = indexing;
    indexing_t *index;
    json_value_t *curr = value;

    while (curr && parse_js_index(&ptr, &index) == PARSER_SUCCESS) {
        switch(index->type) {
            case IDX_KEY:
                if (jtype(curr) != JSON_OBJECT) {
                    free(index);
                    return NULL;
                }
                curr = dict_get(jas_obj(curr)->pairs, index->value.key);
                break;

            case IDX_NUMBER:
                if (jtype(curr) != JSON_ARRAY || index->value.number >= jas_arr(curr)->size) {
                    free(index);
                    return NULL;
                }
                curr = jas_arr(curr)->values[index->value.number];
                break;
        }

        js_index_delete(index);
    }

    return curr;
}

