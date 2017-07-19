#include "../memorystat.h"
#define FIOBJ_NESTING_PROTECTION 0
#include "facil.io/lib/facil/core/types/fiobj.h"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_types.h"

#include "facil.io/lib/facil/core/types/fiobj/fiobj_alloc.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_ary.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_generic.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_hash.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_json.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_misc.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_numbers.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_str.c"
#include "facil.io/lib/facil/core/types/fiobj/fiobj_sym.c"
