//----------------------------------------------------------
// File:        GrDeformWave.cpp
// Author:      Kevin Bray
// Created:     05-20-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrDeformWave.h"

// project includes.
#include "GrMesh.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"
#include "GLSubsys.h"   // for debugging code.


//**********************************************************
// class GrDeformWave
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrDeformWave::GrDeformWave()
{

}

//----------------------------------------------------------
GrDeformWave::~GrDeformWave()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrDeformer*
GrDeformWave::Clone() const
{
    GrDeformWave* clone = new GrDeformWave();
    return clone;
}

//----------------------------------------------------------
void
GrDeformWave::Deform( GrMesh* dst, GrMesh* src, unsigned int srcRangeIdx, unsigned int time )
{
    // if no mesh to deform has been set, simply return.
    B_ASSERT( src != 0 );

    // source and destination vars.
    const GrMesh::SRange& srcRange = src->GetRange( srcRangeIdx );

    // calculate the starting vertex and the number of vertices to deform.
    unsigned int start = srcRange.start;
    unsigned int count = srcRange.count;
    if ( src->GetIndexData() != 0 )
    {
        start = srcRange.minIndex;
        count = srcRange.maxIndex - srcRange.minIndex + 1;
    }

    GrMeshVB* vb = dst->GetVertexData();
    B_ASSERT( count == vb->GetVertexCount() );

    // get the stream sources.
    SVec3* vertexSrc = vb->GetVertices() + start;
    SVec3* normalSrc = vb->GetNormals() + start;
    SVec3* tangentSrc = vb->GetTangents() + start;
    SVec3* binormalSrc = vb->GetBiNormals() + start;
    SVec2* uvSrc = vb->GetTexCoords() + start;

    // get the stream target.
    SVec3* vertexDst = vb->GetVertices();
    SVec3* normalDst = vb->GetNormals();
    SVec3* tangentDst = vb->GetTangents();
    SVec3* binormalDst = vb->GetBiNormals();

    // simulate.
//  Simulate( time );

    // now update vertices.

    // Hard code params for now.  Once we've determined a good algorithm,
    // I'll make it a little more configurable.
    float scale1 = 0.1f;
    float scale2 = 0.05f;
    float scale3 = 0.2f;
    float scale4 = 0.075f;

    float freq1 = 2.0f;
    float freq2 = 4.0f;
    float freq3 = 1.0f;
    float freq4 = 2.666f;

    float speed1 = 0.543f;
    float speed2 = 1.05335f;
    float speed3 = 0.23f;
    float speed4 = 0.7f;

    unsigned int invSpeed1 = (unsigned int)( 1.0f / speed1 * 1000.0f );
    unsigned int invSpeed2 = (unsigned int)( 1.0f / speed2 * 1000.0f );
    unsigned int invSpeed3 = (unsigned int)( 1.0f / speed3 * 1000.0f );
    unsigned int invSpeed4 = (unsigned int)( 1.0f / speed4 * 1000.0f );

    // get the current rendering time from the renderer.
    float waveOffset1 = float( time % invSpeed1 ) / float( invSpeed1 ) * TWO_PI;
    float waveOffset2 = float( time % invSpeed2 ) / float( invSpeed2 ) * TWO_PI;
    float waveOffset3 = -float( time % invSpeed3 ) / float( invSpeed3 ) * TWO_PI;
    float waveOffset4 = -float( time % invSpeed4 ) / float( invSpeed4 ) * TWO_PI;

    // deform.
    float vertTheta;
    float vertSin;
    float vertCos;
    float tsbRotU;

//  GLhandleARB prevShader = bglGetHandleARB( GL_PROGRAM_OBJECT_ARB );
//  bglUseProgramObjectARB( 0 );
//  bglBegin( GL_LINES );
    for ( unsigned int i = 0; i < count; ++i )
    {
        vertTheta = freq1 * vertexSrc[ i ].x + waveOffset1;
//      vertTheta = freq1 * ( uvSrc[ i ].x + waveOffset1 ) * TWO_PI;
        vertSin = Sin( vertTheta );
        vertexDst[ i ] = vertexSrc[ i ] + scale1 * vertSin * normalSrc[ i ];

        tsbRotU = ATan( scale1 * Cos( vertTheta ) );
        vertSin = Sin( tsbRotU );
        vertCos = Cos( tsbRotU );
        normalDst[ i ] = vertCos * normalSrc[ i ] - vertSin * tangentSrc[ i ];
        tangentDst[ i ] = vertCos * tangentSrc[ i ] + vertSin * normalSrc[ i ];
/*
        vertTheta = freq2 * vertexSrc[ i ].z + waveOffset2;
        vertSin = Sin( vertTheta );
        vertexDst[ i ] = vertexDst[ i ] + scale2 * vertSin * normalSrc[ i ];

        tsbRotU = ATan( scale2 * Cos( vertTheta ) );
        vertSin = Sin( tsbRotU );
        vertCos = Cos( tsbRotU );
        normalDst[ i ] = vertCos * normalDst[ i ] - vertSin * binormalSrc[ i ];
        binormalDst[ i ] = vertCos * binormalSrc[ i ] - vertSin * normalDst[ i ];

        vertTheta = freq3 * vertexSrc[ i ].x + waveOffset3;
        vertSin = Sin( vertTheta );
        vertexDst[ i ] = vertexDst[ i ] + scale3 * vertSin * normalSrc[ i ];

        tsbRotU = ATan( scale3 * Cos( vertTheta ) );
        vertSin = Sin( tsbRotU );
        vertCos = Cos( tsbRotU );
        normalDst[ i ] = vertCos * normalDst[ i ] - vertSin * tangentDst[ i ];
        tangentDst[ i ] = vertCos * tangentDst[ i ] + vertSin * normalDst[ i ];

        vertTheta = freq4 * vertexSrc[ i ].z + waveOffset4;
        vertSin = Sin( vertTheta );
        vertexDst[ i ] = vertexDst[ i ] + scale4 * vertSin * normalSrc[ i ];

        tsbRotU = ATan( scale4 * Cos( vertTheta ) );
        vertSin = Sin( tsbRotU );
        vertCos = Cos( tsbRotU );
        normalDst[ i ] = vertCos * normalDst[ i ] - vertSin * binormalDst[ i ];
        binormalDst[ i ] = vertCos * binormalDst[ i ] - vertSin * normalDst[ i ];
*/
//      glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
//      glVertex3f( vertexDst[ i ].x, vertexDst[ i ].y, vertexDst[ i ].z );
//      glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
//      glVertex3f( vertexDst[ i ].x + normalDst[ i ].x, vertexDst[ i ].y + normalDst[ i ].y, vertexDst[ i ].z + normalDst[ i ].z );

//      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
//      glVertex3f( vertexDst[ i ].x, vertexDst[ i ].y, vertexDst[ i ].z );
//      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
//      glVertex3f( vertexDst[ i ].x + binormalDst[ i ].x, vertexDst[ i ].y + binormalDst[ i ].y, vertexDst[ i ].z + binormalDst[ i ].z );

//      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
//      glVertex3f( vertexDst[ i ].x, vertexDst[ i ].y, vertexDst[ i ].z );
//      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
//      glVertex3f( vertexDst[ i ].x + tangentDst[ i ].x, vertexDst[ i ].y + tangentDst[ i ].y, vertexDst[ i ].z + tangentDst[ i ].z );
    }
//  bglEnd();
//  bglUseProgramObjectARB( prevShader );

    // mark the updated destination components as dirty.
    vb->MarkAsDirty( GR_ATTRIB_POSITION_MASK | GR_ATTRIB_TANGENT_MASK | GR_ATTRIB_NORMAL_MASK | GR_ATTRIB_BINORMAL_MASK );
}
