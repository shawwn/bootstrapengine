// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.

// file header.
#include "huffman.h"

// project includes.
#include "stream.h"
#include "bitarray.h"

// std c includes.
#include <assert.h>

// stl includes.
#include <vector>
#include <list>
#include <map>
#include <algorithm>

#define MAX_TREE_DEPTH		1024


//==========================================================
// huffman tree node base class.
class Node
{
public:
	// ctor
	Node( unsigned int weight ) 
		: _weight( weight )
	{ }

	// dtor
	virtual ~Node() { }

	static bool Compare( Node* lhs, Node* rhs )		{	return lhs->_weight < rhs->_weight;	}

	// returns the weight of the node.
	unsigned int	GetWeight() const				{	return _weight;						}

private:
	unsigned int	_weight;
};

//==========================================================
// huffman tree internal node.
class Parent : public Node
{
public:
	// ctor
	Parent( Node* left, Node* right )
		: Node( left->GetWeight() + right->GetWeight() )
		, _left( left )
		, _right( right )
	{ }

	// dtor
	~Parent()
	{
		delete _left;
		delete _right;
	}

	Node*		GetLeft()		{	return _left;	}
	Node*		GetRight()		{	return _right;	}

private:
	Node*		_left;
	Node*		_right;
};

//==========================================================
// huffman tree leaf.
template < typename _T >
class Leaf : public Node
{
public:
	// ctor
	Leaf( unsigned int weight, _T value )
		: Node( weight )
		, _value( value )
	{ }

	// dtor
	~Leaf() { }

	// returns the value of the leaf.
	const _T&	GetValue() const		{	return _value;		}

private:
	_T			_value;
};

//==========================================================
// mapping of a value to a sequence of bits.
struct Mapping
{
	unsigned int	_bitCount;
	unsigned char	_bits[ MAX_TREE_DEPTH ];
};

//==========================================================
// file local functions.

//----------------------------------------------------------
template < typename _T >
void BuildNodes( std::vector< Node* >& nodes, const _T* data, unsigned int count )
{
	// a map of all unique values and their counts.
	std::map< unsigned int, unsigned int > valueCounts;
	std::map< unsigned int, unsigned int >::iterator iter;

	// count all elements.
	while ( count-- > 0 )
	{
		// initialize or advance the current value's count.
		iter = valueCounts.find( *data );
		if ( iter != valueCounts.end() )
			iter->second += 1;
		else
			valueCounts[ *data ] = 1;

		// next value.
		++data;
	}

	// add leaves to our vector.
	nodes.resize( valueCounts.size(), 0 );
	unsigned int i = 0;

	iter = valueCounts.begin();
	for ( ; iter != valueCounts.end(); ++iter, ++i )
		nodes[ i ] = new Leaf< _T >( iter->second, iter->first );

	// sort the elements by their occurence frequency.
	std::sort( nodes.begin(), nodes.end(), Node::Compare );
}

//----------------------------------------------------------
void GroupNodes( std::vector< Node* >& dst, std::vector< Node* >& src, unsigned int max )
{
	// if there is only one node, then the tree is built!
	assert( src.size() > 1 );

	// clear the destination, but don't free any memory.
	dst.resize( 0 );

	// group nodes.
	unsigned int remaining = ( unsigned int )src.size();
	unsigned int idx = 0;
	while ( remaining >= 2 )
	{
		// if we've exceeded the maximum for the current level in
		// the tree, then simply copy remaining elements.
		if ( src[ idx ]->GetWeight() > max || src[ idx+1 ]->GetWeight() > max )
		{
			// simply copy all remaining elements to the next array.
			while ( remaining > 0 )
			{
				dst.push_back( src[ idx ] );
				++idx;
				--remaining;
			}
			break;
		}

		dst.push_back( new Parent( src[ idx ], src[ idx+1 ] ) );
		remaining -= 2;
		idx += 2;
	}

	// copy the last node if necessary.
	if ( remaining > 0 )
		dst.push_back( src[ idx ] );

	// now clear the source since we're done reading everything
	// from it.
	src.resize( 0 );
}

//----------------------------------------------------------
template < typename _T >
void BuildMappings( std::map< _T, Mapping >& mappings, Node* root )
{
	static unsigned int depthCount = 0;
	static char path[ MAX_TREE_DEPTH ];

	// increment the recursion depth.
	++depthCount;
	unsigned int curDepth = depthCount-1;

	// make sure the tree isn't too big!
	assert( curDepth < MAX_TREE_DEPTH );

	// determine what kind of node we're dealing with.
	Leaf< _T >* leaf = dynamic_cast< Leaf< _T >* >( root );
	Parent* parent = dynamic_cast< Parent* >( root );
	if ( leaf )
	{
		// get the mapping for the current value.
		Mapping& mapping = mappings[ leaf->GetValue() ];

		// write out the bit sequence.
		mapping._bitCount = curDepth;
		memcpy( mapping._bits, path, curDepth );
	}
	else
	{
		// recurse left.
		path[ curDepth ] = 0;
		BuildMappings( mappings, parent->GetLeft() );

		// recurse right.
		path[ curDepth ] = 1;
		BuildMappings( mappings, parent->GetRight() );
	}

	// decrement the recursion depth.
	--depthCount;
}

//----------------------------------------------------------
template < typename _T >
void WriteCompressedBits( BitArray& bits, const _T* data, unsigned int count, std::map< _T, Mapping >& mappings )
{
	// now simply write out all of the compressed bits.
	while ( count-- > 0 )
	{
		// get the mapping for the current value.
		const Mapping& curMapping = mappings[ *data ];

		// write out the bits.
		for ( unsigned int i = 0; i < curMapping._bitCount; ++i )
			bits.PushBit( curMapping._bits[ i ] );

		// next value.
		++data;
	}
}

//----------------------------------------------------------
template < typename _T >
void SaveTree( Stream& output, Node* node )
{
	Leaf< _T >* leaf = dynamic_cast< Leaf< _T >* >( node );
	Parent* parent = dynamic_cast< Parent* >( node );
	if ( leaf )
	{
		output.WriteValue< char >( 1 );
		output.WriteValue< _T >( leaf->GetValue() );
	}
	else
	{
		output.WriteValue< char >( 0 );
		SaveTree< _T >( output, parent->GetLeft() );
		SaveTree< _T >( output, parent->GetRight() );
	}
}

//----------------------------------------------------------
template < typename _T >
Node* LoadTree( Stream& input )
{
	// should we load a leaf?
	char leaf = input.ReadValue< char >();
	if ( leaf != 0 )
	{
		_T value = input.ReadValue< _T >();
		return new Leaf< _T >( 0, value );
	}

	// left, right.
	Node* left = LoadTree< _T >( input );
	Node* right = LoadTree< _T >( input );

	// return a new parent.
	return new Parent( left, right );
}


//==========================================================
// global functions.

//----------------------------------------------------------
template < typename _T >
void Encode( Stream& output, const _T* data, unsigned int count )
{
	// make sure incoming parameters are valid.
	assert( data != 0 );
	assert( count > 0 );

	// allocate the first vector and feed in the initial nodes.
	std::vector< Node* >* src = new std::vector< Node* >();
	BuildNodes< _T >( *src, data, count );

	// now allocate the second vector with enough room to hold src->size()-1
	// items.
	std::vector< Node* >* dst = new std::vector< Node* >();
	dst->reserve( src->size()-1 );

	// now build the tree.
	unsigned int power = 1;
	while ( src->size() > 1 )
	{
		// group the nodes to build the tree.
		GroupNodes( *dst, *src, power );

		// swap the source and destination arrays.
		std::swap( src, dst );

		// double the power for the next level in the tree.
		power = ( power < 0x80000000 ) ? ( power << 1 ) : ~0;
	}

	// now the source array should hold the root of the tree.
	assert( src->size() == 1 );

	// build a mapping of value -> bit indices.
	std::map< _T, Mapping > mappings;
	BuildMappings( mappings, src->front() );

	// save the tree.
	SaveTree< _T >( output, src->front() );

	// save the components.
	BitArray bits;
	WriteCompressedBits( bits, data, count, mappings );
	bits.Save( output );

	// done.
	delete src;
	delete dst;
}

//----------------------------------------------------------
template < typename _T >
void Decode( Stream& data, Stream& input )
{
	// read in the tree.
	Node* root = LoadTree< _T >( input );

	// read in the bits.
	BitArray bits;
	bits.Load( input );

	// now decompress.
	unsigned int bitCount = bits.GetBitCount();
	unsigned int curBitIdx = 0;
	Node* curNode = root;
	while ( curBitIdx <= bitCount )
	{
		// determine what kind of node we're dealing with.
		Leaf< _T >* leaf = dynamic_cast< Leaf< _T >* >( curNode );
		Parent* parent = dynamic_cast< Parent* >( curNode );

		// did we hit a leaf?
		if ( leaf )
		{
			// get the leaf's value and reset the current node.
			data.WriteValue< _T >( leaf->GetValue() );
			curNode = root;
			if ( curBitIdx == bitCount )
				break;
			else
				continue;
		}

		// determine which direction we should go.
		int bit = bits.GetBit( curBitIdx );
		curNode = ( bit == 0 ) ? parent->GetLeft() : parent->GetRight();

		// next bit.
		++curBitIdx;
	}
}

//----------------------------------------------------------
void HuffmanEncode8( Stream& output, const unsigned char* src, unsigned int count )
{
	Encode( output, src, count );
}

//----------------------------------------------------------
void HuffmanDecode8( Stream& decompressed, Stream& compressed )
{
	Decode< unsigned char >( decompressed, compressed );
}

//----------------------------------------------------------
void HuffmanEncode16( Stream& output, const unsigned short* src, unsigned int count )
{
	Encode( output, src, count );
}

//----------------------------------------------------------
void HuffmanDecode16( Stream& decompressed, Stream& compressed )
{
	Decode< unsigned short >( decompressed, compressed );
}

//----------------------------------------------------------
void HuffmanEncode32( Stream& output, const unsigned int* src, unsigned int count )
{
	Encode( output, src, count );
}

//----------------------------------------------------------
void HuffmanDecode32( Stream& decompressed, Stream& compressed )
{
	Decode< unsigned int >( decompressed, compressed );
}
/*
//----------------------------------------------------------
unsigned int* Encode32( const unsigned int* src, unsigned int count )
{
	Encode( src, count );
}
*/
