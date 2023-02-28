//----------------------------------------------------------
// File:        GrKFAnimData.h
// Author:      Kevin Bray
// Created:     04-29-06
//
// Purpose:     To contain keyframe animation data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MVec3.h"
#include "MQuat.h"
#include "UFastArray.h"
#include "URefCount.h"

// forward declarations.
#include "UWriter.h"
#include "UReader.h"


//**********************************************************
// class GrKFAnimData
//**********************************************************
class GrKFAnimData : public URefCount
{
public:
    struct SScaleKeyFrame
    {
        unsigned int time;      // 22.10 fixed point
        MVec3 scaleXYZ;
        MVec3 inTan;
        MVec3 outTan;
    };

    struct SRotKeyFrame
    {
        unsigned int time;      // 22.10 fixed point
        MQuat rotate;
    };

    struct STranslateKeyFrame
    {
        unsigned int time;      // 22.10 fixed point
        MVec3 translate;
        MVec3 inTan;
        MVec3 outTan;
    };

    typedef UFastArray< SRotKeyFrame > RotKeyArray;
    typedef UFastArray< SScaleKeyFrame > ScaleKeyArray;
    typedef UFastArray< STranslateKeyFrame > TranslateKeyArray;

    GrKFAnimData( const tstring& targetName, float duration, const UFastArray< SRotKeyFrame >& rotKeys,
        const UFastArray< SScaleKeyFrame >& scaleKeys, const UFastArray< STranslateKeyFrame >& translateKeys );
    GrKFAnimData( UReader& reader );
    ~GrKFAnimData();

    float                       GetDuration() const         {   return _duration;                           }

    const tstring&              GetTargetName() const       {   return _targetName;                         }

    unsigned int                GetRotKFCount()             {   return _rotateKFArray.GetElemCount();       }
    unsigned int                GetScaleKFCount()           {   return _scaleKFArray.GetElemCount();        }
    unsigned int                GetTranslateKFCount()       {   return _translateKFArray.GetElemCount();    }

    const SRotKeyFrame&         GetRotKF( unsigned int idx );
    const SScaleKeyFrame&       GetScaleKF( unsigned int idx );
    const STranslateKeyFrame&   GetTranslateKF( unsigned int idx );

    const RotKeyArray&          GetRotKFArray() const       {   return _rotateKFArray;                      }
    const ScaleKeyArray&        GetScaleKFArray() const     {   return _scaleKFArray;                       }
    const TranslateKeyArray&    GetTranslateKFArray() const {   return _translateKFArray;                   }

    void                        Save( UWriter& writer );
    void                        Load( UReader& reader );

private:
    //typedef UFastArray< SRotKeyFrame > RotKeyArray;
    //typedef UFastArray< SScaleKeyFrame > ScaleKeyArray;
    //typedef UFastArray< STranslateKeyFrame > TranslateKeyArray;

    void                        FixupData();
    void                        Clean();

    tstring                     _targetName;
    RotKeyArray                 _rotateKFArray;
    ScaleKeyArray               _scaleKFArray;
    TranslateKeyArray           _translateKFArray;
    float                       _duration;
};
