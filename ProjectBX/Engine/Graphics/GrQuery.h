//----------------------------------------------------------
// File:		GrQuery.h
// Author:		Kevin Bray
// Created:		09-11-06
//
// Purpose:		To manage a graphics card query.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "URef.h"

//**********************************************************
// class GrQuery
//**********************************************************
class GrQuery : public URefCount
{
public:
	enum EQUERYTYPE
	{
		EQT_OCCLUSION,
		EQT_COUNT
	};

	GrQuery( EQUERYTYPE queryType = EQT_OCCLUSION );
	~GrQuery();

	// returns the number of bits needed to contain a query result.
	unsigned int	GetQueryCounterBits() const;

	// returns true if the query is available or not.
	bool			IsResultAvailable() const;

	// returns the result of the query.  This call will wait until a result is
	// available.
	__int64			GetQueryResult() const;

	// tells OpenGL that commands between the begin and end should contribute
	// to this query.  Only one query may be active at any time.
	void			BeginQuery();
	void			EndQuery();

	// context reset notification.
	void			PreContextReset();
	void			PostContextReset();

private:
	static bool		_queryActive[ EQT_COUNT ];

	unsigned int	_handle;
	unsigned int	_target;
	EQUERYTYPE		_queryType;
	bool			_thisQueryActive;
};
