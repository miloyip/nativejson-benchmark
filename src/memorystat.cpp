#include "config.h"

#include "memorystat.h"

#if USE_MEMORYSTAT

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

void* operator new (std::size_t size) /*throw (std::bad_alloc) */{
    return Memory::Instance().Malloc(size);
}

void* operator new (std::size_t size, const std::nothrow_t&) throw(){
    return Memory::Instance().Malloc(size);
}

void* operator new[](std::size_t size) /*throw (std::bad_alloc)*/ {
    return Memory::Instance().Malloc(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) throw() {
    return Memory::Instance().Malloc(size);
}

void operator delete (void* ptr) throw() {
    Memory::Instance().Free(ptr);
}

void operator delete (void* ptr, const std::nothrow_t&) throw() {
    Memory::Instance().Free(ptr);
}

void operator delete[](void* ptr) throw() {
    Memory::Instance().Free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) throw() {
    Memory::Instance().Free(ptr);
}

#endif // USE_MEMORYSTAT

// Some platforms may not have strdup(). Also this implementation will able to hook malloc().
extern "C" {

char* StrDup(const char* src) {
    size_t size = strlen(src) + 1;
    char* ret = (char*)malloc(size);
    memcpy(ret, src, size);
    return ret;
}

} // extern "C"
