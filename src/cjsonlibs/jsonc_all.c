#include "../memorystat.h"

#if !defined(_MSC_VER) || _MSC_VER >= 1800 // Before VC2013 do not support C99 variable declaration

#if defined(_MSC_VER)
#include <windows.h>
#endif

#include "json-c/arraylist.c"
#include "json-c/debug.c"
#include "json-c/json_c_version.c"
#include "json-c/json_object.c"
#include "json-c/json_object_iterator.c"
#include "json-c/json_tokener.c"
#include "json-c/linkhash.c"
#include "json-c/printbuf.c"
#include "json-c/random_seed.c"

#include "json-c/json_util.c"   // This file undef realloc, put to last

#endif