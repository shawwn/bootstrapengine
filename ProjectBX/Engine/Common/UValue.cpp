//----------------------------------------------------------
// File:		UValue.cpp
// Author:		Kevin Bray
// Created:		12-03-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UValue.h"

//**********************************************************
// class UValue
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UValue::UValue()
: _value( "" )
{

}

//----------------------------------------------------------
UValue::UValue( const tstring& value )
: _value( value )
{

}

//----------------------------------------------------------
UValue::UValue( const char* value )
: _value( value )
{

}

//----------------------------------------------------------
UValue::UValue( int value )
{
	_value << value;
}

//----------------------------------------------------------
UValue::UValue( float value )
{
	_value << value;
}

//----------------------------------------------------------
UValue::UValue( bool value )
{
	_value = value ? "true" : "false";
}

//----------------------------------------------------------
UValue::~UValue()
{

}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
UValue::operator bool () const
{
	return ( _value != "0" ) && ( _stricmp( _value.c_str(), "false" ) != 0 ) && _value.length() > 0;
}

//----------------------------------------------------------
UValue&
UValue::operator = ( const UValue& value )
{
	_value = value._value;
	return *this;
}

//----------------------------------------------------------
UValue&
UValue::operator = ( const tstring& value )
{
	_value = value;
	return *this;
}

//----------------------------------------------------------
UValue&
UValue::operator = ( const char* value )
{
	_value = value;
	return *this;
}

//----------------------------------------------------------
UValue&
UValue::operator = ( int value )
{
	_value.clear();
	_value << value;
	return *this;
}

//----------------------------------------------------------
UValue&
UValue::operator = ( float value )
{
	_value.clear();
	_value << value;
	return *this;
}

//----------------------------------------------------------
UValue&
UValue::operator = ( bool value )
{
	_value = value ? "true" : "false";
	return *this;
}

