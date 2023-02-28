//----------------------------------------------------------
// File:        GrAnimXForm.h
// Author:      Kevin Bray
// Created:     04-08-06
//
// Purpose:     To represent a transform in an easily interpolatable
//              form.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MQuat.h"
#include "MVec3.h"

// forward declarations.
class MMat4x4;

//**********************************************************
// class GrAnimXForm
//**********************************************************
class GrAnimXForm
{
public:
    GrAnimXForm();      // initialize to identity.
    GrAnimXForm( const MQuat& rotate, const MVec3& translate, const MVec3& scale );
    GrAnimXForm( const MMat4x4& xform );
    ~GrAnimXForm() { }

    GrAnimXForm&    operator = ( const MMat4x4& xform );

    void            SetRotate( const MQuat& rotate );
    void            SetTranslate( const MVec3& translate );
    void            SetScale( const MVec3& scale );

    void            SetToIdentity();
    void            BlendToFast( const GrAnimXForm& xform, float t );
    void            Accumulate( const GrAnimXForm& xform );
    void            BuildMatrix( MMat4x4& mat );

private:
    MQuat           _rotate;
    MVec3           _translate;
    MVec3           _scale;
};
