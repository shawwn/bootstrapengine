//----------------------------------------------------------
// File:		UXMLTree.cpp
// Author:		Kevin Bray
// Created:		12-18-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UXMLTree.h"

// project includes.
using namespace irr::io;

//**********************************************************
// class UXMLTree
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UXMLTree::UXMLTree( const char* fileName )
{
	IrrXMLReader* reader = createIrrXMLReader( fileName );
	Construct( reader, fileName );
}

//----------------------------------------------------------
UXMLTree::UXMLTree( IrrXMLReader* reader, const tstring& fileName )
{
	Construct( reader, fileName );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
UXMLTree::GetAttribValue( tstring& text, const char* name )
{
	AttribMap::iterator iter = _attribs.find( name );
	if ( iter == _attribs.end() )
		return false;
	text = iter->second;
	return true;
}

//----------------------------------------------------------
bool
UXMLTree::GetAttribValue( int& value, const char* name )
{
	tstring valueStr;
	if ( !GetAttribValue( valueStr, name ) )
		return false;
	value = atoi( valueStr.c_str() );
	return true;
}

//----------------------------------------------------------
bool
UXMLTree::GetAttribValue( unsigned int& value, const char* name )
{
	tstring valueStr;
	if ( !GetAttribValue( valueStr, name ) )
		return false;
	value = ( unsigned int )atoi( valueStr.c_str() );
	return true;
}

//----------------------------------------------------------
unsigned int
UXMLTree::GetChildCount() const
{
	return ( unsigned int )_children.size();
}

//----------------------------------------------------------
URef< UXMLTree >
UXMLTree::GetChild( unsigned int idx )
{
	B_ASSERT( idx < _children.size() );
	if ( idx >= _children.size() )
		return 0;
	return _children[ idx ];
}

//----------------------------------------------------------
URef< UXMLTree >
UXMLTree::GetChild( const char* name, unsigned int occ )
{
	// find the child with the name passed in.
	const unsigned int childCount = GetChildCount();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		UXMLTree* child = GetChild( i );
		B_ASSERT( child != 0 );

		// if we found it, return a smart pointer to it.
		if ( child->GetName() == name && occ-- == 0 )
			return child;
	}

	// not found.
	return 0;
}

//----------------------------------------------------------
URef< UXMLTree >
UXMLTree::FindChild( const char* name, unsigned int occ )
{
	return FindChild( occ, name );
}

/*
//----------------------------------------------------------
void
UXMLTree::DebugPrint()
{
	static unsigned int depth = 0;
	++depth;

	// allocate formatting whitespace.
	char* ws = ( char* )alloca( depth + 1 );
	MemSet( ws, ' ', depth );
	ws[ depth ] = '\0';

	// name.
	PrintF( "%s%s\n%s{", ws, _name.c_str(), ws );

	// attributes.
	PrintF( "%s[ATTRIBS]\n", ws );
	PrintF( "%s{\n", ws );
	AttribMap::iterator iter = _attribs.begin();
	for ( unsigned int i = 0; i < _attribs.size(); ++i, ++iter )
		PrintF( "%s%s=%s\n", ws, iter->first.c_str(), iter->second.c_str() );
	PrintF( "%s}\n", ws );

	// text.
	PrintF( "%s\n", _text.c_str() );

	// recurse.
	for ( unsigned int i = 0; i < _children.size(); ++i )
		_children[ i ]->DebugPrint();

	PrintF( "%s}\n", ws );

	--depth;
}
*/

//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UXMLTree::Construct( IrrXMLReader* reader, const tstring& fileName )
{
	// malformed data!  Simply return.
	while( reader->getNodeType() != EXN_ELEMENT )
	{
		if ( !reader->read() )
		{
			PrintF( "Warning: XML file '%s' is malformed...\n", _fileName.c_str() );
			return;
		}
	}

	// store the node name.
	const char* nodeName = reader->getNodeName();
	_name = ( nodeName != 0 ) ? nodeName : "";

	// get attributes.
	for ( int i = 0; i < reader->getAttributeCount(); ++i )
		_attribs[ reader->getAttributeName( i ) ] = reader->getAttributeValue( i );

	// if we're empty then we have no text or children.
	if ( reader->isEmptyElement() )
		return;

	// now scan through children.  On the first iteration, we're advancing past
	// the element that defines this node.  On subsequent iterations, we're
	// advancing past comments or the end elements of children.
	while ( reader->read() )
	{
		// do we need to create a new child?
		if ( reader->getNodeType() == EXN_ELEMENT )
			_children.push_back( new UXMLTree( reader, fileName ) );
		else if ( reader->getNodeType() == EXN_TEXT )
		{
			const char* text = reader->getNodeData();
			_text += ( text != 0 ) ? text : "";
		}
		else if ( reader->getNodeType() == EXN_ELEMENT_END )
		{
			// we've hit our end node!
			break;
		}
		else
		{
			// either a comment or unknown type - either way, ignore.
		}
	}
}

//----------------------------------------------------------
URef< UXMLTree >
UXMLTree::FindChild( unsigned int& occ, const char* name )
{
	if ( GetName() == name && occ-- == 0 )
		return this;

	// recurse children.
	const unsigned int childCount = GetChildCount();
	for ( unsigned int i = 0; i < childCount; ++i )
	{
		UXMLTree* child = GetChild( i );
		B_ASSERT( child != 0 );

		// recurse children.
		UXMLTree* found = child->FindChild( occ, name );
		if ( found )
			return found;
	}

	// not found!
	return 0;
}

