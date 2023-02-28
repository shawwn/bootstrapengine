//----------------------------------------------------------
// File:        GrTexGen.cpp
// Author:      Kevin Bray
// Created:     07-17-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrTexGen.h"

// project includes.
#include "GrRenderTargetMgr.h"
#include "GrRenderTarget.h"
#include "GrRenderbuffer.h"
#include "GrFramebuffer.h"
#include "GrTextureBase.h"
#include "GrCamera.h"
#include "GrScene.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GLSubsys.h"
#include "GrMeshInst.h"
#include "GrMesh.h"

GrTexGen* gGrTexGen = 0;

//**********************************************************
// class GrTexGen
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrTexGen::GrTexGen( unsigned int lod )
: _inReflectGen( false )
, _lod( Clamp( lod, 0U, 6U ) )
{
    B_ASSERT( gGrTexGen == 0 );
    B_ASSERT( gGrRenderTargetMgr != 0 );
    gGrTexGen = this;

    // determine a valid format to use.
    GrTextureBase::EFORMAT format = GrTextureBase::EF_RGBA32; //gGrFramebuffer->IsHDR() ? GrTextureBase::EF_RGBA64F : GrTextureBase::EF_RGBA32;

    // get the white texture.
    _white = gGrTextureMgr->FindTexture( GR_SYSTEX_WHITE );
}

//----------------------------------------------------------
GrTexGen::~GrTexGen()
{
    gGrTexGen = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrTexGen::AddReflection( const GrMeshInst* meshInst, int range )
{
    B_ASSERT( meshInst != 0 );
    Add( _reflections, meshInst, range );
}

//----------------------------------------------------------
void
GrTexGen::RemoveReflection( const GrMeshInst* meshInst, int range )
{
    Remove( _reflections, meshInst, range );
}

//----------------------------------------------------------
void
GrTexGen::AddPortal( const GrMeshInst* meshInst, int range )
{
    B_ASSERT( meshInst != 0 );
    Add( _portals, meshInst, range );
}

//----------------------------------------------------------
void
GrTexGen::RemovePortal( const GrMeshInst* meshInst, int range )
{
    Remove( _portals, meshInst, range );
}

//----------------------------------------------------------
void
GrTexGen::GenTextures( const GrCamera* camera )
{
    // generate reflections.
    GenReflections( camera );
    GenPortals( camera );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrTexGen::GenReflections( const GrCamera* camera )
{
    B_ASSERT( gGrScene != 0 );
    B_ASSERT( !_inReflectGen );

    // this is so queries to gGrTexReflectGen->InReflectGen() will return true if called
    // during rendering the reflection.
    _inReflectGen = true;

    // switch the face winding orders.
    bglFrontFace( GL_CW );

    // store the current framebuffer and then make our framebuffer current.
    GrRenderTarget* prevTarget = GrRenderTarget::GetCurrentRT();
    B_ASSERT( prevTarget != 0 );

    // go through each reflection and find the most 'noticable' one.
    const unsigned int reflectionCount = _reflections.GetElemCount();
    unsigned int bestReflectiveObj = reflectionCount;
    MPlane reflectPlane;    // the best plane to render.
    MVec3 posOnPlane;
    float curDistSqr = FLT_MAX;
    for ( unsigned int i = 0; i < reflectionCount; ++i )
    {
        // get the reflection plane.
        const SRenderTexture& reflection = _reflections[ i ];
    
        // check to see if the current object is visible and if not, skip it.
        if ( !camera->GetFrustum().IsOBoxInside( reflection.meshInst->GetRangeOBox( reflection.range ) ) )
            continue;

        // get the current plane of reflection.  Skip if the camera is behind the plane.
        reflection.meshInst->GetRangePlane( reflectPlane, posOnPlane, reflection.range );
        posOnPlane += 0.05f * reflectPlane.GetNormal();
        if ( reflectPlane.Dist( camera->GetPos() ) < 0.0f )
            continue;

        // build the reflected camera and render.
        GrCamera reflectedCamera = camera->BuildReflectedCamera( reflectPlane );

        // get the material we're working with and apply white to it so we're
        // not both rendering from and to the reflection at the same time.
        GrMaterial* material = reflection.meshInst->GetMaterial( reflection.range );
        ApplyTexture( material, GrTextureStage::kGenReflection, _white );

        // bind the reflection render target and render the reflection.
        reflection.reflection->Bind();

        // render the texture.
        gGrScene->RenderTexture( &reflectedCamera, posOnPlane );

        // apply the newly created texture to the material.
        ApplyTexture( material, GrTextureStage::kGenReflection, reflection.reflection->GetColorTex( 0 ) );
    }

    // restore the face winding orders.
    bglFrontFace( GL_CCW );

    // bind the previous render target.
    prevTarget->Bind();

    // clear the _reflectGen flag as we're no longer generating the reflection.
    _inReflectGen = false;
}

//----------------------------------------------------------
void
GrTexGen::GenPortals( const GrCamera* camera )
{

}

//----------------------------------------------------------
void
GrTexGen::ApplyTexture( GrMaterial* material, GrTextureStage::GenTextureFlag flag, URef< GrTextureBase > texture )
{
    unsigned int passCount = material->GetPassCount();
    for ( unsigned int pass = 0; pass < passCount; ++pass )
    {
        GrMaterialPass* curPass = material->GetPass( pass );
        for ( unsigned int stage = 0; stage < ES_COUNT; ++stage )
        {
            // get the current stage and apply the texture if necessary.
            if ( curPass->GetStage( ( ESTAGE )stage ).GetGenTextureFlag() == flag )
                curPass->SetStageTexture( ( ESTAGE )stage, texture );
        }
    }

    passCount = material->GetPostProcessPassCount();
    for ( unsigned int pass = 0; pass < passCount; ++pass )
    {
        GrMaterialPass* curPass = material->GetPostProcessPass( pass );
        for ( unsigned int stage = 0; stage < ES_COUNT; ++stage )
        {
            // get the current stage and apply the texture if necessary.
            if ( curPass->GetStage( ( ESTAGE )stage ).GetGenTextureFlag() == flag )
                curPass->SetStageTexture( ( ESTAGE )stage, texture );
        }
    }
}

//----------------------------------------------------------
void
GrTexGen::Add( RenderTextureList& rtList, const GrMeshInst* meshInst, int range )
{
    B_ASSERT( meshInst != 0 );

    // check to make sure the reflection was not already added.
    const unsigned int count = rtList.GetElemCount();
    for ( unsigned int i = 0; i < count; ++i )
    {
        if ( rtList[ i ].meshInst == meshInst && rtList[ i ].range == range )
            return;
    }

    URef< GrRenderTarget > rt = BuildRenderTarget();

    // fill out a new reflection entry.
    SRenderTexture entry = 
    {
        rt,
        meshInst,
        range,
    };

    // add the new reflection.
    rtList.Push( entry );
}

//----------------------------------------------------------
void
GrTexGen::Remove( RenderTextureList& rtList, const GrMeshInst* meshInst, int range )
{
    const unsigned int count = rtList.GetElemCount();
    for ( unsigned int i = 0; i < count; ++i )
    {
        if ( rtList[ i ].meshInst == meshInst && rtList[ i ].range == range )
        {
            rtList.Erase( i );
            return;
        }
    }
}

//----------------------------------------------------------
URef< GrRenderTarget >
GrTexGen::BuildRenderTarget()
{
    // figure out what the render target settings should be.
//  unsigned int width = gShWnd->GetWidth() >> _lod;
//  unsigned int height = gShWnd->GetHeight() >> _lod;
    unsigned int width = 1024 >> _lod;
    unsigned int height = 1024 >> _lod;
    GrRenderTargetMgr::EFORMAT format = GrRenderTargetMgr::EF_RGBA32; //gGrFramebuffer->IsHDR() ? GrRenderTargetMgr::EF_RGBA64F : GrRenderTargetMgr::EF_RGBA32;

    // recreate the reflection render target.
    URef< GrTextureBase > reflectTexture = gGrRenderTargetMgr->CreateRendertexture( width, height, format,
        GrTextureBase::kNoMipMap, GrTextureBase::ETT_2D );
    URef< GrRenderbuffer > depthBuffer = gGrRenderTargetMgr->CreateRenderbuffer( width, height, GrRenderTargetMgr::EF_DEPTH );
    URef< GrRenderTarget > reflection = gGrRenderTargetMgr->CreateRenderTarget( reflectTexture, depthBuffer );

    // return the reflection render target.
    return reflection;
}
