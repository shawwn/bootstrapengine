//----------------------------------------------------------
// File:        GrUberTexRasterizer.cpp
// Author:      Kevin Bray
// Created:     02-16-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdUberTexRasterizer.h"

// project includes.
#include "EdBrush.h"
#include "EdBrushImage.h"

// engine includes.
#include "GrMeshInst.h"
#include "GrImage.h"
#include "GrDebug.h"

#define MASK_STAGE              7
#define PLANAR_STENCIL_STAGE    8
#define DEPTH_STENCIL_STAGE     9

static void CalcLine( float* line, const SVec2& p0, const SVec2& p1 );
static unsigned char Blend( unsigned char c0, unsigned char c1, unsigned char t );

// gets an image from the brush-image if the brush-image isn't
// null.  If it is null, then defImage is returned.
inline GrImage* GetImage( EdBrushImage* brushImage )
{
    if ( brushImage )
        return brushImage->GetImage();
    return 0;
}

#define TEST_RASTERIZER         0
#if TEST_RASTERIZER
static void TestVertexProg( SVec4& output, SVec4* varying, const SVec4* vertexConstants, const SPolygonVertex& vertex )
{
    output.x = vertex.position.x;
    output.y = vertex.position.y;
    output.z = vertex.position.z;
    output.w = 1.0f;
}

static void TestFragmentProg( unsigned char** output, unsigned int* outputBPP, const SVec4* fragmentConstants,
                              const SVec4* varying, GrImage** textures )
{
    output[ 0 ][ 0 ] = 0xFF;    // B
    output[ 0 ][ 1 ] = 0xFF;    // G
    output[ 0 ][ 2 ] = 0xFF;    // R
    //output[ 0 ][ 3 ] = 0xFF;  // A
}
#endif

//**********************************************************
// class GrUberTexRasterizer
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdUberTexRasterizer::EdUberTexRasterizer()
: _white( "white", 0xFFFFFFFF )
, _black( "black", 0x00000000 )
, _gray( "gray", 0x7F7F7F7F )
, _autoBake( false )
{
    // configure the rasterizer.
    _rasterizer.SetSamplerColor( ES_BUMP,               0x7F7F7F7F );
    _rasterizer.SetSamplerColor( ES_NORMAL,             0x7F7F7FFF );
    _rasterizer.SetSamplerColor( ES_DIFFUSE,            0xFFFFFFFF );
    _rasterizer.SetSamplerColor( ES_SPECULAR,           0x00000000 );
    _rasterizer.SetSamplerColor( ES_EMISSIVE,           0x00000000 );
    _rasterizer.SetSamplerColor( ES_AMBIENT,            0xFFFFFFFF );
    _rasterizer.SetSamplerColor( ES_COUNT,              0x00000000 );
    _rasterizer.SetSamplerColor( MASK_STAGE,            0xFFFFFFFF );
    _rasterizer.SetSamplerColor( PLANAR_STENCIL_STAGE,  0xFFFFFFFF );
    _rasterizer.SetSamplerColor( DEPTH_STENCIL_STAGE,   0xFFFFFFFF );

    // enable a fill convention that causes triangles to overlap slightly.
    _rasterizer.SetRenderLastPixel( true );

#if TEST_RASTERIZER
    // perform a rendering test.
    SPolygonVertex vertices[ 3 ] =
    {
        {
            SVec3( 0.0f, 0.0f, 0.0f ),
            SVec3( 0.0f, 0.0f,-1.0f ),
            SVec3( 1.0f, 0.0f, 0.0f ),
            SVec3( 0.0f, 1.0f, 0.0f ),
            { 0xFF, 0xFF, 0xFF, 0xFF },
            SVec2( 0.0f, 0.0f ),
            SVec2( 0.0f, 0.0f ) 
        },
        {
            SVec3( 1.0f, 0.0f, 0.0f ),
            SVec3( 0.0f, 0.0f,-1.0f ),
            SVec3( 1.0f, 0.0f, 0.0f ),
            SVec3( 0.0f, 1.0f, 0.0f ),
            { 0xFF, 0xFF, 0xFF, 0xFF },
            SVec2( 1.0f, 0.0f ),
            SVec2( 0.0f, 0.0f ) 
        },
        {
            SVec3( 1.0f, 1.0f, 0.0f ),
            SVec3( 0.0f, 0.0f,-1.0f ),
            SVec3( 1.0f, 0.0f, 0.0f ),
            SVec3( 0.0f, 1.0f, 0.0f ),
            { 0xFF, 0xFF, 0xFF, 0xFF },
            SVec2( 1.0f, 1.0f ),
            SVec2( 0.0f, 0.0f ) 
        },
    };

    GrPolygon polygon( 0, vertices, 3, 0 );

    // allocate the render target.
    unsigned char* target = new unsigned char[ 4 * 1024 * 1024 ];
    MemSet( target, 0, 4 * 1024 * 1024 );

    // configure the rasterizer.
    _rasterizer.SetRenderTarget( 0, target, 1024, 1024, 4 );
    _rasterizer.SetVertexProgram( TestVertexProg );
    _rasterizer.SetFragmentProgram( TestFragmentProg );

    // render the test polygon.
    _rasterizer.RenderPolygon( polygon );

    // save the image.
    GrSaveTGA( "rendertest.tga", target, 1024, 1024, 4 );
#endif
}

//----------------------------------------------------------
EdUberTexRasterizer::~EdUberTexRasterizer()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdUberTexRasterizer::Clear()
{

}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetBrush( URef< EdBrush > brush )
{
    // evict the previous brush and precache the new brush.
    if ( _brush != 0 )
        _brush->EvictStageSources();
    if ( brush )
        brush->PreCacheStageSources();

    // set the current brush.
    _brush = brush;

    // configure texture stages.
    if ( brush )
    {
        for ( unsigned int i = 0; i <= ES_COUNT; ++i )
        {
            EdBrushImage* brushImage = brush->GetStageSource( ( ESTAGE )i );
            _rasterizer.SetSamplerTexture( i, GetImage( brushImage ) );
        }

        // set the mask.
        _rasterizer.SetSamplerTexture( MASK_STAGE, GetImage( brush->GetMask() ) );

        // set the vertex and fragment shaders.
        SetVertexProgram( brush->GetVertexProgram() );
        SetFragmentProgram( brush->GetFragmentProgram() );
    }
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetSamplerColor( ESTAGE stage, unsigned int color )
{
    _rasterizer.SetSamplerColor( stage, color );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetStencils( URef< EdBrushImage > planarStencil, URef< EdBrushImage > depthStencil )
{
    // set the stencil textures.
    _rasterizer.SetSamplerTexture( PLANAR_STENCIL_STAGE, GetImage( planarStencil ) );
    _rasterizer.SetSamplerTexture( DEPTH_STENCIL_STAGE, GetImage( depthStencil ) );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetRenderTarget( GrMeshInst* target )
{
    _target = target;
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetVertexConstant( unsigned int index, const SVec4& vec )
{
    _rasterizer.SetVertexConstant( index, vec );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetVertexConstants( unsigned int index, const MMat4x4& sourceMatrix )
{
    _rasterizer.SetVertexConstants( index, sourceMatrix );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetFragmentConstant( unsigned int index, const SVec4& vec )
{
    _rasterizer.SetFragmentConstant( index, vec );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetFragmentConstants( unsigned int index, const MMat4x4& sourceMatrix )
{
    _rasterizer.SetFragmentConstants( index, sourceMatrix );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetVertexProgram( GrSoftwareRasterizer::VertexProgram vp )
{
    _rasterizer.SetVertexProgram( vp );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::SetFragmentProgram( GrSoftwareRasterizer::FragmentProgram fp )
{
    _rasterizer.SetFragmentProgram( fp );
}

//----------------------------------------------------------
void
EdUberTexRasterizer::RenderTriangleList( const SVec3* positions, const SVec2* texCoords,
                                         unsigned int triCount, const MMat4x4* uvXForm )
{
    // render triangles.
    SPolygonVertex vertices[ 3 ];
    for ( unsigned int i = 0; i < triCount; ++i, positions += 3, texCoords += 3 )
    {
        for ( unsigned int j = 0; j < 3; ++j )
        {
            vertices[ j ].position  = positions[ j ];
            vertices[ j ].uv        = texCoords[ j ];
        }
        RenderTriangle( vertices, uvXForm );
    }
}

//----------------------------------------------------------
void
EdUberTexRasterizer::RenderTriangleList( const SVec3* positions, const SVec2* texCoords,
                                         const unsigned short* indices, unsigned int triCount,
                                         const MMat4x4* uvXForm )
{
    // render triangles.
    SPolygonVertex vertices[ 3 ];
    for ( unsigned int i = 0; i < triCount; ++i, indices += 3 )
    {
        for ( unsigned int j = 0; j < 3; ++j )
        {
            vertices[ j ].position  = positions[ indices[ j ] ];
            vertices[ j ].uv        = texCoords[ indices[ j ] ];
        }
        RenderTriangle( vertices, uvXForm );
    }
}

//----------------------------------------------------------
void
EdUberTexRasterizer::RenderTriangle( const SPolygonVertex* vertices, const MMat4x4* uvXForm )
{
/*
    // calculate the target width in floating point.
    float fTargetWidth = ( float )( int )_target->GetWidth();
    float fTargetHeight = ( float )( int )_target->GetHeight();

    // temporary vertex data.
    SPolygonVertex vertexTemp[ 3 ];
    for ( unsigned int i = 0; i < 3; ++i )
        vertexTemp[ i ] = vertices[ i ];

    // convert the units from pixels to blocks.
    fTargetWidth /= gGrUberTextureThread->GetLayerDim();
    fTargetHeight /= gGrUberTextureThread->GetLayerDim();

    // move the current triangle into block space.
    SVec2 bsTexCoord0( vertices[ 0 ].uv.x, vertices[ 0 ].uv.y );
    SVec2 bsTexCoord1( vertices[ 1 ].uv.x, vertices[ 1 ].uv.y );
    SVec2 bsTexCoord2( vertices[ 2 ].uv.x, vertices[ 2 ].uv.y );

    // apply the culling transform.
    if ( uvXForm != 0 )
    {
        const MMat4x4& uvXFormRef = *uvXForm;
        MVec3 tc0( bsTexCoord0.x, bsTexCoord0.y, 0.0f );
        MVec3 tc1( bsTexCoord1.x, bsTexCoord1.y, 0.0f );
        MVec3 tc2( bsTexCoord2.x, bsTexCoord2.y, 0.0f );

        uvXFormRef.TransformCoordNoPerspFast( tc0 );
        uvXFormRef.TransformCoordNoPerspFast( tc1 );
        uvXFormRef.TransformCoordNoPerspFast( tc2 );

        bsTexCoord0.x = tc0.GetX();
        bsTexCoord0.y = tc0.GetY();

        bsTexCoord1.x = tc1.GetX();
        bsTexCoord1.y = tc1.GetY();

        bsTexCoord2.x = tc2.GetX();
        bsTexCoord2.y = tc2.GetY();
    }

    bsTexCoord0.x *= fTargetWidth;
    bsTexCoord0.y *= fTargetHeight;
    bsTexCoord1.x *= fTargetWidth;
    bsTexCoord1.y *= fTargetHeight;
    bsTexCoord2.x *= fTargetWidth;
    bsTexCoord2.y *= fTargetHeight;

    // calculate the bounds for the triangle in block space.
    SVec2 min = bsTexCoord0;
    min.Min3( bsTexCoord1, bsTexCoord2 );
    SVec2 max = bsTexCoord0;
    max.Max3( bsTexCoord1, bsTexCoord2 );

    // convert into blocks.
    int startX = ( int )min.x;
    int startY = ( int )min.y;
    int endX = ( int )Ceil( max.x );
    int endY = ( int )Ceil( max.y );

    // calculate line coefficients from block space coordinates.
    float line0[ 3 ];
    float line1[ 3 ];
    float line2[ 3 ];
    CalcLine( line0, bsTexCoord0, bsTexCoord1 );
    CalcLine( line1, bsTexCoord1, bsTexCoord2 );
    CalcLine( line2, bsTexCoord2, bsTexCoord0 );

    // figure out which blocks the triangle resides in.
    int coverage = 0;
    for ( int blockY = startY; blockY < endY; ++blockY )
    {
        // iterate through each block along X.
        for ( int blockX = startX; blockX < endX; ++blockX )
        {
            // render the triangle to the current block.
            GrUberTileLayer* layerBlock = GetBlock( blockX * gGrUberTextureThread->GetLayerDimInTiles(),
                blockY * gGrUberTextureThread->GetLayerDimInTiles() );
            if ( !layerBlock )
            {
                // bake if necessary.
                if ( _autoBake )
                {
                    // bake and then request the block again.
                    gGrUberTextureThread->BakeLayers();
                    layerBlock = GetBlock( blockX * gGrUberTextureThread->GetLayerDimInTiles(),
                        blockY * gGrUberTextureThread->GetLayerDimInTiles() );
                }
                else
                    continue;
            }
            layerBlock->BindToRasterizer( _rasterizer );

            // adjust the texture coordinates to be relative to the
            // current write-cache block.
            float uMul = fTargetWidth;
            float vMul = fTargetHeight;
            float uAdd = -( float )( 2 * blockX ) + fTargetWidth - 1.0f;
            float vAdd = -( float )( 2 * blockY ) + fTargetHeight - 1.0f;

            // set the viewport transform to deal with the fact that we're in a block.
            _rasterizer.SetViewportTransform( SVec2( fTargetWidth, fTargetHeight ), SVec2( uAdd, vAdd ) );

            // rasterize the triangle.
            _rasterizer.RenderTriangleList( vertexTemp, 1 );
        }
    }
*/
}

//----------------------------------------------------------
void
EdUberTexRasterizer::RenderTriangleListAndBake( const SVec3* positions, const SVec2* texCoords,
                                                unsigned int triCount )
{
    unsigned int vertexCount = 3 * triCount;
    SPolygonVertex* vertices = new SPolygonVertex[ vertexCount ];
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        vertices[ i ].position = positions[ i ];
        vertices[ i ].uv = texCoords[ i ];
    }

    RenderTrianglesAndBake( vertices, triCount );
    delete[] vertices;
}

//----------------------------------------------------------
void
EdUberTexRasterizer::RenderTriangleListAndBake( const SVec3* positions, const SVec2* texCoords,
                                                const unsigned short* indices, unsigned int triCount )
{
    unsigned int vertexCount = 3 * triCount;
    SPolygonVertex* vertices = new SPolygonVertex[ vertexCount ];
    for ( unsigned int i = 0; i < vertexCount; ++i )
    {
        vertices[ i ].position = positions[ indices[ i ] ];
        vertices[ i ].uv = texCoords[ indices[ i ] ];
    }

    RenderTrianglesAndBake( vertices, triCount );
    delete[] vertices;
}

//----------------------------------------------------------
void
EdUberTexRasterizer::RenderTrianglesAndBake( const SPolygonVertex* vertices, unsigned int triCount )
{
/*
    // calculate the target width in floating point.
    float fTargetWidth = ( float )( int )_target->GetWidth();
    float fTargetHeight = ( float )( int )_target->GetHeight();

    // convert the units from pixels to blocks.
    fTargetWidth /= gGrUberTextureThread->GetLayerDim();
    fTargetHeight /= gGrUberTextureThread->GetLayerDim();

    // convert into blocks.
    int startX = 0;
    int startY = 0;
    int endX = ( int )_target->GetWidth() / gGrUberTextureThread->GetLayerDim();
    int endY = ( int )_target->GetHeight() / gGrUberTextureThread->GetLayerDim();

    // figure out which blocks the triangle resides in.
    int coverage = 0;
    for ( int blockY = startY; blockY < endY; ++blockY )
    {
        // iterate through each block along X.
        for ( int blockX = startX; blockX < endX; ++blockX )
        {
            // now render every triangle that affects this block.
            bool blockRendered = false;
            for ( unsigned int i = 0; i < triCount; ++i )
            {
                // move the current triangle into block space.
                SVec2 bsTexCoord0( fTargetWidth * vertices[ 3 * i + 0 ].uv.x, fTargetHeight * vertices[ 3 * i + 0 ].uv.y );
                SVec2 bsTexCoord1( fTargetWidth * vertices[ 3 * i + 1 ].uv.x, fTargetHeight * vertices[ 3 * i + 1 ].uv.y );
                SVec2 bsTexCoord2( fTargetWidth * vertices[ 3 * i + 2 ].uv.x, fTargetHeight * vertices[ 3 * i + 2 ].uv.y );

                // calculate the bounds for the triangle in block space.
                SVec2 min = bsTexCoord0;
                min.Min3( bsTexCoord1, bsTexCoord2 );
                SVec2 max = bsTexCoord0;
                max.Max3( bsTexCoord1, bsTexCoord2 );

                if ( ( float )blockX < min.x || ( float )blockX > max.x )
                    continue;
                if ( ( float )blockY < min.y || ( float )blockY > max.y )
                    continue;

                if ( !blockRendered )
                {
                    // render the triangle to the current block.
                    GrUberTileLayer* layerBlock = GetBlock( blockX * gGrUberTextureThread->GetLayerDimInTiles(),
                        blockY * gGrUberTextureThread->GetLayerDimInTiles() );
                    if ( !layerBlock )
                    {
                        // bake if necessary.
                        if ( _autoBake )
                        {
                            // bake and then request the block again.
                            gGrUberTextureThread->BakeLayers();
                            layerBlock = GetBlock( blockX * gGrUberTextureThread->GetLayerDimInTiles(),
                                blockY * gGrUberTextureThread->GetLayerDimInTiles() );
                        }
                        else
                            continue;
                    }
                    layerBlock->BindToRasterizer( _rasterizer );

                    // adjust the texture coordinates to be relative to the
                    // current write-cache block.
                    float uMul = fTargetWidth;
                    float vMul = fTargetHeight;
                    float uAdd = -( float )( 2 * blockX ) + fTargetWidth - 1.0f;
                    float vAdd = -( float )( 2 * blockY ) + fTargetHeight - 1.0f;

                    // set the viewport transform to deal with the fact that we're in a block.
                    _rasterizer.SetViewportTransform( SVec2( fTargetWidth, fTargetHeight ), SVec2( uAdd, vAdd ) );

                    // set the block rendered flag to true.
                    blockRendered = true;
                }

                // rasterize the triangle.
                _rasterizer.RenderTriangleList( vertices + 3 * i, 1 );
            }
        }
    }

    gGrUberTextureThread->BakeLayers();
*/
}

//----------------------------------------------------------
void
EdUberTexRasterizer::Bake()
{

}


//==========================================================
// private methods
//==========================================================


//**********************************************************
// file global functions
//**********************************************************

//----------------------------------------------------------
void CalcLine( float* line, const SVec2& p0, const SVec2& p1 )
{
    // calculate the line coefficients.
    line[ 0 ] =  p1.y - p0.y;
    line[ 1 ] = -p1.x + p0.x;
    line[ 2 ] = -( line[ 0 ] * p1.x + line[ 1 ] * p1.y );
}


//**********************************************************
// shaders
//**********************************************************
/*
//----------------------------------------------------------
void EdUberTexRasterizer::ColorVertexProgram( SVec4& output, SVec4* varying, const SVec4* vertexConstants, const SPolygonVertex& vertex )
{
    // calculate the output.
    output.x = 2.0f * vertex.uv.x - 1.0f;
    output.y = 2.0f * vertex.uv.y - 1.0f;
    output.z = 0.0f;
    output.w = 1.0f;

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
}

//----------------------------------------------------------
void EdUberTexRasterizer::ColorFragmentProgram( unsigned int** output, const SVec4* fragmentConstants, const SVec4* varying, GrImage** textures )
{
    // per-pixel clip based on the mask texture coordinates.
    if ( varying[ 1 ].x < 0.0f || varying[ 1 ].x > 1.0f ||
         varying[ 1 ].y < 0.0f || varying[ 1 ].y > 1.0f )
    {
        return;
    }

    // write out red for the time being.
//  *( output[ ES_EMISSIVE ] ) = 0xFFFF0000;
//  return;

    // depth test.
    unsigned int* d24m8 = output[ 7 ];
    unsigned int dstDepth = ( *d24m8 >> 8 );
    unsigned char dstMask = ( *d24m8 & 0xFF );
    unsigned int srcDepth = ( unsigned int )( int )( varying[ 1 ].z * 16777215.0f );

    // do we fail the depth test?  If so, return.
    if( 0 )
    {
        if ( srcDepth > dstDepth )
            return;
    }

    // fetch the mask if necessary.
    GrImage* maskTexture = textures[ 7 ];
    unsigned char srcMask = 0xFF;
    if ( maskTexture )
    {
        // calculate the mask texture coordinates.
        float maskX = varying[ 1 ].x;
        float maskY = varying[ 1 ].y;
        int maskPelX = ( int )( maskX * ( int )maskTexture->GetWidth() );
        int maskPelY = ( int )( maskY * ( int )maskTexture->GetHeight() );
        maskPelX = Clamp( maskPelX, 0, ( int )maskTexture->GetWidth()-1 );
        maskPelY = Clamp( maskPelY, 0, ( int )maskTexture->GetHeight()-1 );

        // fetch the data from the mask.
        unsigned int bpp = maskTexture->GetBytesPerPixel();
        unsigned char* src = ( unsigned char* )maskTexture->GetImageData() + (bpp-1);
        srcMask = src[ ( maskPelY * maskTexture->GetWidth() + maskPelX ) * bpp ];
    }

    // check to see if the pixel has already been rendered.
    bool pixelRendered = dstMask != 0;

    // perform the mask test.  Take the maximum if we're adding texture.
    // Take the minimum if we're removing texture.
    if ( fragmentConstants[ 0 ].x > 0.5f )
    {
        if ( srcMask > dstMask )
            dstMask = srcMask;
    }
    else
    {
        if ( srcMask < dstMask )
            dstMask = srcMask;
    }

    // write out the depth and the mask values.
    *d24m8 = ( srcDepth << 8 ) | dstMask;

    // if the pixel has already been rendered, skip it.
    //if ( dstMask == 0 || pixelRendered )
        //return;

    // calculate source texture coordinates.
    for ( unsigned int i = 0; i <= ES_COUNT; ++i )
    {
        // check for a source image.
#if 1
        GrImage* curSource = textures[ i ];
        if ( curSource == 0 )
            continue;

        // get the source image lookup.
        int sourcePelX = ( int )( varying[ 0 ].x * ( int )curSource->GetWidth() );
        int sourcePelY = ( int )( varying[ 0 ].y * ( int )curSource->GetHeight() );
        sourcePelX %= curSource->GetWidth();
        sourcePelY %= curSource->GetHeight();

        // write to the current output.
        unsigned int bpp = curSource->GetBytesPerPixel();
        unsigned int srcWidth = curSource->GetWidth();

        // get the current source image pointer.
        const unsigned char* srcData = ( const unsigned char* )curSource->GetImageData();
        srcData += ( sourcePelY * srcWidth + sourcePelX ) * bpp;
    
        unsigned char* dst = ( unsigned char* )( output[ i ] );
        if ( bpp == 1 )
        {
            dst[ 0 ] = srcData[ 0 ];
            dst[ 1 ] = srcData[ 0 ];
            dst[ 2 ] = srcData[ 0 ];
            dst[ 3 ] = srcData[ 0 ];
        }
        else if ( bpp == 2 )
        {
            dst[ 0 ] = srcData[ 0 ];
            dst[ 1 ] = srcData[ 0 ];
            dst[ 2 ] = srcData[ 0 ];
            dst[ 3 ] = srcData[ 1 ];
        }
        else if ( bpp == 3 )
        {
            dst[ 0 ] = srcData[ 0 ];
            dst[ 1 ] = srcData[ 1 ];
            dst[ 2 ] = srcData[ 2 ];
            dst[ 3 ] = 0xFF;
        }
        else // ( bpp == 4 )
        {
            dst[ 0 ] = srcData[ 0 ];
            dst[ 1 ] = srcData[ 1 ];
            dst[ 2 ] = srcData[ 2 ];
            dst[ 3 ] = srcData[ 3 ];
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
*/