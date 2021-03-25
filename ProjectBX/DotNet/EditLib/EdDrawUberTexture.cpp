//----------------------------------------------------------
// File:		EdDrawUberTexture.cpp
// Author:		Kevin Bray
// Created:		07-05-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdDrawUberTexture.h"

#if 0
// engine includes.
#include "GrRenderer.h"
#include "GrMeshInst.h"
#include "GrMesh.h"
#include "GrMaterialMgr.h"
#include "GrMaterial.h"
#include "GrMaterialPass.h"
#include "GrUberTexture.h"
#include "GrMeshVB.h"
#include "GrMeshIB.h"
#include "GrCamera.h"
#include "GrProjection.h"
#include "GrPixelBufferMgr.h"
#include "GrRenderTargetMgr.h"
#include "GrRenderTarget.h"
#include "GrShaderMgr.h"
#include "GrUberTextureMgr.h"
#include "MSphere.h"
#include "MAABox.h"

#define UBERMESH_COMPONENTS		( GR_VERTEX_POSITION | GR_VERTEX_NORMAL | GR_VERTEX_BINORMAL | GR_VERTEX_TANGENT | GR_VERTEX_TEXCOORD )
#define MAX_TEMP_SIZE			2048
#define UBERTEX_CACHE_SIZE		2048

// singleton pointer.
EdDrawUberTexture* gEdDrawUberTexture = 0;

//**********************************************************
// class EdDrawUberTexture
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdDrawUberTexture::EdDrawUberTexture()
: _renderAmbience( &_streamingIB, 0 )
, _renderUberTexture( 1024, 1024, 4 )
, _meshInst( 0 )
, _target( 0 )
, _targetDC( 0 )
, _targetRC( 0 )
, _targetWidth( 0 )
, _targetHeight( 0 )
, _primaryDC( 0 )
, _primaryRC( 0 )
{
	B_ASSERT( gEdDrawUberTexture == 0 );
	gEdDrawUberTexture = this;

	// create a material for the ubertexture.  Note that we keep the
	// material around so that if the engine were in a different
	// module, the deletion would happen in the same module as the
	// creation (EdLib).
	GrMaterial::SInfo matInfo;
	matInfo.castShadow = false;
	matInfo.passCount = 0;
	matInfo.passes = 0;
	matInfo.postProcessPassCount = 0;
	matInfo.postProcessPasses = 0;
	matInfo.receiveShadow = false;
	matInfo.sortType = GrMaterial::EST_OPAQUE;
	matInfo.stipple = false;
	_uberMaterial = new GrMaterial( "#ubertexture", matInfo );

	// create the material pass.
	SMaterialPassInfo passInfo;
	passInfo.enableState = 0;
	passInfo.writeMask = GR_RGBA;
	passInfo.textures.SetStageTexColorMod( ES_BUMP, GrColor( 0 ) );
	passInfo.textures.SetStageTexColorAdd( ES_BUMP, GrColor( 0 ) );
	_uberMaterial->AddPass( passInfo );

	// create the quad geometry.  Note that we keep the mesh around
	// so that if the engine were in a different module, the deletion
	// would happen in the same module as the creation (EdLib).
	GrMesh::SRange range;
	range.minIndex = 0;
	range.maxIndex = 3;
	range.start = 0;
	range.count = 6;
	range.bounds = MSphere( MVec3( 0.5f, 0.5f, 0.0f ), 0.707107f );
	range.aaBox = MAABox( MVec3( 0.5f, 0.5f, 0.0f ), MVec3( 0.5f, 0.5f, 0.0f ) );
	_mesh = new GrMesh( "#ubertexture_mesh", UBERMESH_COMPONENTS, 4, 6, &range, 1 );

	// fill out the mesh with a quad.
	URef< GrMeshVB > meshVB = _mesh->GetVertexData();
	SVec3* positions = meshVB->GetVertices();
	SVec3* normals = meshVB->GetNormals();
	SVec3* tangents = meshVB->GetTangents();
	SVec3* biNormals = meshVB->GetBiNormals();
	SVec2* texCoords = meshVB->GetTexCoords();

	// positions.
	positions[ 0 ] = SVec3( 0.0f, 0.0f, 0.0f );
	positions[ 1 ] = SVec3( 0.0f, 1.0f, 0.0f );
	positions[ 2 ] = SVec3( 1.0f, 1.0f, 0.0f );
	positions[ 3 ] = SVec3( 1.0f, 0.0f, 0.0f );

	// normals.
	normals[ 0 ] = SVec3( 0.0f, 0.0f, 1.0f );
	normals[ 1 ] = SVec3( 0.0f, 0.0f, 1.0f );
	normals[ 2 ] = SVec3( 0.0f, 0.0f, 1.0f );
	normals[ 3 ] = SVec3( 0.0f, 0.0f, 1.0f );

	// tangents.
	tangents[ 0 ] = SVec3( 1.0f, 0.0f, 0.0f );
	tangents[ 1 ] = SVec3( 1.0f, 0.0f, 0.0f );
	tangents[ 2 ] = SVec3( 1.0f, 0.0f, 0.0f );
	tangents[ 3 ] = SVec3( 1.0f, 0.0f, 0.0f );

	// biNormals.
	biNormals[ 0 ] = SVec3( 0.0f, 1.0f, 0.0f );
	biNormals[ 1 ] = SVec3( 0.0f, 1.0f, 0.0f );
	biNormals[ 2 ] = SVec3( 0.0f, 1.0f, 0.0f );
	biNormals[ 3 ] = SVec3( 0.0f, 1.0f, 0.0f );

	// texCoords.
	texCoords[ 0 ] = SVec2( 0.0f, 0.0f );
	texCoords[ 1 ] = SVec2( 0.0f, 1.0f );
	texCoords[ 2 ] = SVec2( 1.0f, 1.0f );
	texCoords[ 3 ] = SVec2( 1.0f, 0.0f );

	// mark everything as dirty.
	meshVB->MarkAsDirty( UBERMESH_COMPONENTS );

	// fill out the mesh indices.
	URef< GrMeshIB > meshIB = _mesh->GetIndexData();
	unsigned short* indices = meshIB->GetIndices();

	// indices.
	indices[ 0 ] = 0;
	indices[ 1 ] = 1;
	indices[ 2 ] = 2;
	indices[ 3 ] = 2;
	indices[ 4 ] = 3;
	indices[ 5 ] = 0;

	// mark indices as dirty.
	meshIB->MarkAsDirty();

	// create the quad mesh instance.
	_meshInst = new GrMeshInst( _mesh, &_uberMaterial, MMat4x4() );

	// now create the temporary store.
	URef< GrTextureBase > colorTarget = gGrRenderTargetMgr->CreateRendertexture( MAX_TEMP_SIZE, MAX_TEMP_SIZE, GrRenderTargetMgr::EF_RGBA32,
		GrTextureBase::kNoMipMap | GrTextureBase::kNoCompress, GrTextureBase::ETT_2D );
	_tempTarget = gGrRenderTargetMgr->CreateRenderTarget( colorTarget );
}

//----------------------------------------------------------
EdDrawUberTexture::~EdDrawUberTexture()
{
	// free any target information.
	if ( _target != 0 )
	{
		ReleaseDC( _target, _targetDC );
		wglDeleteContext( _targetRC );
	}

	// clean up after ourselves.
	delete _meshInst;

	gEdDrawUberTexture = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
bool
EdDrawUberTexture::SetTarget( HWND window )
{
	// simply return if the call is redundant.
	if ( window == _target )
		return true;

	// clear any existing target information.
	if ( _target != 0 )
	{
		ReleaseDC( _target, _targetDC );
		wglDeleteContext( _targetRC );
	}

	// setup the pixel format.
	static const PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),    
		1,						/* version */
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,       /* support double-buffering */
		PFD_TYPE_RGBA,          /* framebuffer color type */
		24,						/* preferred color depth */
		8, 0, 8, 0, 8, 0,       /* color bits (ignored) */
		8,						/* no alpha buffer */
		0,						/* alpha bits (ignored) */
		0,						/* no accumulation buffer */
		0, 0, 0, 0,				/* accum bits (ignored) */
		0,						/* depth buffer */
		0,						/* no stencil buffer */
		0,						/* no auxiliary buffers */
		PFD_MAIN_PLANE,         /* main layer */
		0,						/* reserved */
		0, 0, 0,				/* no layer, visible, damage masks */
	};

	// get the device context.
	_targetDC = GetDC( window );
	if( !_targetDC )
		return false;

	// select the pixel format.
	int pixelFormat = ChoosePixelFormat( _targetDC, &pfd );
	if( pixelFormat == 0 )
	{
		ReleaseDC( window, _targetDC );
		return false;
	}

	// set the pixel format to the device context.
	if( !SetPixelFormat( _targetDC, pixelFormat, &pfd ) )
	{
		ReleaseDC( window, _targetDC );
		return false;
	}

	// create the rendering context.
	_targetRC = wglCreateContext( _targetDC );
	if( !_targetRC )
	{
		ReleaseDC( window, _targetDC );
		return false;
	}

	// try to get the main rendering context.
	_primaryRC = wglGetCurrentContext();
	if ( !_primaryRC )
	{
		ReleaseDC( window, _targetDC );
		return false;
	}

	// get the main rendering context's device context.
	_primaryDC = wglGetCurrentDC();
	if ( !_primaryDC )
	{
		ReleaseDC( window, _targetDC );
	}

	// share resources from the primary render context.
	if ( !wglShareLists( _primaryRC, _targetRC ) )
	{
		DWORD error = GetLastError();
		ReleaseDC( window, _targetDC );
		return false;
	}

	// store the target window.
	_target = window;

	// get information about the target.
	UpdateTargetSize();

	// return true to indicate success.
	return true;
}

//----------------------------------------------------------
void
EdDrawUberTexture::Render( GrUberTexture* uberTexture, float startU, float startV, float endU, float endV )
{
	// determine the size of the viewport.
	UpdateTargetSize();

	// bind the render target.
	GrRenderTarget* prevTarget = GrRenderTarget::GetCurrentRT();
	URect vpRect( _targetWidth, _targetHeight );
	_tempTarget->SetViewportRect( vpRect );
	_tempTarget->Bind();

	// clear the render target to green.
	GrClear( 0.0f, 1.0f, 0.0f, 0.0f );

	// set the ubertexture to the material.
	GrMaterialPass* pass = _uberMaterial->GetPass( 0 );
	pass->SetUberTexture( uberTexture );

	// create a camera from the UV coordinates specified.
	float width = endU - startU;
	float height = endV - startV;
	GrProjection proj( 0.0f, width, 0.0f, height, 10.0f );
	GrCamera camera( MVec3( startU, startV, 1.0f ), MMat3x3(), proj );

	// add our mesh to the render list.
	_renderList.AddMeshInst( _meshInst, false ); 

	// render the quad with the ubertexture on it.
	_renderUberTexture.BuildAtlas( _renderList, camera, 0 );
	_renderAmbience.RenderAmbience( _postRenderList, _renderList, camera, GrColor( 1.0f ), 0, EGQ_ULTRA );
/*
	gGrRenderer->BeginRender();
	gGrRenderer->RenderAmbience( _renderList, camera, GrColor( 1.0f ) );
	gGrRenderer->EndRender( camera );
*/
	// all done with the render lists, so clear them.
	_renderList.Clear();
	_postRenderList.Clear();

	// clear the ubertexture from the material.
	pass->SetUberTexture( 0 );

	// restore the previous render target binding.
	prevTarget->Bind();

	// copy over the rendered data if necessary.
	if ( _target )
	{
		// get the handle of the rendered texture.
		URef< GrTextureBase > colorTex = _tempTarget->GetColorTex( 0 );
#if 1
		GrTexture* dbgTex = ( GrTexture* )( GrTextureBase* )colorTex;
		bool debugSave = false;
		if ( debugSave )
			dbgTex->DebugSave( "DrawUberTemp.tga" );
#endif
		unsigned int texHandle = colorTex->GetGLHandle();

		// calculate the UVs.
		float maxU = ( _targetWidth / ( float )MAX_TEMP_SIZE );
		float maxV = ( _targetHeight / ( float )MAX_TEMP_SIZE );

		// now copy from the temporary buffer over to the target.
		wglMakeCurrent( _targetDC, _targetRC );

		// setup the viewport.
		bglViewport( 0, 0, vpRect.GetWidth(), vpRect.GetHeight() );

		// clear the target.
		bglClearColor( 1.0f, 0.0f, 0.0f, 0.0f );
		bglClear( GL_COLOR_BUFFER_BIT );

		// load identity matrices so that everything is already in post
		// projection space.
		bglMatrixMode( GL_PROJECTION );
		bglLoadIdentity();
		bglMatrixMode( GL_MODELVIEW );
		bglLoadIdentity();

		// configure render states.
		bglDisable( GL_DEPTH_TEST );
		bglDisable( GL_CULL_FACE );
		bglDisable( GL_BLEND );
		bglDisable( GL_ALPHA_TEST );

		// make sure no shaders are bound.
		bglUseProgramObjectARB( 0 ); 

		// setup the texture environment.
		bglActiveTexture( 0 );
		bglBindTexture( GL_TEXTURE_2D, texHandle );
		bglEnable( GL_TEXTURE_2D );
		bglTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		// make sure no additional texture stages are enabled.
		bglActiveTexture( GL_TEXTURE1 );
		bglDisable( GL_TEXTURE_2D );

		// do the copy here.
		bglBegin( GL_QUADS );
		{
			bglTexCoord2f( 0.0f, 0.0f );
			bglColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
			bglVertex3f( -1.0f, -1.0f, 0.0f );

			bglTexCoord2f( 0.0f, maxV );
			bglColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
			bglVertex3f( -1.0f,  1.0f, 0.0f );

			bglTexCoord2f( maxU, maxV );
			bglColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
			bglVertex3f(  1.0f,  1.0f, 0.0f );

			bglTexCoord2f( maxU, 0.0f );
			bglColor4f( 1.0f, 0.0f, 0.0f, 0.0f );
			bglVertex3f(  1.0f, -1.0f, 0.0f );
		}
		bglEnd();

		GLenum error = bglGetError();
		switch ( error )
		{
		case GL_INVALID_ENUM:
			PrintF( "OpenGL Error: Invalid enumeration specified!" );
			break;
		case GL_INVALID_VALUE:
			PrintF( "OpenGL Error: Invalid value specified!" );
			break;
		case GL_INVALID_OPERATION:
			PrintF( "OpenGL Error: Invalid operation!" );
			break;
		default:
			break;
		};
		B_ASSERT( error == GL_NO_ERROR );

		// present the rendered data.
		SwapBuffers( _targetDC );

		// restore the previous rendering context.
		wglMakeCurrent( _primaryDC, _primaryRC );
	}

	// update the ubertexture manager and pretend that we're running at
	// 100 fps.
	gGrUberTextureMgr->Update( 10 );
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdDrawUberTexture::UpdateTargetSize()
{
	// get the client rectangle.
	RECT rect;
	if ( !GetClientRect( _target, &rect ) )
		return;

	// check to see if we need to update our ubertexture renderer.
	unsigned int newTargetWidth = Min( ( int )rect.right, MAX_TEMP_SIZE );
	unsigned int newTargetHeight = Min( ( int )rect.bottom, MAX_TEMP_SIZE );
	if ( newTargetWidth != _targetWidth || newTargetHeight != _targetHeight )
	{
		// free rendertargets and such.  Make sure that the render target manager
		// performs garbage collection.
		_renderUberTexture.PreContextReset();
		gGrRenderTargetMgr->CollectGarbage();
		gGrPixelBufferMgr->CollectGarbage();

		// force recreation of the render ubertexture internal buffers.
		_renderUberTexture.PostContextReset( newTargetWidth, newTargetHeight );
		_targetWidth = newTargetWidth;
		_targetHeight = newTargetHeight;
	}
}
#endif
