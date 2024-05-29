#pragma once

//needed for sync primitives.
#if defined(WIN32)
#   include <windows.h>
#elif defined(_XBOX)
#   include <xtl.h>
#endif

class PhAllocator : public PxUserAllocator
{
public:
    PhAllocator();
    virtual     ~PhAllocator();

    void        reset();

    void*       malloc(size_t size);
    void*       malloc(size_t size, PxMemoryType type);
    void*       mallocDEBUG(size_t size, const char* file, int line);
    void*       mallocDEBUG(size_t size, const char* file, int line, const char* className, PxMemoryType type);
    void*       realloc(void* memory, size_t size);
    void        free(void* memory);

    size_t*     mMemBlockList;
    PxU32       mMemBlockListSize;
    PxU32       mMemBlockFirstFree;
    PxU32       mMemBlockUsed;

    PxI32       mNbAllocatedBytes;
    PxI32       mHighWaterMark;
    PxI32       mTotalNbAllocs;
    PxI32       mNbAllocs;
    PxI32       mNbReallocs;
#if defined(WIN32) || defined(_XBOX)
    CRITICAL_SECTION allocatorLock;

    void LockAlloc()
    {
        EnterCriticalSection(&allocatorLock);
    }
    void UnlockAlloc()
    {
        LeaveCriticalSection(&allocatorLock);
    }
#else
    // TODO: other platforms should have a thread safe allocator as well.
    void LockAlloc(){}
    void UnlockAlloc(){}
#endif
};
