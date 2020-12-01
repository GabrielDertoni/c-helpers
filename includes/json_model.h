/**
 * Json Model Module
 *
 * A module designed to parse only required information from a json structure
 * into some C types that are described using the Generic module.
 *
 * NAMING CONVETIONS:
 *
 * [ Parsing Functions ]
 *
 * parser_result_t json_parse_model_X(
 *     const char *ptr,
 *     const char **endptr,
 *     void *data,
 *     const generic_Y_t *generic,
 *     const int options
 * );
 *
 * where X is the json type that will be modeled by the generic counterpart Y.
 *
 * The functions with this type of definition will try to parse a specific json
 * type X from the string pointed by ptr. If it is successfull and generic is
 * not NULL, generic will be used to fill up the memory pointed by data. If
 * generic is NULL and the options allow for PARSE_AT_LEAST, the function will
 * be successfull but nothing will be written to data. Otherwise the function
 * will fail.
 *
 * Therefore if one wishes to simply find the end of a json token, one could do
 *
 * char *endptr = ptr;
 * json_parse_model_X(ptr, &endptr, NULL, NULL, PARSE_AT_LEAST);
 *
 * then if the function returns PARSER_SUCCESS, n = endptr - ptr will be the 
 * number of bytes read. Otherwise, if it fails, endptr - ptr = 0.
 *
 */


#ifndef __JSON_MODEL_H__
#define __JSON_MODEL_H__

#include <parsing_utils.h>
#include <generic.h>

/* Parsing options */

// Json may have fields that are not represented in the structure.
#define PARSE_AT_LEAST 0b01

// Structure may have fields that are not represented in the json.
#define PARSE_AT_MOST  0b10

// Structure may have fields that are not represented in the json, but also some
// fields in the json may not be represented in the struct.
#define PARSE_SOME     0b11

// Structure and json must match exactly.
#define PARSE_EXACTLY  0b00

// Describes every type that a json value can have.
typedef enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_UNDEFINED,
    JSON_NULL,
} json_type_t;

// Parses an entire json file.
parser_result_t json_parse_model_file(
    const char *fname,
    void *data,
    const generic_t *generic,
    const int options
);

// Parses any json value.
parser_result_t json_parse_model(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_t *generic,
    const int options
);

// Parses json values of type object. Like { "key": <value>, "key2": <value> }
parser_result_t json_parse_model_object(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_struct_t *generic,
    const int options
);

// Parses key-value pairs. Like "key": <value>
// This function has a special parameter wfields wich indicates wich fields of
// the struct have been written to. If the i-th field has been written, then
// wfields[i] = true.
parser_result_t json_parse_model_pair(
    const char *ptr,
    char **endptr,
    void *data,
    bool wfields[],
    const generic_struct_t *generic,
    const int options
);

// Parses json array. Like [ <value>, <value>, <value> ]
parser_result_t json_parse_model_array(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_struct_t *generic,
    const int options
);

// Parses json string. Like "string here"
parser_result_t json_parse_model_string(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_pointer_t *generic,
    const int options
);

// Parses json number. Like 123.456
parser_result_t json_parse_model_number(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_double_t *generic,
    const int options
);

// Parses json boolean. Like true.
parser_result_t json_parse_model_bool(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_bool_t *generic,
    const int options
);

// Parses json null. Like null.
parser_result_t json_parse_model_null(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_pointer_t *generic,
    const int options
);

// Parses json undefined. Like undefined.
parser_result_t json_parse_model_undefined(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_t *generic,
    const int options
);

#endif
