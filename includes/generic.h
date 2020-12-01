/**
 *
 *
 * NAMING CONVENTIONS:
 * -------------------
 * [ Types ]
 *
 * generic_X_t:
 * The generic type that represents a C type X.
 *
 * [ Functions ]
 *
 * generic_X_<action>(generic_X_t *generic):
 * Performes <action> regearding the `generic` struct that represents type X.
 *
 * generic_<action>(generic_t *generic):
 * Performes <action> regarding the type informed by `generic`. Those functions
 * are usually only wrappers that call generic_X_<action> with X beeing the type
 * informed by `generic`.
 *
 * gX_<action>(void *data, generic_X_t *generic):
 * Performes <action> on some `data` of type X represented by `generic`.
 *
 * g<action>(void *data, generic_t *generic):
 * Performes <action> on some `data` represented by `generic`. Those functions
 * are only wrappers that call gX_<action> where X informed by `generic`.
 *
 * [ Macros ]
 *
 * GEN_X:
 * Shorthand for generic_X_create cast to generic_t*.
 *
 * gis_X:
 * Verifies if a generic is of type X. Read "generic is type X?".
 *
 * gas_X:
 * Asserts a generic is of type X, then cast it to the generic that represents X.
 * Read "interpret generic as type X".
 *
 * [ Global variables ]
 *
 * generic_X:
 * A stack allocated global variable that is defined when a generic will be used
 * very frequently so that there is no need for reallocating each time. This is
 * the case for types such as the primitive C types.
 */


#ifndef __GENERIC_H__
#define __GENERIC_H__

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#define gen_type(v) (*(generic_type_t *)(v))

#define gis_void(v)   (gen_type(v) == GTYPE_VOID  )
#define gis_bool(v)   (gen_type(v) == GTYPE_BOOL  )
#define gis_int(v)    (gen_type(v) == GTYPE_INT   )
#define gis_float(v)  (gen_type(v) == GTYPE_FLOAT )
#define gis_double(v) (gen_type(v) == GTYPE_DOUBLE)
#define gis_char(v)   (gen_type(v) == GTYPE_CHAR  )
#define gis_arr(v)    (gen_type(v) == GTYPE_ARRAY )
#define gis_struct(v) (gen_type(v) == GTYPE_STRUCT)
#define gis_ptr(v)    (gen_type(v) == GTYPE_POINTER)

// Always secure generic_X_t* -> generic_t*
#define gas_gen(v)    ((generic_t   *)(v))
#define gas_void(v)   ({ assert(gis_void  (v)); (generic_void_t    *)(v); })
#define gas_bool(v)   ({ assert(gis_bool  (v)); (generic_bool_t    *)(v); })
#define gas_int(v)    ({ assert(gis_int   (v)); (generic_int_t     *)(v); })
#define gas_float(v)  ({ assert(gis_float (v)); (generic_float_t   *)(v); })
#define gas_double(v) ({ assert(gis_double(v)); (generic_double_t  *)(v); })
#define gas_char(v)   ({ assert(gis_char  (v)); (generic_char_t    *)(v); })
#define gas_arr(v)    ({ assert(gis_arr   (v)); (generic_array_t   *)(v); })
#define gas_struct(v) ({ assert(gis_struct(v)); (generic_struct_t  *)(v); })
#define gas_ptr(v)    ({ assert(gis_ptr   (v)); (generic_pointer_t *)(v); })

#define GEN_BOOL             gas_gen(generic_bool_create())
#define GEN_INT              gas_gen(generic_int_create())
#define GEN_FLOAT            gas_gen(generic_float_create())
#define GEN_DOUBLE           gas_gen(generic_double_create())
#define GEN_CHAR             gas_gen(generic_char_create())
#define GEN_POINTER(args...) gas_gen(generic_pointer_create(args))
#define GEN_ARR(args...)     gas_gen(generic_array_create(args))
#define GEN_STRUCT(args...)  gas_gen(generic_struct_create(args))
#define GEN_SFIELD(args...)  generic_struct_field(args)

typedef enum {
    GTYPE_VOID,
    GTYPE_BOOL,
    GTYPE_INT,
    GTYPE_FLOAT,
    GTYPE_DOUBLE,
    GTYPE_CHAR,
    GTYPE_ARRAY,   // A statically allocated array.
    GTYPE_STRUCT,
    GTYPE_POINTER,
} generic_type_t;

typedef void generic_t;

typedef struct {
    generic_t *type;
    char *name;
} generic_field_t;

typedef struct {
    generic_type_t type;
    generic_field_t *fields;
    size_t nfields;
} generic_struct_t;

typedef struct {
    generic_type_t type;
    generic_t *inner;
    size_t nelem;
} generic_array_t;

typedef struct {
    generic_type_t type;
    generic_t *inner;
} generic_pointer_t;

typedef generic_type_t generic_void_t;
typedef generic_type_t generic_int_t;
typedef generic_type_t generic_float_t;
typedef generic_type_t generic_double_t;
typedef generic_type_t generic_char_t;
typedef generic_type_t generic_bool_t;

// Global variables.
extern generic_int_t    generic_int;
extern generic_float_t  generic_float;
extern generic_double_t generic_double;
extern generic_char_t   generic_char;
extern generic_bool_t   generic_bool;

/* Operations on all generics */

size_t generic_sizeof(generic_t *generic);
void generic_free(generic_t *generic);

/* Operation on data */

void *galloc(generic_pointer_t *generic);
void gfree(void *data, generic_t *generic);

/* Operations on typed generics */

generic_struct_t *generic_struct_va_create(size_t nfields, va_list args);
generic_struct_t *generic_struct_create(size_t nfields, ...);
generic_field_t generic_struct_field(char *fname, generic_t *inner);
void *gstruct_get_field(void *data, char *fname, generic_struct_t *generic);
void *gstruct_get_field_by_index(void *data, const int idx, const generic_struct_t *generic);

generic_array_t *generic_array_create(generic_t *inner, size_t nelem);
void generic_array_free(generic_array_t *generic);
void garray_free(void *data, generic_array_t *generic);

generic_pointer_t *generic_pointer_create(generic_t *inner);
void generic_pointer_free(generic_pointer_t *generic);
void gpointer_free(void *data, generic_pointer_t *generic);

generic_int_t *generic_int_create();
void generic_int_free(generic_int_t *generic);

generic_float_t *generic_float_create();
void generic_float_free(generic_float_t *generic);

generic_double_t *generic_double_create();
void generic_double_free(generic_double_t *generic);

generic_char_t *generic_char_create();
void generic_char_free(generic_char_t *generic);

generic_bool_t *generic_bool_create();
void generic_bool_free(generic_bool_t *generic);

/* Derived from generics */

/**
 * Verifies if a generic is a valid representation for a vector, that is:
 * struct {
 *  X *field0;
 *  int field1;
 *  ...
 * }
 * where X is an arbitrary type. There may be other fields afterwards.
 */
#define gis_vec(v)    ({ \
    __typeof__(v) __gen = (v); \
    generic_struct_t *__stct; \
    gis_struct(__gen) && \
    (__stct = gas_struct(__gen))->nfields >= 2 && \
    gis_ptr(__stct->fields[0].type) && \
    gis_int(__stct->fields[1].type); \
})

#define GEN_STRING gas_gen(generic_string_create())
#define GEN_VECTOR(args...) gas_gen(generic_vector_create(args))

typedef struct {
    void *vec;
    int size;
} gvec_t;

typedef generic_pointer_t generic_string_t;

// Vectors
generic_struct_t *generic_vector_create(generic_t *inner);

// Strings
extern generic_pointer_t generic_str;

generic_string_t *generic_string_create();
void generic_string_free(generic_string_t *generic);

#endif
