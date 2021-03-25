#pragma once

#include "common_afx.h"
#include "gl.h"

#ifdef _MSC_VER
#  define ALIGNED_VAR( size, def ) __declspec( align( size ) ) def
#else
#  define ALIGNED_VAR( size, def ) def __attribute__( ( aligned( size ) ) )
#  define __forceinline __attribute__((always_inline))

//FIXME: replace these with versions that actually do checks?
#  define _itoa_s( num, buf, length, base ) itoa( num, buf, base )
#  define _ltoa_s( num, buf, length, base ) ltoa( num, buf, base )
// requires a C99 compatible preprocessor...
#  define _snprintf_s( buf, buf_size, count, ... ) do					\
	{																	\
		snprintf( buf, count, __VA_ARGS__ );							\
		/* make sure we always return a properly terminated string */	\
		buf[ ( buf_size )-1 ] = 0;										\
	} while ( 0 )

#endif
