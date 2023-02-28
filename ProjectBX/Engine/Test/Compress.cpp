#include "test_afx.h"

// file header.
#include "Compress.h"

#define ALIGN_ADDR( addr, start, mask )     ( start + ( ( ( size_t )addr - ( size_t )start ) & mask ) )

//----------------------------------------------------------
void
Compress( void* dstData, unsigned int& dstSize, const void* srcData, unsigned int srcSize, void* workspace )
{
    // partition the incoming data into the desired type.
    compress_type_t* src = ( compress_type_t* )srcData;
    compress_type_t* dst = ( compress_type_t* )dstData;

    // calculate the block address mask.
    unsigned int blockMask = ~( LZB_WORDS_PER_BLOCK - 1 );

    // initialize the current size specifier.
    compress_type_t* size = dst;
    *size = 0;
    ++dst;

    // look for any matching sequences.
    compress_type_t* cur = src;
    compress_type_t* end = src + srcSize;
    while ( cur < end )
    {
        // get values from the block's start to the current location.
        compress_type_t matchLen = 0;
        compress_type_t* block = ALIGN_ADDR( cur, src, blockMask );
        compress_type_t* matchSrc = block;
        compress_type_t* matchEnd = cur;
        compress_type_t* bestMatch = matchSrc;

        // scan for matches.
        while ( matchSrc < matchEnd )
        {
            compress_type_t curLen = 0;
            compress_type_t* ms = matchSrc;
            compress_type_t* mc = cur;
            while ( *ms++ == *mc++ )
                ++curLen;

            if ( curLen > matchLen )
            {
                matchLen = curLen;
                bestMatch = matchSrc;
            }

            ++matchSrc;
        }

        // check to see if a valid match was found.  If not, simply
        // output bytes.
        if ( matchLen <= 2 )
        {
            // do we need to start a new run?
            if ( *size > 253 )
            {
                size = dst;
                *size = 0;
                ++dst;
            }

            // simply write two uncompressed bytes.
            *size -= 2;
            *dst++ = *cur++;
            *dst++ = *cur++;
        }
        else
        {
            // write compression info.
            size_t offset = matchSrc - block;
            *size = ( compress_type_t )matchLen;
            *dst++ = ( compress_type_t )offset;
            cur += matchLen;

            // update the size pointer.
            size = dst;
            *size = 0;
            ++dst;
        }
    }

    // return the destination size.
    dstSize = ( unsigned int )( ( size_t )dst - ( size_t )dstData );
}

//----------------------------------------------------------
void
Decompress( void* dstData, unsigned int& dstSize, const void* srcData, unsigned int srcSize )
{
    // partition the incoming data into the desired type.
    compress_type_t* src = ( compress_type_t* )srcData;
    compress_type_t* dst = ( compress_type_t* )dstData;

    // calculate the block address mask.
    unsigned int blockMask = ~( LZB_WORDS_PER_BLOCK - 1 );

    // decompress the current block.
    compress_type_t* cur = dst;
    compress_type_t* end = src + srcSize;
    while ( src < end )
    {
        // get the start and the length.
        compress_type_t length = *src++;

        // emit values.
        if ( length < 0 )
        {
            // write raw data.
            while ( length++ < 0 )
                *cur++ = *src++;
        }
        else
        {
            // get the starting location in the block.
            compress_type_t start = *src++;
    
            // duplicate values.
            compress_type_t* matchSrc = ALIGN_ADDR( cur, dst, blockMask );
            while ( length-- > 0 )
                *cur++ = *matchSrc++;
        }
    }

    // return the destination size.
    dstSize = ( unsigned int )( ( size_t )dst - ( size_t )dstData );
}