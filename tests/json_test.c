#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "test_utils.h"
#include <colors.h>
#include <dict.h>
#include <generic.h>
#include <json.h>

bool test_json_object() {
    char *test = "{\n"
    "\t\"number_here\": 17,\n"
    "\t\"huge_number\": 29147124124014,\n"
    "\t\"maybe_null\": null,\n"
    "\t\"maybe_undefined\": undefined,\n"
    "\t\"nested_obj\": {\"inner_key\": {\"inner_inner\": \"value\", \"other_key\": 69}},\n"
    "\t\"arrays!\": [1,2,3,[],[[]], { \"inside_array\": [[[]]]}],\n"
    "}";
    json_object_t *json_object;
    assert_eq(json_parse_object(&test, &json_object), PARSER_SUCCESS);
    assert_neq(json_object, NULL);

    json_value_delete((json_value_t *)json_object);
    return true;
}

bool test_json_object_simple() {
    // Empty object.
    char *test = "{   }END OF STRING";
    json_object_t *json_object;
    assert_eq(json_parse_object(&test, &json_object), PARSER_SUCCESS);
    assert_neq(json_object, NULL);
    assert_eq(strcmp(test, "END OF STRING"), 0);
    assert_eq(json_object->self.type, JSON_OBJECT);

    json_value_delete((json_value_t *)json_object);
    // Some string entries.
    test =
    "{\n"
    "\t\"key\": \"value (string value)\",\n\r"
    "\t\"key2\": \"value1 (string value)\",\n"
    "\t\"key3\": \"value2 (string value)\",\n"
    "}END OF STRING";

    assert_eq(json_parse_object(&test, &json_object), PARSER_SUCCESS);
    assert_neq(json_object, NULL);
    assert_eq(strcmp(test, "END OF STRING"), 0);
    assert_eq(json_object->self.type, JSON_OBJECT);

    assert_eq(dict_get_size(json_object->pairs), 3);
    dict_t *dict = json_object->pairs;

    assert_eq(dict_contains(dict, "key"), true);
    assert_eq(jtype(dict_get(dict, "key")), JSON_STRING);
    assert_eq(strcmp(jas_str(dict_get(dict, "key"))->content, "value (string value)"), 0);

    assert_eq(dict_contains(dict, "key2"), true);
    assert_eq(jtype(dict_get(dict, "key2")), JSON_STRING);
    assert_eq(strcmp(jas_str(dict_get(dict, "key2"))->content, "value1 (string value)"), 0);

    assert_eq(dict_contains(dict, "key3"), true);
    assert_eq(jtype(dict_get(dict, "key3")), JSON_STRING);
    assert_eq(strcmp(jas_str(dict_get(dict, "key3"))->content, "value2 (string value)"), 0);

    json_value_delete((json_value_t *)json_object);
    return true;
}

bool test_json_array() {
    char *test = "[\"Hello World!\", \"Second string\", undefined, null, [\"nested array[]\", { \"key\": \"value\" }], {\"another key\": [null, \"crazy deep\"]}]END OF STRING";
    json_array_t *json_array;
    assert_eq(json_parse_array(&test, &json_array), PARSER_SUCCESS);
    assert_neq(json_array, NULL);
    assert_eq(strcmp(test, "END OF STRING"), 0);
    assert_eq(json_array->self.type, JSON_ARRAY);

    assert_eq(json_array->size, 6);
    json_value_t **values = json_array->values;

    // Type assertions.
    assert_eq(jtype(values[0]), JSON_STRING);
    assert_eq(jtype(values[1]), JSON_STRING);
    assert_eq(jtype(values[2]), JSON_UNDEFINED);
    assert_eq(jtype(values[3]), JSON_NULL);
    assert_eq(jtype(values[4]), JSON_ARRAY);
    assert_eq(jtype(values[5]), JSON_OBJECT);

    // Value assertions.
    assert_eq(strcmp(jas_str(values[0])->content, "Hello World!"), 0);
    assert_eq(strcmp(jas_str(values[1])->content, "Second string"), 0);

    json_value_t **values2 = jas_arr(values[4])->values;
    assert_eq(jtype(values2[0]), JSON_STRING);
    assert_eq(jtype(values2[1]), JSON_OBJECT);

    assert_eq(strcmp(jas_str(values2[0])->content, "nested array[]"), 0);

    dict_t *dict = jas_obj(values2[1])->pairs;
    assert_eq(dict_contains(dict, "key"), true);
    assert_eq(jtype(dict_get(dict, "key")), JSON_STRING);
    assert_eq(strcmp(jas_str(dict_get(dict, "key"))->content, "value"), 0);

    dict_t *dict2 = jas_obj(values[5])->pairs;
    assert_eq(dict_contains(dict2, "another key"), true);
    assert_eq(jtype(dict_get(dict2, "another key")), JSON_ARRAY);

    json_value_t **values3 = jas_arr(dict_get(dict2, "another key"))->values;
    assert_eq(jtype(values3[0]), JSON_NULL);
    assert_eq(jtype(values3[1]), JSON_STRING);

    assert_eq(strcmp(jas_str(values3[1])->content, "crazy deep"), 0);

    json_value_delete((json_value_t *)json_array);
    return true;
}

bool test_json_array_simple() {
    char *test = "[\"Hello World!\", null, undefined, [\"nested here\"], [], [[[]]], 1234.1234] rest of the string";
    json_array_t *json_array;
    assert_eq(json_parse_array(&test, &json_array), PARSER_SUCCESS);
    assert_neq(json_array, NULL);
    assert_eq(strcmp(test, " rest of the string"), 0);
    assert_eq(json_array->self.type, JSON_ARRAY);

    assert_eq(json_array->size, 7);
    json_value_t **values = json_array->values;

    assert_eq(jtype(values[0]), JSON_STRING);
    assert_eq(jtype(values[1]), JSON_NULL);
    assert_eq(jtype(values[2]), JSON_UNDEFINED);
    assert_eq(jtype(values[3]), JSON_ARRAY);
    assert_eq(jtype(values[4]), JSON_ARRAY);
    assert_eq(jtype(values[5]), JSON_ARRAY);
    assert_eq(jtype(values[6]), JSON_NUMBER);

    assert_eq(strcmp(jas_str(values[0])->content, "Hello World!"), 0);

    assert_eq(jas_arr(values[3])->size, 1);
    json_value_t **values2 = jas_arr(values[3])->values;

    assert_eq(jtype(values2[0]), JSON_STRING);
    assert_eq(strcmp(jas_str(values2[0])->content, "nested here"), 0);

    assert_eq(jas_arr(values[4])->size, 0);
    assert_eq(jas_arr(values[5])->size, 1);

    values2 = jas_arr(values[5])->values;
    assert_eq(jtype(values2[0]), JSON_ARRAY);
    assert_eq(jas_arr(values2[0])->size, 1);

    values2 = jas_arr(values2[0])->values;
    assert_eq(jtype(values2[0]), JSON_ARRAY);
    assert_eq(jas_arr(values2[0])->size, 0);

    assert_eq(jas_num(values[6])->number, 1234.1234);

    json_value_delete((json_value_t *)json_array);
    return true;
}

bool test_json_string() {
    char *test = "\"Hello World!\" rest of the string";
    json_string_t *json_str;
    assert_eq(json_parse_string(&test, &json_str), PARSER_SUCCESS);
    assert_neq(json_str, NULL);
    assert_eq(strcmp(test, " rest of the string"), 0);
    assert_eq(json_str->self.type, JSON_STRING);
    assert_eq(strcmp(json_str->content, "Hello World!"), 0);

    json_value_delete((json_value_t *)json_str);
    return true;
}

bool test_json_string_fail() {
    char *test = "\"Hello World! rest of the string";
    json_string_t *json_str;
    assert_eq(json_parse_string(&test, &json_str), PARSER_FAIL);
    assert_eq(json_str, NULL);
    return true;
}

bool test_json_string_escaping() {
    char *test = "\"Hello World!\\\"Aaaand, some inner stuff using escaping\\\"\" rest of the string";
    json_string_t *json_str;
    assert_eq(json_parse_string(&test, &json_str), PARSER_SUCCESS);
    assert_neq(json_str, NULL);
    assert_eq(strcmp(test, " rest of the string"), 0);
    assert_eq(json_str->self.type, JSON_STRING);
    assert_eq(strcmp(json_str->content, "Hello World!\\\"Aaaand, some inner stuff using escaping\\\""), 0);

    json_value_delete((json_value_t *)json_str);

    // "A string \\"
    test = "\"A string \\\\\" (some failed escaping that wont work properly)\"";
    assert_eq(json_parse_string(&test, &json_str), PARSER_SUCCESS);
    assert_neq(json_str, NULL);
    assert_eq(strcmp(test, " (some failed escaping that wont work properly)\""), 0);
    assert_eq(json_str->self.type, JSON_STRING);
    assert_eq(strcmp(json_str->content, "A string \\\\"), 0);

    json_value_delete((json_value_t *)json_str);
    return true;
}

bool test_json_number() {
    char *test = "1234.1234 rest of the string";
    json_number_t *json_num;
    assert_eq(json_parse_number(&test, &json_num), PARSER_SUCCESS);
    assert_neq(json_num, NULL);
    assert_eq(strcmp(test, " rest of the string"), 0);
    assert_eq(json_num->self.type, JSON_NUMBER);
    assert_eq(json_num->number, 1234.1234);

    json_value_delete((json_value_t *)json_num);
    return true;
}

bool test_json_number_fail() {
    char *test = "would you look at that, no number at all!";
    json_number_t *json_num;
    assert_eq(json_parse_number(&test, &json_num), PARSER_FAIL);
    assert_eq(json_num, NULL);
    return true;
}

bool test_json_undefined() {
    char *test = "undefinedsome rest, other stuff here idk";
    json_undefined_t *json_undefined;
    assert_eq(json_parse_undefined(&test, &json_undefined), PARSER_SUCCESS);
    assert_neq(json_undefined, NULL);
    assert_eq(strcmp(test, "some rest, other stuff here idk"), 0);
    assert_eq(json_undefined->self.type, JSON_UNDEFINED);

    json_value_delete((json_value_t *)json_undefined);
    return true;
}

bool test_json_undefined_fail() {
    char *test = "undefindsome rest, other stuff here idk";
    json_undefined_t *json_undefined;
    assert_eq(json_parse_undefined(&test, &json_undefined), PARSER_FAIL);
    assert_eq(json_undefined, NULL);
    return true;
}

bool test_json_null() {
    char *test = "nullsome rest, other stuff here idk";
    json_null_t *json_null;
    assert_eq(json_parse_null(&test, &json_null), PARSER_SUCCESS);
    assert_neq(json_null, NULL);
    assert_eq(strcmp(test, "some rest, other stuff here idk"), 0);
    assert_eq(json_null->self.type, JSON_NULL);

    json_value_delete((json_value_t *)json_null);
    return true;
}

bool test_json_null_fail() {
    char *test = "nllsome rest, other stuff here idk";
    json_null_t *json_null;
    assert_eq(json_parse_null(&test, &json_null), PARSER_FAIL);
    assert_eq(json_null, NULL);
    return true;
}

int main(int argc, char *argv[]) {
    TEST_SETUP();

    test_fn(test_json_null());
    test_fn(test_json_null_fail());
    test_fn(test_json_undefined());
    test_fn(test_json_undefined_fail());
    test_fn(test_json_number());
    test_fn(test_json_number_fail());
    test_fn(test_json_string());
    test_fn(test_json_string_fail());
    test_fn(test_json_string_escaping());
    test_fn(test_json_array_simple());
    test_fn(test_json_array());
    test_fn(test_json_object_simple());
    test_fn(test_json_object());

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}

