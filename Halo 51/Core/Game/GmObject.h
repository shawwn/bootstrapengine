//----------------------------------------------------------
// File:        GmEntity.h
// Author:      Shawn Presser
// Created:     12-20-08
//
// Purpose:     To represent a game object, which has both 
//              a graphical and a physical representation.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header
#include "GmEntity.h"

// forward declarations.
class NxActor;

//**********************************************************
// class GmObject
//**********************************************************
class GmObject : public GmEntity
{
    friend class GmEntityMgr;

protected:
    GmObject( const tstring& name, URef< GrModel > model, NxActor* actor );
    ~GmObject();

public:
    // this is used to describe how a game object should be constructed.
    struct SInfo
    {
        SInfo();

        // the path to the object's graphics model.
        UPath modelFilePath;

        // transform.
        MMat4x4 transform;

        // physical properties.
        NxBodyDesc bodyDesc;

        // density.
        float density;
    };

    NxActor*            GetActor() const                {   return _actor;  }

    // updates the game object.
    virtual void        Update();

private:
    NxActor*            _actor;
};
