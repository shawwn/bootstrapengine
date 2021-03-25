#include "common_afx.h"

// header.
#include "UUTF8.h"


//----------------------------------------------------------
// adds a Unicode-2 character to the string.
tstring operator + ( const tstring& byteStr, wchar_t word )
{
	tstring newStr;
	return newStr;
}

//----------------------------------------------------------
// adds a Unicode-2 string to the string.
tstring operator + ( const tstring& byteStr, wchar_t *wordStr )
{
	tstring newStr;
	return newStr;
}

//----------------------------------------------------------
// adds a Unicode-2 string to the string.
tstring& operator += ( tstring& byteStr, wchar_t *wordStr )
{
	return byteStr;
}

//----------------------------------------------------------
// makes a Unicode-2 string from a UTF-8 string.
wchar_t *MakeUnicode( const tstring& str )
{
	// determine the length of the string.
	return new wchar_t[ 1 ];
}

//----------------------------------------------------------
// makes a UTF-8 string from a Unicode-2 string.
tstring MakeUTF8( wchar_t *wordStr )
{
	return tstring();
}

//----------------------------------------------------------
// converts a UTF-16 character into a UTF-8 character.
tstring
UTF16toUTF8( UINT_PTR utf16 )
{
	// convert to unicode, then convert to UTF-8.
	unsigned long uniChar = 0;
	unsigned short lowWord = (unsigned short)( utf16 & 0xFFFF );
	if ( lowWord < 0xD800 || lowWord > 0xDFFF )
	{
		uniChar = lowWord;
	}
	else
	{
		unsigned short highWord = (unsigned short)( ( utf16 & 0xFFFF0000 ) >> 16 );
		B_ASSERT( lowWord >= 0xD800 && lowWord <= 0xDBFF );
		B_ASSERT( highWord >= 0xDC00 && highWord <= 0xDFFF );
		uniChar = ( ( lowWord & 0x3FF ) | ( ( highWord & 0x3FF ) << 10 ) );
	}

	// we only deal with Unicode-2 format.
	B_ASSERT( uniChar < 65536 );

	// convert the unichar to UTF-8.
	return UnicodeToUTF8( ( unsigned short )( uniChar & 0xFFFF ) );
}

//----------------------------------------------------------
// converts a UCS-2 to UTF-8 encoding.
tstring
UnicodeToUTF8( unsigned short uniChar )
{
	tstring str;
	if ( uniChar >= 0x800 )
	{
		// 3-byte UTF-8 sequence.
		char byte0 = 0xE0 | ( ( uniChar & 0xF000 ) >> 12 );
		char byte1 = 0x80 | ( ( uniChar & 0x0FC0 ) >> 6 );
		char byte2 = 0x80 | ( uniChar & 0x003F );
		str += byte0;
		str += byte1;
		str += byte2;
	}
	else if ( uniChar >= 0x80 )
	{
		// 2-byte UTF-8 sequence.
		char byte0 = 0xC0 | ( ( uniChar & 0x07C0 ) >> 6 );
		char byte1 = 0x80 | ( ( uniChar & 0x3F ) );
		str += byte0;
		str += byte1;
	}
	else
	{
		// 1-byte UTF-8 char.
		char byte0 = ( char )uniChar;
		str += byte0;
	}
	return str;
}

//----------------------------------------------------------
// converts an Ascii character to UTF-8 encoding.
extern tstring AsciiToUTF8( unsigned char ch )
{
	tstring str;
	if ( ch >= 0x80 )
	{
		// 2-byte UTF-8 sequence.
		char byte0 = 0xC0 | ( ( ch & 0x07C0 ) >> 6 );
		char byte1 = 0x80 | ( ( ch & 0x3F ) );
		str += byte0;
		str += byte1;
	}
	else
	{
		// 1-byte UTF-8 char.
		char byte0 = ( char )ch;
		str += byte0;
	}
	return str;
}

//----------------------------------------------------------
// converts an Ascii string to UTF-8 encoding.
extern tstring AsciiToUTF8( tstring str )
{
	// convert a bunch of chars to UTF8.
	tstring outStr;
	size_t len = str.length();
	for ( size_t i = 0; i < len; ++i )
	{
		outStr += AsciiToUTF8( ( unsigned char )str.c_str()[ i ] );
	}
	return outStr;
}
