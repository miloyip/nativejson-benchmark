#include "memorystat.h"

#ifdef USE_MEMORYSTAT

extern "C" {

void* MemoryStatMalloc(size_t size) {
    return Memory::Instance().Malloc(size);
}

void* MemoryStatCalloc(size_t num, size_t size) {
    void *p = Memory::Instance().Malloc(size * num);
    if (p)
        memset(p, 0, size * num);
    return p;
}

void* MemoryStatRealloc(void* ptr, size_t size) {
    return Memory::Instance().Realloc(ptr, size);
}

void MemoryStatFree(void* ptr) {
    Memory::Instance().Free(ptr);
}

} // extern "C"

#endif
