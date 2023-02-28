#include "common_afx.h"

//-----------------------
// libraries.

// MessageBoxA.
#pragma comment( lib, "User32.lib" )

// defines.
#define TEMP_MEM_SIZE			1048576

static int PrintF_Default( const char *txt, ... );
static void HandleError_Default( const char *txt, ... );

int ( *PrintF )( const char *txt, ... ) = PrintF_Default;
void ( *HandleError )( const char *fmt, ... ) = HandleError_Default;

#ifdef _DEBUG
#ifdef _MSC_VER
void BootstrapDebugBreak()
{
	__asm { int 3 };
}
#endif
#endif

static volatile int _tempMemGate = 0;
static DECLARE_ALIGNED_16( unsigned char, _tempMem[ TEMP_MEM_SIZE ] );

// exceptions disabled!
//void __cdecl std::_Throw( const std::exception& );
//void ( __cdecl* std::_Raise_handler )( const std::exception& ) = std::_Throw;

//==========================================================
// global functions
//==========================================================

void
VSPrintF( tstring& out, const char* fmt, va_list marker )
{
#ifdef _MSC_VER

	// allocate room for the new string on the stack.  Note
	// that this is performance friendly, but may cause a
	// stack overflow.
	int bufSize = _vscprintf( fmt, marker );
	char *buf = ( char * )alloca( bufSize + 1 );
	vsprintf_s( buf, bufSize+1, fmt, marker );

	// set the output.
	out = buf;
#else
	for ( int size = 128; ; size *= 2 ) 
	{
		char *buf = new char[ size + 1 ];
		va_list temp_marker;
		va_copy( temp_marker, marker );
		int ret = vsnprintf( buf, size, fmt, temp_marker );
		va_end( temp_marker );
		if ( ret < 0 ) 
		{
			// mingw and glibc<2.1 return -1 when the string doesn't fit
			// so we have to try again with a bigger buffer
			delete[] buf;
		}
		else if ( ret >= size )
		{
			// c99/glibc>=2.1 return the actual size of the final string
			// whether it fits or not, so we can just use that directly
			delete[] buf;
			char *buf = new char[ ret + 2 ];
			va_copy( temp_marker, marker );
			vsnprintf( buf, ret + 1, fmt, temp_marker );
			va_end( temp_marker );
			out = buf;
			delete[] buf;
			return;
		} else 
		{
			// otherwise,  string fit so we are done
			out = buf;
			delete[] buf;
			return;
		}
	}
#endif

}


//----------------------------------------------------------
void
SPrintF( tstring& out, const char *fmt, ... )
{
	va_list marker;
	va_start( marker, fmt );
	VSPrintF( out, fmt, marker );
	va_end( marker );
}

//----------------------------------------------------------
size_t
StrLenUTF8( const char* str )
{
	int len = ::MultiByteToWideChar( CP_UTF8, 0, str, -1, 0, 0 )-1;
	return size_t( len < 0 ? 0 : len );
}

//----------------------------------------------------------
const char*
NextCharUTF8( const char* str )
{
	B_ASSERT( str != 0 );

	// null terminator?
	if ( *str == 0 )
		return 0;

	// advance to the next character.
	const char* next = 0;
	if ( ((*str)&0xF0) == 0xF0 )
		next = str+4;
	else if ( ((*str)&0xE0) == 0xE0 )
		next = str+3;
	else if ( ((*str)&0xC0) == 0xC0 )
		next = str+2;
	else if ( ((*str)&0x80) == 0x80 )
		next = str+1;

	// return the next character or NULL if the character is the null character.
	return *next == 0 ? 0 : next;
}

//----------------------------------------------------------
unsigned int
GetCharLenUTF8( const char* chr )
{
	B_ASSERT( chr != 0 );

	// null terminator?
	if ( *chr == 0 )
		return 0;

	// return the character len.
	const char* next = 0;
	if ( ((*chr)&0xF0) == 0xF0 )
		return 4;
	else if ( ((*chr)&0xE0) == 0xE0 )
		return 3;
	else if ( ((*chr)&0xC0) == 0xC0 )
		return 2;

	// must be a single byte character.  If not, this wasn't a pointer to a valid
	// UTF-8 character.  Either way, we'll return 1.
	return 1;
}

//----------------------------------------------------------
unsigned int
HashMem32( const void* mem, unsigned int size, unsigned int hashStart )
{
	// hash the string.
	const char* str = ( const char* )mem;
	const int kPrime = 16777619;
	for ( unsigned int i = 0; i < size; ++i )
	{
		// hash the current character.
		int ch = str[ i ] & 0xFF;
		hashStart = ( hashStart * kPrime );
		hashStart = hashStart ^ ch;
	}

	// return the new hash.
	return hashStart;
}

//----------------------------------------------------------
unsigned __int64
HashMem64( const void* mem, unsigned int size, unsigned __int64 hashStart )
{
	// hash the string.
	const char* str = ( const char* )mem;
	const __int64 kPrime = 1099511628211LL;
	for ( unsigned int i = 0; i < size; ++i )
	{
		// hash the current character.
		__int64 ch = str[ i ] & 0xFF;
		hashStart = ( hashStart * kPrime );
		hashStart = hashStart ^ ch;
	}

	// return the new hash.
	return hashStart;
}

//----------------------------------------------------------
// this is necessary for exception handling to be disabled.
/*
void __cdecl std::_Throw( const std::exception& )
{
	B_ASSERT( false );
}*/

//==========================================================
// local functions
//==========================================================

//----------------------------------------------------------
static
int
PrintF_Default( const char *fmt, ... )
{
	va_list marker;
	va_start( marker, fmt );

	// allocate room for the new string on the stack.  Note
	// that this is performance friendly, but may cause a
	// stack overflow.
#ifdef _MSC_VER
	int bufSize = _vscprintf( fmt, marker );
	char *buf = ( char * )alloca( bufSize + 1 );
	vsprintf_s( buf, bufSize + 1, fmt, marker );
#else
	//// more portable version, should work on linux with glibc >= 2.1
	//char *buf = ( char * )alloca( 1 );
	//int bufSize = vsnprintf( buf, 1, fmt, marker );
	////vsnprintf leaves marker in undefined state, so reset it
	//va_end( marker );
	//va_start( marker, fmt );
	//buf = ( char * )alloca( bufSize + 1 );
	//vsnprintf( buf, bufSize+1, fmt, marker );
	//va_end( marker );
	tstring temp;
	VSPrintF( temp, fmt, marker );
	const char *buf = temp.c_str();
#endif
	va_end( marker );
	
	OutputDebugString( buf );

	// ignored.  The return is for compatibility with printf().
	return 0;
}


//----------------------------------------------------------
static
void
HandleError_Default( const char *fmt, ... )
{
	va_list marker;
	va_start( marker, fmt );

	// allocate room for the new string on the stack.  Note
	// that this is performance friendly, but may cause a
	// stack overflow.
#ifdef _MSC_VER
	int bufSize = _vscprintf( fmt, marker );
	char *buf = ( char * )alloca( bufSize + 1 );
	vsprintf_s( buf, bufSize + 1, fmt, marker );
#else
	//// more portable version, should work on linux with glibc >= 2.1
	////FIXME: windows vsnprintf returns -1 if trunctated, so this doesn't work very reliably
	//char *buf = ( char * )alloca( 1024 );
	//int bufSize = vsnprintf( buf, 1024, fmt, marker );
	//va_end( marker );
	//if ( bufSize >= 128 ) 
	//{
	//	//vsnprintf leaves marker in undefined state, so reset it
	//	va_start( marker, fmt );
	//	buf = ( char * )alloca( bufSize + 1 );
	//	vsnprintf( buf, bufSize+1, fmt, marker );
	//	va_end( marker );
	//} else 
	//{
	//	buf[1023] = 0;
	//}
	tstring temp;
	VSPrintF( temp, fmt, marker );
	const char *buf = temp.c_str();
#endif
	va_end( marker );

	// display the error message.
	MessageBox( NULL, buf, "Error!", MB_OK | MB_ICONERROR );

	// exit.
	exit( -1 );
}

//==========================================================
// aligned memory allocation
//==========================================================

//----------------------------------------------------------
void*
AlignedAlloc( unsigned int alignmentPow2, unsigned int size )
{
	B_ASSERT( IsPow2( alignmentPow2 ) && alignmentPow2 >= sizeof( char* ) );

#ifdef _MSC_VER
	return _aligned_malloc( size, alignmentPow2 );
#else
	// allocate the memory.
	unsigned int mask = alignmentPow2 - 1;
	char* mem = new char[ size + mask + alignmentPow2 ];

	// first, align the memory.
	char* aligned = mem + mask + alignmentPow2;
	aligned = ( char* )( ( size_t )aligned & ~mask );

	// reserve some space to store the original pointer and add the alignment.
	char* origPtr = aligned;
	origPtr -= sizeof( char* );
	*( ( char** )origPtr ) = mem;

	// now return the aligned memory.
	return aligned;
#endif
}

//----------------------------------------------------------
void
AlignedFree( void* alignedMemory )
{
#ifdef _MSC_VER
	_aligned_free( alignedMemory );
#else
	// get the original pointer.
	char* mem = ( char* )alignedMemory;
	mem -= sizeof( char* );
	mem = *( ( char** )mem );

	// free the original memory.
	delete[] mem;
#endif
}

//==========================================================
// temporary memory allocation
//==========================================================

//----------------------------------------------------------
void* TempAlloc( unsigned int size )
{
	// could we gain ownership over the temporary memory?  If so,
	// simply return it.
	if ( size <= TEMP_MEM_SIZE && LockSwap( &_tempMemGate, 1 ) == 0 )
		return _tempMem;
	else
		return AlignedAlloc( 16, size );
}

//----------------------------------------------------------
void TempFree( void* mem )
{
	if ( mem == _tempMem )
		LockSwap( &_tempMemGate, 0 );
	else
		AlignedFree( mem );
}

//==========================================================
// fast memory copy
//==========================================================

// upper limit for movsd type copy
// The smallest copy uses the X86 "movsd" instruction, in an optimized
// form which is an "unrolled loop".
#define TINY_BLOCK_COPY 64

// upper limit for movq/movq copy w/SW prefetch
// Next is a copy that uses the MMX registers to copy 8 bytes at a time,
// also using the "unrolled loop" optimization. This code uses
// the software prefetch instruction to get the data into the cache.
#define IN_CACHE_COPY 64 * 1024

// upper limit for movq/movntq w/SW prefetch
// For larger blocks, which will spill beyond the cache, it's faster to
// use the Streaming Store instruction MOVNTQ. This write instruction
// bypasses the cache and writes straight to main memory. This code also
// uses the software prefetch instruction to pre-read the data.
// USE 64 * 1024 FOR THIS VALUE IF YOU'RE ALWAYS FILLING A "CLEAN CACHE"
#define UNCACHED_COPY 197 * 1024

// no limit for movq/movntq w/block prefetch
#define BLOCK_PREFETCH_COPY infinity

// # of 64-byte blocks (cache lines) for block prefetch
// For the largest size blocks, a special technique called Block Prefetch
// can be used to accelerate the read operations. Block Prefetch reads
// one address per cache line, for a series of cache lines, in a short loop.
// This is faster than using software prefetch. The technique is great for
// getting maximum read bandwidth, especially in DDR memory systems.
#define CACHEBLOCK 80h

//----------------------------------------------------------
void MemCopy( void* dest, const void* src, unsigned int n )
{
#ifdef _MSC_VER
	__asm {
		mov		ecx, [n]			; number of bytes to copy
		mov		edi, [dest]			; destination
		mov		esi, [src]			; source
		mov		ebx, ecx			; keep a copy of count

		cld
		cmp		ecx, TINY_BLOCK_COPY
		jb		$memcpy_ic_3		; tiny? skip mmx copy

		cmp		ecx, 32*1024		; don't align between 32k-64k because
		jbe		$memcpy_do_align	; it appears to be slower
		cmp		ecx, 64*1024
		jbe		$memcpy_align_done

	$memcpy_do_align:
		mov		ecx, 8				; a trick that's faster than rep movsb...
		sub		ecx, edi			; align destination to qword
		and		ecx, 111b			; get the low bits
		sub		ebx, ecx			; update copy count
		neg		ecx					; set up to jump into the array
		add		ecx, offset $memcpy_align_done
		jmp		ecx					; jump to array of movsb's

	align 4
		movsb
		movsb
		movsb
		movsb
		movsb
		movsb
		movsb
		movsb
	
	$memcpy_align_done:				; destination is dword aligned
		mov		ecx, ebx			; number of bytes left to copy
		shr		ecx, 6				; get 64-byte block count
		jz		$memcpy_ic_2		; finish the last few bytes

		cmp		ecx, IN_CACHE_COPY/64	; too big 4 cache? use uncached copy
		jae		$memcpy_uc_test

		// This is small block copy that uses the MMX registers to copy 8 bytes
		// at a time. It uses the "unrolled loop" optimization, and also uses
		// the software prefetch instruction to get the data into the cache.
	align 16
	$memcpy_ic_1:					; 64-byte block copies, in-cache copy

		prefetchnta [esi + (200*64/34+192)]		; start reading ahead
		movq	mm0, [esi+0]		; read 64 bits
		movq	mm1, [esi+8]
		movq	[edi+0], mm0		; write 64 bits
		movq	[edi+8], mm1		;	note: the normal movq writes the
		movq	mm2, [esi+16]		;	data to cache; a cache line will be
		movq	mm3, [esi+24]		;	allocated as needed, to store the data
		movq	[edi+16], mm2
		movq	[edi+24], mm3
		movq	mm0, [esi+32]
		movq	mm1, [esi+40]
		movq	[edi+32], mm0
		movq	[edi+40], mm1
		movq	mm2, [esi+48]
		movq	mm3, [esi+56]
		movq	[edi+48], mm2
		movq	[edi+56], mm3

		add		esi, 64				; update source pointer
		add		edi, 64				; update destination pointer
		dec		ecx ; count down
		jnz		$memcpy_ic_1		; last 64-byte block?

	$memcpy_ic_2:
		mov		ecx, ebx			; has valid low 6 bits of the byte count
	$memcpy_ic_3:
		shr		ecx, 2				; dword count
		and		ecx, 1111b			; only look at the "remainder" bits
		neg		ecx					; set up to jump into the array
		add		ecx, offset $memcpy_last_few
		jmp		ecx					; jump to array of movsd's

	$memcpy_uc_test:
		cmp		ecx, UNCACHED_COPY/64	; big enough? use block prefetch copy
		jae		$memcpy_bp_1

	$memcpy_64_test:
		or		ecx, ecx			; tail end of block prefetch will jump here
		jz		$memcpy_ic_2		; no more 64-byte blocks left

		// For larger blocks, which will spill beyond the cache, it's faster to
		// use the Streaming Store instruction MOVNTQ. This write instruction
		// bypasses the cache and writes straight to main memory. This code also
		// uses the software prefetch instruction to pre-read the data.
	align 16
	$memcpy_uc_1:					; 64-byte blocks, uncached copy
		prefetchnta [esi + (200*64/34+192)] ; start reading ahead
		movq	mm0,[esi+0]			; read 64 bits
		add		edi,64				; update destination pointer
		movq	mm1,[esi+8]
		add		esi,64				; update source pointer
		movq	mm2,[esi-48]
		movntq	[edi-64], mm0		; write 64 bits, bypassing the cache
		movq	mm0,[esi-40]		; note: movntq also prevents the CPU
		movntq	[edi-56], mm1		; from READING the destination address
		movq	mm1,[esi-32]		; into the cache, only to be over-written
		movntq	[edi-48], mm2		; so that also helps performance
		movq	mm2,[esi-24]
		movntq	[edi-40], mm0
		movq	mm0,[esi-16]
		movntq	[edi-32], mm1
		movq	mm1,[esi-8]
		movntq	[edi-24], mm2
		movntq	[edi-16], mm0
		dec ecx
		movntq [edi-8], mm1
		jnz $memcpy_uc_1			; last 64-byte block?

		jmp $memcpy_ic_2			; almost done

		// For the largest size blocks, a special technique called Block Prefetch
		// can be used to accelerate the read operations. Block Prefetch reads
		// one address per cache line, for a series of cache lines, in a short loop.
		// This is faster than using software prefetch. The technique is great for
		// getting maximum read bandwidth, especially in DDR memory systems.
	$memcpy_bp_1:					; large blocks, block prefetch copy
		cmp		ecx, CACHEBLOCK		; big enough to run another prefetch loop?
		jl		$memcpy_64_test		; no, back to regular uncached copy
		mov		eax, CACHEBLOCK / 2		; block prefetch loop, unrolled 2X
		add		esi, CACHEBLOCK * 64	; move to the top of the block
	align 16
	$memcpy_bp_2:
		mov		edx, [esi-64]		; grab one address per cache line
		mov		edx, [esi-128]		; grab one address per cache line
		sub		esi, 128			; go reverse order
		dec		eax					; count down the cache lines
		jnz		$memcpy_bp_2		; keep grabbing more lines into cache

		mov eax, CACHEBLOCK			; now that it's in cache, do the copy
	align 16
	$memcpy_bp_3:
		movq	mm0, [esi ]			; read 64 bits
		movq	mm1, [esi+ 8]
		movq	mm2, [esi+16]
		movq	mm3, [esi+24]
		movq	mm4, [esi+32]
		movq	mm5, [esi+40]
		movq	mm6, [esi+48]
		movq	mm7, [esi+56]
		add		esi, 64				; update source pointer
		movntq [edi ], mm0			; write 64 bits, bypassing cache
		movntq [edi+ 8], mm1		; note: movntq also prevents the CPU
		movntq [edi+16], mm2		; from READING the destination address
		movntq [edi+24], mm3		; into the cache, only to be over-written,
		movntq [edi+32], mm4		; so that also helps performance
		movntq [edi+40], mm5
		movntq [edi+48], mm6
		movntq [edi+56], mm7
		add edi, 64					; update dest pointer

		dec eax						; count down

		jnz $memcpy_bp_3			; keep copying
		sub ecx, CACHEBLOCK			; update the 64-byte block count
		jmp $memcpy_bp_1			; keep processing blocks

		// The smallest copy uses the X86 "movsd" instruction, in an optimized
		// form which is an "unrolled loop". Then it handles the last few bytes.
	align 4
		movsd
		movsd ; perform last 1-15 dword copies
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd ; perform last 1-7 dword copies
		movsd
		movsd
		movsd
		movsd
		movsd
		movsd

	$memcpy_last_few:				; dword aligned from before movsd's
		mov		ecx, ebx			; has valid low 2 bits of the byte count
		and		ecx, 11b			; the last few cows must come home
		jz		$memcpy_final		; no more, let's leave
		rep		movsb				; the last 1, 2, or 3 bytes

	$memcpy_final:
		emms ; clean up the MMX state
		sfence ; flush the write buffer
		mov eax, [dest] ; ret value = destination pointer
	}
#else
	return memcpy( dest, src, n );
#endif
}

//----------------------------------------------------------
void 
StrTokenize( TokenContainer& tokens, const char* str )
{
	size_t len = strlen( str );

	// create a copy of the string to tokenize.
	char* localCopy = new char[ len+1 ];
	MemCopy( localCopy, str, (unsigned int)len+1 );

	// tokenize it.
	char* context = 0;
	char* token = strtok_s( localCopy, "\n\r\t ", &context );
	while( token )
	{
		tokens.push_back( token );
		token = strtok_s( 0, "\n\r\t ", &context );
	}

	// free the local copy.
	delete [] localCopy;
}

//----------------------------------------------------------
tstring&
StrToLower( tstring& str )
{
	size_t strLen = str.length();
	for ( size_t i = 0; i < strLen; ++i )
		str[ i ] = _tolower( str[ i ] );
	return str;
}
