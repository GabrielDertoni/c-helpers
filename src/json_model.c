#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include <parsing_utils.h>
#include <generic.h>
#include <file.h>
#include <json_model.h>

// Shorthands for parser results.
#define OK PARSER_SUCCESS
#define FAIL PARSER_FAIL

// Assertion macros.
#define ASSERT_OK(result) if ((result) != OK) return FAIL
#define ASSERT(value) if (!(value)) return FAIL

// Offsets the memory addres by n bytes.
#define memoff(v, n) ((void *)((byte_t *)(v) + (n)))

// Allocates double the previous allocation ammoung (alloc) on ptr.
#define ALLOC_DOUBLE(ptr, alloc, chunk) ({ \
    if ((alloc *= 2) == 0) alloc = (chunk); \
    ptr = realloc(ptr, alloc); \
})

// Just for representing a single byte.
typedef unsigned char byte_t;

static bool jtype_gen_eq(const json_type_t json, const generic_t *generic) {
    switch (json) {
        case JSON_OBJECT:
            return gen_type(generic) == GTYPE_STRUCT;

        case JSON_ARRAY:
            // A json array is modeled as
            /* struct {
             *  X *vector;
             *  int size;
             * };
             * where X is some type. When modeling a json array, different types
             * within an array are not allowed.
             */
            return    gen_type(generic) == GTYPE_STRUCT
                   && gas_struct(generic)->nfields == 2
                   && gen_type(gas_struct(generic)->fields[0].type) == GTYPE_POINTER
                   // && !strcmp(gas_struct(generic)->fields[0].name, "vector")
                   && gen_type(gas_struct(generic)->fields[1].type) == GTYPE_INT;
                   // && !strcmp(gas_struct(generic)->fields[1].name, "size");

        case JSON_STRING:
            return gen_type(generic) == GTYPE_POINTER &&
                   gen_type(gas_ptr(generic)->inner) == GTYPE_CHAR;

        case JSON_NUMBER:
            return gen_type(generic) == GTYPE_DOUBLE;

        case JSON_BOOL:
            return gen_type(generic) == GTYPE_BOOL;

        case JSON_NULL:
        case JSON_UNDEFINED:
            return false;
    }
    return false; // Should never happen.
}

parser_result_t json_parse_model_file(
    const char *fname,
    void *data,
    const generic_t *generic,
    const int options
) {
    char *contents = fread_contents((char *)fname);
    char *endptr;

    parser_result_t result = json_parse_model(contents, &endptr, data, generic, options);
    skip_space(&endptr);
    if (*endptr != '\0') result = FAIL;

    free(contents);
    return result;
}

parser_result_t json_parse_model_poiter(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_pointer_t *gen,
    const int opt
) {
    void *alloc = malloc(generic_sizeof(gen->inner));
    ASSERT_OK(json_parse_model(ptr, endptr, alloc, gen->inner, opt));
    memcpy(data, &alloc, sizeof(void *));

    return OK;
}

parser_result_t json_parse_model(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_t *gen,
    const int opt
) {
    parser_result_t result;
    if (gen) {
        switch (gen_type(gen)) {
            case GTYPE_POINTER:
                if (jtype_gen_eq(JSON_STRING, gen))
                    result = json_parse_model_string(ptr, endptr, data, gas_ptr(gen), opt);
                else
                    result = json_parse_model_poiter(ptr, endptr, data, gas_ptr(gen), opt);

                if (result != OK)
                    result = json_parse_model_null(ptr, endptr, data, gas_ptr(gen), opt);

                return result;

            case GTYPE_STRUCT:
                if (jtype_gen_eq(JSON_ARRAY, gen))
                    return json_parse_model_array(ptr, endptr, data, gas_struct(gen), opt);
                else
                    return json_parse_model_object(ptr, endptr, data, gas_struct(gen), opt);

            case GTYPE_DOUBLE:
                return json_parse_model_number(ptr, endptr, data, gas_double(gen), opt);

            case GTYPE_BOOL:
                return json_parse_model_bool(ptr, endptr, data, gas_bool(gen), opt);

            // TODO
            case GTYPE_ARRAY:

            default:
                if (!(opt & PARSE_AT_MOST)) return FAIL;
                return json_parse_model_undefined(ptr, endptr, data, gen, opt);
        }
        return OK;
    } else {
        // If gen is NULL, parse json based only on if it is valid.
        if ((result = json_parse_model_null     (ptr, endptr, NULL, NULL, opt)) == OK ||
            (result = json_parse_model_undefined(ptr, endptr, NULL, NULL, opt)) == OK ||
            (result = json_parse_model_bool     (ptr, endptr, NULL, NULL, opt)) == OK ||
            (result = json_parse_model_number   (ptr, endptr, NULL, NULL, opt)) == OK ||
            (result = json_parse_model_string   (ptr, endptr, NULL, NULL, opt)) == OK ||
            (result = json_parse_model_array    (ptr, endptr, NULL, NULL, opt)) == OK ||
            (result = json_parse_model_object   (ptr, endptr, NULL, NULL, opt)) == OK )
            return OK;

        return result;
    }
}

parser_result_t json_parse_model_pair(
    const char *ptr,
    char **endptr,
    void *data,
    bool wfields[],
    const generic_struct_t *gen,
    const int opt
) {
    char *s = (char *)ptr;
    char *str;

    if (endptr) *endptr = s;

    skip_space(&s);
    ASSERT(parse_char('"', &s));

    str = parse_util_unescaped("\"", &s);
    size_t nchars = s - str;

    ASSERT(parse_char('"', &s));
    skip_space(&s);

    ASSERT(parse_char(':', &s));
    skip_space(&s);

    // Finds the correct field.
    generic_field_t *field = NULL;
    size_t off = 0;
    if (gen) {
        field = gen->fields;
        int i;
        for (i = 0; i < gen->nfields; i++, off += generic_sizeof((field++)->type))
            if (strlen(field->name) == nchars &&
                !strncmp(str, field->name, nchars))
                break;

        if (i == gen->nfields) field = NULL;
        // If the field is already written, free its contents to overwrite.
        else if (wfields[i]) gfree(memoff(data, off), field->type);
        // Set the field as written.
        else wfields[i] = true;
    }
    ASSERT_OK(json_parse_model(s, &s, memoff(data, off), field ? field->type : NULL, opt));

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_object(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_struct_t *gen,
    const int options
) {
    char *s = (char *)ptr;

    if (endptr) *endptr = s;

    ASSERT(gen || (options & PARSE_AT_LEAST));

    skip_space(&s);
    ASSERT(parse_char('{', &s));

    bool wfields[gen ? gen->nfields : 0];
    memset(wfields, 0, sizeof(wfields));

    skip_space(&s);
    while (json_parse_model_pair(s, &s, data, wfields, gen, options) == OK) {
        skip_space(&s);
        if (!parse_char(',', &s))
            break;

        skip_space(&s);
    }

    if (!parse_char('}', &s)) {
        // NOTE: Maybe not all fields have been read before it finds an error.
        size_t off = 0;
        generic_field_t *field = gen->fields;
        for (int i = 0; i < gen->nfields; i++, off += generic_sizeof((field++)->type))
            if (wfields[i]) gfree(memoff(data, off), field->type);

        return FAIL;
    }

    // If not PARSE_AT_MOST, means that every field that is in the struct, must
    // also be in the json. In that case, if some field has not been written to,
    // fail. If a field has not been filled in but its a pointer, set it to NULL.
    if (gen) {
        for (int i = 0; i < gen->nfields; i++) {
            if (!wfields[i]) {
                if (!(options & PARSE_AT_MOST))
                    return FAIL;
                else if (gen_type(gen->fields[i].type) == GTYPE_POINTER)
                    memset(gstruct_get_field_by_index(data, i, gen), 0, sizeof(void *));
            }
        }
    }

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_array(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_struct_t *generic,
    const int options
) {
    char *s = (char *)ptr;

    if (endptr) *endptr = s;

    ASSERT(generic || (options & PARSE_AT_LEAST));
    // Assert that if generic is not NULL, it is a valid vector.
    ASSERT(!generic || gis_vec(generic));

    // First field of a generic vector is where elements are stored.
    generic_t *gen_el;
    size_t elsize;

    if (generic) {
        gen_el = gas_ptr(generic->fields[0].type)->inner;
        elsize = generic_sizeof(gen_el);
    } else {
        gen_el = NULL;
        elsize = 0;
    }

    void *vec = NULL;
    size_t alloc = 0;
    int i = 0;

    skip_space(&s);
    ASSERT(parse_char('[', &s));

    byte_t item[elsize];
    skip_space(&s);
    for (i = 0; json_parse_model(s, &s, &item, gen_el, options) == OK;) {
        skip_space(&s);
        if (generic) {
            if (i * elsize >= alloc) ALLOC_DOUBLE(vec, alloc, 2 * elsize);
            memcpy(memoff(vec, i * elsize), &item, elsize);
        }
        i++;

        if (!parse_char(',', &s)) break;
        skip_space(&s);
    }

    if (!parse_char(']', &s)) {
        if (generic) {
            for (int j = 0; j < i; j++)
                gfree(memoff(vec, j * elsize), gen_el);

            free(vec);
        }
        return FAIL;
    }

    if (generic) {
        memcpy(data, &vec, sizeof(void *));
        memcpy(memoff(data, sizeof(void *)), &i, sizeof(int));
    }

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_string(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_string_t *generic,
    const int options
) {
    char *s = (char *)ptr;
    char *str;

    if (endptr) *endptr = s;

    ASSERT(generic || (options & PARSE_AT_LEAST));

    skip_space(&s);
    ASSERT(parse_char('"', &s));

    str = parse_util_unescaped("\"", &s);

    ASSERT(parse_char('"', &s));

    if (generic) {
        str = strndup(str, s - str - 1);
        memcpy(data, &str, sizeof(char *));
    }

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_number(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_double_t *generic,
    const int options
) {
    char *s = (char *)ptr;
    double number;
    char *numend;

    if (endptr) *endptr = s;

    ASSERT(generic || (options & PARSE_AT_LEAST));

    skip_space(&s);
    number = strtod(ptr, &numend);

    if (s == numend) return FAIL;
    s = numend;

    if (generic) memcpy(data, &number, sizeof(double));

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_bool(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_bool_t *generic,
    const int options
) {
    char *s = (char *)ptr;
    bool value;

    if (endptr) *endptr = s;

    ASSERT(generic || (options & PARSE_AT_LEAST));

    skip_space(&s);
    if (parse_string("true", &s))
        value = true;
    else if (parse_string("false", &s))
        value = false;
    else
        return FAIL;

    if (generic) memcpy(data, &value, sizeof(bool));

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_null(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_pointer_t *generic,
    const int options
) {
    char *s = (char *)ptr;

    if (endptr) *endptr = s;

    ASSERT(generic || (options & PARSE_AT_LEAST));

    skip_space(&s);
    ASSERT(parse_string("null", &s));

    if (generic)
        memset(data, 0, sizeof(void *));

    if (endptr) *endptr = s;
    return OK;
}

parser_result_t json_parse_model_undefined(
    const char *ptr,
    char **endptr,
    void *data,
    const generic_t *generic,
    const int options
) {
    char *s = (char *)ptr;

    if (endptr) *endptr = s;

    ASSERT(generic || (options & PARSE_AT_LEAST));

    skip_space(&s);
    ASSERT(parse_string("undefined", &s));

    // An undefined value must not be defined in the struct, no matter what.
    if (endptr) *endptr = s;
    return OK;
}
