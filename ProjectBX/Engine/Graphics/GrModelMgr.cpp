//----------------------------------------------------------
// File:        GrModelMgr.h
// Author:      Kevin Bray
// Created:     01-31-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrModelMgr.h"

// project includes.
#include "GrModel.h"
#include "RFileMgr.h"
#include "RFile.h"

// 10 second unused time.
#define UNUSED_TIME_AMOUNT          10000
#define GARBAGE_COLLECT_GRANULARITY 10000

GrModelMgr* gGrModelMgr = 0;

//**********************************************************
// class GrModelMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrModelMgr::GrModelMgr()
: _curTime( 0 )
, _lastCleanTime( 0 )
{
    gGrModelMgr = this;
}

//----------------------------------------------------------
GrModelMgr::~GrModelMgr()
{
    _models.clear();
    gGrModelMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrModelMgr::Update( unsigned int timeDelta )
{
    // collect garbage about every 10 seconds.
    _curTime += timeDelta;
    unsigned int delta = _curTime - _lastCleanTime;
    if ( delta > GARBAGE_COLLECT_GRANULARITY )
    {
        CollectGarbage();
        _lastCleanTime = _curTime;
    }
}

//----------------------------------------------------------
URef< GrModel >
GrModelMgr::GetModel( const UPath& filePath, const UPath& instanceName, bool clone, bool cloneLights )
{
    // now try to load the model.
    GrModel* model = 0;
    ModelMap::iterator iter = _models.find( filePath );
    if ( iter == _models.end() )
    {
        // load the model from the disk.
        URef< RFile > file = gRFileMgr->GetFile( filePath, RFileMgr::kFlagRead );
        if ( !file )
        {
            // if we couldn't read the file, return NULL.
            PrintF( "Error: model %s is missing.\n", filePath.GetPathString().c_str() );
            return 0;
        }

        unsigned int modelSize = ( unsigned int )file->GetSize();
        const void* modelBuffer = file->GetData();

        // now try to read in the model file.  Note that we let the
        // reader delete modelBuffer for us.
        UReader modelReader( modelBuffer, modelSize, false );

        // create a new model.
        SModelEntry& entry = _models[ filePath ];
        model = new GrModel( filePath, instanceName, modelReader );
        entry.model = model;
        entry.lastUsed = _curTime;

        // purge the file data.
        file->Purge();
    }
    else
    {
        // get the model and update it's time stamp.
        model = iter->second.model;
        iter->second.lastUsed = _curTime;
    }

    // return a clone of the model or the original model.
    if ( clone )
        return model->Clone( instanceName.GetPathString(), false, cloneLights );
    return model;
}

//----------------------------------------------------------
void
GrModelMgr::AddModel( URef< GrModel > model )
{
    SModelEntry entry;
    entry.model = model;
    entry.lastUsed = _curTime;
    _models[ model->GetFileName() ] = entry;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrModelMgr::CollectGarbage()
{
    ModelMap::iterator iter = _models.begin();
    const ModelMap::iterator end = _models.end();
    while ( iter != end )
    {
        // are we holding the last reference to the object?
        SModelEntry& curEntry = iter->second;
        if ( curEntry.model->GetRef() == 1 )
        {
            // if we've gone too long without being used, clear the
            // current model.
            unsigned int delta = _curTime - curEntry.lastUsed;
            if ( delta > UNUSED_TIME_AMOUNT )
                _models.erase( iter++ );
            else
                ++iter;
        }
        else
            ++iter;
    }
}
