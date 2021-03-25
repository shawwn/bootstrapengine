//----------------------------------------------------------
// File:		UValue.h
// Author:		Kevin Bray
// Created:		12-03-06
//
// Purpose:		To provide a universal type.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UValue
//**********************************************************
class UValue
{
public:
	UValue( const tstring& value );
	UValue( const char* value );
	UValue( int value );
	UValue( float value );
	UValue( bool value );
	UValue();
	~UValue();

	operator const char* () const		{	return _value.c_str();						}
	operator tstring () const			{	return _value;								}
	operator int () const				{	return atoi( _value.c_str() );				}
	operator bool () const;
	operator float () const				{	return ( float )atof( _value.c_str() );		}

	UValue& operator = ( const UValue& value );
	UValue& operator = ( const tstring& value );
	UValue& operator = ( const char* value );
	UValue& operator = ( int value );
	UValue& operator = ( float value );
	UValue& operator = ( bool value );

private:
	tstring			_value;
};
