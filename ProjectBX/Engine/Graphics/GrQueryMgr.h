//----------------------------------------------------------
// File:		GrQueryMgr.h
// Author:		Kevin Bray
// Created:		09-10-06
//
// Purpose:		To manage graphics card queries.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrQuery.h"
#include "UFastArray.h"

//**********************************************************
// class GrQueryMgr
//**********************************************************
class GrQueryMgr
{
public:
	GrQueryMgr();
	~GrQueryMgr();

	// gets or creates an unused query.
	URef< GrQuery >		GetQuery( GrQuery::EQUERYTYPE type );

	// context reset notification.
	void				PreContextReset();
	void				PostContextReset();

private:
	typedef UFastArray< URef< GrQuery > > QueryArray;

	// array of queries.
	QueryArray			_queries[ GrQuery::EQT_COUNT ];
};
