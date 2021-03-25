// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.

// class header.
#include "lzw.h"

// project includes.
#include "stream.h"
#include "crc32.h"

// stl includes.
#include <vector>

#define DIC_SHIFT		20
#define MAX_ENTRY_SIZE	64
#define DIC_COUNT		( 1 << ( 32 - DIC_SHIFT ) )
#define DIC_HASH_MASK	( DIC_COUNT-1 )
#define DIC_MASK		( ~( ( DIC_HASH_MASK ) << DIC_SHIFT ) )

template < typename _T >
struct SDicEntry
{
	SDicEntry( const _T& initVal )
	: _itemCount( 1 )
	{
		_items[ 0 ] = initVal;
	}

	unsigned int		_itemCount;
	_T					_items[ MAX_ENTRY_SIZE+1 ];
};

//----------------------------------------------------------
template < typename _T >
unsigned int HashDic( const _T& curVal )
{
	unsigned int hash = HashString( &curVal, sizeof( curVal ) );
	return ( hash & DIC_HASH_MASK );
}

//----------------------------------------------------------
template < typename _T >
void BuildInitialDic( std::vector< SDicEntry< _T > >* dics, const _T* data, unsigned int count )
{
	// scan through the data adding to the dictionary.
	while ( count-- > 0 )
	{
		const _T& cur = data[ count ];

		// determine what dictionary to use.
		unsigned int dicIdx = HashDic( cur );
		std::vector< SDicEntry< _T > >& dic = dics[ dicIdx ];

		// try to find the item in the dictionary.
		unsigned int i = 0;
		for ( ; i < dic.size(); ++i )
		{
			// try to find the item in the dictionary.
			if ( cur == dic[ i ]._items[ 0 ] )
				break;
		}

		// if the item wasn't found, add it.
		if ( i == dic.size() )
			dic.push_back( SDicEntry< _T >( cur ) );
	}
}

//----------------------------------------------------------
template < typename _T >
bool CheckMatch( const _T** curVal, SDicEntry< _T >& curDicEntry )
{
	unsigned int dicEntryItemCount = curDicEntry._itemCount;
	unsigned int i = 0;
	for ( ; i < dicEntryItemCount; ++i, ++( *curVal ) )
	{
		if ( *( *curVal ) != curDicEntry._items[ i ] )
			break;
	}

	return ( i == dicEntryItemCount );
}

//----------------------------------------------------------
template < typename _T >
size_t FindMatch( std::vector< SDicEntry< _T > >* dics, const _T** data, size_t& count )
{
	// get the first character to determine what dictionary we should look in to.
	unsigned int dicHashIdx = HashDic( **data );

	// scan the dictionary in reverse.
	std::vector< SDicEntry< _T > >& dic = dics[ dicHashIdx ];
	size_t dicIdx = dic.size();
	while ( dicIdx-- > 0 )
	{
		// get the current dictionary entry.
		SDicEntry< _T >& curDicEntry = dic[ dicIdx ];

		// now try to match it with the source data.
		size_t dicEntryItemCount = curDicEntry._itemCount;
		if ( dicEntryItemCount <= count )
		{
			// if a match was found, add a new dictionary entry including the next
			// character in the sequence (if possible).
			const _T* curVal = *data;
			if ( CheckMatch( &curVal, curDicEntry ) )
			{
				// add the new dictionary entry if possible.
				if ( dicEntryItemCount < count && dicEntryItemCount < MAX_ENTRY_SIZE )
				{
					// create a new entry.
					SDicEntry< _T > newEntry = curDicEntry;
					newEntry._items[ curDicEntry._itemCount ] = *curVal;
					++newEntry._itemCount;

					// add the new entry to the dictionary.
					dic.push_back( newEntry );
				}

				// advance the data pointer.
				*data += dicEntryItemCount;
				count -= dicEntryItemCount;
				break;
			}
		}
	}

	// a valid entry had to have been found!
	assert( dicIdx != ~0 && dicIdx < DIC_MASK );

	// return the dictionary entry for the current item.
	return ( dicIdx ) | ( dicHashIdx << DIC_SHIFT );
}

//----------------------------------------------------------
template < typename _T >
void LZWEncode( Stream& output, const _T* data, unsigned int count )
{
	// build the initial dictionaries.
	std::vector< SDicEntry< _T > > dics[ DIC_COUNT ];
	BuildInitialDic< _T >( dics, data, count );

	// write the initial dictionaries.
	for ( unsigned int i = 0; i < DIC_COUNT; ++i )
	{
		std::vector< SDicEntry< _T > >& dic = dics[ i ];
		output.WriteValue( dic.size() );
		for ( size_t j = 0; j < dic.size(); ++j )
			output.WriteValue( dic[ j ]._items[ 0 ] );
	}

	// skip some space that we'll go back and fill with the token count.
	unsigned int countOffset = output.GetOffset();
	output.SetOffset( countOffset + 4 );

	// now go through all values looking for dictionary matches.
	unsigned int tokenCount = 0;
	while ( count > 0 )
	{
		// find the match.
		size_t idx = FindMatch< _T >( dics, &data, count );

		// write the match to the stream.
		output.WriteValue( idx );

		// increment the token count.
		++tokenCount;
	}

	// now write out the token count.
	unsigned int curOffset = output.GetOffset();
	output.SetOffset( countOffset );
	output.WriteValue( tokenCount );
	output.SetOffset( curOffset );
}

//----------------------------------------------------------
template < typename _T >
void LZWDecode( Stream& output, Stream& input )
{
	unsigned int mask = DIC_HASH_MASK;

	// active dictionary.
	std::vector< SDicEntry< _T > > dics[ DIC_COUNT ];

	// read the initial dictionaries.
	for ( unsigned int i = 0; i < DIC_COUNT; ++i )
	{
		size_t initDicSize = input.ReadValue< size_t >();
		for ( size_t j = 0; j < initDicSize; ++j )
		{
			dics[ i ].push_back( SDicEntry< _T >( input.ReadValue< _T >() ) );
		}
	}

	// now go through all tokens and decompress.
	size_t prevDicEntry = ~0;
	size_t count = input.ReadValue< unsigned int >();
	while ( count-- > 0 )
	{
		// get the current token and make sure it's within the correct
		// range.
		size_t curTok = input.ReadValue< size_t >();
		size_t curDicIdx = ( curTok >> DIC_SHIFT ) & DIC_HASH_MASK;
		std::vector< SDicEntry< _T > >& dic = dics[ curDicIdx ];
		std::vector< SDicEntry< _T > >* prevDic = 0;
		size_t prevDicTok = prevDicEntry & DIC_MASK;
		if ( prevDicEntry != ~0 )
			prevDic = dics + ( ( prevDicEntry >> DIC_SHIFT ) & DIC_HASH_MASK );

		// make sure the current token is valid for the current dictionary.
		assert( ( curTok & DIC_MASK ) <= dic.size() );

		// check for the special case where a value was added to the dictionary
		// and then immediately matched during compression.
		if ( ( curTok & DIC_MASK ) == dic.size() )
		{
			// make sure there was a previous dictionary entry.  Otherwise, how
			// could this happen?
			assert( prevDicEntry != ~0 && dic[ prevDicEntry & DIC_MASK ]._itemCount < MAX_ENTRY_SIZE );

			// take the last dictionary entry and duplicate it's starting character.
			SDicEntry< _T > newEntry = dic[ prevDicEntry & DIC_MASK ];
			newEntry._items[ newEntry._itemCount ] = newEntry._items[ 0 ];
			++newEntry._itemCount;
			dic.push_back( newEntry );

			// clear the prevDicEntry pointer because we just created a new entry.
			prevDicEntry = ~0;
		}

		// write the decompressed values.
		SDicEntry< _T >& curDicEntry = dic[ curTok & DIC_MASK ];
		for ( unsigned int i = 0; i < curDicEntry._itemCount; ++i )
			output.WriteValue( curDicEntry._items[ i ] );

		// add a new dictionary entry if necessary.
		if ( prevDicEntry != ~0 && prevDic->at( prevDicTok )._itemCount < MAX_ENTRY_SIZE )
		{
			SDicEntry< _T > newEntry = prevDic->at( prevDicTok );
			newEntry._items[ newEntry._itemCount ] = curDicEntry._items[ 0 ];
			++newEntry._itemCount;
			prevDic->push_back( newEntry );
		}

		// store the prevDicEntry pointer.
		prevDicEntry = curTok;
	}
}


//==========================================================
// global functions.

//----------------------------------------------------------
void LZWEncode8( Stream& output, const unsigned char* data, unsigned int count )
{
	LZWEncode< unsigned char >( output, data, count );
}

//----------------------------------------------------------
void LZWDecode8( Stream& decompressed, Stream& compressed )
{
	LZWDecode< unsigned char >( decompressed, compressed );
}

//----------------------------------------------------------
void LZWEncode32( Stream& output, const unsigned int* data, unsigned int count )
{
	LZWEncode< unsigned int >( output, data, count );
}

//----------------------------------------------------------
void LZWDecode32( Stream& decompressed, Stream& compressed )
{
	LZWDecode< unsigned int >( decompressed, compressed );
}
