//----------------------------------------------------------
// File:        GrQueryMgr.cpp
// Author:      Kevin Bray
// Created:     09-10-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrQueryMgr.h"


//**********************************************************
// class GrQueryMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrQueryMgr::GrQueryMgr()
{

}

//----------------------------------------------------------
GrQueryMgr::~GrQueryMgr()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
URef< GrQuery >
GrQueryMgr::GetQuery( GrQuery::EQUERYTYPE type )
{
    // get a reference to the array we're pulling from.
    QueryArray& queryArray = _queries[ type ];

    // check for a free query.
    unsigned int queryCount = queryArray.GetElemCount();
    for ( unsigned int i = 0; i < queryCount; ++i )
    {
        // if the current query is unused, then return it.
        if ( queryArray[ i ]->GetRef() == 1 )
            return queryArray[ i ];
    }

    // if we got here, then there are no free queries available.  Create
    // a new one, add it to the list of queries.
    GrQuery* newQuery = new GrQuery( type );
    queryArray.Push( newQuery );

    // return the new query.
    return newQuery;
}

//----------------------------------------------------------
void
GrQueryMgr::PreContextReset()
{
    // notify all queries.
    for ( unsigned int i = 0; i < GrQuery::EQT_COUNT; ++i )
    {
        QueryArray& queryArray = _queries[ i ];
        unsigned int queryCount = queryArray.GetElemCount();
        for ( unsigned int j = 0; j < queryCount; ++j )
            queryArray[ j ]->PreContextReset();
    }
}

//----------------------------------------------------------
void
GrQueryMgr::PostContextReset()
{
    // notify all queries.
    for ( unsigned int i = 0; i < GrQuery::EQT_COUNT; ++i )
    {
        QueryArray& queryArray = _queries[ i ];
        unsigned int queryCount = queryArray.GetElemCount();
        for ( unsigned int j = 0; j < queryCount; ++j )
            queryArray[ j ]->PostContextReset();
    }
}
