#include "../memorystat.h"

#include "workflow-json-parser/rbtree.c"
#define json_object_push	avoid_conflict_json_object_push
#define json_array_push		avoid_conflict_json_array_push
#include "workflow-json-parser/json_parser.c"
