//----------------------------------------------------------
// File:        GrKFInterpolator.h
// Author:      Kevin Bray
// Created:     04-29-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrKFInterpolator.h"

// project includes.
#include "GrKFAnimData.h"
#include "GrAnimXForm.h"
#include "GrAnimXFormCache.h"


//**********************************************************
// class GrKFInterpolator
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrKFInterpolator::GrKFInterpolator()
: _kfData( 0 )
, _curRotKFIdx( 0 )
, _curScaleKFIdx( 0 )
, _curTranslateKFIdx( 0 )
, _dataFlags( 0 )
, _output( 0 )
, _outputXFormIdx( 0 )
, _duration( 0.0f )
{

}

//----------------------------------------------------------
GrKFInterpolator::GrKFInterpolator( URef< GrKFAnimData > kfData )
: _kfData( kfData )
, _curRotKFIdx( 0 )
, _curScaleKFIdx( 0 )
, _curTranslateKFIdx( 0 )
, _dataFlags( 0 )
, _output( 0 )
, _outputXFormIdx( 0 )
, _duration( kfData->GetDuration() )
{
    B_ASSERT( kfData != 0 );
    _dataFlags |= ( kfData->GetRotKFCount() > 0 ) ? kHasRotKFs : 0;
    _dataFlags |= ( kfData->GetScaleKFCount() > 0 ) ? kHasScaleKFs : 0;
    _dataFlags |= ( kfData->GetTranslateKFCount() > 0 ) ? kHasTranslateKFs : 0;
}

//----------------------------------------------------------
GrKFInterpolator::~GrKFInterpolator()
{

}


//==========================================================
// public methods.
//==========================================================

//----------------------------------------------------------
void
GrKFInterpolator::Init( URef< GrKFAnimData > kfData )
{
    B_ASSERT( kfData != 0 );

    _kfData = kfData;
    _curRotKFIdx = 0;
    _curScaleKFIdx = 0;
    _curTranslateKFIdx = 0;
    _dataFlags = 0;
    _output = 0;
    _outputXFormIdx = 0;
    _duration = kfData->GetDuration();

    _dataFlags |= ( kfData->GetRotKFCount() > 0 ) ? kHasRotKFs : 0;
    _dataFlags |= ( kfData->GetScaleKFCount() > 0 ) ? kHasScaleKFs : 0;
    _dataFlags |= ( kfData->GetTranslateKFCount() > 0 ) ? kHasTranslateKFs : 0;
}

//----------------------------------------------------------
URef< GrKFAnimData >
GrKFInterpolator::GetKFData() const
{
    return _kfData;
}

//----------------------------------------------------------
bool
GrKFInterpolator::SetOutput( GrAnimXFormCache* output )
{
    _output = output;
    if ( _output )
    {
        // check the index of the transform we're driving.  If it does not
        // exist, then we failed to set the output.
        _outputXFormIdx = _output->GetXFormIdx( _kfData->GetTargetName() );
        if ( _outputXFormIdx == ~0 )
        {
            _output = 0;
            return false;
        }
    }

    // success!
    return true;
}

//----------------------------------------------------------
void
GrKFInterpolator::ApplyPose( float time, float blendWeight, float clampDuration, bool clamp )
{
    // if we don't have an output, then don't do anything!
    if ( _output == 0 )
        return;

    if ( time > _duration )
    {
        // the clamp duration ensures that an animation channel is clamped
        // until it's parent animation completes.
        if ( clamp || time < clampDuration )
            time = _duration;
        else
            time = Fract( time / _duration ) * _duration;
    }

    static const MVec3 kOne( 1.0f, 1.0f, 1.0f );
    MQuat rotate;
    MVec3 translate( 0.0f, 0.0f, 0.0f );
    MVec3 scale( 1.0f, 1.0f, 1.0f );

    // convert the incoming time from seconds to 22.10 fixed point.
    unsigned int fixedTime = ( unsigned int )( time * 1024.0f );
    if ( _dataFlags & kHasRotKFs )
    {
        // find the correct keyframes to work with.
        const GrKFAnimData::SRotKeyFrame* curRotKF = 0;
        const GrKFAnimData::SRotKeyFrame* nextRotKF = 0;
        FindKeyFrame( curRotKF, nextRotKF, _curRotKFIdx, _kfData->GetRotKFArray(), fixedTime );

        // simply lerp to the desired rotation.
        float t = CalcT( fixedTime, curRotKF->time, nextRotKF->time );
        rotate = curRotKF->rotate.SlerpTo( nextRotKF->rotate, t );
        rotate = rotate;
    }

    if ( _dataFlags & kHasScaleKFs )
    {
        // find the correct keyframes to work with.
        const GrKFAnimData::SScaleKeyFrame* curScaleKF = 0;
        const GrKFAnimData::SScaleKeyFrame* nextScaleKF = 0;
        FindKeyFrame( curScaleKF, nextScaleKF, _curScaleKFIdx, _kfData->GetScaleKFArray(), fixedTime );

        // perform a cubic bezier interpolation to the desired scale.
        float t = CalcT( fixedTime, curScaleKF->time, nextScaleKF->time );
        scale = Lerp( curScaleKF->scaleXYZ, nextScaleKF->scaleXYZ, t );
//      scale = Bezier3( curScaleKF->scaleXYZ, curScaleKF->outTan, nextScaleKF->inTan, nextScaleKF->scaleXYZ, t );
    }

    if ( _dataFlags & kHasTranslateKFs )
    {
        // find the correct keyframes to work with.
        const GrKFAnimData::STranslateKeyFrame* curTranslateKF = 0;
        const GrKFAnimData::STranslateKeyFrame* nextTranslateKF = 0;
        FindKeyFrame( curTranslateKF, nextTranslateKF, _curTranslateKFIdx, _kfData->GetTranslateKFArray(), fixedTime );

        // perform a cubic bezier interpolation to the desired translation.
        float t = CalcT( fixedTime, curTranslateKF->time, nextTranslateKF->time );
        translate = Lerp( curTranslateKF->translate, nextTranslateKF->translate, t );
//      translate = Bezier3( curTranslateKF->translate, curTranslateKF->outTan, nextTranslateKF->inTan, nextTranslateKF->translate, t );
    }

    // slerp from identity!
    rotate.BlendFromIdentFast( blendWeight );
    translate *= blendWeight;
    scale = blendWeight * ( scale - kOne );
    _output->AccumXForm( _outputXFormIdx, GrAnimXForm( rotate, translate, scale ) );
}


//==========================================================
// private methods.
//==========================================================

//----------------------------------------------------------
float
GrKFInterpolator::CalcT( unsigned int fixedTime, unsigned int curKFTime, unsigned int nextKFTime )
{
    // find where we lie between the two keyframes.
    unsigned int range = nextKFTime - curKFTime;

    // the maximum allowed time between keyframes is (2^22)-1 (~4095 seconds).
    // If the time exceeds this, then this algorithm will fail.  If this is not
    // acceptible, then this algorithm will need to be rewritten.
    B_ASSERT( range < (1<<22) );

    // calculate t.  Note we can speed this up considerably (and increase accuracy)
    // by building the float 't' manually (/1024 is just a change to the exponent!).
    unsigned int tFixed = ( ( fixedTime - curKFTime ) << 10 ) / range;
    float t = float( tFixed ) / 1024.0f;

    // return t.
    return t;
}
