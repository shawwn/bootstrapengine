//----------------------------------------------------------
// File:        GrLightMgr.h
// Author:      Kevin Bray
// Created:     03-12-06
//
// Purpose:     To cache and manage loaded lights.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "UPath.h"

// std c++ includes.
#include <vector>
#include <map>

class GrLight;
struct SLightInfo;

//**********************************************************
// class GrLightMgr
//**********************************************************
class GrLightMgr
{
public:
    GrLightMgr();
    ~GrLightMgr();

    // set light is usually called when a script is loaded.
    void                SetLight( const UPath& id, const SLightInfo& lightInfo );
    URef< GrLight >     GetLight( const UPath& id, tstring& error );
    URef< GrLight >     FindLight( const UPath& id );

    URef< GrLight >     CreateLight( const UPath& id, const SLightInfo& lightInfo );

    // clones a light.
    URef< GrLight >     GetOrCloneLight( const UPath& newLightId, const UPath& fromLight, tstring& error );
    URef< GrLight >     GetOrCloneLight( const UPath& newLightId, const GrLight* fromLight );

    // this can be used to access all loaded lights.
    unsigned int        GetLightCount();
    URef< GrLight >     GetLightByIdx( unsigned int idx );

    // reloads the script a given material resides in.
    bool                ReloadLight( const tstring& name, tstring& error );
    void                ReloadAll();

    // updates all lights.
    void                Update();

    // clears and deletes unreferenced lights currently loaded.
    void                CollectGarbage();

    // returns the name of the script a material should reside in.
    void                GetScriptName( tstring& scriptName, const tstring& lightName );

private:
    typedef std::vector< URef< GrLight > >  LightVec;
    typedef std::map< UPath, GrLight* >     LightMap;

    void                AddLight( GrLight* light );

    LightVec            _lightVec;
    LightMap            _lightMap;
};
extern GrLightMgr* gGrLightMgr;
