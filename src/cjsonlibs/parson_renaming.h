#pragma once

// Renaming to prevent using same name in other C libraries
#define json_parse_file                 parson_json_parse_file
#define json_parse_file_with_comments   parson_json_parse_file_with_comments
#define json_parse_string               parson_json_parse_string
#define json_parse_string_with_comments parson_json_parse_string_with_comments
#define json_object_get_value           parson_json_object_get_value
#define json_object_get_string          parson_json_object_get_string
#define json_object_get_object          parson_json_object_get_object
#define json_object_get_array           parson_json_object_get_array
#define json_object_get_number          parson_json_object_get_number
#define json_object_get_boolean         parson_json_object_get_boolean
#define json_object_dotget_value        parson_json_object_dotget_value
#define json_object_dotget_string       parson_json_object_dotget_string
#define json_object_dotget_object       parson_json_object_dotget_object
#define json_object_dotget_array        parson_json_object_dotget_array
#define json_object_dotget_number       parson_json_object_dotget_number
#define json_object_dotget_boolean      parson_json_object_dotget_boolean
#define json_object_get_count           parson_json_object_get_count
#define json_object_get_name            parson_json_object_get_name
#define json_array_get_value            parson_json_array_get_value
#define json_array_get_string           parson_json_array_get_string
#define json_array_get_object           parson_json_array_get_object
#define json_array_get_array            parson_json_array_get_array
#define json_array_get_number           parson_json_array_get_number
#define json_array_get_boolean          parson_json_array_get_boolean
#define json_array_get_count            parson_json_array_get_count
#define json_value_get_type             parson_json_value_get_type
#define json_value_get_object           parson_json_value_get_object
#define json_value_get_array            parson_json_value_get_array
#define json_value_get_string           parson_json_value_get_string
#define json_value_get_number           parson_json_value_get_number
#define json_value_get_boolean          parson_json_value_get_boolean
#define json_value_free                 parson_json_value_free
#define json_validate                   parson_json_validate
