// precompiled header that includes operating system specific files.

#define _WIN32_WINNT		0x0500

#ifdef WIN32
#include <windows.h>
#endif

// function prototypes to ease the reading of this file.
inline int _FS_Swap( volatile int* gate, int value );
inline int _FS_CompareAndSwap( volatile int* gate, int value, int compare );
inline void* _FS_AlignedAlloc( unsigned int size, unsigned int alignmentPow2 );
inline void _FS_AlignedFree( void* ptr );

//#define FS_USE_WIDECHAR

#if !defined( FSchar )
# if defined FS_USE_WIDECHAR
typedef wchar_t FSchar;
#  define FS_T( x )		L##x
# else
typedef char FSchar;
#  define FS_T( x )		x
# endif
#endif

#if !defined( FSsize )
typedef __int64 FSsize;
#define FSSIZE_LOW( x )				( ( unsigned int )( ( ( FSsize )( x ) ) & 0xFFFFFFFF ) )
#define FSSIZE_HIGH( x )			( ( unsigned int )( ( ( FSsize )( x ) >> 32 ) & 0xFFFFFFFF ) )
#define MAKE_FSIZE( low, high )		( ( ( ( FSsize )( high ) ) << 32 ) | ( ( FSsize )( low ) ) )
#endif

//==========================================================
// Native word size based defines and macros
//==========================================================
#define _FS_NAME_MANGLE_INTERNAL( x, y )	x##y
#define _FS_NAME_MANGLE( x, y )				_FS_NAME_MANGLE_INTERNAL( x, y )

// determine the native word size (the size of a pointer) for the
// given compile target.  _FS_P32 indicates 32-bit pointers, where
// _FS_P64 indicates 64-bit pointers.
#if defined( WIN32 )
# if defined( _X86_ )
#  define _FS_P32
#  define _FS_PADPTR		unsigned int _FS_NAME_MANGLE( _pad, __LINE__ )
# elif defined( _AMD64_ ) || defined( _IA64_ )
#  define _FS_P64
#  define _FS_PADPTR
# endif
#else
# error Add defines for the native pointer size!
#endif
#if defined _FS_P32
# if defined _MSC_VER
#  define _FS_PTR_TO_INT( x )			( int )( unsigned int )( size_t )( x )
#  define _FS_INT_TO_PTR( x )			( void* )( size_t )( unsigned int )( x )
# else
#  define _FS_PTR_TO_INT( x )			( int )( unsigned int )( x )
#  define _FS_INT_TO_PTR( x )			( void* )( unsigned int )( x )
# endif
#elif defined _FS_P64
# if defined _MSC_VER
#  define _FS_PTR_TO_INT( x )			( __int64 )( unsigned __int64 )( size_t )( x )
#  define _FS_INT_TO_PTR( x )			( void* )( size_t )( unsigned __int64 )( x )
# else
#  define _FS_PTR_TO_INT( x )			( __int64 )( unsigned __int64 )( x )
#  define _FS_INT_TO_PTR( x )			( void* )( unsigned __int64 )( x )
# endif
#endif

//==========================================================
// Begin implementation
//==========================================================
#ifdef _MSC_VER

# ifdef _FS_P32

#ifndef B_NO_INTERLOCKED_INTRINSICS
#include <intrin.h>
#pragma intrinsic( _InterlockedExchange, _InterlockedCompareExchange, _InterlockedDecrement, _InterlockedIncrement )

inline int _FS_Swap( volatile int* gate, int value )
{
	return ( int )_InterlockedExchange( ( volatile long* )gate, ( long )value );
}

inline int _FS_CompareAndSwap( volatile int* gate, int value, int compare )
{
	return ( int )_InterlockedCompareExchange( ( volatile long* )gate, ( long )value, ( long )compare );
}

inline int _FS_Decrement( volatile int* lock )
{
	return ( int )_InterlockedDecrement( ( volatile long* )lock );
}

inline int _FS_Increment( volatile int* lock )
{
	return ( int )_InterlockedIncrement( ( volatile long* )lock );
}

inline void _FS_Pause()
{
	__asm pause;
}
#else
inline int _FS_Swap( volatile int* gate, int value ) { return 0; }
inline int _FS_CompareAndSwap( volatile int* gate, int value, int compare ) { return 0; }
inline int _FS_Decrement( volatile int* lock ) { return 0; }
inline int _FS_Increment( volatile int* lock ) { return 0; }
inline void _FS_Pause() { }
#endif

inline void* _FS_AlignedAlloc( unsigned int size, unsigned int alignmentPow2 )
{
	return _aligned_malloc( size, alignmentPow2 );
}

inline void _FS_AlignedFree( void* ptr )
{
	_aligned_free( ptr );
}

# else
# error Define interlocking functions for 64-bit build targets!
# endif

#endif
