//----------------------------------------------------------
// File:        EdBrush.cpp
// Author:      Kevin Bray
// Created:     05-17-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdBrush.h"

// engine includes.
#include "EdBrushImage.h"
#include "GrImage.h"


// shader forward declarations.
void BasicVertexProgram( SVec4& output, SVec4* varying, const SVec4* vertexConstants, const SPolygonVertex& vertex );
void BasicFragmentProgram( unsigned char** output, unsigned int* outputBPP, const SVec4* fragmentConstants,
                           const SVec4* varying, const SVec4* gradDX, const SVec4* gradDY, GrSoftwareSampler* textures );


//**********************************************************
// class EdBrush
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdBrush::EdBrush( const tstring& name, const SInfo& info )
: _name( name )
, _vertexProgram( 0 )
, _fragmentProgram( 0 )
{
    // determine the short name.
    size_t pos = name.find_last_of( '/' );
    if ( pos != tstring::npos )
        _shortName = name.substr( pos + 1 );
    else
        _shortName = name;

    // store the image and shader info.
    Rebuild( info );
}

//----------------------------------------------------------
EdBrush::~EdBrush()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdBrush::Rebuild( const SInfo& info )
{
    // store the vertex program.
    if ( info.vertexProgram )
        _vertexProgram = info.vertexProgram;
    else
        _vertexProgram = BasicVertexProgram;

    // store the fragment program.
    if ( info.fragmentProgram )
        _fragmentProgram = info.fragmentProgram;
    else
        _fragmentProgram = BasicFragmentProgram;

    // store the images.
    for ( unsigned int i = 0; i <= ES_COUNT; ++i )
        _images[ i ] = info.images[ i ];

    // store the mask.
    _mask = info.mask;
}

//----------------------------------------------------------
void
EdBrush::SetMask( URef< EdBrushImage > image )
{
    _mask = image;
}

//----------------------------------------------------------
URef< EdBrushImage >
EdBrush::GetMask() const
{
    return _mask;
}

//----------------------------------------------------------
void
EdBrush::SetStageSource( ESTAGE stage, URef< EdBrushImage > image )
{
    B_ASSERT( stage <= ES_COUNT );
    _images[ stage ] = image;
}

//----------------------------------------------------------
URef< EdBrushImage >
EdBrush::GetStageSource( ESTAGE stage ) const
{
    B_ASSERT( stage <= ES_COUNT );
    return _images[ stage ];
}

//----------------------------------------------------------
void
EdBrush::PreCacheStageSources()
{
    for ( unsigned int i = 0; i <= ES_COUNT; ++i )
        if ( _images[ i ] )
            _images[ i ]->PreCache();
}

//----------------------------------------------------------
void
EdBrush::EvictStageSources()
{
    for ( unsigned int i = 0; i <= ES_COUNT; ++i )
        if ( _images[ i ] )
            _images[ i ]->Evict();  
}


//**********************************************************
// shaders.
//**********************************************************

#define TRIANGLE_SETUP_SPEED_TEST       0

//----------------------------------------------------------
void
BasicVertexProgram( SVec4& output, SVec4* varying, const SVec4* vertexConstants, const SPolygonVertex& vertex )
{
    // calculate the output.
    output.x = 2.0f * vertex.uv.x - 1.0f;
    output.y = 2.0f * vertex.uv.y - 1.0f;
    output.z = 0.0f;
    output.w = 1.0f;
#if TRIANGLE_SETUP_SPEED_TEST
    return;
#endif
    // calculate the source texture coordinates.
    varying[ 0 ].x = 0.5f * vertexConstants[ 0 ].Dot( vertex.position ) + 0.5f;
    varying[ 0 ].y = 0.5f * vertexConstants[ 1 ].Dot( vertex.position ) + 0.5f;
    varying[ 0 ].z = 0.0f;
    varying[ 0 ].w = 1.0f;

    // calculate the mask texture coordinates.
    varying[ 1 ].x = 0.5f * vertexConstants[ 4 ].Dot( vertex.position ) + 0.5f;
    varying[ 1 ].y = 0.5f * vertexConstants[ 5 ].Dot( vertex.position ) + 0.5f;
    varying[ 1 ].z = vertexConstants[ 6 ].Dot( vertex.position );
    varying[ 1 ].w = 1.0f;

    // calculate the gradient for the normal rotation.
    varying[ 2 ].x = vertex.uv.x;
    varying[ 2 ].y = vertex.uv.y;
    varying[ 2 ].z = 0.0f;
    varying[ 2 ].w = 0.0f;
}

//----------------------------------------------------------
void
BasicFragmentProgram( unsigned char** output, unsigned int* outputBPP, const SVec4* fragmentConstants,
                      const SVec4* varying, const SVec4* gradDX, const SVec4* gradDY, GrSoftwareSampler* samplers )
{
#if TRIANGLE_SETUP_SPEED_TEST
    for ( unsigned int i = 0; i <= ES_COUNT; ++i )
    {
        unsigned int* dst = ( unsigned int* )( output[ i ] );
        dst[ 0 ] = 0;
    }

    return;
#endif

    bool shouldClip = ( fragmentConstants[ 0 ].y >= 1.0f );
    if( shouldClip )
    {
        // per-pixel clip based on the mask texture coordinates.
        if ( varying[ 1 ].x < 0.0f || varying[ 1 ].x > 1.0f ||
             varying[ 1 ].y < 0.0f || varying[ 1 ].y > 1.0f )
        {
            return;
        }
    }

    // depth test.
    unsigned char* mask = output[ 7 ];
    unsigned char curMask = ( *mask & 0xFF );

    // calculate the blend mode.  0 for additive, 1 for subtractive.
    unsigned int blendMode = ( *( unsigned int* )( &( fragmentConstants[ 0 ].x ) ) ) == 0;
    unsigned int peakMask = ( blendMode - 1 ) & 0xFF; 

    // fetch the mask if necessary.
    unsigned char srcMask = samplers[ 7 ].Sample( varying[ 1 ] ) & 0xFF;

    // invert the source mask if necessary.
    if ( blendMode > 0 )
        srcMask = ~srcMask;

    // perform the mask test.  Take the maximum if we're adding texture.
    // Take the minimum if we're removing texture.
    unsigned int oneMinusBlendMode = 1 - blendMode;
    unsigned int lhs = blendMode * curMask + oneMinusBlendMode * srcMask;
    unsigned int rhs = blendMode * srcMask + oneMinusBlendMode * curMask;
    if ( lhs < rhs )
        return;

    // write out the mask value.
    *mask = srcMask;

    // simply return if we're writing the mask only.
    if ( ( *( unsigned int* )( &( fragmentConstants[ 0 ].z ) ) ) != 0 )
        return;

    // calculate source texture coordinates.
    for ( unsigned int i = 0; i <= ES_COUNT; ++i )
    {
#if 1
        if ( i != ES_NORMAL )
        {
            // fetch the current stage color.
            unsigned int color = samplers[ i ].Sample( varying[ 0 ] );

            // break down the current color into it's components.
            unsigned int temp[ 4 ];
            temp[ 0 ] = ( color >>  0 ) & 0xFF;
            temp[ 1 ] = ( color >>  8 ) & 0xFF;
            temp[ 2 ] = ( color >> 16 ) & 0xFF;
            temp[ 3 ] = ( color >> 24 ) & 0xFF;

            // encode the write value.
            unsigned char* dst = output[ i ];
            unsigned int dstBPP = outputBPP[ i ];
            switch( dstBPP )
            {
            case 4:
                dst[ 3 ] = ( unsigned char )temp[ 3 ];
            case 3:
                dst[ 2 ] = ( unsigned char )temp[ 2 ];
                dst[ 1 ] = ( unsigned char )temp[ 1 ];
                dst[ 0 ] = ( unsigned char )temp[ 0 ];
                break;

            case 2:
                dst[ 1 ] = ( unsigned char )temp[ 3 ];
            case 1:
                dst[ 0 ] = ( unsigned char )temp[ 2 ];
                break;
            }
        }
        else
        {
            // make sure that the normal output stage is valid.
            B_ASSERT( outputBPP[ i ] == 2 );

            // get the normal for the current texture coordinate.
            SVec2 normalXY = samplers[ i ].SampleBump( *( SVec2* )varying, *( SVec2* )gradDX, *( SVec2* )gradDY, 8.0f );

            // write the normal.
            unsigned char* dst = output[ i ];
            dst[ 0 ] = ( unsigned int )Clamp( ( 0.5f * normalXY.x + 0.5f ) * 255.0f, 0.0f, 255.0f );    // X
            dst[ 1 ] = ( unsigned int )Clamp( ( 0.5f * normalXY.y + 0.5f ) * 255.0f, 0.0f, 255.0f );    // Y
        }
#else
        unsigned char* dst = ( unsigned char* )( output[ i ] );
        dst[ 0 ] = 0xFF;
        dst[ 1 ] = 0xFF;
        dst[ 2 ] = 0xFF;
        dst[ 3 ] = 0xFF;
#endif
    }
}
