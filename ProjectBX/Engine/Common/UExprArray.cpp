//----------------------------------------------------------
// File:        UExprArray.cpp
// Author:      Kevin Bray
// Created:     07-05-05
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "common_afx.h"

// class header.
#include "UExprArray.h"

// project includes.
#include "UExprTerm.h"

//**********************************************************
// class UExprArray
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
UExprArray::UExprArray( const tstring& name, float indexScale, bool smooth, bool clamp )
: _name( name )
, _indexScale( indexScale )
, _smooth( smooth )
, _clamp( clamp )
{
    _terms.reserve( 40 );
}

//----------------------------------------------------------
UExprArray::~UExprArray()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
UExprArray::AddElement( URef< UExprTerm > term )
{
    _terms.push_back( term );
}

//----------------------------------------------------------
float
UExprArray::GetValue( float index ) const
{
    if ( _terms.size() == 0 )
    {
        // no elements, then return 0.
        return 0.0f;
    }

    // scale the incoming index by the address scale.
    index *= _indexScale;

    // evaluate.
    int arrayIndex = int( index );
    int arraySize = int( _terms.size() );
    int arrayMax = arraySize - 1;
    if ( _smooth )
    {
        if ( _clamp )
        {
            // on either end?
            if ( arrayIndex >= arrayMax )
                return _terms[ arrayMax ]->GetValue();
            else if ( arrayIndex < 0 )
                return _terms[ 0 ]->GetValue();

            // if not on the end, do two lookups and lerp using
            // the fractional component of the index.
            float value0 = _terms[ arrayIndex ]->GetValue();
            float value1 = _terms[ arrayIndex + 1 ]->GetValue();
            float frac = index - Floor( index );
            return Lerp( value0, value1, frac );
        }
        else
        {
            // do two lookups and lerp using the fractional
            // component of the index.
            float value0 = _terms[ ( unsigned int )arrayIndex % arraySize ]->GetValue();
            float value1 = _terms[ ( unsigned int )( arrayIndex + 1 ) % arraySize ]->GetValue();
            float frac = index - Floor( index );        
            return Lerp( value0, value1, frac );
        }
    }

    // clamp or wrap.
    arrayIndex = _clamp ? Clamp( arrayIndex, 0, arrayMax ) : arrayIndex % arraySize;

    // do the lookup.
    return _terms[ arrayIndex ]->GetValue();
}

//----------------------------------------------------------
bool
UExprArray::IsDynamic( float index ) const
{
    if ( _terms.size() == 0 )
    {
        // no elements, so not dynamic..
        return false;
    }

    // evaluate.
    int arrayIndex = int( index );
    int arraySize = int( _terms.size() );
    int arrayMax = arraySize - 1;
    if ( _smooth )
    {
        if ( _clamp )
        {
            // on either end?
            if ( arrayIndex >= arrayMax )
                return _terms[ arrayMax ]->IsDynamic();
            else if ( arrayIndex < 0 )
                return _terms[ 0 ]->IsDynamic();

            float frac = index - Floor( index );
            if ( frac > 0.01 && frac < 0.99 )
            {
                return _terms[ arrayIndex ]->IsDynamic() || _terms[ arrayIndex + 1 ]->IsDynamic();
            }
            else if ( frac <= 0.01 )
            {
                return _terms[ arrayIndex ]->IsDynamic();
            }
            else
            {
                return _terms[ arrayIndex + 1 ]->IsDynamic();
            }
        }
        else
        {
            // do two lookups and lerp using the fractional
            // component of the index.
            arrayIndex = ( arrayIndex % arraySize );
            unsigned int nextArrayIndex = ( arrayIndex + 1 ) % arraySize;
            float frac = index - Floor( index );
            if ( frac > 0.01 && frac < 0.99 )
            {
                return _terms[ arrayIndex ]->IsDynamic() || _terms[ nextArrayIndex ]->IsDynamic();
            }
            else if ( frac <= 0.01 )
            {
                return _terms[ arrayIndex ]->IsDynamic();
            }
            else
            {
                return _terms[ nextArrayIndex ]->IsDynamic();
            }
        }
    }

    // clamp or wrap.
    arrayIndex = _clamp ? Clamp( arrayIndex, 0, arrayMax ) : arrayIndex % arraySize;

    // do the lookup.
    return _terms[ arrayIndex ]->IsDynamic();
}
