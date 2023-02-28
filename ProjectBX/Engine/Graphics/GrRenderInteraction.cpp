//----------------------------------------------------------
// File:        GrRenderInteraction.cpp
// Author:      Kevin Bray
// Created:     11-14-05
// Copyright © 2004 Bootstrap Games
//----------------------------------------------------------
#include "engine_afx.h"

// class header.
#include "GrRenderInteraction.h"

GrRenderInteraction* gGrRenderInteraction = 0;

//**********************************************************
// class GrRenderInteraction
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrRenderInteraction::GrRenderInteraction()
: _inRender( false )
{
    B_ASSERT( gGrRenderInteraction == 0 );
    gGrRenderInteraction = this;
}

//----------------------------------------------------------
GrRenderInteraction::~GrRenderInteraction()
{
    gGrRenderInteraction = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrRenderInteraction::BeginRender()
{
    // make sure BeginRender() hasn't already been called!
    B_ASSERT( _inRender == false );
    _inRender = true;
}

//----------------------------------------------------------
void
GrRenderInteraction::EndRender()
{
    // make sure BeginRender() was called first!
    B_ASSERT( _inRender == true );
    _inRender = false;

    _camera = 0;
    _vertexProg = 0;
    _fragmentProg = 0;
    _light = 0;
    _pass = 0;
}

//----------------------------------------------------------
void
GrRenderInteraction::SetRenderMode( ERENDERMODE mode )
{
    
}

//----------------------------------------------------------
void
GrRenderInteraction::SetTransformMode( ETRANSFORMMODE mode )
{

}

//----------------------------------------------------------
void
GrRenderInteraction::SetCamera( const GrCamera* camera )
{

}

//----------------------------------------------------------
void
GrRenderInteraction::SetLight( const GrLight* light )
{

}

//----------------------------------------------------------
void
GrRenderInteraction::SetMaterialPass( GrMaterialPass* materialPass )
{

}

//----------------------------------------------------------
void
GrRenderInteraction::SetTransform( const MMat4x4& modelToWorld )
{

}

//----------------------------------------------------------
void
GrRenderInteraction::Render( const GrMesh* mesh, unsigned int range )
{

}
