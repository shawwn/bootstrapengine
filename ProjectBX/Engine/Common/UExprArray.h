//----------------------------------------------------------
// File:		UExprArray.h
// Author:		Kevin Bray
// Created:		07-05-05
//
// Purpose:		To contain an array that is dereferenced by UExprArrayTerm.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URefCount.h"

// std c++ includes.
#include <vector>

// forward declarations.
class UExprTerm;

//**********************************************************
// class UExprArray
//**********************************************************
class UExprArray : public URefCount
{
	// friend.
	friend class UExprMgr;

	// ctor.
	UExprArray( const tstring& name, float indexScale, bool smooth, bool clamp );
public:
	~UExprArray();

	const tstring&	GetName() const							{	return _name;			}

	void			SetAddressScale( float scale )			{	_indexScale = scale;	}
	void			AddElement( URef< UExprTerm > term );

	float			GetValue( float index ) const;
	bool			IsDynamic( float index ) const;

private:
	typedef std::vector< URef< UExprTerm > > TermVector;

	tstring			_name;
	TermVector		_terms;
	float			_indexScale;
	bool			_smooth;
	bool			_clamp;
};

