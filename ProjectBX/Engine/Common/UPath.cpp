//----------------------------------------------------------
// File:		UPath.cpp
// Author:		Kevin Bray
// Created:		05-30-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UPath.h"
#include <algorithm>

//**********************************************************
// class UPath
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UPath::UPath( const tstring& str )
{
	CreatePath( str );
}

//----------------------------------------------------------
UPath::UPath( const char* path )
{
	CreatePath( tstring(path) );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
UPath
UPath::operator + ( const UPath& path ) const
{
	// do nothing if the path passed in doesn't contain anything.
	if ( path._path.length() == 0 )
		return _path;

	// simply append the path.
	return _path + "/" + path._path;
}

//----------------------------------------------------------
UPath&
UPath::operator += ( const UPath& path )
{
	// do nothing if the path passed in doesn't contain anything.
	if ( path._path.length() == 0 )
		return *this;

	// simply append the path.
	_path += "/";
	_path += path._path;
	return *this;
}

//----------------------------------------------------------
bool
UPath::GetDriveRoot( tstring& root ) const
{
	// get the location of the drive specifier.
	tstring::size_type loc = _path.find_first_of( ':' );
	if ( loc == -1 )
		return false;

	// get the root path and append a backslash.
	root = _path.substr( 0, loc + 1 );
	root = "\\";

	// return true to indicate that a root path was specified.
	return true;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
UPath::CreatePath( const tstring& str )
{
	bool first = true; 
	tstring::const_iterator start = str.begin();

	//loop through the '\' or '/' separated components of the path
	while ( start != str.end() ) 
	{
		const char *separators = "/\\";
		tstring::const_iterator end = std::find_first_of( start, str.end(), separators, separators + strlen( separators ) );
		tstring temp( start, end );

		// we don't allow [..] as a directory component.  Simply skip it.
		if ( temp == ".." )
		{
			if ( end == str.end() )
				break;
			else
				start = ++end;
			continue;
		}

		// and we skip any components that are empty or only a '.'
		if ( ( temp != "" ) && ( temp != "." ) )
		{
			// separate the components with '/', but no leading '/'
			if ( !first )
				_path += '/';
			first = false;
			// convert to lower case
			for ( tstring::iterator it = temp.begin(); it != temp.end(); ++it )
			{
				*it = tolower( *it );
			} 
			_path += temp;
		}
		if ( end == str.end() ) 
			break;
		else
			start = ++end;
	}
}
