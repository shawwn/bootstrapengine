#pragma once

// boundary alignment.
#define LZB_ALIGNMENT		1

typedef signed char compress_type_t;

#define LZB_BLOCK_SIZE			256
#define LZB_MACHINE_WORD_SIZE	sizeof( compress_type_t )
#define LZB_WORDS_PER_BLOCK		(LZB_BLOCK_SIZE / sizeof( compress_type_t ))
#define LZB_HASH_WINDOW_SIZE	8
#define LZB_WORK_SPACE_SIZE		(WORDS_PER_BLOCK * HASH_WINDOW_SIZE)

// compresses data.  Note that the source data and the destination
// pointer must be aligned to the value specified by LZB_ALIGNMENT!
void Compress( void* dst, unsigned int& dstSize, const void* src, unsigned int srcSize, void* workspace );

// decompress data.  Note that the source data and the destination
// pointer must be aligned to the value specified by LZB_ALIGNMENT!
void Decompress( void* dst, unsigned int& dstSize, const void* src, unsigned int srcSize );
