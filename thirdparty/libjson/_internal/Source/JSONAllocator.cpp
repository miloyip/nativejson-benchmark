#include "JSONAllocator.h"

#if defined(JSON_MEMORY_CALLBACKS) || defined(JSON_MEMORY_POOL)
#include "JSONMemory.h"

void * JSONAllocatorRelayer::alloc(size_t bytes) json_nothrow {
	return JSONMemory::json_malloc(bytes);
}

void JSONAllocatorRelayer::dealloc(void * ptr) json_nothrow {
	JSONMemory::json_free(ptr);
}
#endif
