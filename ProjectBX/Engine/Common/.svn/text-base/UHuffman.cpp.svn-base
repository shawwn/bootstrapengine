//----------------------------------------------------------
// File:		UHuffman.cpp
// Author:		Kevin Bray
// Created:		10-03-08
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UHuffman.h"

// project includes.
#include "UBitReader.h"
#include "UBitWriter.h"

// std c++ includes.
#include <algorithm>
#include <set>

struct SNode
{
	// ctor.
	SNode() : parent( 0 ), score( 0 ), value( 0 ) { }

	// internal node data.
	SNode* parent;
	unsigned int score;
	unsigned char value;
};

#define REV( x )				( ( ( x & 0x01 ) << 7 ) | \
								  ( ( x & 0x02 ) << 5 ) | \
								  ( ( x & 0x04 ) << 3 ) | \
								  ( ( x & 0x08 ) << 1 ) | \
								  ( ( x & 0x10 ) >> 1 ) | \
								  ( ( x & 0x20 ) >> 3 ) | \
								  ( ( x & 0x40 ) >> 5 ) | \
								  ( ( x & 0x80 ) >> 7 ) )

static const unsigned char bitReverse[ 256 ] = {
	REV(   0 ), REV(   1 ), REV(   2 ), REV(   3 ), REV(   4 ), REV(   5 ), REV(   6 ), REV(   7 ),
	REV(   8 ), REV(   9 ), REV(  10 ), REV(  11 ), REV(  12 ), REV(  13 ), REV(  14 ), REV(  15 ),
	REV(  16 ), REV(  17 ), REV(  18 ), REV(  19 ), REV(  20 ), REV(  21 ), REV(  22 ), REV(  23 ),
	REV(  24 ), REV(  25 ), REV(  26 ), REV(  27 ), REV(  28 ), REV(  29 ), REV(  30 ), REV(  31 ),
	REV(  32 ), REV(  33 ), REV(  34 ), REV(  35 ), REV(  36 ), REV(  37 ), REV(  38 ), REV(  39 ),
	REV(  40 ), REV(  41 ), REV(  42 ), REV(  43 ), REV(  44 ), REV(  45 ), REV(  46 ), REV(  47 ),
	REV(  48 ), REV(  49 ), REV(  50 ), REV(  51 ), REV(  52 ), REV(  53 ), REV(  54 ), REV(  55 ),
	REV(  56 ), REV(  57 ), REV(  58 ), REV(  59 ), REV(  60 ), REV(  61 ), REV(  62 ), REV(  63 ),
	REV(  64 ), REV(  65 ), REV(  66 ), REV(  67 ), REV(  68 ), REV(  69 ), REV(  70 ), REV(  71 ),
	REV(  72 ), REV(  73 ), REV(  74 ), REV(  75 ), REV(  76 ), REV(  77 ), REV(  78 ), REV(  79 ),
	REV(  80 ), REV(  81 ), REV(  82 ), REV(  83 ), REV(  84 ), REV(  85 ), REV(  86 ), REV(  87 ),
	REV(  88 ), REV(  89 ), REV(  90 ), REV(  91 ), REV(  92 ), REV(  93 ), REV(  94 ), REV(  95 ),
	REV(  96 ), REV(  97 ), REV(  98 ), REV(  99 ), REV( 100 ), REV( 101 ), REV( 102 ), REV( 103 ),
	REV( 104 ), REV( 105 ), REV( 106 ), REV( 107 ), REV( 108 ), REV( 109 ), REV( 110 ), REV( 111 ),
	REV( 112 ), REV( 113 ), REV( 114 ), REV( 115 ), REV( 116 ), REV( 117 ), REV( 118 ), REV( 119 ),
	REV( 120 ), REV( 121 ), REV( 122 ), REV( 123 ), REV( 124 ), REV( 125 ), REV( 126 ), REV( 127 ),
	REV( 128 ), REV( 129 ), REV( 130 ), REV( 131 ), REV( 132 ), REV( 133 ), REV( 134 ), REV( 135 ),
	REV( 136 ), REV( 137 ), REV( 138 ), REV( 139 ), REV( 140 ), REV( 141 ), REV( 142 ), REV( 143 ),
	REV( 144 ), REV( 145 ), REV( 146 ), REV( 147 ), REV( 148 ), REV( 149 ), REV( 150 ), REV( 151 ),
	REV( 152 ), REV( 153 ), REV( 154 ), REV( 155 ), REV( 156 ), REV( 157 ), REV( 158 ), REV( 159 ),
	REV( 160 ), REV( 161 ), REV( 162 ), REV( 163 ), REV( 164 ), REV( 165 ), REV( 166 ), REV( 167 ),
	REV( 168 ), REV( 169 ), REV( 170 ), REV( 171 ), REV( 172 ), REV( 173 ), REV( 174 ), REV( 175 ),
	REV( 176 ), REV( 177 ), REV( 178 ), REV( 179 ), REV( 180 ), REV( 181 ), REV( 182 ), REV( 183 ),
	REV( 184 ), REV( 185 ), REV( 186 ), REV( 187 ), REV( 188 ), REV( 189 ), REV( 190 ), REV( 191 ),
	REV( 192 ), REV( 193 ), REV( 194 ), REV( 195 ), REV( 196 ), REV( 197 ), REV( 198 ), REV( 199 ),
	REV( 200 ), REV( 201 ), REV( 202 ), REV( 203 ), REV( 204 ), REV( 205 ), REV( 206 ), REV( 207 ),
	REV( 208 ), REV( 209 ), REV( 210 ), REV( 211 ), REV( 212 ), REV( 213 ), REV( 214 ), REV( 215 ),
	REV( 216 ), REV( 217 ), REV( 218 ), REV( 219 ), REV( 220 ), REV( 221 ), REV( 222 ), REV( 223 ),
	REV( 224 ), REV( 225 ), REV( 226 ), REV( 227 ), REV( 228 ), REV( 229 ), REV( 230 ), REV( 231 ),
	REV( 232 ), REV( 233 ), REV( 234 ), REV( 235 ), REV( 236 ), REV( 237 ), REV( 238 ), REV( 239 ),
	REV( 240 ), REV( 241 ), REV( 242 ), REV( 243 ), REV( 244 ), REV( 245 ), REV( 246 ), REV( 247 ),
	REV( 248 ), REV( 249 ), REV( 250 ), REV( 251 ), REV( 252 ), REV( 253 ), REV( 254 ), REV( 255 ),
};

//**********************************************************
// class UHuffman
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UHuffman::UHuffman()
: _compDst( 0 )
, _decompSrc( 0, 0 )
{

}

//----------------------------------------------------------
UHuffman::~UHuffman()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
unsigned int
UHuffman::Compress( unsigned char* result, unsigned char* codeLengths, const void* srcData,
					unsigned int srcSize )
{
	// get the source.
	unsigned char* src = ( unsigned char* )srcData;

	// build the code length table.
	CalcCodeLengths( codeLengths, src, srcSize );

	// set the outgoing stream data.
	SetCompStream( result, codeLengths );

	// write out each value.
	while ( srcSize-- > 0 )
		WriteValue( *src++ );

	// finish writing and return the total number of bits written.
	return FinishWriting();
}

//----------------------------------------------------------
unsigned int
UHuffman::Decompress( unsigned char* result, const unsigned char* codeLengths, const void* compressedData,
					  unsigned int srcBits )
{
	// set the stream data.
	SetDecompStream( codeLengths, ( unsigned char* )compressedData, srcBits );

	// use the stream functions.
	unsigned char* dst = result;
	while ( !IsReadFinished() )
		*dst++ = ReadValue();

	// return the total number of bytes decompressed.
	return ( unsigned int )( size_t )( dst - result );
}

//----------------------------------------------------------
void
UHuffman::CalcCodeLengths( unsigned char* codeLengths, const unsigned char* srcData,
						   unsigned int srcSize )
{
	// Note that this function is not fast.  It can be optimized
	// greatly if needed.
	unsigned int scores[ 256 ];
	MemSet( scores, 0, sizeof( scores ) );

	// compute the current score.
	for ( unsigned int i = 0; i < srcSize; ++i )
		scores[ srcData[ i ] ] += 1;

	// now calculate code lengths using the other variant of this
	// function.
	CalcCodeLengths( codeLengths, scores );
}

//----------------------------------------------------------
void
UHuffman::CalcCodeLengths( unsigned char* codeLengths, const unsigned int* scores )
{
	// Note that this function is not fast.  It can be optimized
	// greatly if needed.

	// build a huffman tree and use the depth of a character in the
	// tree as it's code length.

	struct SNodeCmp
	{
		bool operator () ( const SNode* lhs, const SNode* rhs ) const		{	return ( lhs->score < rhs->score );	}
		bool operator () ( const SNode& lhs, const SNode& rhs ) const		{	return ( lhs.score > rhs.score );	}
	};

	SNode nodes[ 511 ];
	SNode* leaves = nodes;
	SNode* branches = nodes + 256;

	// fill out leaves.
	for ( unsigned int i = 0; i < 256; ++i )
	{
		leaves[ i ].value = ( unsigned char )i;
		leaves[ i ].score = scores[ i ];
	}

	// build the tree.
	std::sort( leaves, leaves + 256, SNodeCmp() );

	// calculate the total number of leaves.
	unsigned int nodeCount = 0;
	while ( leaves[ nodeCount ].score > 0 )
		++nodeCount;

	// initialize unused codes to zero.
	for ( unsigned int i = nodeCount; i < 256; ++i )
		codeLengths[ leaves[ i ].value ] = 0;

	// if there is only one node, set it's value to 1 and return.
	if ( nodeCount == 1 )
	{
		codeLengths[ leaves[ 0 ].value ] = 1;
		return;
	}

	// build the queue.
	std::multiset< SNode*, SNodeCmp > queue;
	for ( unsigned int i = 0; i < nodeCount; ++i )
		queue.insert( leaves + i );

	// build the tree.
	while ( queue.size() > 1 )
	{
		// set the left node.
		SNode* left = *queue.begin();
		left->parent = branches;
		queue.erase( queue.begin() );

		// set the right node.
		SNode* right = *queue.begin();
		right->parent = branches;
		queue.erase( queue.begin() );

		// calculate the score for the current branch.
		branches->score = left->score + right->score;

		// insert the new node and advance to the next free branch.
		queue.insert( branches++ );
	}
	SNode* root = *queue.begin();

	// now calculate code lengths.
	for ( unsigned int i = 0; i < nodeCount; ++i )
	{
		// traverse up the tree to determine it's depth (and
		// subsequently the length of the huffman code).
		unsigned int len = 0;
		SNode* cur = leaves + i;
		while ( cur != root )
		{
			++len;
			cur = cur->parent;
		}

		// make sure that the tree doesn't have codes with more than
		// 16-bits as it will break our decoding algorithm.
		B_ASSERT( len <= 16 );

		// store the length in the output array.
		codeLengths[ leaves[ i ].value ] = ( unsigned char )len;
	}
}

//----------------------------------------------------------
void
UHuffman::SetCompStream( unsigned char* compressedData, const unsigned char* codeLengths )
{
	// build the internal dictionary.
	SetCompCodeLengths( codeLengths );

	// set the target pointer.
	SetCompStreamFast( compressedData );
}

//----------------------------------------------------------
void
UHuffman::SetCompCodeLengths( const unsigned char* codeLengths )
{
	// build the internal dictionary.
	BuildCompTables( codeLengths );
}

//----------------------------------------------------------
void
UHuffman::SetCompStreamFast( unsigned char* compressedData )
{
	// set the target pointer.
	_compDst.Reset( compressedData );
}

//----------------------------------------------------------
unsigned int
UHuffman::GetValueLen( unsigned char value ) const
{
	return _cmpLenTable[ value ];
}

//----------------------------------------------------------
void
UHuffman::WriteValue( unsigned char value )
{
	// write the current value to the output stream.
	_compDst.WriteBits( _cmpCodes[ value ], _cmpLenTable[ value ] );
}

//----------------------------------------------------------
unsigned int
UHuffman::FinishWriting()
{
	// make sure data is flushed out of the bit-writer.
	_compDst.Finish();

	// return the bit count.
	return _compDst.GetBitCount();
}

//----------------------------------------------------------
void
UHuffman::SetDecompStream( const unsigned char* codeLengths, const unsigned char* compressedData,
						   unsigned int srcBitCount )
{
	// build decompression tables.
	SetDecompCodeLengths( codeLengths );

	// set the source data.
	SetDecompStreamFast( compressedData, srcBitCount );
}

//----------------------------------------------------------
void
UHuffman::SetDecompCodeLengths( const unsigned char* codeLengths )
{
	// build decompression tables.
	BuildDecompTables( codeLengths );
}

//----------------------------------------------------------
void
UHuffman::BuildDecompFastTables( unsigned char* symTable, unsigned char* lenTable, unsigned char* rareSymTable,
								 unsigned short* maxCodeTable, unsigned char* rareCodeOffsetTable,
								 const unsigned char* codeLengths )
{
	// build the decompression tables.
	BuildDecompTables( codeLengths );

	// copy the data to the tables provided.
	MemCopy( symTable, _symTable, 256 );
	MemCopy( lenTable, _lenTable, 256 );
	MemCopy( rareSymTable, _rareSymTable, 512 );
	MemCopy( maxCodeTable, _maxCodeTable, 17 * sizeof( unsigned short ) );
	MemCopy( rareCodeOffsetTable, _rareCodeOffsetTable, 17 * sizeof( unsigned short ) );
}

//----------------------------------------------------------
void
UHuffman::SetDecompFastTables( const unsigned char* symTable, const unsigned char* lenTable,
							   const unsigned char* rareSymTable, const unsigned short* maxCodeTable,
							   const unsigned char* rareCodeOffsetTable )
{
	// copy the data to our internal tables.
	MemCopy( _symTable, symTable, 256 );
	MemCopy( _lenTable, lenTable, 256 );
	MemCopy( _rareSymTable, rareSymTable, 512 );
	MemCopy( _maxCodeTable, maxCodeTable, 17 * sizeof( unsigned short ) );
	MemCopy( _rareCodeOffsetTable, rareCodeOffsetTable, 17 * sizeof( unsigned short ) );
}

//----------------------------------------------------------
void
UHuffman::SetDecompStreamFast( const unsigned char* compressedData, unsigned int srcBitCount )
{
	_decompSrc.Reset( compressedData, srcBitCount );
}

//----------------------------------------------------------
unsigned char
UHuffman::ReadValue()
{
	// get the current value.
	unsigned int value = _decompSrc.PeekBits( 8 );

	// determine if the value is a short code or a long code.
	unsigned int codeLen = _lenTable[ value ];
	if ( codeLen != 0 )
	{
		// advance past the bits and return the symbol that the code
		// refers to.
		_decompSrc.SkipBits( codeLen );
		return _symTable[ value ];
	}

	// if we got here, then we need to read a long code.
	value = _decompSrc.PeekBits( 16 );

	// determine how long the code is.
	unsigned int revValue = ReverseBits( ( unsigned short )value );
	unsigned int bitCount = 8;
	for ( unsigned int i = 9; i <= 16; ++i )
		bitCount += ( revValue <= _maxCodeTable[ i ] );

	// calculate the code for the current value.
	unsigned int code = revValue >> ( 16 - bitCount );

	// lookup the offset for the given number of bits.
	unsigned int offset = _rareCodeOffsetTable[ bitCount ];

	// skip bits.
	_decompSrc.SkipBits( bitCount );

	// return the value.
	return _rareSymTable[ offset + code ];
}

//----------------------------------------------------------
bool
UHuffman::IsReadFinished()
{
	// return true when there are no more bits in the source bit
	// stream.
	return _decompSrc.IsFinished();
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
bool
UHuffman::DicSort( const SDicEntry& lhs, const SDicEntry& rhs )
{
	// sort by value if the code lengths are equal.
	if ( lhs.codeLen == rhs.codeLen )
		return lhs.value < rhs.value;

	// sort by code length, ascending in value.
	return lhs.codeLen > rhs.codeLen;
}

//----------------------------------------------------------
void
UHuffman::BuildDic( SDicEntry* dic, const unsigned char* codeLengths )
{
	// make sure the end of the array is set.
	dic[ 256 ].codeLen = 0;

	// build the dictionary.
	unsigned int symCount = 0;
	for ( unsigned int i = 0; i < 256; ++i )
	{
		// store information about the current symbol.
		dic[ i ].value = i;
		dic[ i ].codeLen = codeLengths[ i ];
		dic[ i ].code = 0;
	}

	// sort the dictionary by code length first, by value second.
	std::sort( dic, dic + 256, DicSort );

	// build the codes.
	unsigned short code = 0;
	SDicEntry* cur = dic;
	while ( cur->codeLen > 0 )
	{
		// store the current code in reverse.
		cur->code = 0;
		unsigned short src = code;
		unsigned int j = cur->codeLen - 1;
		for ( unsigned int i = 0; i < cur->codeLen; ++i, --j, src >>= 1 )
			cur->code |= ( src & 1 ) << j;

		// increment the code for the next entry.
		code += 1;

		// remove a bit if necessary.
		code >>= ( cur[ 0 ].codeLen - cur[ 1 ].codeLen );

		// advance to the next dictionary entry.
		++cur;
	}
#if 0
	// check the codes to make sure they're valid.
	size_t count = cur - dic;
	for ( unsigned int i = 0; i < count; ++i )
	{
		for ( unsigned int j = i + 1; j < count; ++j )
		{
			B_ASSERT( ( dic[ i ].code & dic[ j ].code ) != dic[ j ].code );
		}
	}
#endif
}

//----------------------------------------------------------
void
UHuffman::BuildCompTables( const unsigned char* codeLengths )
{
	// build the dictionary.
	SDicEntry dic[ 257 ];
	BuildDic( dic, codeLengths );

	// now build the compression tables.
	for ( unsigned int i = 0; i < 256; ++i )
	{
		_cmpCodes[ dic[ i ].value ] = dic[ i ].code;
		_cmpLenTable[ dic[ i ].value ] = dic[ i ].codeLen;
	}
}

//----------------------------------------------------------
void
UHuffman::BuildDecompTables( const unsigned char* codeLengths )
{
	// build the dictionary.
	SDicEntry dic[ 257 ];
	BuildDic( dic, codeLengths );

	// clear the max codes lookup table.
	for ( unsigned int i = 0; i < 17; ++i )
		_maxCodeTable[ i ] = 0;

	// left justify codes to show that they increase in value.
	unsigned short justifiedCodes[ 256 ];
	for ( unsigned int i = 0; i < 256; ++i )
	{
		// get the code in reverse.
		justifiedCodes[ i ] = ReverseBits( dic[ i ].code );
		_maxCodeTable[ dic[ i ].codeLen ] = Max( _maxCodeTable[ dic[ i ].codeLen ], justifiedCodes[ i ] );
#if 0
		unsigned int code = justifiedCodes[ i ]; // | ( 1 << dic[ i ].codeLen );
		unsigned int codeLen = dic[ i ].codeLen;
		PrintF( "0x%08X, %d\n", code, codeLen );
#endif
	}

	// set extra bits to allow for other code data.
	for ( unsigned int i = 0; i < 17; ++i )
	{
		unsigned int extraBits = ( 1 << ( 16 - i ) ) - 1;
		_maxCodeTable[ i ] |= extraBits;
	}

	// now build the decompression tables for common values.
	_rareCodeOffsetTable[ 16 ] = 0;
	_rareCodeOffsetTable[ dic[ 0 ].codeLen ] = 0;
	unsigned int curCodeLength = dic[ 0 ].codeLen;
	unsigned int curCodeOffset = 0;
	unsigned int curMaxCode = 0;
	for ( unsigned int i = 0; i < 256; ++i )
	{
		SDicEntry& curEntry = dic[ i ];
		if ( curEntry.codeLen == 0 )
			continue;

		// determine if we have a short code or a long code.
		if ( curEntry.codeLen <= 8 )
		{
			// get the number of unused bits in an 8-bit sequence.
			unsigned int highBits = ( 8 - curEntry.codeLen );
			unsigned int valueCount = 1 << highBits;
			for ( unsigned int j = 0; j < valueCount; ++j )
			{
				// calculate the current index and fill out the
				// table.
				unsigned int index = ( j << curEntry.codeLen ) + curEntry.code;
				_symTable[ index ] = curEntry.value;
				_lenTable[ index ] = curEntry.codeLen;
			}
		}
		else
		{
			// mark the current entry as being part of a longer code.
			unsigned int index = curEntry.code & 0xFF;
			_lenTable[ index ] = 0;

			// check to see if we're changing long code lengths.
			// Because we know codes are sorted from longest to
			// shortest, we can simply pack rare symbols here.
			if ( curCodeLength != curEntry.codeLen )
			{
				curCodeOffset += ( curMaxCode + 1 );
				curCodeLength = curEntry.codeLen;
				_rareCodeOffsetTable[ curCodeLength ] = curCodeOffset;
				curMaxCode = 0;
			}

			// add the current code to the group.
			unsigned int code = ReverseBits( curEntry.code ) >> ( 16 - curCodeLength );
			curMaxCode = Max( curMaxCode, code );

			// calculate the entry for the current rare symbol.
			_rareSymTable[ curCodeOffset + code ] = curEntry.value;
		}
	}
}

//----------------------------------------------------------
unsigned short
UHuffman::ReverseBits( unsigned short src )
{
	unsigned short dst;
	dst = bitReverse[ src & 0xFF ] << 8;
	dst |= bitReverse[ src >> 8 ];
	return dst;
}

// ye olde code.
#if 0
// this contains the number of bits used to store each score in the
// huffman data.  Note that this should only be set to 8, 16, or 32.
// Note that this can have a slight but negative effect on the
// compression ratio for data sets larger than 2^SCOREBITCOUNT.
#define SCORE_BYTE_COUNT		1
#define SCORE_BIT_COUNT			8

// structure that maps a score to a value.
struct STreeNode
{
	unsigned short value;
	unsigned short score;
	STreeNode* parent;
	STreeNode* children[ 2 ];
};

// structure for rare symbols.
struct SRareSymbol
{
	unsigned char symbol;
	unsigned short code;
	unsigned short codeLen;
};

bool RareSymbolSort( const SRareSymbol& lhs, const SRareSymbol& rhs )
{
	unsigned short lhsMask = lhs.code & 0xFF;
	unsigned short rhsMask = rhs.code & 0xFF;
	if ( lhsMask == rhsMask )
	{
		// return the lesser code.
		if ( lhs.codeLen == rhs.codeLen )
			return lhs.code < rhs.code;

		// return the lesser code length.
		return lhs.codeLen < rhs.codeLen;
	}

	// return the lesser mask.
	return lhsMask < rhsMask;
}

// this should be used to sort scores.
class ScoreSort
{
public:
	bool operator() ( const STreeNode* lhs, const STreeNode* rhs )
	{
		return ( lhs->score < rhs->score );
	}
};

// File local function declarations.  Note that the node \pool must
// contain 513 nodes (2 * 256 + 1).
static STreeNode* BuildTree( STreeNode** nodes, unsigned int* minDepth = 0 );
static void BuildBitLengths( unsigned char* lengths, const unsigned int* scores );

//**********************************************************
// class UHuffman
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UHuffman::UHuffman()
: _bitStream( 0 )
, _bitCount( 0 )
, _ownBits( false )
{
	
}

//----------------------------------------------------------
UHuffman::~UHuffman()
{
	if ( _ownBits )
		delete[] _bitStream;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UHuffman::Encode( const void* srcData, unsigned int srcSize )
{
	// initialize the value components of the SValueScore structure.
	STreeNode nodeData[ 513 ];
	STreeNode* nodes[ 513 ];
	for ( unsigned int i = 0; i < 513; ++i )
	{
		nodeData[ i ].value = i;
		nodeData[ i ].score = 0;
		nodeData[ i ].children[ 0 ] = 0;
		nodeData[ i ].children[ 1 ] = 0;
		nodeData[ i ].parent = 0;
		nodes[ i ] = nodeData + i;
	}

	// compute the occurance count for every value and use that as the
	// score.
	unsigned char* src = ( unsigned char* )srcData;
	unsigned char* end = src + srcSize;
	unsigned int maxScore = 0;
	unsigned int scores[ 256 ];
	MemSet( scores, 0, sizeof( scores ) );
	while ( src != end )
	{
		// advance the current score.
		scores[ *src ] += 1;
		maxScore = Max( scores[ *src++ ], maxScore );
	}

	// figure out how many bits to remove for the quantized scores.
	unsigned int adjust = 0;
	unsigned int maxCount = Log2( maxScore ) + 1;
	if ( maxCount > SCORE_BIT_COUNT )
		adjust = maxCount - SCORE_BIT_COUNT;

	// quantize the scores.
	unsigned int mask = ( 1 << adjust ) - 1;
	for ( unsigned int i = 0; i < 256; ++i )
	{
		unsigned int roundUp = ( scores[ i ] & mask ) != 0;
		scores[ i ] >>= adjust;
		scores[ i ] += roundUp;
		nodeData[ i ].score = ( unsigned short )scores[ i ];
	}

	// build the tree.
	STreeNode* root = BuildTree( nodes );

	// allocate space to store the compressed data as well as some
	// extra space for the header information.
	_bitStream = new unsigned char[ srcSize + 513 + 4 ];
	_ownBits = true;

	// get the first byte in the outgoing bit stream.
	unsigned int bitIndex = 0;
	unsigned char* dst = ( unsigned char* )_bitStream;

#if SCORE_BYTE_COUNT == 1
	// write out an RLE encoded score table.
	unsigned int runLen = 0;
	for ( unsigned int i = 0; i < 256; i += runLen )
	{
		// calculate the maximum allowable run length.
		unsigned int maxLen = 256 - i;

		// calculate the current run length.
		runLen = 0;
		STreeNode* curNode = nodeData + i;
		STreeNode* laNode = curNode;
		while ( maxLen-- > 0 && laNode->score == curNode->score )
		{
			++runLen;
			++laNode;
		}

		// output the run length and the score.
		*dst++ = runLen;
		*dst++ = ( unsigned char )curNode->score;
	}
#else
#error implement me!
#endif
	// make sure dst is dword aligned.
	if ( ( size_t )dst & 2 )
	{
		*dst++ = 0;
		*dst++ = 0;
	}

	// clear the output bytes to zero.
	MemSet( dst, 0, srcSize );
	unsigned int dstBit = 0;

	// now encode the data.
	src = ( unsigned char* )srcData;
	while ( src != end )
	{
		// get the node for the current byte and traverse up to the
		// root.
		unsigned int curBits = 0;
		unsigned int count = 0;
		STreeNode* node = nodeData + *src++;
		while ( node != root )
		{
			// get a 1 or a zero.
			curBits <<= 1;
			curBits |= ( ( node == node->parent->children[ 1 ] ) != 0 );
			node = node->parent;
			++count;
		}

		// write out bits to our bit stream.
		unsigned int curSel = 1;
		while ( count-- > 0 )
		{
			// write out the current bit.
			dst[ dstBit >> 3 ] |= ( ( curBits & curSel ) != 0 ) << ( dstBit & 7 );
			curSel += curSel;
			dstBit += 1;
		}
	}

	// record the bit count.
	_bitCount = dstBit;
}

//----------------------------------------------------------
void
UHuffman::SetEncodedBits( const void* src, unsigned int bitCount )
{
	// delete the current bit stream if one was set and we own it.
	if ( _ownBits )
		delete[] _bitStream;

	// extract the source.
	_bitStream = ( void* )src;
	_bitCount = bitCount;
	_ownBits = false;
}

//----------------------------------------------------------
unsigned int
UHuffman::Decode( void* dstData ) const
{
	// initialize the value components of the SValueScore structure.
	STreeNode nodeData[ 513 ];
	STreeNode* nodes[ 513 ];
	for ( unsigned int i = 0; i < 513; ++i )
	{
		nodeData[ i ].value = i;
		nodeData[ i ].score = 0;
		nodeData[ i ].children[ 0 ] = 0;
		nodeData[ i ].children[ 1 ] = 0;
		nodeData[ i ].parent = 0;
		nodes[ i ] = nodeData + i;
	}

	// read in the value scores.
	unsigned int index = 0;
	unsigned char* src = ( unsigned char* )_bitStream;
	while ( index < 256 )
	{
		// initialize the node scores.
		unsigned char runLen = *src++;
		unsigned char score = *src++;
		while ( runLen-- > 0 )
			nodeData[ index++ ].score = score;
	}

	// make sure src is dword aligned.
	if ( ( size_t )src & 2 )
		src += 2;

	// build the huffman tree.
	unsigned int minDepth = 0;
	STreeNode* root = BuildTree( nodes, &minDepth );

	// calculate a hash table that's at least the size of the minimum depth.
	minDepth = Min( minDepth, 8U );
	STreeNode* minHash[ 256 ];

	// populate the min-value hash table.
	STreeNode* node = root;
	unsigned int bit = 1;
	unsigned int minCount = 1 << minDepth;
	unsigned int minMask = minCount - 1;
	for ( unsigned int i = 0; i < minCount; ++i )
	{
		node = root;
		bit = 1;
		for ( unsigned int j = 0; j < minDepth; ++j )
		{
			node = node->children[ ( i & bit ) != 0 ];
			bit += bit;
		}

		// store the node in the hash table.
		minHash[ i ] = node;
	}

	// decode the data.
	node = root;
	unsigned int curBit = 0;
	unsigned int* srcDwords = ( unsigned int* )src;
	unsigned char* dst = ( unsigned char* )dstData;
#if 1
	unsigned int k8 = 8;
	unsigned int k12 = 12;
	unsigned int bitCount = _bitCount;
	unsigned int result = 0;
	__asm
	{
		; ecx = current bit.
		; eax = current dword.
		; ebx = current dword bit.
		xor		ecx, ecx
		mov		esi, srcDwords
		mov		edi, dst

	$loop:
		mov		eax, ecx						; calculate the source dwords and bit indices.
		mov		ebx, ecx
		shr		eax, 3
		and		eax, ~3;
		and		ebx, 31
		add		ecx, minDepth					; advance the source bits

		movd	mm1, [ esi + eax + 4 ]
		movd	mm0, [ esi + eax ]				; load in two dwords and pack them together.
		psllq	mm1, 32
		por		mm0, mm1

		movd	mm2, ebx						; get the bits we need to process now.
		psrlq	mm0, mm2
		movq	mm1, mm0

		movd	eax, mm1						; perform the min-hash lookup.
		and		eax, minMask
		mov		edx, [ minHash + 4 * eax ]

		mov		ebx, [ edx ]					; check the value to see if the node is a leaf.
		cmp		bx, 256
		jge		$walk

		mov		BYTE PTR [ edi ], bl			; write the current value.
		add		edi, 1

		cmp		ecx, bitCount					; determine whether or not to loop.
		jl		$loop

	$walk:
		; we need to walk the tree.
		mov		ebx, minCount					; set ebx to minCount.

	$walk_loop:
		movd	eax, mm0			; THIS IS EXPENSIVE AND IS CAUSING PROBLEMS.

#if 1
		test	eax, ebx						; place either the left node or the right node into

		; this runs better on non-uniform data sets (which is what
		; huffman coding is best suited to deal with)
		setnz	al
		and		eax, 0x000000FF
		add		eax, eax
		add		eax, eax
		mov		edx, [ edx + eax + 8 ]			; edx for traversal.
#else
		cmovz	edx, [ edx + 8  ]
		cmovnz  edx, [ edx + 12 ]
#endif

		add		ebx, ebx						; multiply ebx by two to get the next bit.
		mov		eax, [ edx ]
		add		ecx, 1							; increment the bit counter.
		cmp		ax, 256
		jge		$walk_loop

		mov		BYTE PTR [ edi ], al			; write the current value.
		add		edi, 1

		cmp		ecx, bitCount					; determine whether or not to loop.
		jl		$loop

		; all done!
		emms

		sub		edi, dst						; return the number of bytes written.
		mov		result, edi
	}

	return result;
#else
	while ( curBit < _bitCount )
	{
		// grab the minimum number of bits off of the bit stream.
		unsigned int bitIdx = curBit & 31;
		unsigned int dwordIdx = curBit >> 5;

		// expand to 32-bits to avoid dealing with boundary conditions.
		unsigned int bits = srcDwords[ dwordIdx ];
		unsigned int bitsNext = srcDwords[ dwordIdx + 1 ];
		__int64 totalBits = bitsNext;
		totalBits <<= 32;
		totalBits |= bits;
		totalBits >>= bitIdx;
		bits = ( unsigned int )( totalBits & 0xFFFFFFFF );
/*
		// shift.
		__asm
		{
			mov		eax, bits
			mov		ebx, bitsNext
			mov		cl, byte ptr bitIdx
			shrd	eax, ebx, cl
			mov		bits, eax
		}
*/
		// advance the current bit and get the node we should use to
		// start traversal.
		node = minHash[ bits & minMask ];
		curBit += minDepth;

		// scan through any remaining bits until we hit a leaf.
		unsigned int bitPos = minCount;
		while ( node->value >= 256 )
		{
			node = node->children[ ( bits & bitPos ) != 0 ];
			bitPos += bitPos;
			++curBit;
		}

		// write out the data.
		*dst++ = ( unsigned char )node->value;
	}

	// return the size of the data.
	return ( unsigned int )( dst - ( unsigned char* )dstData );
#endif
}

//----------------------------------------------------------
void
UHuffman::EncodeFast( const void* srcData, unsigned int srcSize )
{
	// initialize the value components of the SValueScore structure.
	STreeNode nodeData[ 513 ];
	STreeNode* nodes[ 513 ];
	for ( unsigned int i = 0; i < 513; ++i )
	{
		nodeData[ i ].value = i;
		nodeData[ i ].score = 0;
		nodeData[ i ].children[ 0 ] = 0;
		nodeData[ i ].children[ 1 ] = 0;
		nodeData[ i ].parent = 0;
		nodes[ i ] = nodeData + i;
	}

	// compute the occurance count for every value and use that as the
	// score.
	unsigned char* src = ( unsigned char* )srcData;
	unsigned char* end = src + srcSize;
	unsigned int maxScore = 0;
	unsigned int scores[ 256 ];
	unsigned int sum = 0;
	MemSet( scores, 0, sizeof( scores ) );
	while ( src != end )
	{
		// advance the current score.
		scores[ *src ] += 1;
		maxScore = Max( scores[ *src++ ], maxScore );
	}

	// figure out how many bits to remove for the quantized scores.
	unsigned int adjust = 0;
	unsigned int maxCount = Log2( maxScore ) + 1;
	if ( maxCount > SCORE_BIT_COUNT )
		adjust = maxCount - SCORE_BIT_COUNT;

	// quantize the scores.
	unsigned int mask = ( 1 << adjust ) - 1;
	for ( unsigned int i = 0; i < 256; ++i )
	{
		unsigned int roundUp = ( scores[ i ] & mask ) != 0;
		scores[ i ] >>= adjust;
		scores[ i ] += roundUp;
		nodeData[ i ].score = ( unsigned short )scores[ i ];
	}

	// build the tree.
	STreeNode* root = BuildTree( nodes );

	// allocate space to store the compressed data as well as some
	// extra space for the header information.
	_bitStream = new unsigned char[ srcSize + 513 + 4 ];
	_ownBits = true;

	// get the first byte in the outgoing bit stream.
	UBitWriter writer( ( unsigned char* )_bitStream );

	// tables to contain precomputed lookup information.
	unsigned char bitCounts[ 256 ];
	unsigned char symbols[ 256 ];
	unsigned char rareSymbols[ 256 ];	// symbols for codes greater than 8-bits in length.
//	unsigned char bitLenOffsets[ 17 ];	// offsets into the rare symbol table for group starts.
//	unsigned int minCodes[ 17 ];		// note that we only need 9..16.

	// internal array for rare symbols.
	SRareSymbol rareSymbolInfo[ 256 ];
	unsigned int rareSymbolCount = 0;

	// make sure our tables are initialized to valid values if
	// necessary.
//	memset( minCodes, 0xFF, sizeof( minCodes ) );
//	memset( bitLenOffsets, 0, sizeof( bitLenOffsets ) );
	memset( symbols, 0, sizeof( symbols ) );
	memset( rareSymbols, 0, sizeof( rareSymbols ) );
	memset( bitCounts, 0, sizeof( bitCounts ) );

	// precompute bit counts and symbols for sequences of bits.
	unsigned int bitCount = 0;
	unsigned int shift = 0;
	STreeNode* child = 0;
	STreeNode* curNode = 0;

	// determine the bit codes for each character.
	for ( unsigned int i = 0; i < 256; ++i )
	{
		// get the current node and make sure it's used.
		STreeNode* node = nodeData + i;
		if ( node->parent == 0 )
			continue;

		// determine the bit-code and the number of bits for the
		// current value.
		unsigned int bitCode = 0;
		unsigned int bitCount = 0;
		do
		{
			// determine what bit to set.
			bitCode <<= 1;
			bitCode |= ( node == node->parent->children[ 1 ] );

			// increment the bit count.
			++bitCount;

			// move up the tree.
			node = node->parent;
		}
		while ( node != root );

		// fill in the tables.
		if ( bitCount <= 8 )
		{
			// determine how many times the current code will appear
			// in the look-up tables.
			unsigned int bitsLeft = ( 8 - bitCount );
			unsigned int entryCount = 1 << bitsLeft;
			for ( unsigned int j = 0; j < entryCount; ++j )
			{
				// add the current entry to our tables.
				unsigned int tableIndex = ( j << bitCount ) | bitCode;
				symbols[ tableIndex ] = ( unsigned char )i;

				// write out the bit count for the current symbol.
				bitCounts[ tableIndex ] = bitCount - 1;
			}
		}
		else
		{
			// store information about the rare symbol.
			SRareSymbol* symbol = rareSymbolInfo + rareSymbolCount++;

			// store the symbol information.
			symbol->symbol = i;
			symbol->code = bitCode;
			symbol->codeLen = bitCount;

			// initialize the current bit-count to 0.  Entries in the
			// symbol table can refer to multiple long codes.
			unsigned int tableIndex = bitCode & 0xFF;
			bitCounts[ tableIndex ] = Max( bitCounts[ tableIndex ], ( unsigned char )( bitCount - 1 ) );
		}
	}

	// sort rare symbols by bit-count, then by code.
	std::sort( rareSymbolInfo, rareSymbolInfo + rareSymbolCount, RareSymbolSort );

	// redirect low code-bits to the appropriate location in the
	// table.
	unsigned int groupSize = 0;
	unsigned int offset = 0;
	for ( unsigned int i = 0; i < rareSymbolCount; ++i )
	{
		// get the low 8-bits of the original code page.
		unsigned int lowBits = rareSymbolInfo[ i ].code & 0xFF;

		// store the indirection to the start of this code page
		// in the low symbol table.
		symbols[ lowBits ] = offset;

		// calculate how big of a region we need.
		unsigned int maxBitCount = bitCounts[ lowBits ] - 7;
		unsigned int regionSize = 1 << maxBitCount;

		// determine how many items are in the current code page.
		unsigned int la = i + 1;
		while ( ( rareSymbolInfo[ la ].code & 0xFF ) == lowBits )
			++la;

		// now populate the table for every item.
		unsigned int highBits = lowBits >> 8;
		for ( unsigned int j = i + 1; j <= la; ++j )
		{
			unsigned int code = j - offset;
			if ( ( code & highBits ) == highBits )
				rareSymbols[ offset + j ] = rareSymbolInfo[ j ].symbol;
		}

		// advance the offset.
		offset += regionSize;
/*
		// determine how many items are in the current code page.
		unsigned int la = i + 1;
		while ( ( rareSymbolInfo[ la ].code & 0xFF ) == lowBits )
			++la;

		// write the symbols to the current code page.
		for ( unsigned int j = i; j < la; ++j )
			rareSymbols[ offset + ( rareSymbolInfo[ j ].code >> 8 ) ] = rareSymbolInfo[ j ].symbol;

		// get the maximum value for the current codepage.
		unsigned int maxValue = ( rareSymbolInfo[ la - 1 ].code >> 8 );

		// advance the offset by the maximum value, plus one.
		offset += maxValue + 1;
*/
	}

	// write out the bit count data for 8-bit and smaller entries.
	// A value of 0 means that the bit count is larger than 8.
	for ( unsigned int i = 0; i < 256; ++i )
		writer.WriteBits( bitCounts[ i ], 4 );

	// write out symbols.
	for ( unsigned int i = 0; i < 256; ++i )
		writer.WriteBits( symbols[ i ], 8 );

	// write out rare symbols.
	for ( unsigned int i = 0; i < 256; ++i )
		writer.WriteBits( rareSymbols[ i ], 8 );
/*
	// write out the minimum values for long codes.
	for ( unsigned int i = 9; i < 17; ++i )
		writer.WriteBits( minCodes[ i ], i );

	// write out the offsets.
	for ( unsigned int i = 9; i < 17; ++i )
		writer.WriteBits( bitLenOffsets[ i ], 8 );
*/
	// write the data.
	src = ( unsigned char* )srcData;
	while ( src != end )
	{
		// get the node for the current byte and traverse up to the
		// root.
		unsigned int bits = 0;
		unsigned int bitCount = 0;
		STreeNode* node = nodeData + *src++;
		while ( node != root )
		{
			// write either a 1 or a zero.
			bits <<= 1;
			bits |= ( node == node->parent->children[ 1 ] );

			// increment the bit count.
			++bitCount;

			// get our parent.
			node = node->parent;
		}

		// store the bits for the current value.
		writer.WriteBits( bits, bitCount );
	}

	// make sure we flush any remaining bits.
	writer.Finish();

	// record the bit count.
	_bitCount = writer.GetBitCount();
}

//----------------------------------------------------------
unsigned int
UHuffman::DecodeFast( void* dest )
{
	UBitReader reader( ( unsigned char* )_bitStream, ( _bitCount + 7 ) / 8 );

	// various tables to load.
	unsigned char bitCounts[ 256 ];
	unsigned char symbols[ 256 ];
	unsigned char rareSymbols[ 256 ];
	unsigned int minCodesTest[ 17 ];
	unsigned int minCodes[ 17 ];
	unsigned char bitLenOffsets[ 17 ];

	MemSet( minCodes, 0, sizeof( minCodes ) );
	MemSet( minCodesTest, 0, sizeof( minCodesTest ) );
	MemSet( bitLenOffsets, 0, sizeof( bitLenOffsets ) );

	// read in the bit count table.
	for ( unsigned int i = 0; i < 256; ++i )
		bitCounts[ i ] = reader.ReadBits( 4 );

	// read in the symbol table.
	for ( unsigned int i = 0; i < 256; ++i )
		symbols[ i ] = reader.ReadBits( 8 );

	// read in rare symbols.
	for ( unsigned int i = 0; i < 256; ++i )
		rareSymbols[ i ] = reader.ReadBits( 8 );
/*
	// read in the minimum values for long codes.
	for ( unsigned int i = 9; i < 17; ++i )
	{
		minCodes[ i ] = reader.ReadBits( i );
		minCodesTest[ i ] = ( 1 << i ) | minCodes[ i ];	// left justify.
	}

	// write out the offsets.
	for ( unsigned int i = 9; i < 17; ++i )
		bitLenOffsets[ i ] = reader.ReadBits( 8 );
*/
	// peek 8-bits of data.
	unsigned char* output = ( unsigned char* )dest;
	unsigned int index = 0;
	int bitsRemaining = reader.GetBitsRemaining();
	while ( bitsRemaining > 0 )
	{
		// get the current symbol and look up it's bit count.
		unsigned int value = reader.PeekBits( 8 );
		unsigned int bitCount = bitCounts[ value ] + 1;
		if ( bitCount <= 8 )
		{
			// write out the value.
			output[ index++ ] = symbols[ value ];
			reader.SkipBits( bitCount );
			bitsRemaining -= bitCount;
		}
		else
		{
			// look up the long bit code.
			value = reader.PeekBits( 16 );

			// get the code page in the rare symbol table.
			unsigned char rareSymbolOffset = symbols[ value & 0xFF ];

			// calculate a mask for the current value.
			unsigned int mask = ( 1 << bitCount ) - 1;
			unsigned int code = ( value & mask ) >> 8;

			// we need a mask here for value.  Otherwise, it can be
			// too big and address outside of the codepage.  perhaps
			// we can use some method of figuring out how many bits
			// are used here?
			output[ index++ ] = rareSymbols[ rareSymbolOffset + code ];

			// skip past the bits used.
//			reader.SkipBits( bitsUsed );
		}
	}

	// return the index.
	return index;
}


//==========================================================
// public methods
//==========================================================


//**********************************************************
// File local functions
//**********************************************************

//----------------------------------------------------------
STreeNode*
BuildTree( STreeNode** nodes, unsigned int* minDepth )
{
	// build the tree.
	STreeNode** branches = nodes + 256;
	STreeNode** leaves = nodes;

	// get the current node to use as a branch
	STreeNode** curBranch = branches;

	// eliminate all unused leaves.
	STreeNode** dstLeaf = leaves;
	STreeNode** curLeaf = leaves;
	STreeNode** endLeaf = leaves + 256;
	while ( curLeaf != endLeaf )
	{
		if ( ( *curLeaf )->score != 0 )
			*dstLeaf++ = *curLeaf;
		++curLeaf;
	}

	// create two priority queues for tree generation.
	typedef std::multiset< STreeNode*, ScoreSort > PriorityQueue;
	PriorityQueue nodeQueue;

	// pair up items in the first queue and stick them in the second
	// queue.
	unsigned int nodeCount = ( unsigned int )( dstLeaf - leaves );
	for ( unsigned int i = 0; i < nodeCount; ++i )
		nodeQueue.insert( leaves[ i ] );

	// now pair items.
	while ( nodeQueue.size() > 1 )
	{
		// build a new node.  Add the first child.
		STreeNode* child0 = *nodeQueue.begin();
		( *curBranch )->children[ 0 ] = child0;
		( *curBranch )->score = child0->score;
		child0->parent = ( *curBranch );
		nodeQueue.erase( nodeQueue.begin() );

		// add the second child if necessary.
		if ( nodeQueue.size() > 0 )
		{
			STreeNode* child1 = *nodeQueue.begin();
			( *curBranch )->children[ 1 ] = child1;
			( *curBranch )->score += child1->score;
			child1->parent = ( *curBranch );
			nodeQueue.erase( nodeQueue.begin() );
		}

		// insert the new node into the opposite queue.
		nodeQueue.insert( *curBranch++ );
	}

	// now we should have our tree.
	return ( *nodeQueue.begin() );
/*
	// sort the list of items from most used to least used.
	unsigned int nodeCount = ( unsigned int )( dstLeaf - leaves );
	std::sort( leaves, leaves + nodeCount, ScoreSort() );

	// alter the scores of the leaves so that they are linear.
	for ( unsigned int i = 0; i < nodeCount; ++i )
		leaves[ i ]->score = nodeCount - i;

	// the first item in the list of leaves is guaranteed to be the most
	// shallow leaf in the tree.
	STreeNode* primeLeaf = leaves[ 0 ];

	// now repeatedly merge leaves.
	STreeNode** dstNode = 0;
	do
	{
		// get the last pair of nodes in the array.
		STreeNode** curDst = leaves + nodeCount - 2;
		STreeNode** curPair = curDst;

		( *curBranch )->children[ 0 ] = *curPair++;
		( *curBranch )->children[ 0 ]->parent = *curBranch;
		( *curBranch )->score = ( *curBranch )->children[ 0 ]->score;
	 
		( *curBranch )->children[ 1 ] = *curPair;
		( *curBranch )->children[ 1 ]->parent = *curBranch;
		( *curBranch )->score += ( *curBranch )->children[ 1 ]->score;

		// place the node in the array where the previous node used
		// to be.
		*curDst = *curBranch++;

		// subtract one from the total node count.
		--nodeCount;

		// bubble sort the last node into the correct position.
		unsigned int i = nodeCount;
		while ( i-- > 1 )
		{
			if ( leaves[ i - 1 ]->score < leaves[ i ]->score )
				Swap( leaves[ i - 1 ], leaves[ i ] );
			else
				break;
		}
	}
	while ( nodeCount > 1 );

	// does the user want the minimum depth?
	if ( minDepth )
	{
		// calculate the minimum depth of the tree.
		unsigned int minNodeDepth = 0;
		while ( primeLeaf != *leaves )
		{
			primeLeaf = primeLeaf->parent;
			++minNodeDepth;
		}

		// return the minimum depth to the caller.
		*minDepth = minNodeDepth;
	}

	// if we're all done, return the root node of the tree.
	return *leaves;
*/
}

//----------------------------------------------------------
void
BuildBitCodes( unsigned char* lengths, const unsigned int* scores )
{
	// build nodes.
	STreeNode nodes[ 511 ];
	STreeNode* leaves = nodes;
	STreeNode* branches = nodes + 256;
	for ( unsigned int i = 0; i < 256; ++i )
	{
		leaves[ i ].value = i;
		leaves[ i ].score = scores[ i ];
		leaves[ i ].parent = 0;
		leaves[ i ].children[ 0 ] = leaves[ i ].children[ 1 ] = 0;
	}

	// sort the list of items from highest score to lowest score.
	std::sort( leaves, leaves + 256, ScoreSort() );

	// determine the total number of elements.
	unsigned int nodeCount = 0;
	while ( leaves[ nodeCount ].score != 0 )
		++nodeCount;

	// merge leaves.
	STreeNode** dstNode = 0;
	do
	{
		// get the last pair of nodes in the array.
		STreeNode** curDst = leaves + nodeCount - 2;
		STreeNode** curPair = curDst;

		( *curBranch )->children[ 0 ] = *curPair++;
		( *curBranch )->children[ 0 ]->parent = *curBranch;
		( *curBranch )->score = ( *curBranch )->children[ 0 ]->score;
	 
		( *curBranch )->children[ 1 ] = *curPair;
		( *curBranch )->children[ 1 ]->parent = *curBranch;
		( *curBranch )->score += ( *curBranch )->children[ 1 ]->score;

		// place the node in the array where the previous node used
		// to be.
		*curDst = *curBranch++;

		// subtract one from the total node count.
		--nodeCount;

		// bubble sort the last node into the correct position.
		unsigned int i = nodeCount;
		while ( i-- > 1 )
		{
			if ( leaves[ i - 1 ]->score < leaves[ i ]->score )
				Swap( leaves[ i - 1 ], leaves[ i ] );
			else
				break;
		}
	}
	while ( nodeCount > 1 );

	// if we're all done, return the root node of the tree.
	return *leaves;
}
#endif
