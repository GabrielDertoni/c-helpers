#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <generic.h>

#include <stdio.h>

#define memoff(v, n) ((void *)((byte_t *)(v) + (n)))

typedef unsigned char byte_t;

/* Global variables */

generic_bool_t   generic_bool   = GTYPE_BOOL;
generic_int_t    generic_int    = GTYPE_INT;
generic_float_t  generic_float  = GTYPE_FLOAT;
generic_double_t generic_double = GTYPE_DOUBLE;
generic_char_t   generic_char   = GTYPE_CHAR;

generic_string_t generic_str = {
    .type = GTYPE_POINTER,
    .inner = &generic_char
};

generic_pointer_t *generic_pointer_create(generic_t *inner) {
    generic_pointer_t *pointer = (generic_pointer_t *)malloc(sizeof(generic_pointer_t));
    pointer->type = GTYPE_POINTER;
    pointer->inner = inner;
    return pointer;
}

void generic_pointer_free(generic_pointer_t *generic) {
    if (generic != &generic_str) {
        generic_free(generic->inner);
        free(generic);
    }
}

void gpointer_free(void *data, generic_pointer_t *generic) {
    if (!*(void **)data) return;
    // gpointer_free is shallow.
    // gfree(*(void **)data, generic->inner);
    free(*(void **)data);
}

/* Generic Struct */

generic_field_t generic_struct_field(char *fname, generic_t *type) {
    generic_field_t field = {
        .name = strdup(fname),
        .type = type,
    };
    return field;
}

generic_struct_t *generic_struct_va_create(size_t nfields, va_list args) {
    generic_struct_t *gstruct = (generic_struct_t *)malloc(sizeof(generic_struct_t));
    gstruct->type = GTYPE_STRUCT;
    gstruct->nfields = nfields;
    gstruct->fields = (generic_field_t *)calloc(nfields, sizeof(generic_field_t));

    for (int i = 0; i < nfields; i++)
        gstruct->fields[i] = va_arg(args, generic_field_t);

    return gstruct;
}

generic_struct_t *generic_struct_create(size_t nfields, ...) {
    va_list args;
    va_start(args, nfields);
    generic_struct_t *gstruct = generic_struct_va_create(nfields, args);
    va_end(args);

    return gstruct;
}

void generic_struct_free(generic_struct_t *g) {
    generic_field_t *f = g->fields;
    for (int i = 0; i < g->nfields; i++, f++) {
        generic_free(f->type);
        free(f->name);
    }
    free(g->fields);
    free(g);
}

static size_t struct_sizeof(generic_struct_t *generic) {
    size_t size = 0;
    for (int i = 0; i < generic->nfields; i++)
        size += generic_sizeof(generic->fields[i].type);

    return size;
}

/* Operations on struct data */

void *gstruct_get_field(void *data, char *fname, generic_struct_t *generic) {
    size_t off = 0;
    generic_field_t *fields = generic->fields;

    for (int i = 0; i < generic->nfields; i++) {
        if (!strcmp(fname, fields[i].name))
            return memoff(data, off);

        off += generic_sizeof(fields[i].type);
    }
    return NULL;
}

void *gstruct_get_field_by_index(void *data, const int idx, const generic_struct_t *generic) {
    size_t off = 0;
    generic_field_t *fields = generic->fields;

    int i;
    for (i = 0; i < idx && i < generic->nfields; i++)
        off += generic_sizeof(fields[i].type);

    if (i < idx) return NULL;
    return memoff(data, off);
}

void gstruct_free(void *data, generic_struct_t *g) {
    generic_field_t *f = g->fields;
    size_t off = 0;
    for (int i = 0; i < g->nfields; i++, off += generic_sizeof((f++)->type))
        gfree(memoff(data, off), f->type);
}

/* Generic array */

generic_array_t *generic_array_create(generic_t *inner, size_t nelem) {
    generic_array_t *generic = (generic_array_t *)malloc(sizeof(generic_array_t));
    generic->type = GTYPE_ARRAY;
    generic->inner = inner;
    generic->nelem = nelem;
    return generic;
}

void generic_array_free(generic_array_t *generic) {
    generic_free(generic->inner);
    free(generic);
}

void garray_free(void *data, generic_array_t *generic) {
    const size_t elsize = generic_sizeof(generic->inner);
    for (int i = 0; i < generic->nelem; i++)
        gfree(memoff(data, i * elsize), generic->inner);
}

/* Other generics */

inline generic_int_t *generic_int_create() { return &generic_int; }
inline void generic_int_free(generic_int_t *generic) {
    if (generic != &generic_int) free(generic);
}

inline generic_float_t *generic_float_create() { return &generic_float; }
inline void generic_float_free(generic_float_t *generic) {
    if (generic != &generic_float) free(generic);
}

inline generic_double_t *generic_double_create() { return &generic_double; }
inline void generic_double_free(generic_double_t *generic) {
    if (generic != &generic_double) free(generic);
}

inline generic_char_t *generic_char_create() { return &generic_char; }
inline void generic_char_free(generic_char_t *generic) {
    if (generic != &generic_char) free(generic);
}

inline generic_bool_t *generic_bool_create() { return &generic_bool; }
inline void generic_bool_free(generic_bool_t *generic) {
    if (generic != &generic_bool) free(generic);
}

inline generic_string_t *generic_string_create() { return &generic_str; }
inline void generic_string_free(generic_string_t *generic) {
    if (generic != &generic_str) free(generic);
}


size_t generic_sizeof(generic_t *generic) {
    if (!generic) return 0;

    switch (gen_type(generic)) {
        case GTYPE_POINTER:
            return sizeof(void *);

        case GTYPE_STRUCT:
            return struct_sizeof(gas_struct(generic));

        case GTYPE_ARRAY:
            // Fixed size arrays are not allowed, therefore its always a pointer.
            return sizeof(void *);

        case GTYPE_CHAR:
            return sizeof(char);

        case GTYPE_DOUBLE:
            return sizeof(double);

        case GTYPE_FLOAT:
            return sizeof(float);

        case GTYPE_INT:
            return sizeof(int);

        case GTYPE_BOOL:
            return sizeof(bool);

        case GTYPE_VOID:
        default:
            return 0;
    }
}

void generic_free(generic_t *generic) {
    if (!generic) return;

    switch (gen_type(generic)) {
        case GTYPE_POINTER:
            generic_pointer_free(gas_ptr(generic));
            break;

        case GTYPE_STRUCT:
            generic_struct_free(gas_struct(generic));
            break;

        case GTYPE_ARRAY:
            generic_array_free(gas_arr(generic));
            break;

        case GTYPE_CHAR:
            generic_char_free(gas_char(generic));
            break;

        case GTYPE_DOUBLE:
            generic_double_free(gas_double(generic));
            break;

        case GTYPE_FLOAT:
            generic_float_free(gas_float(generic));
            break;

        case GTYPE_INT:
            generic_int_free(gas_int(generic));
            break;

        case GTYPE_BOOL:
            generic_bool_free(gas_bool(generic));
            break;

        case GTYPE_VOID:
            break;
    }
}

void gfree(void *data, generic_t *generic) {
    if (!generic) return;

    switch (gen_type(generic)) {
        case GTYPE_POINTER:
            gpointer_free(data, gas_ptr(generic));
            break;

        case GTYPE_STRUCT:
            gstruct_free(data, gas_struct(generic));
            break;

        case GTYPE_ARRAY:
            garray_free(data, gas_arr(generic));
            break;

        case GTYPE_CHAR:
        case GTYPE_DOUBLE:
        case GTYPE_FLOAT:
        case GTYPE_INT:
        case GTYPE_BOOL:
        case GTYPE_VOID:
            break;
    }
}

generic_struct_t *generic_vector_create(generic_t *inner) {
    return GEN_STRUCT(2,
        GEN_SFIELD("vector", GEN_POINTER(inner)),
        GEN_SFIELD("size"  , GEN_INT)
    );
}

/*
struct b {
    char sla;
}
struct a {
    int f1;
    double arr[5];
    struct b coisas[10];
};

generic_struct_t {
    .type = GTYPE_STRUCT,
    .fields = [
        generic_field_t {
            .inner = & GTYPE_INT,
            .name = "f1",
        },
        generic_field_t {
            .inner = generic_array_t {
                .type = GTYPE_ARRAY,
                .inner = & GTYPE_DOUBLE,
            },
            .name = "arr",
        },
        generic_field_t {
            .inner = generic_struct_t {
                .type = GTYPE_STRUCT,
                .fields = [
                    generic_field_t {
                        .inner = & GTYPE_CHAR,
                        .name = "sla",
                    },
                ],
                .nfields = 1,
            },
            .name = "coisas",
        },
    ],
    .nfields = 2,
}
*/

