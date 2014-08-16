#include "../memorystat.h"

#include "yajl/yajl_common.h"
#undef YAJL_MAX_DEPTH 
#define YAJL_MAX_DEPTH 1024

#include "yajl/src/yajl.c"
#include "yajl/src/yajl_alloc.c"
#include "yajl/src/yajl_buf.c"
#include "yajl/src/yajl_encode.c"
#include "yajl/src/yajl_gen.c"
#include "yajl/src/yajl_lex.c"
#include "yajl/src/yajl_parser.c"
#include "yajl/src/yajl_tree.c"
