// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once

// define our minimum windows version as Windows XP.
#define _WIN32_WINNT			0x0500

// exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

// turn off C++ exceptions
//#define _HAS_EXCEPTIONS 0
#define _INC_TYPEINFO

// TODO: reference additional headers your program requires here
#include <windows.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <vector>

// unfortunately, namespacing float.h and math.h doesn't play well 
// with 3rdParty libraries such as PhysX, so disable that for now.
#define __INTERNAL_MATH_IMPL

#if 0
namespace __INTERNAL_MATH_IMPL
{
	#include <math.h>
	#include <float.h>
};
#else
#include <math.h>
#include <float.h>
#endif

#include <malloc.h>

//#define _HIGH_PRECISION

// include BootstrapFS.
#include "filesystem_afx.h"

// types.
#include <mmintrin.h>
#include <xmmintrin.h>
#ifdef _MSC_VER
typedef int				__declspec( align( 16 ) )		int_aligned_16;
typedef unsigned int	__declspec( align( 16 ) )		uint_aligned_16;
typedef float			__declspec( align( 16 ) )		float_aligned_16;
typedef double			__declspec( align( 16 ) )		double_aligned_16;
#define DECLARE_ALIGNED( type, name, alignment )		type __declspec( align( alignment ) ) name
#define DECLARE_ALIGNED_16( type, name )				type __declspec( align( 16 ) ) name
#elif __GNUC__
typedef int				int_aligned_16					__attribute__ (( aligned( 16 ) ));
typedef unsigned int	uint_aligned_16					__attribute__ (( aligned( 16 ) ));
typedef float			float_aligned_16				__attribute__ (( aligned( 16 ) ));
typedef double			double_aligned_16				__attribute__ (( aligned( 16 ) ));
#define DECLARE_ALIGNED( type, name, alignment )		type name __attribute__ (( aligned( alignment ) ));
#define DECLARE_ALIGNED_16( type, name )				type name __attribute__ (( aligned( 16 ) ));
#endif

typedef std::string tstring;

// A macro that can be used to append one value to another.
#define BOOTSTRAP_MACRO_APPEND_DO_IT( x, y )				x##y
#define BOOTSTRAP_MACRO_APPEND( x, y )					BOOTSTRAP_MACRO_APPEND_DO_IT( x, y )

// template class that's used for our static assertions.  Note that
// it has this ridiculously long name to avoid the chance of any name
// collisions.
#define BOOTSTRAP_STATIC_ASSERT_MANGLE_LINE_NUMBER( x )		BOOTSTRAP_MACRO_APPEND( x, __LINE__ )

template < bool success > class BootstrapStaticAssertFail;
template <> class BootstrapStaticAssertFail< true > { };

// static assertion macro.
#define B_STATIC_ASSERT( x )								\
	class BOOTSTRAP_STATIC_ASSERT_MANGLE_LINE_NUMBER( bootstrap_static_assert_fail_class ) { \
		inline void fail()									\
		{													\
			BootstrapStaticAssertFail< ( x ) > failure;		\
			failure = failure;								\
		}													\
	};

// static assertion macro.
#define B_GLOBAL_STATIC_ASSERT( x )						\
		typedef BootstrapStaticAssertFail< ( x ) > bootstrap_static_assert_fail_type

#ifdef _DEBUG
# ifdef _MSC_VER
void BootstrapDebugBreak();
#  define B_ASSERT( x )	{ if ( !( x ) ) { BootstrapDebugBreak(); } }
# else
#  define B_ASSERT( x )	{ if ( !( x ) ) { DebugBreak(); } }
# endif
#else
#define B_ASSERT( x )
#endif

// utility functions.
extern void VSPrintF( tstring& out, const char *fmt, va_list marker );
extern void SPrintF( tstring& out, const char *fmt, ... );

// debug output/console output function.
extern int ( *PrintF )( const char *fmt, ... );
extern void ( *HandleError )( const char *fmt, ... );

#include "URef.h"
#include "UConstRef.h"

#define B_ERROR( msg )	{	B_ASSERT( false );	HandleError( "%s, line %d:\n\n%s", __FILE__, __LINE__, ( tstring( "" ) + msg ).c_str() );	}

#define PI				3.141592f
#define HALF_PI			1.570796f
#define TWO_PI			6.283184f
#define ONE_OVER_PI		0.318309f
#define ONE_OVER_TWO_PI	0.1591545f
#define SQRT2			1.414213f
#define COS_45			0.707106f

#define E				2.71828183f

// MS specific type.
#if !defined( _MSC_VER )
typedef long long __int64;
#endif

inline bool	IsPow2( unsigned int i )	{	return ( !( ( i-1 )&i ) )&&( i!=0 );	}
inline unsigned int FindPow2( unsigned int v )
{
	// if v == 0, simply return 1.
	if ( !v )
		return 1;

	// find a power of 2.
	if ( !IsPow2( v ) )
	{
		while ( !IsPow2( v ) )
			v = v & ( v - 1 );
		v <<= 1;
	}
	return v;
}
template < typename _T > _T Min( const _T& a, const _T& b )		{	return a < b ? a : b;		}
template < typename _T > _T Max( const _T& a, const _T& b )		{	return a > b ? a : b;		}
template < typename _T > _T Min3( const _T& a, const _T& b, const _T& c )	{	return Min( a, Min( b, c ) );	}
template < typename _T > _T Max3( const _T& a, const _T& b, const _T& c )	{	return Max( a, Max( b, c ) );	}
template < typename _T > _T Clamp( const _T& a, const _T& min, const _T& max )
{
	_T t = ( a > max ? max : a );
	return ( t < min ? min : t );
}
template < typename _T > void Swap( _T& a, _T& b )				{	_T c = a; a = b; b = c;		}
template < typename _T > _T Sqr( const _T& v )					{	return v*v;					}

// string functions.
inline tstring& operator << ( tstring& s, float f ) { std::ostringstream ss; ss << std::fixed << f; s += ss.str(); return s; }
inline tstring& operator << ( tstring& s, int f ) { std::ostringstream ss; ss << f; s += ss.str(); return s; }
inline tstring& operator << ( tstring& s, unsigned int f ) { std::ostringstream ss; ss << f; s += ss.str(); return s; }

// math functions.
inline float DegToRad( float deg ) { return deg * PI / 180.0f; }
inline float RadToDeg( float rad ) { return rad * 180.0f / PI; }
inline float Tan( float theta ) { return __INTERNAL_MATH_IMPL::tanf( theta ); }
inline float Cos( float theta ) { return __INTERNAL_MATH_IMPL::cosf( theta ); }
inline float Sin( float theta ) { return __INTERNAL_MATH_IMPL::sinf( theta ); }
inline float ATan( float v ) { return __INTERNAL_MATH_IMPL::atanf( v ); }
inline float ATan2( float y, float x ) { return __INTERNAL_MATH_IMPL::atan2f( y, x ); }
inline float ACos( float v ) { B_ASSERT( v <= 1.0f && v >= -1.0f ); return __INTERNAL_MATH_IMPL::acosf( v ); }
inline float ASin( float v ) { B_ASSERT( v <= 1.0f && v >= -1.0f ); return __INTERNAL_MATH_IMPL::asinf( v ); }
inline float Floor( float v ) { return __INTERNAL_MATH_IMPL::floorf( v ); }
inline float Ceil( float v ) { return __INTERNAL_MATH_IMPL::ceilf( v ); }
inline float Fract( float v ) { return v - Floor( v ); }
inline float Sqrt( float v ) { return __INTERNAL_MATH_IMPL::sqrtf( v ); }
inline float Log( float v ) { return __INTERNAL_MATH_IMPL::logf( v ); }
inline float Log2( float v ) { return __INTERNAL_MATH_IMPL::log10f( v ) * 3.3219280948873623478703194294894f; }
inline unsigned int Log2( unsigned int v )
{
	unsigned int i = 0;
	while ( v != 0 )
	{
		v >>= 1;
		++i;
	}
	return ( i-1 );
}
inline float Log10( float v ) { return __INTERNAL_MATH_IMPL::log10f( v ); }
inline float InvSqrtFast( float v )
{
	// newton approximation.
#ifdef __GNUC__
	// new GCC is picky about aliasing pointers of different types, so
	// use a union instead
	union { float f; int i; } f = { v };
	f.i = 0x5F3759DF - ( f.i >> 1 );
	return ( 3.0f - v * f.f * f.f ) * f.f * 0.5f;
#else
	float vHalf = 0.5f*v;
	int i = *(int*)&v;
	i = 0x5F3759DF - (i>>1);	// initial guess.
	v = *(float*)&i;
	v = v*(1.5f-vHalf*v*v);		// newton step.
	return v;
#endif
}
inline float Pow( float v, float exp ) { return __INTERNAL_MATH_IMPL::powf( v, exp ); }
inline float Exp( float v ) { return __INTERNAL_MATH_IMPL::expf( v ); }
template < typename _T > _T Abs( _T v ) { return ( v < 0 ) ? ( -v ) : v; }
inline float Abs( float v ) { return __INTERNAL_MATH_IMPL::fabsf( v ); }
inline float Sign( float v ) { if ( v > 0.0f ) return 1.0f; else if ( v < 0.0f ) return -1.0f; return 0.0f; }
inline bool IsNaN( float v ) { return ( __INTERNAL_MATH_IMPL::_isnan( v ) != 0 ); }
inline double AbsD( double v ) { return __INTERNAL_MATH_IMPL::fabs( v ); }
inline double SqrtD( double v ) { return __INTERNAL_MATH_IMPL::sqrt( v ); }
inline bool ApproxEqual( float x, float y, float epsilon = 0.0001f ) { return Abs( x - y ) <= epsilon; }

// UTF8 functions.
size_t StrLenUTF8( const char* str );
const char* NextCharUTF8( const char* str );	// returns NULL if the terminating char was hit.
unsigned int GetCharLenUTF8( const char* chr );

// helpful function for solving a quadratic equation.
template < typename _T > void SolveQuadratic( float& posInt, float& negInt, const _T& a, const _T& b, const _T& c )
{
	// returns the y intercept.
	float underSqrt = Sqrt( b*b - 4.0f * a * c );
	posInt = ( -b + underSqrt ) / ( 2.0f * a );
	negInt = ( -b - underSqrt ) / ( 2.0f * a );
}

// memory functions.
inline void MemSet( void* dst, unsigned int v, unsigned int size ) { B_ASSERT( dst != 0 && v < 256 ); memset( dst, v, size ); }
extern void MemCopy( void* dst, const void* src, unsigned int size );

// allocates memory aligned to certain boundaries.
void* AlignedAlloc( unsigned int alignmentPow2, unsigned int size );
void AlignedFree( void* alignedMemory );

// allocates memory from a temporary pool if possible.  If the pool
// is already in use, this function returns memory allocated on the
// heap.  The memory returned by this function is guaranteed to be
// aligned to a 16-byte boundary.
void* TempAlloc( unsigned int size );
void TempFree( void* mem );

// calling the type _T confuses gcc since _T( prototype ) looks like a call
// to the _T macro in tchar.h
template < typename _TT > _TT* AlignedNew( unsigned int alignmentPow2, const _TT& prototype )
{
	void* mem = AlignedAlloc( alignmentPow2, sizeof( _TT ) );
	return new( mem ) _TT( prototype );
}

template < typename T > void AlignedDelete( T* object )
{
	( *object ).~T();
	AlignedFree( ( void* )object );
}

// hash functions.
unsigned int HashMem32( const void* mem, unsigned int size, unsigned int hashStart = 2166136261UL );
unsigned __int64 HashMem64( const void* mem, unsigned int size, unsigned __int64 hashStart = 14695981039346656037ULL );

#undef memset
#undef memcpy

template < typename _T > _T Align16( _T v )
{
	// determine the 16-byte aligned pointer.
	size_t offset = ( size_t )v;
	offset = ( offset + 0xF ) & 0xFFFFFFF0;
	return ( _T )offset;
}

template < typename _T > _T Lerp( const _T& v0, const _T& v1, float t )
{
	return ( _T )( ( 1.0f - t ) * v0 + t * v1 );
}

template < typename _T > _T Bezier3( const _T& v0, const _T& cp1, const _T& cp2, const _T& v2, float t )
{
	float t2 = t*t;
	float t3 = t*t2;
	float oneMinusT = (1.0f-t);
	float oneMinusT2 = oneMinusT * oneMinusT;
	float oneMinusT3 = oneMinusT * oneMinusT2;
	return oneMinusT3*v0 + 3.0f*t*oneMinusT2*cp1 + 3.0f*t2*oneMinusT*cp2 + t3*v2;
}

#ifdef _MSC_VER

#define LOCK_FREE			0
#define LOCK_TAKEN			1

#ifndef B_NO_INTERLOCKED_INTRINSICS
#include <intrin.h>
#pragma intrinsic( _InterlockedExchange, _InterlockedCompareExchange, _InterlockedDecrement, _InterlockedIncrement )

inline int LockSwap( volatile int* dst, int src )
{
	return ( int )_InterlockedExchange( ( volatile long* )dst, ( long )src );
}

inline int LockCompareAndSwap( volatile int* dst, int src, int cmp )
{
	return ( int )_InterlockedCompareExchange( ( volatile long* )dst, ( long )src, ( long )cmp );
}

inline int LockDecrement( volatile int* dst )
{
	return ( int )_InterlockedDecrement( ( volatile long* )dst );
}

inline int LockIncrement( volatile int* dst )
{
	return ( int )_InterlockedIncrement( ( volatile long* )dst );
}

inline void LockPause()
{
	__asm pause;
}
#else
inline int LockSwap( volatile int* dst, int src ) { return 0; }
inline int LockCompareAndSwap( volatile int* dst, int src, int cmp ) { return 0; }
inline int LockDecrement( volatile int* dst ) { return 0; }
inline int LockIncrement( volatile int* dst ) { return 0; }
inline void LockPause() { }
#endif

inline volatile int* InitLock( volatile int* dst )
{
	LockSwap( dst, LOCK_FREE );
	return dst;
}

inline void TakeLockWait( volatile int* dst )
{
	// wait until a lock is freed.
	while ( LockSwap( dst, LOCK_TAKEN ) == LOCK_TAKEN )
		LockPause();
}

inline bool TryTakeLock( volatile int* dst )
{
	// return true if the lock was taken, false if not.
	return ( LockSwap( dst, LOCK_TAKEN ) != LOCK_TAKEN );
}

inline void FreeLock( volatile int* dst )
{
	LockSwap( dst, LOCK_FREE );
}

inline bool IsLockFree( volatile int* dst )
{
	if ( LockSwap( dst, LOCK_TAKEN ) == LOCK_FREE )
	{
		LockSwap( dst, LOCK_FREE );
		return true;
	}
	return true;
}

inline bool IsLockTaken( volatile int* dst )
{
	return !IsLockFree( dst );
}

inline void WaitLockFree( volatile int* dst )
{
	TakeLockWait( dst );
	FreeLock( dst );
}

#endif
/*
class StrLessPred
{
public:
	void operator () ( const tstring& a, const tstring& b ) const {	return ( stricmp( a.c_str(), b.c_str() ) < 0 ); }
};
*/

// breaks an input string into tokens.
typedef std::vector< tstring > TokenContainer;
void StrTokenize( TokenContainer& tokens, const char* str );
tstring& StrToLower( tstring& str );

// fast erase for vectors.  This does not maintain element order.
template < typename _T, class iterator >
void FastVectorErase( std::vector< _T >& vec, iterator iter )
{
	*iter = vec.back();
	vec.pop_back();
}

// portability defines
#ifndef _MSC_VER
#define fopen_s(FP,name,mode) *(FP)=fopen(name,mode)
#endif

