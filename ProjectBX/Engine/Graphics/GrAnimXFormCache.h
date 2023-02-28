//----------------------------------------------------------
// File:        GrAnimXFormCache.h
// Author:      Kevin Bray
// Created:     04-08-06
//
// Purpose:     Contains a list of transforms and their targets.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrAnimXForm.h"

// forward declarations.
class GrModel;
class GrModelNode;

//**********************************************************
// class GrAnimXFormCache
//**********************************************************
class GrAnimXFormCache
{
public:
    GrAnimXFormCache( GrModel* modelTarget );
    ~GrAnimXFormCache();

    // changes the target.
    void            SetTarget( GrModel* modelTarget );

    // transform management.
    unsigned int    GetXFormIdx( const tstring& name );
    void            AccumXForm( unsigned int idx, const GrAnimXForm& xform );

    // applies the transforms in the cache to the target.
    void            ApplyToTarget();

private:
    void            BuildArrays();
    unsigned int    CountNodes( GrModelNode* tree );
    void            RecordTargets( GrModelNode* tree );
    void            Clean();

    GrModel*        _modelTarget;
    GrModelNode**   _nodeTargets;
    GrAnimXForm*    _xformCache;
    MMat4x4*        _bindPose;
    unsigned int    _xformCount;
};
