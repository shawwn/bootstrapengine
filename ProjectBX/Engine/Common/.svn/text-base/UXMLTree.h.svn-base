//----------------------------------------------------------
// File:		UXMLTree.h
// Author:		Kevin Bray
// Created:		12-18-05
//
// Purpose:		To build an XML hierarchy in memory.  This isn't as efficient
//				as simply walking the file in a linear fashion, however, this
//				is much more intuitive.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"
#include "URef.h"
#include "IrrXML/IrrXML.h"

// std c++ includes.
#include <vector>
#include <map>

//**********************************************************
// class UXMLTree
//**********************************************************
class UXMLTree : public URefCount
{
public:
	UXMLTree( const char* fileName );
	UXMLTree( irr::io::IrrXMLReader* reader, const tstring& fileName );
	~UXMLTree() { }

	const tstring&		GetName() const				{	return _name;		}
	const tstring&		GetText() const				{	return _text;		}
	bool				GetAttribValue( tstring& text, const char* name );
	bool				GetAttribValue( int& value, const char* name );
	bool				GetAttribValue( unsigned int& value, const char* name );
	unsigned int		GetChildCount() const;
	URef< UXMLTree >	GetChild( unsigned int idx );
	URef< UXMLTree >	GetChild( const char* name, unsigned int occ = 0 );

	// finds an occurence of a node in the subhierarchy.
	URef< UXMLTree >	FindChild( const char* name, unsigned int occ = 0 );

	// test function that prints out the tree.
//	void				DebugPrint();

private:
	typedef std::vector< URef< UXMLTree > > ChildVec;
	typedef std::map< tstring, tstring > AttribMap;

	URef< UXMLTree >	FindChild( unsigned int& occ, const char* name );
	void				Construct( irr::io::IrrXMLReader* reader, const tstring& fileName );

	tstring				_fileName;
	ChildVec			_children;
	AttribMap			_attribs;
	tstring				_text;
	tstring				_name;
};
