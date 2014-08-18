#pragma once

#define USE_MEMORYSTAT

#ifdef USE_MEMORYSTAT

#ifdef __cplusplus
#include <new>
#include <cstdlib>
#include <cstring>
#ifdef __GNUC__
#include <malloc.h>
#endif

struct MemoryStat {
    size_t mallocCount;
    size_t reallocCount;
    size_t freeCount;
    size_t currentSize;
    size_t peakSize;
};

#define MEMORYSTAT_VC_OPTIMIZATION 1 // _MSC_VER

class Memory {
public:
    static Memory& Instance() {
        static Memory memory;
        return memory;
    }

    void* Malloc(size_t size) {
        void* p = malloc(size);

        stat_->currentSize += GetMallocSize(p);
        if (stat_->peakSize < stat_->currentSize)
            stat_->peakSize = stat_->currentSize;
        stat_->mallocCount++;

        return p;
    }

    void* Realloc(void* ptr, size_t size) {
        if (ptr) {
            size_t originalSize = GetMallocSize(ptr);
            stat_->currentSize -= originalSize;
            stat_->freeCount++;
        }

        void *p = realloc(ptr, size);

        stat_->currentSize += GetMallocSize(p);
        if (stat_->peakSize < stat_->currentSize)
            stat_->peakSize = stat_->currentSize;
        stat_->reallocCount++;

        return p;
    }

    void Free(void* ptr) {
        if (ptr) {
            size_t size = GetMallocSize(ptr);
            stat_->currentSize -= size;
            stat_->freeCount++;
            free(ptr);
        }
    }

    const MemoryStat& GetStat() { return *stat_; }

    MemoryStat* SetStat(MemoryStat* stat) {
        MemoryStat* old = stat_;
        stat_ = stat;
        return old;
    }

private:
    size_t GetMallocSize(void* ptr) {
#if _MSC_VER
        return _msize(ptr);
#elif __GNUC__
        return malloc_usable_size(ptr);
#else
#error Must implement Memory::GetMallocSize() in memorystat.h
#endif
    }

    Memory() : stat_(&global_) { memset(&global_, 0, sizeof(global_)); }

    MemoryStat global_;
    MemoryStat* stat_;
};

class MemoryStatScope {
public:
    MemoryStatScope() : old_(Memory::Instance().SetStat(&local_)) {
        memset(&local_, 0, sizeof(local_));        
    }

    ~MemoryStatScope() {
        old_->mallocCount += local_.mallocCount;
        old_->reallocCount += local_.reallocCount;
        old_->freeCount += local_.freeCount;
        old_->peakSize += local_.peakSize;
        old_->currentSize += local_.currentSize;
        Memory::Instance().SetStat(old_);
    }

private:
    MemoryStat local_;
    MemoryStat* old_;
};

#define MEMORYSTAT_SCOPE() MemoryStatScope scope##__LINE__

inline void* operator new (std::size_t size) /*throw (std::bad_alloc) */{
    return Memory::Instance().Malloc(size);
}

inline void* operator new (std::size_t size, const std::nothrow_t&) throw(){
    return Memory::Instance().Malloc(size);
}

inline void* operator new[](std::size_t size) /*throw (std::bad_alloc)*/ {
    return Memory::Instance().Malloc(size);
}

inline void* operator new[](std::size_t size, const std::nothrow_t&) throw() {
    return Memory::Instance().Malloc(size);
}

inline void operator delete (void* ptr) throw() {
    Memory::Instance().Free(ptr);
}

inline void operator delete (void* ptr, const std::nothrow_t&) throw() {
    Memory::Instance().Free(ptr);
}

inline void operator delete[](void* ptr) throw() {
    Memory::Instance().Free(ptr);
}

inline void operator delete[](void* ptr, const std::nothrow_t&) throw() {
    Memory::Instance().Free(ptr);
}

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#else
#include <stdlib.h>
#endif

extern void* MemoryStatMalloc(size_t size);
extern void* MemoryStatRealloc(void* ptr, size_t size);
extern void MemoryStatFree(void* ptr);

#ifdef __cplusplus
}
#endif

#define malloc MemoryStatMalloc
#define realloc MemoryStatRealloc
#define free MemoryStatFree

#ifdef __cplusplus
namespace std {
    inline void* MemoryStatMalloc(size_t size) {
        return ::MemoryStatMalloc(size);
    }
    inline void* MemoryStatRealloc(void* ptr, size_t size) {
        return ::MemoryStatRealloc(ptr, size);
    }
    inline void MemoryStatFree(void* ptr) {
        return ::MemoryStatFree(ptr);
    }
};
#endif

#else

#define MEMORYSTAT_SCOPE()

#endif
