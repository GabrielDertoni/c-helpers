#include <stdio.h>
#include <stdlib.h>
#include <json.h>

#define ASSERT_OK(result) if ((result) != OK) return FAIL
#define ASSERT(value) if (!(value)) return FAIL

#define memoff(v, n) ((void *)((byte_t *)(v) + (n)))

typedef unsigned char byte_t;

static bool jtype_gentype_eq(json_value_t *json, generic_t *generic) {
    switch (jtype(json)) {
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

parser_result_t json_model_pointer(json_value_t *json, generic_pointer_t *generic, void *data, int options) {
    void *alloc = malloc(generic_sizeof(generic->inner));
    ASSERT_OK(json_model(json, generic->inner, alloc, options));
    memcpy(data, &alloc, sizeof(void *));

    return OK;
}

parser_result_t json_model_string(json_string_t *json, generic_pointer_t *generic, void *data, int options) {
    char *str = strdup(json->content);
    memcpy(data, &str, sizeof(char *));

    return OK;
}

parser_result_t json_model_array(json_array_t *json, generic_struct_t *generic, void *data, int options) {
    void *ptr = data;
    generic_pointer_t *gen_vec = gas_ptr(generic->fields[0].type);

    void *vec = malloc(json->size * generic_sizeof(gen_vec->inner));
    memcpy(ptr, &vec, sizeof(void *));
    memcpy(memoff(ptr, sizeof(void *)), &json->size, sizeof(uint));

    json_type_t type;
    for (int i = 0; i < json->size; i++) {
        if (i == 0) type = jtype(json->values[0]);
        else ASSERT(type != jtype(json->values[0]));

        ASSERT_OK(json_model(json->values[i], gen_vec->inner, vec, options));
        vec = memoff(vec, generic_sizeof(gen_vec->inner));
    }

    return OK;
}

parser_result_t json_model_object(json_object_t *json, generic_struct_t *generic, void *data, int options) {
    void *ptr = data;

    generic_field_t *fields = generic->fields;
    for (int i = 0; i < generic->nfields; i++) {
        json_value_t *value = dict_get(json->pairs, fields[i].name);
        ASSERT(value);

        ASSERT_OK(json_model(value, fields[i].type, ptr, options));
        ptr = memoff(ptr, generic_sizeof(fields[i].type));
    }

    return OK;
}

parser_result_t json_model(json_value_t *json, generic_t *generic, void *data, int options) {
    if(!jtype_gentype_eq(json, generic)) return FAIL;

    switch (jtype(json)) { 
        case JSON_OBJECT:
            return json_model_object(jas_obj(json), gas_struct(generic), data, options);

        case JSON_ARRAY:
            return json_model_array(jas_arr(json), gas_struct(generic), data, options);

        case JSON_STRING:
            return json_model_string(jas_str(json), gas_ptr(generic), data, options);
            break;

        case JSON_NUMBER:
            memcpy(data, &jas_num(json)->number, sizeof(double));
            break;

        // TODO: finish all of the possibilities.

        default:
            break;
    }
    return OK;
}

parser_result_t json_model_file(char *fname, generic_t *generic, void *data, int options) {
    json_value_t *parsed;
    if (json_parse_file(fname, &parsed) == OK &&
        json_model(parsed, generic, &data, options) == OK)
        return OK;

    return FAIL;
}
