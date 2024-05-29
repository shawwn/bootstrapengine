#pragma once

#include "PxDefaultStreams.h"
#include "foundation/PxSimpleTypes.h"
using namespace physx;

#if 0
class UserStream : public PxStream
{
public:
    UserStream(const char* filename, bool load);
    virtual                     ~UserStream();

    virtual     PxU8            readByte()                              const;
    virtual     PxU16           readWord()                              const;
    virtual     PxU32           readDword()                             const;
    virtual     float           readFloat()                             const;
    virtual     double          readDouble()                            const;
    virtual     void            readBuffer(void* buffer, PxU32 size)    const;

    virtual     PxStream&       storeByte(PxU8 b);
    virtual     PxStream&       storeWord(PxU16 w);
    virtual     PxStream&       storeDword(PxU32 d);
    virtual     PxStream&       storeFloat(PxReal f);
    virtual     PxStream&       storeDouble(PxF64 f);
    virtual     PxStream&       storeBuffer(const void* buffer, PxU32 size);

    FILE*           fp;
};
#endif

class MemoryWriteBuffer : public PxOutputStream
{
public:
    MemoryWriteBuffer();
    virtual                     ~MemoryWriteBuffer();
    void            clear();

    virtual     PxOutputStream&       storeByte(PxU8 b);
    virtual     PxOutputStream&       storeWord(PxU16 w);
    virtual     PxOutputStream&       storeDword(PxU32 d);
    virtual     PxOutputStream&       storeFloat(PxReal f);
    virtual     PxOutputStream&       storeDouble(PxF64 f);
    virtual     PxOutputStream&       storeBuffer(const void* buffer, PxU32 size);

    virtual uint32_t write(const void* src, uint32_t count) {
        storeBuffer(src, count);
        return count;
    }

    PxU32           currentSize;
    PxU32           maxSize;
    PxU8*           data;
};

class MemoryReadBuffer : public PxInputStream
{
public:
    MemoryReadBuffer(const PxU8* data);
    virtual                     ~MemoryReadBuffer();

    virtual     PxU8            readByte()                              const;
    virtual     PxU16           readWord()                              const;
    virtual     PxU32           readDword()                             const;
    virtual     float           readFloat()                             const;
    virtual     double          readDouble()                            const;
    virtual     void            readBuffer(void* buffer, PxU32 size)    const;

    virtual uint32_t read(void* dest, uint32_t count) {
        readBuffer(dest, count);
        return count;
    }

    mutable     const PxU8*     buffer;
};
