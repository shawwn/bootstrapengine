// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.

// file header.
#include "lz77.h"

// project includes.
#include "stream.h"

// stl includes.
#include <algorithm>

//----------------------------------------------------------
template < typename _T >
size_t MatchSequence( const _T** sequence, const _T* buf, const _T* src, const _T* end, unsigned int maxMatchCount )
{
	static unsigned int maxMatch = 0;

	// initialize the incoming vars.
	size_t sequenceLength = 0;
	*sequence = 0;

	// try to match the sequence at the end of the usable span.
	while ( buf != src )
	{
		// scan ahead looking for the best match.
		const _T* curBuf = buf;
		const _T* curSrc = src;
		while ( curSrc != end && *curBuf == *curSrc && ( unsigned int )( curSrc - src ) < maxMatchCount )
		{
			++curBuf;
			++curSrc;
		}

		// calculate the match length.
		size_t curMatchLength = curBuf - buf;
		if ( curMatchLength > sequenceLength )
		{
			*sequence = buf;
			sequenceLength = curMatchLength;
			maxMatch = std::max( maxMatch, sequenceLength );
		}

		// next.
		++buf;
	}

	// return the length of the sequence.
	return sequenceLength;
}

//----------------------------------------------------------
template < typename _T >
void LZEncode( Stream& output, const _T* data, unsigned int count, unsigned int bufSize )
{
	// setup our encoding pointers.
	const _T* buf = data;
	const _T* cur = data;
	const _T* end = data + count;

	// reserve memory to increase efficiency.
	output.Reserve( count * sizeof( _T ) );

	// calculate valid values for our tokens.
	unsigned int maxValue = ( ( unsigned int )~0 ) >> 1;	// all but the high bit.
	unsigned int flagMask = ~maxValue;						// just the high bit.
	assert( maxValue > bufSize );

	// write the total size of the source data.
	output.WriteValue( count );

	// keep a location for the output stream size.
	unsigned int sizeOffs = output.GetOffset();
	output.WriteValue< unsigned int >( 0 );

	// now advance the length of the buffer.
	unsigned int spanOffs = output.GetOffset();
	output.WriteValue< unsigned int >( 0 );
	const _T* blockStart = data;
	while ( count > 0 )
	{
		// scan from buf to cur looking for the current substring.
		const _T* sequence = 0;
		size_t matchLen = MatchSequence( &sequence, buf, cur, end, maxValue );

		// did we find a match big enough to merit the match tokens?
		if ( matchLen * sizeof( _T ) > 64 )
		{
			// calculate the length of uncompressed data.
			__int64 uncompLen = ( cur - blockStart );

			// if there is no uncompressed data, then there is no sense leaving
			// an "uncompressed sequence token" in the data stream.
			if ( uncompLen > 0 )
				output.WriteValue( spanOffs, ( unsigned int )uncompLen );
			else
				output.SetOffset( output.GetOffset() - sizeof( unsigned int ) );

			// write the compression tokens.
			output.WriteValue( flagMask | ( unsigned int )( cur - sequence ) );
			output.WriteValue( ( unsigned int )matchLen );
			cur += matchLen;
			count -= ( unsigned int )matchLen;
			if ( cur > ( buf + bufSize ) )
				buf = cur - bufSize;

			// start a new uncompressed block.
			spanOffs = output.GetOffset();
			output.WriteValue< unsigned int >( 0 );
			blockStart = cur;

			// continue on and avoid advancing 'cur' and 'buf'.
			continue;
		}
		else
		{
			// we're writing uncompressed data.
			output.WriteValue( *cur );
		}

		// next.
		--count;
		++cur;
		if ( cur > ( buf + bufSize ) )
			++buf;
	}

	// end the current block.
	output.WriteValue( spanOffs, ( unsigned int )( cur - blockStart ) );

	// write out the size.
	output.WriteValue( sizeOffs, output.GetOffset() );
}

//----------------------------------------------------------
template < typename _T >
void LZDecode( Stream& output, Stream& input )
{
	// calculate valid values for our tokens.
	unsigned int maxValue = ( ( unsigned int )~0 ) >> 1;	// all but the high bit.
	unsigned int flagMask = ~maxValue;						// just the high bit.

	// get the total uncompressed item count.
	unsigned int count = input.ReadValue< unsigned int >();
	output.Reserve( count * sizeof( _T ) );

	// get the total compressed byte count.
	unsigned int size = input.ReadValue< unsigned int >();

	// read everything in the stream.
	while ( input.GetOffset() < size )
	{
		// get the first token in the output.
		unsigned int token = input.ReadValue< unsigned int >();

		// read a compressed block.
		if ( token & flagMask )
		{
			// get the negative offset to the current address along with the
			// length of the duplicate sequence.
			unsigned int negOffset = token & maxValue;
			unsigned int dupLength = input.ReadValue< unsigned int >();

			// get the pointer to the start of the sequence.
			const _T* seqEnd = ( const _T* )( ( unsigned char* )output.GetBufferPtr() + output.GetOffset() );
			const _T* seqSrc = seqEnd - negOffset;

			// now copy the sequence.
			const _T* curSeq = seqSrc;
			while ( dupLength-- > 0 )
			{
				// write the current value.
				output.WriteValue( *curSeq );

				// increment the current sequence pointer and wrap if
				// necessary.
				++curSeq;
				if ( curSeq == seqEnd )
					curSeq = seqSrc;
			}
		}
		else
		{
			// read in uncompressed values.
			unsigned int uncompCount = token & maxValue;
			while ( uncompCount-- > 0 )
				output.WriteValue( input.ReadValue< _T >() );
		}
	}
}

//----------------------------------------------------------
void LZEncode8( Stream& output, const unsigned char* data, unsigned int count, unsigned int bufSize )
{
	LZEncode( output, data, count, bufSize );
}

//----------------------------------------------------------
void LZDecode8( Stream& output, Stream& input )
{
	LZDecode< unsigned char >( output, input );
}

//----------------------------------------------------------
void LZEncode32( Stream& output, const unsigned int* data, unsigned int count, unsigned int bufSize )
{
	LZEncode( output, data, count, bufSize );
}

//----------------------------------------------------------
void LZDecode32( Stream& output, Stream& input )
{
	LZDecode< unsigned int >( output, input );
}
