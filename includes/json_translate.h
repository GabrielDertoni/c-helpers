#ifndef __JSON_TRANSLATE_H__
#define __JSON_TRANSLATE_H__

#include <parsing_utils.h>
#include <generic.h>
#include <json.h>

parser_result_t json_model(json_value_t *json, generic_t *generic, void *data, int options);

#endif
