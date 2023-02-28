//----------------------------------------------------------
// File:		UPath.h
// Author:		Kevin Bray
// Created:		05-30-05
//
// Purpose:		To provide a clean way to represent paths strings.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UPath
//**********************************************************
class UPath
{
public:
	UPath() { }
	UPath( const tstring& path );
	UPath( const char* path );
	~UPath() { }

	// returns the path string.
	const tstring&	GetPathString() const						{	return _path;						}

	// various string operations.
	bool			operator < ( const UPath& path ) const		{	return _path < path._path;			}
	bool			operator == ( const UPath& path ) const		{	return _path == path._path;			}
	bool			operator != ( const UPath& path ) const		{	return _path != path._path;			}
	UPath			operator + ( const UPath& path ) const;
	UPath&			operator += ( const UPath& path );

	// if this path contains a drive specifier, then the drive's root path
	// is returned in 'root' and this function returns true.  If a drive is
	// not specified in this path, this function returns false.
	bool			GetDriveRoot( tstring& root ) const;

private:
	//void			CreatePath( const char* path, unsigned int pathLen );
	void CreatePath( const tstring& str );
	
	tstring			_path;
};
