//----------------------------------------------------------
// File:        GrRenderUtils.cpp
// Author:      Kevin Bray
// Created:     10-20-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrRenderUtil.h"

// project includes.
#include "GrMeshBufferMgr.h"
#include "GrMeshVB.h"
#include "GrTypes.h"
#include "GrCamera.h"
#include "GrColor.h"
#include "GrShader.h"
#include "GrShaderMgr.h"
#include "GrConfig.h"
#include "MAABox.h"
#include "MOBox.h"

// simple color shader.
const char* colorShaderVP =
"attribute vec3 a_Position;\n"
"attribute vec4 a_Color;\n"
"varying vec4 v_Color;\n"
"uniform mat4 u_ModelViewProjectionMatrix;\n"
"void main() {\n"
" gl_Position = u_ModelViewProjectionMatrix * vec4( a_Position.xyz, 1.0 );\n"
" v_Color = a_Color;\n"
"}\n";

const char* colorShaderFP = 
"varying vec4 v_Color;\n"
"void main() {\n"
" gl_FragColor = v_Color;\n"
"}\n";

struct STexVertex
{
    SVec3 position;
    SVec2 texCoord;
};

struct SColorVertex
{
    SVec3 position;
    unsigned long color;
};

// singleton pointer.
GrRenderUtil* gGrRenderUtil = 0;

//**********************************************************
// class GrRenderUtil
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderUtil::GrRenderUtil()
{
    B_ASSERT( gGrRenderUtil == 0 );
    gGrRenderUtil = this;
    CreateResources();
}

//----------------------------------------------------------
GrRenderUtil::~GrRenderUtil()
{
    gGrRenderUtil = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrRenderUtil::DrawScreenQuad( float minS, float minT, float maxS, float maxT )
{
    // setup the vertex data.
    STexVertex vertices[ 4 ];
    vertices[ 0 ].position = SVec3( -1.0f, 1.0f, 0.0f );
    vertices[ 0 ].texCoord.x = minS;
    vertices[ 0 ].texCoord.y = maxT;
    vertices[ 1 ].position = SVec3( 1.0f, 1.0f, 0.0f );
    vertices[ 1 ].texCoord.x = maxS;
    vertices[ 1 ].texCoord.y = maxT;
    vertices[ 2 ].position = SVec3( -1.0f, -1.0f, 0.0f );
    vertices[ 2 ].texCoord.x = minS;
    vertices[ 2 ].texCoord.y = minT;
    vertices[ 3 ].position = SVec3( 1.0f, -1.0f, 0.0f );
    vertices[ 3 ].texCoord.x = maxS;
    vertices[ 3 ].texCoord.y = minT;

    // upload the vertex data to the device.
    _quadVB->UploadUncached( 0, vertices, sizeof( vertices ) );

    // bind the screen-quad vertex buffer for rendering.
    _quadVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TEXCOORD_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_TRIANGLE_STRIP, 0, 4 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawQuad( float x, float y, float w, float h, float minS, float minT, float maxS, float maxT )
{
    // setup the vertex data.
    STexVertex vertices[ 4 ];
    vertices[ 0 ].position = SVec3( x, y + h, 0.0f );
    vertices[ 0 ].texCoord.x = minS;
    vertices[ 0 ].texCoord.y = maxT;
    vertices[ 1 ].position = SVec3( x + w, y + h, 0.0f );
    vertices[ 1 ].texCoord.x = maxS;
    vertices[ 1 ].texCoord.y = maxT;
    vertices[ 2 ].position = SVec3( x, y, 0.0f );
    vertices[ 2 ].texCoord.x = minS;
    vertices[ 2 ].texCoord.y = minT;
    vertices[ 3 ].position = SVec3( x + w, y, 0.0f );
    vertices[ 3 ].texCoord.x = maxS;
    vertices[ 3 ].texCoord.y = minT;

    // upload the vertex data to the device.
    _quadVB->UploadUncached( 0, vertices, sizeof( vertices ) );

    // bind the screen-quad vertex buffer for rendering.
    _quadVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TEXCOORD_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_TRIANGLE_STRIP, 0, 4 );
}

//----------------------------------------------------------
void
GrRenderUtil::SetupColoredRender( const GrCamera& camera, bool wireframe, bool additive )
{
    // configure render states for rendering.
    GrSetState( GR_DEPTHTEST | ( wireframe ? GR_WIREFRAME : 0 ) | ( additive ? GR_BLEND : 0 ) );
    GrSetWriteEnable( GR_DEPTH | GR_RGBA );
    GrSetDepthMode( EDM_LESSEQUAL );

    if ( additive )
    {
        GrSetBlendMode( EBM_SRC_ALPHA, EBM_ONE );
    }

    // bind the color shader.
    _colorShader->Bind( EGQ_LOW );
    _colorShader->SetEngineParam4x4fv( GrShader::EP_MODEL_VIEW_PROJECTION_MATRIX, camera.GetViewProjMatrix() );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredPoint( const GrCamera& camera, const MVec3& pos, const GrColor& color, unsigned int sizeInPixels )
{
    B_ASSERT( false && "TESTME!" );

    // get the viewport's dimensions.   
    int vpX = 0;
    int vpY = 0;
    int vpWidth = 0;
    int vpHeight = 0;
    GrGetViewport( vpX, vpY, vpWidth, vpHeight );

    // calculate half of the point's width in virtual coordinates.  Note that
    // this is half because virtual screen space goes from -1..1.
    float halfPointWidth = sizeInPixels / ( float )vpWidth;
    float halfPointHeight = sizeInPixels / ( float )vpHeight;

    // transform the point into screen space.
    MVec3 ssCenter = camera.GetViewProjMatrix().TransformCoord( pos );

    // calculate the lower-left corner.
    float minX = ssCenter.GetX() - halfPointWidth;
    float minY = ssCenter.GetY() - halfPointHeight;

    // calculate the upper-right corner.
    float maxX = ssCenter.GetX() + halfPointWidth;
    float maxY = ssCenter.GetY() + halfPointHeight;

    // write the positions.
    SVec3* positions = _colorVB->GetVertices();
    positions[ 0 ] = SVec3( minX, maxY, ssCenter.GetZ() );
    positions[ 1 ] = SVec3( maxX, maxY, ssCenter.GetZ() );
    positions[ 2 ] = SVec3( minX, minY, ssCenter.GetZ() );
    positions[ 3 ] = SVec3( maxX, minY, ssCenter.GetZ() );

    // calculate the packed color.
    unsigned long packedColor = 0;
    color.PackToRGBA( ( unsigned char* )&packedColor );

    // write the colors.
    unsigned long* colors = _colorVB->GetColors();
    colors[ 0 ] = packedColor;
    colors[ 1 ] = packedColor;
    colors[ 2 ] = packedColor;
    colors[ 3 ] = packedColor;

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_TRIANGLE_STRIP, 0, 4 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredSphere( const MVec3& pos, float radius, const GrColor& color, unsigned int slices )
{
    // clamp the number of slices to 16.
    slices = Min( slices, 16U );

    // force the slice count to be even.
    slices &= ~1;

    // figure out the half slice count.
    int halfSlices = slices / 2;
    float invHalfSlices = 1.0f / ( float )halfSlices;

    // scan from top to bottom.
    float theta1 = 0.0f;
    float theta2 = 0.0f;
    float phi1 = 0.0f;
    float phi2 = 0.0f;

    // pack the color to RGBA.
    unsigned long packedColor = 0;
    color.PackToRGBA( ( unsigned char* )&packedColor );

    // get the positions and colors from the vertex buffer.
    unsigned int vertexDataSize = slices * slices * 8 * sizeof( SColorVertex );
    SColorVertex* vertices = ( SColorVertex* )TempAlloc( vertexDataSize );

    // now pack vertices.
    SColorVertex* curVertex = vertices;
    for ( int i = 0; i < ( int )slices; ++i )
    {
        // calculate the position of the current ring.
        float t1 = ( ( i + 0 ) - halfSlices ) * invHalfSlices;
        float t2 = ( ( i + 1 ) - halfSlices ) * invHalfSlices;
        float y1 = radius * Sin( t1 * HALF_PI );
        float y2 = radius * Sin( t2 * HALF_PI );
        float ringWidth1 = radius * Cos( t1 * HALF_PI );
        float ringWidth2 = radius * Cos( t2 * HALF_PI );

        // draw each ring.
        for ( int j = 0; j < ( int )slices; ++j )
        {
            float t1 = ( ( j + 0 ) - halfSlices ) * invHalfSlices;
            float t2 = ( ( j + 1 ) - halfSlices ) * invHalfSlices;

            // X.
            float x1 = ringWidth1 * Sin( t1 * HALF_PI );
            float x2 = ringWidth2 * Sin( t1 * HALF_PI );
            float x3 = ringWidth2 * Sin( t2 * HALF_PI );
            float x4 = ringWidth1 * Sin( t2 * HALF_PI );

            // Z.
            float z1 = ringWidth1 * Cos( t1 * HALF_PI );
            float z2 = ringWidth2 * Cos( t1 * HALF_PI );
            float z3 = ringWidth2 * Cos( t2 * HALF_PI );
            float z4 = ringWidth1 * Cos( t2 * HALF_PI );

            // quad 1.
            curVertex->position = SVec3( pos.GetX() + x1, pos.GetY() + y1, pos.GetZ() + z1 );
            curVertex->color = packedColor;
            ++curVertex;
            curVertex->position = SVec3( pos.GetX() + x2, pos.GetY() + y2, pos.GetZ() + z2 );
            curVertex->color = packedColor;
            ++curVertex;
            curVertex->position = SVec3( pos.GetX() + x3, pos.GetY() + y2, pos.GetZ() + z3 );
            curVertex->color = packedColor;
            ++curVertex;
            curVertex->position = SVec3( pos.GetX() + x4, pos.GetY() + y1, pos.GetZ() + z4 );
            curVertex->color = packedColor;
            ++curVertex;

            // quad 2.
            curVertex->position = SVec3( pos.GetX() - x1, pos.GetY() - y1, pos.GetZ() - z1 );
            curVertex->color = packedColor;
            ++curVertex;
            curVertex->position = SVec3( pos.GetX() - x2, pos.GetY() - y2, pos.GetZ() - z2 );
            curVertex->color = packedColor;
            ++curVertex;
            curVertex->position = SVec3( pos.GetX() - x3, pos.GetY() - y2, pos.GetZ() - z3 );
            curVertex->color = packedColor;
            ++curVertex;
            curVertex->position = SVec3( pos.GetX() - x4, pos.GetY() - y1, pos.GetZ() - z4 );
            curVertex->color = packedColor;
            ++curVertex;
        }
    }

    // upload the data to the card.
    _colorVB->UploadUncached( 0, vertices, vertexDataSize );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_QUADS, 0, slices * slices * 8 );

    // free vertex memory.
    TempFree( vertices );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredLine( const MVec3& pos1, const MVec3& pos2, const GrColor& color )
{
    DrawColoredLine( pos1, pos2, color, color );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredLine( const MVec3& pos1, const MVec3& pos2, const GrColor& color1, const GrColor& color2 )
{
    // pack the color to RGBA.
    unsigned long packedColor1 = 0;
    color1.PackToRGBA( ( unsigned char* )&packedColor1 );
    unsigned long packedColor2 = 0;
    color2.PackToRGBA( ( unsigned char* )&packedColor2 );

    // now pack vertices.
    SColorVertex vertices[ 2 ];
    vertices[ 0 ].position = SVec3( pos1.GetX(), pos1.GetY(), pos1.GetZ() );
    vertices[ 0 ].color = packedColor1;
    vertices[ 1 ].position = SVec3( pos2.GetX(), pos2.GetY(), pos2.GetZ() );
    vertices[ 1 ].color = packedColor2;

    // upload the data to the card.
    _colorVB->UploadUncached( 0, vertices, sizeof( vertices ) );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_LINES, 0, 2 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredTriangle( const MVec3& pos1, const MVec3& pos2, const MVec3& pos3, const GrColor& color )
{
    DrawColoredTriangle( pos1, pos2, pos3, color, color, color );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredTriangle( const MVec3& pos1, const MVec3& pos2, const MVec3& pos3, const GrColor& color1, const GrColor& color2, const GrColor& color3 )
{
    // pack the color to RGBA.
    unsigned long packedColor1 = 0;
    color1.PackToRGBA( ( unsigned char* )&packedColor1 );
    unsigned long packedColor2 = 0;
    color2.PackToRGBA( ( unsigned char* )&packedColor2 );
    unsigned long packedColor3 = 0;
    color3.PackToRGBA( ( unsigned char* )&packedColor3 );

    // now pack vertices.
    SColorVertex vertices[ 3 ];
    vertices[ 0 ].position = SVec3( pos1.GetX(), pos1.GetY(), pos1.GetZ() );
    vertices[ 0 ].color = packedColor1;
    vertices[ 1 ].position = SVec3( pos2.GetX(), pos2.GetY(), pos2.GetZ() );
    vertices[ 1 ].color = packedColor2;
    vertices[ 2 ].position = SVec3( pos3.GetX(), pos3.GetY(), pos3.GetZ() );
    vertices[ 2 ].color = packedColor3;

    // upload the data to the card.
    _colorVB->UploadUncached( 0, vertices, sizeof( vertices ) );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_TRIANGLES, 0, 3 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredCone( const MVec3& pos, float theta, float length, const GrColor& color, unsigned int slices )
{
    // make sure slices is even.
    slices &= ~1;

    // calculate some useful vars.
    int halfSlices = slices / 2;
    float invSlices = 1.0f / slices;
    float invHalfSlices = 1.0f / halfSlices;
    float lengthDivSlices = length * invSlices;
    float stepDelta = Tan( theta ) * lengthDivSlices;
    float radius1 = 0.0f;
    float radius2 = stepDelta;

    // pack the color to RGBA.
    unsigned long packedColor = 0;
    color.PackToRGBA( ( unsigned char* )&packedColor );

    // get the positions and colors from the vertex buffer.
    SVec3* curPosition = _colorVB->GetVertices();
    unsigned long* curColor = _colorVB->GetColors();

    // now pack vertices.
    for ( int i = 0; i < ( int )slices; ++i )
    {
        float z1 = -( i + 0 ) * lengthDivSlices;
        float z2 = -( i + 1 ) * lengthDivSlices;

        // render the current slice.
        for ( int j = 0; j < ( int )slices; ++j )
        {
            float t1 = ( ( j + 0 ) - halfSlices ) * invHalfSlices;
            float t2 = ( ( j + 1 ) - halfSlices ) * invHalfSlices;
            
            // calculate x and y.
            float x1 = radius1 * Sin( t1 * HALF_PI );
            float x2 = radius2 * Sin( t1 * HALF_PI );
            float x3 = radius2 * Sin( t2 * HALF_PI );
            float x4 = radius1 * Sin( t2 * HALF_PI );
            float y1 = radius1 * Cos( t1 * HALF_PI );
            float y2 = radius2 * Cos( t1 * HALF_PI );
            float y3 = radius2 * Cos( t2 * HALF_PI );
            float y4 = radius1 * Cos( t2 * HALF_PI );

            // draw the current quad.
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curColor++ = packedColor;
            *curPosition++ = SVec3( pos.GetX() + x1, pos.GetY() + y1, pos.GetZ() + z1 );
            *curPosition++ = SVec3( pos.GetX() + x2, pos.GetY() + y2, pos.GetZ() + z2 );
            *curPosition++ = SVec3( pos.GetX() + x3, pos.GetY() + y3, pos.GetZ() + z2 );
            *curPosition++ = SVec3( pos.GetX() + x4, pos.GetY() + y4, pos.GetZ() + z1 );
            *curPosition++ = SVec3( pos.GetX() - x1, pos.GetY() - y1, pos.GetZ() + z1 );
            *curPosition++ = SVec3( pos.GetX() - x2, pos.GetY() - y2, pos.GetZ() + z2 );
            *curPosition++ = SVec3( pos.GetX() - x3, pos.GetY() - y3, pos.GetZ() + z2 );
            *curPosition++ = SVec3( pos.GetX() - x4, pos.GetY() - y4, pos.GetZ() + z1 );
        }

        radius1 = radius2;
        radius2 += stepDelta;
    }

    // mark vertex components as dirty.
    _colorVB->MarkAsDirty( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_QUADS, 0, 24 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredCube( const MVec3& pos, float length, const GrColor& color )
{
    // draw the box.
    float halfLen = 0.5f * length;
    MVec3 minExts( pos.GetX() - halfLen, pos.GetY() - halfLen, pos.GetZ() - halfLen );
    MVec3 maxExts( pos.GetX() + halfLen, pos.GetY() + halfLen, pos.GetZ() + halfLen );
    DrawColoredAABox( minExts, maxExts, color );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredAABox( const MAABox& aaBox, const GrColor& color )
{
    // draw the box.
    DrawColoredAABox( aaBox.GetCenter() - aaBox.GetHalfExts(), aaBox.GetCenter() + aaBox.GetHalfExts(),
        color );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredAABox( const MVec3& minExts, const MVec3& maxExts, const GrColor& color )
{
    // pack the color to RGBA.
    unsigned long packedColor = 0;
    color.PackToRGBA( ( unsigned char* )&packedColor );

    // vertex data.
    SColorVertex vertices[ 24 ];
    SColorVertex* curVertex = vertices;

    // get the positions and colors from the vertex buffer.
    SVec3* curPosition = _colorVB->GetVertices();
    unsigned long* curColor = _colorVB->GetColors();

    // now pack vertices.
    // -Z.
    curVertex->position = SVec3( minExts.GetX(), minExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), minExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), maxExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), maxExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;

    // +Z.
    curVertex->position = SVec3( maxExts.GetX(), minExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), minExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), maxExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), maxExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;

    // -Y.
    curVertex->position = SVec3( minExts.GetX(), minExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), minExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), minExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), minExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;

    // +Y.
    curVertex->position = SVec3( maxExts.GetX(), maxExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), maxExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), maxExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), maxExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;

    // -X.
    curVertex->position = SVec3( minExts.GetX(), minExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), maxExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), maxExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( minExts.GetX(), minExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;

    // +X.
    curVertex->position = SVec3( maxExts.GetX(), maxExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), minExts.GetY(), maxExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), minExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;
    curVertex->position = SVec3( maxExts.GetX(), maxExts.GetY(), minExts.GetZ() );
    curVertex->color = packedColor;
    ++curVertex;

    // upload the vertex data to the device.
    _colorVB->UploadUncached( 0, vertices, sizeof( vertices ) );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_QUADS, 0, 24 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredOBox( const MMat4x4& transform, const MVec3& halfExtents, const GrColor& color )
{
    // grab all 8 corners of the box.
    MOBox box( transform, halfExtents );
    MVec3 corners[8];
    box.GetCorners( corners );

    MVec3 leftBottomNear  = corners[0];
    MVec3 leftBottomFar   = corners[1];
    MVec3 leftTopNear     = corners[2];
    MVec3 leftTopFar      = corners[3];
    MVec3 rightBottomNear = corners[4];
    MVec3 rightBottomFar  = corners[5];
    MVec3 rightTopNear    = corners[6];
    MVec3 rightTopFar     = corners[7];

    // pack the color to RGBA.
    unsigned long packedColor = 0;
    color.PackToRGBA( ( unsigned char* )&packedColor );

    // get the positions and colors from the vertex buffer.
    SVec3* curPosition = _colorVB->GetVertices();
    unsigned long* curColor = _colorVB->GetColors();

    // near face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );

    // far face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );

    // bottom face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );
    *curPosition++ = SVec3( leftBottomFar.GetX(), leftBottomFar.GetY(), leftBottomFar.GetZ() );
    *curPosition++ = SVec3( rightBottomFar.GetX(), rightBottomFar.GetY(), rightBottomFar.GetZ() );
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );

    // top face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( leftTopFar.GetX(), leftTopFar.GetY(), leftTopFar.GetZ() );
    *curPosition++ = SVec3( rightTopFar.GetX(), rightTopFar.GetY(), rightTopFar.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );

    // left face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( leftTopFar.GetX(), leftTopFar.GetY(), leftTopFar.GetZ() );
    *curPosition++ = SVec3( leftBottomFar.GetX(), leftBottomFar.GetY(), leftBottomFar.GetZ() );

    // right face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );
    *curPosition++ = SVec3( rightBottomFar.GetX(), rightBottomFar.GetY(), rightBottomFar.GetZ() );
    *curPosition++ = SVec3( rightTopFar.GetX(), rightTopFar.GetY(), rightTopFar.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );

    // mark vertex components as dirty.
    _colorVB->MarkAsDirty( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_QUADS, 0, 24 );
}

//----------------------------------------------------------
void
GrRenderUtil::DrawColoredFrustum( const GrFrustum& frustum, const GrColor& color )
{
    MVec3 leftBottomNear  = frustum.GetNearBottomLeft();
    MVec3 leftBottomFar   = frustum.GetFarBottomLeft();
    MVec3 leftTopNear     = frustum.GetNearTopLeft();
    MVec3 leftTopFar      = frustum.GetFarTopLeft();
    MVec3 rightBottomNear = frustum.GetNearBottomRight();
    MVec3 rightBottomFar  = frustum.GetFarBottomRight();
    MVec3 rightTopNear    = frustum.GetNearTopRight();
    MVec3 rightTopFar     = frustum.GetFarTopRight();

    // pack the color to RGBA.
    unsigned long packedColor = 0;
    color.PackToRGBA( ( unsigned char* )&packedColor );

    // get the positions and colors from the vertex buffer.
    SVec3* curPosition = _colorVB->GetVertices();
    unsigned long* curColor = _colorVB->GetColors();

    // near face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );

    // far face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );

    // bottom face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftBottomFar.GetX(), leftBottomFar.GetY(), leftBottomFar.GetZ() );
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );
    *curPosition++ = SVec3( rightBottomFar.GetX(), rightBottomFar.GetY(), rightBottomFar.GetZ() );

    // top face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( rightTopFar.GetX(), rightTopFar.GetY(), rightTopFar.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( leftTopFar.GetX(), leftTopFar.GetY(), leftTopFar.GetZ() );

    // left face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( leftBottomFar.GetX(), leftBottomFar.GetY(), leftBottomFar.GetZ() );
    *curPosition++ = SVec3( leftTopFar.GetX(), leftTopFar.GetY(), leftTopFar.GetZ() );
    *curPosition++ = SVec3( leftTopNear.GetX(), leftTopNear.GetY(), leftTopNear.GetZ() );
    *curPosition++ = SVec3( leftBottomNear.GetX(), leftBottomNear.GetY(), leftBottomNear.GetZ() );

    // right face.
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curColor++ = packedColor;
    *curPosition++ = SVec3( rightTopFar.GetX(), rightTopFar.GetY(), rightTopFar.GetZ() );
    *curPosition++ = SVec3( rightBottomFar.GetX(), rightBottomFar.GetY(), rightBottomFar.GetZ() );
    *curPosition++ = SVec3( rightBottomNear.GetX(), rightBottomNear.GetY(), rightBottomNear.GetZ() );
    *curPosition++ = SVec3( rightTopNear.GetX(), rightTopNear.GetY(), rightTopNear.GetZ() );

    // mark vertex components as dirty.
    _colorVB->MarkAsDirty( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // setup.
    _colorVB->Bind( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK );

    // issue the draw call.
    GrDrawArrays( EPT_QUADS, 0, 24 );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrRenderUtil::CreateResources()
{
    // create and initialize the quad vertex buffer.
    _quadVB = gGrMeshBufferMgr->CreateMeshVB( 4, GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TEXCOORD_MASK,
        GrMeshVB::kDynamic );
    SVec3* positions = _quadVB->GetVertices();
    positions[ 0 ] = SVec3( -1.0f, 1.0f, 0.0f );
    positions[ 1 ] = SVec3( 1.0f, 1.0f, 0.0f );
    positions[ 2 ] = SVec3( -1.0f, -1.0f, 0.0f );
    positions[ 3 ] = SVec3( 1.0f, -1.0f, 0.0f );
    _quadVB->MarkAsDirty( GR_ATTRIB_POSITION_MASK );

    // create and initialize the color VB.
    _colorVB = gGrMeshBufferMgr->CreateMeshVB( 2048, GR_ATTRIB_POSITION_MASK | GR_ATTRIB_COLOR_MASK,
        GrMeshVB::kDynamic );

    // create the color shader.
    _colorShader = gGrShaderMgr->GetShader( "GrRenderUtil_colorShader", colorShaderVP, colorShaderFP );
}
