//----------------------------------------------------------
// File:        GrKFInterpolator.h
// Author:      Kevin Bray
// Created:     04-29-06
//
// Purpose:     To interpolate keyframe animation data and create an
//              animation transform for an animation channel.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UFastArray.h"

// forward declarations.
class GrKFAnimData;
class GrAnimXForm;
class GrAnimXFormCache;

//**********************************************************
// GrKFInterpolator
//**********************************************************
class GrKFInterpolator
{
public:
    // pass in the keyframe data to the interpolator.
    GrKFInterpolator();
    GrKFInterpolator( URef< GrKFAnimData > kfData );
    ~GrKFInterpolator();

    // initializes the animation... this is useful for arrays of interpolators.
    void                    Init( URef< GrKFAnimData > kfData );

    // returns a pointer to the keyframe data that we're using.
    URef< GrKFAnimData >    GetKFData() const;

    // sets the player the interpolator should write to.
    bool                    SetOutput( GrAnimXFormCache* output );

    // gets the pose for a given instant in time.
    void                    ApplyPose( float time, float blendWeight, float clampDuration, bool clamp );

private:
    template< typename _T >
    void                    FindKeyFrame( const _T*& curKF, const _T*& nextKF, unsigned int& curKFIdx, const UFastArray< _T >& kfArray, unsigned int fixedTime );

    float                   CalcT( unsigned int fixedTime, unsigned int curKFTime, unsigned int nextKFTime );

    static const unsigned int   kHasRotKFs = 1;
    static const unsigned int   kHasScaleKFs = 2;
    static const unsigned int   kHasTranslateKFs = 4;

    URef< GrKFAnimData >    _kfData;
    unsigned int            _curRotKFIdx;
    unsigned int            _curScaleKFIdx;
    unsigned int            _curTranslateKFIdx;
    unsigned int            _dataFlags;
    GrAnimXFormCache*       _output;
    unsigned int            _outputXFormIdx;
    float                   _duration;
};

//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
template< typename _T >
void
GrKFInterpolator::FindKeyFrame( const _T*& curKF, const _T*& nextKF, unsigned int& curKFIdx, const UFastArray< _T >& kfArray, unsigned int fixedTime )
{
    // get the number of keyframes, excluding the terminating keyframe.
    unsigned int kfCount = kfArray.GetElemCount() - 1;
    B_ASSERT( curKFIdx < kfCount );

    curKF = &kfArray[ curKFIdx ];

    // find the appropriate keyframe pair.
    if ( fixedTime < curKF->time )
    {
        curKFIdx = 0;
        curKF = &kfArray[ curKFIdx ];
        nextKF = &kfArray[ curKFIdx + 1 ];
    }
    else
        nextKF = &kfArray[ curKFIdx + 1 ];

    // scan ahead to find the appropriate keyframe.
    while ( fixedTime > nextKF->time )
    {
        // next keyframe.
        ++curKFIdx;
        curKFIdx %= kfCount;

        // get the next keyframe pair.
        curKF = nextKF;
        nextKF = &kfArray[ curKFIdx + 1 ];
    }
}
