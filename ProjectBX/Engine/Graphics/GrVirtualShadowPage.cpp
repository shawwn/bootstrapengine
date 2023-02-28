//----------------------------------------------------------
// File:		GrVirtualShadowPage.cpp
// Author:		Kevin Bray
// Created:		04-12-07
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrVirtualShadowPage.h"

// project includes.
#include "GrVirtualShadowSegmentSet.h"
#include "GrRenderTarget.h"
#include "GrRenderTargetMgr.h"
#include "GrRenderbuffer.h"
#include "GrTextureBase.h"
#include "GrTextureMgr.h"
#include "GrShaderMgr.h"
#include "GrShader.h"
#include "GrLight.h"
#include "GrUtil.h"
#include "GrImage.h"
#include "GrDebug.h"
#include "GLSubsys.h"
#include "MAABox.h"

#define INDIR_MOD_ADD_HANDLE		0

// size of each shadow map border in pixels.
#define SHADOW_BORDER_SIZE			64

//**********************************************************
// class GrVirtualShadowPage
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrVirtualShadowPage::GrVirtualShadowPage( unsigned int shadowPageSize, unsigned int shadowMapSize, unsigned int indirTexSize )
: _shadowMapCount( Sqr( shadowPageSize / shadowMapSize ) )
, _texAddrStep( 1.0f / float( shadowPageSize / shadowMapSize ) )
, _shadowPageSize( shadowPageSize )
, _shadowMapSize( shadowMapSize )
, _shadowMapBorder( SHADOW_BORDER_SIZE )
, _prevRT( 0 )
{
	// make sure the shadow page and the shadow map sizes are a power of two.  This
	// is required because of hardware and floating point limitations.
	B_ASSERT( IsPow2( shadowPageSize ) );
	B_ASSERT( IsPow2( shadowMapSize ) );
	B_ASSERT( IsPow2( indirTexSize ) );

	// allocate indirection buffers.
	URef< GrRenderbuffer > indirDepth = gGrRenderTargetMgr->CreateRenderbuffer( indirTexSize, indirTexSize, GrRenderTargetMgr::EF_DEPTH );
	URef< GrTextureBase > indirCube = gGrRenderTargetMgr->CreateRendertexture( indirTexSize, indirTexSize, GrRenderTargetMgr::EF_RGBA128F,
		GrTextureBase::kNoMipMap, GrTextureBase::ETT_CUBE );

	// allocate indirection render targets.
	_shadowIndirCube = gGrRenderTargetMgr->CreateRenderTarget( indirCube, indirDepth );

	// create the shadow indirection shader.
	_shadowIndirShader = gGrShaderMgr->GetShader( "VSM/indir" );
	_shadowIndirShader->InitUserParam( "u_IndirModAdd", INDIR_MOD_ADD_HANDLE );

	// fill out the shadow data structure.
	_shadowData.indirCube = _shadowIndirCube->GetColorTex( 0 );

	// fill out the no shadow data structure.
	// create the default indirection texture cube.
	GrImage* images[ 6 ];
	char imageName[] = "indir_cube_face_0";
	size_t t = strlen( imageName )-1;
	for ( unsigned int i = 0; i < 6; ++i )
	{
		imageName[ t ] = '0' + i;
		images[ i ] = new GrImage( imageName, 0x7F7F7F7F );
	}
	_noShadowData.indirCube = gGrTextureMgr->GetTextureCubeImages( images, 1, GrTextureBase::EF_RGBA32,
		GrTextureBase::kNoMipMap | GrTextureBase::kNoCompress );

	// build the texgen bias matrix.
	_texGenBias = MMat4x4( 0.5f, 0.0f, 0.0f, 0.5f,
						   0.0f, 0.5f, 0.0f, 0.5f,
						   0.0f, 0.0f, 1.0f, 0.0f,
						   0.0f, 0.0f, 0.0f, 1.0f );
}

//----------------------------------------------------------
GrVirtualShadowPage::~GrVirtualShadowPage()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrVirtualShadowPage::GenShadowInfo( std::vector< SShadowInfo >& shadows, const GrCamera& viewerCamera,
									GrVirtualShadowSegmentSet& segments, const GrLight& light )
{
	const float border = float( SHADOW_BORDER_SIZE ) / _shadowIndirCube->GetWidth();
	const float invIndirCubeWidth = 1.0f / ( float )_shadowIndirCube->GetWidth();

	// clear current rendering state.
	GrSetState( 0 );

	// bind the indirection render target.
	_shadowIndirCube->Bind();

	// determine what kind of light we're dealing with.
	if ( light.IsParallel() || ( light.GetProjection().IsOrtho() && light.GetProjTexture() ) )
	{
		// parallel light.
		// set the active face on the indirection cube to -Z
		_shadowIndirCube->SetActiveFace( GrTextureCube::CF_NEGATIVEZ );

		// clear the current face.
		GrSetWriteEnable( GR_RGBA );
		GrClear( 1.0f, 1.0f, 1.0f );

		// calculate the indirection camera.
		GrCamera indirCamera;
		CalcProjCamera( indirCamera, light );

		// update the segments with the indirection camera.
		segments.UpdateSegments( indirCamera, border, 0.0f );

		// allocate the shadow maps for the current viewer camera.
		AllocateShadowMaps( shadows, segments, indirCamera, 0, 16 );

		// create a camera that represents the transform from world space
		// to cube space -Z.
		GrCamera faceCamera( light.GetPos(), light.GetWorldTransform().GetRotate(), light.GetProjection() );

		// note that we need to create a special projection matrix that
		// assumes divide by Z.  This matrix must assume an unspecified
		// near clipping plane.
		float width = ( light.GetProjection().GetRight() - light.GetProjection().GetLeft() );
		float height = ( light.GetProjection().GetTop() - light.GetProjection().GetBottom() );
		float maxDim = Max( width, height );
		float invMaxDim = 1.0f / maxDim;
		MMat4x4 faceProjMat = faceCamera.GetProj().GetMatrix();

		// select the -Z cube face.  Note that this has to be undone when
		// looking up data from the shadow.
		faceProjMat( 0, 0 ) = 1.0f;
		faceProjMat( 1, 1 ) = 1.0f;
		faceProjMat( 2, 2 ) = 0.0f;
		faceProjMat( 2, 3 ) = -1.0f;

		// set the W column.  This gets used for the Z compare term.
		faceProjMat( 3, 2 ) = -1.0f / light.GetCachedRange();
		faceProjMat( 3, 3 ) = 0.0f;

		// scale the X and Y, leaving Z untouched so that we select the
		// -Z face with the above projection matrix.
		MMat4x4 scaleMat( 2.0f / width, 0.0f, 0.0f, 0.0f,
						  0.0f, 2.0f / height, 0.0f, 0.0f,
						  0.0f, 0.0f, 1.0f, 0.0f,
						  0.0f, 0.0f, 0.0f, 1.0f );

		// set the shadow data matrix for the cube indirection texture lookup.
		_shadowData.shadowMatrix = faceProjMat * scaleMat * faceCamera.GetViewMatrix();
	}
	else if ( light.GetProjTexture() != 0 )
	{
#if 0
		// remove the orientation, but keep the translation.
		GrLight* nonConstLight = ( GrLight* )&light;
		nonConstLight->SetTransform( MMat4x4( nonConstLight->GetWorldTransform().GetTranslate() ) );
#endif

		// set the active face on the indirection cube to -Z
		_shadowIndirCube->SetActiveFace( GrTextureCube::CF_NEGATIVEZ );

		// clear the current face.
		GrSetWriteEnable( GR_RGBA );
		GrClear( 1.0f, 1.0f, 1.0f );

		// calculate the indirection camera.
		GrCamera indirCamera;
		CalcProjCamera( indirCamera, light );

		// update the segments with the indirection camera.
		float distBias = 0.0f;
		if ( !indirCamera.GetProj().IsOrtho() )
			distBias = 2.0f * invIndirCubeWidth;
		segments.UpdateSegments( indirCamera, border, distBias );

		// allocate the shadow maps for the current viewer camera.
		AllocateShadowMaps( shadows, segments, indirCamera, 0, 16 );

		// create a camera that represents the transform from world space
		// to cube space -Z.
		GrCamera cubeCamera( light.GetPos(), light.GetWorldTransform().GetRotate(), light.GetProjection() );

		// note that we need to create a special projection matrix that
		// assumes divide by Z.  This matrix must assume an unspecified
		// near clipping plane.
		MMat4x4 faceProjMat = cubeCamera.GetProj().GetMatrix();
		faceProjMat( 2, 2 ) *= -1.0f;
		faceProjMat( 2, 3 ) = 0.0f;
		faceProjMat( 3, 2 ) = 0.0f;
		faceProjMat( 3, 3 ) = 1.0f;

		// set the shadow data matrix for the cube indirection texture lookup.
		_shadowData.shadowMatrix = faceProjMat * cubeCamera.GetViewMatrix();
	}
	else
	{
		// current shadow map index.
		unsigned int curShadowMapIdx = 0;

		// calculate the cameras for a cube map.
		GrCamera indirCameras[ 6 ];
		CalcLightCubeCameras( indirCameras, light );

		// get ordered faces.
		GrTextureCube::ECUBEFACE faceLut[ 6 ];
		GetOrderedFaces( faceLut, viewerCamera, light );

		// scaling bias (constant bias).
		float distBias = 2.0f * invIndirCubeWidth;

		// point light.
		// go through each cube face.
		for ( unsigned int f = 0; f < 6; ++f )
		{
			// get the current face.
			GrTextureCube::ECUBEFACE face = faceLut[ f ];

			// get the current indirection camera and update segment information.
			const GrCamera& curIndirCamera = indirCameras[ face ];
			segments.UpdateSegments( curIndirCamera, border, distBias );

			// check to see if any segments are visible.
			unsigned int segment = 0;
			for ( ; segment < segments.GetSegmentCount(); ++segment )
				if ( segments.IsSegmentVisible( segment ) )
					break;

			// skip the current face if no segments are visible.
			if ( segment == segments.GetSegmentCount() )
				continue;

			// set the indirection cube render target.
			_shadowIndirCube->SetActiveFace( face );

			// clear the current face.
			GrSetWriteEnable( GR_RGBA );
			GrClear();

			// determine how many shadow maps we can have.  Note that there is a special
			// case here that allocates only one shadow map if:
			//  1.) the viewer camera and the indirection camera are facing the same
			//		direction.
			//	2.) the indir camera's origin is inside of the viewer frustum.
			unsigned int maxShadowIdx = _shadowMapCount - 5 + f;
			if ( viewerCamera.GetLookDir().Dot( curIndirCamera.GetLookDir() ) > 0.0f &&
				 viewerCamera.GetFrustum().IsPointInside( curIndirCamera.GetPos() ) )
				maxShadowIdx = curShadowMapIdx + 1;

			// allocate the shadow maps for each segment.
			curShadowMapIdx = AllocateShadowMaps( shadows, segments, curIndirCamera, curShadowMapIdx, maxShadowIdx );
		}

		// set the shadow data matrix for the cube indirection texture lookup.
		_shadowData.shadowMatrix = MMat4x4( -light.GetPos() );
	}
}

//----------------------------------------------------------
unsigned int
GrVirtualShadowPage::AllocateShadowMaps( std::vector< SShadowInfo >& shadows, const GrVirtualShadowSegmentSet& segments,
										 const GrCamera& indirCamera, unsigned int startShadowMapIdx, unsigned int maxShadowIdx )
{
	// make sure there are shadows we can use.  Otherwise, what's the
	// point?
	B_ASSERT( startShadowMapIdx < _shadowMapCount );

	// render each segment for the current face camera.
	unsigned int segmentCount = GetLastSegment( segments );
	unsigned int s = segmentCount;
	while ( s-- > 0 )
	{
		// if the segment isn't visible, then skip the current face.
		if ( !segments.IsSegmentVisible( s ) )
			continue;

		// Now allocate the shadow map.
		shadows.push_back( SShadowInfo() );
		AllocateShadowMap( shadows.back(), segments, s, indirCamera, startShadowMapIdx );
		++startShadowMapIdx;

		// if we've exhausted all shadow map segments allocated to us,
		// then break.  This ensures that there will always be enough
		// shadow maps so that things will at least work.
		if ( startShadowMapIdx == maxShadowIdx )
			break;
	}

	// return the current shadow map we're on.
	return startShadowMapIdx;
}

//----------------------------------------------------------
void
GrVirtualShadowPage::SetActiveShadowMap( const SShadowInfo& shadowMapInfo )
{
	// set the current viewport.
	unsigned int shadowMapIdx = shadowMapInfo.shadowMapIdx;
	unsigned int shadowsAcross = _shadowPageSize / _shadowMapSize;
	int x = shadowMapIdx % shadowsAcross * _shadowMapSize;
	int y = shadowMapIdx / shadowsAcross * _shadowMapSize;
	GrViewport( x, y, shadowMapInfo.width, shadowMapInfo.height );
}

//----------------------------------------------------------
const GrVirtualShadowPage::SRenderData&
GrVirtualShadowPage::Finish()
{
#if 1
	// save the current face if desired.
	bool saveCurFace = false;
	if ( saveCurFace )
	{
		char imageName[] = "shadowmap.tga";

		unsigned int dim = _shadowPageSize;
		unsigned char* pels = new unsigned char[ dim * dim ];
		bglReadPixels( 0, 0, dim, dim, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, pels );
		GrSaveTGA( imageName, pels, dim, dim, 1 );
		saveCurFace = false;
	}
#endif

/*
	// make sure there was a previous render target.
	B_ASSERT( _prevRT );

	// bind the previous render target and clear the _prevRT.
	_prevRT->Bind();
	_prevRT = 0;
*/
	// return the shadow data.
	return _shadowData;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrVirtualShadowPage::GetOrderedFaces( GrTextureCube::ECUBEFACE* faceLut, const GrCamera& viewerCamera, const GrLight& light )
{
#if 1
	MVec3 shadowEyePos = viewerCamera.GetPos() - light.GetPos();

	// sort X Y and Z based on their absolute values.
	MVec3 absShadowEyePos( Abs( shadowEyePos.GetX() ), Abs( shadowEyePos.GetY() ), Abs( shadowEyePos.GetZ() ) );
	int xy = absShadowEyePos.GetX() > absShadowEyePos.GetY() ? 0 : 1;
	int xz = absShadowEyePos.GetX() > absShadowEyePos.GetZ() ? 0 : 2;
	int yz = absShadowEyePos.GetY() > absShadowEyePos.GetZ() ? 1 : 2;

	int first = ( xy == 0 ? xz : yz );
	int second = ( xy == 0 ? ( xz ^ 2 ) : ( yz ^ 3 ) );
	int third = 3 - first - second;

	faceLut[ 0 ] = ( GrTextureCube::ECUBEFACE )( 2 * first  + ( shadowEyePos[ first  ] < 0 ? 1 : 0 ) );
	faceLut[ 1 ] = ( GrTextureCube::ECUBEFACE )( 2 * second + ( shadowEyePos[ second ] < 0 ? 1 : 0 ) );
	faceLut[ 2 ] = ( GrTextureCube::ECUBEFACE )( 2 * third  + ( shadowEyePos[ third  ] < 0 ? 1 : 0 ) );
	faceLut[ 3 ] = ( GrTextureCube::ECUBEFACE )( 2 * third  + ( shadowEyePos[ third  ] < 0 ? 0 : 1 ) );
	faceLut[ 4 ] = ( GrTextureCube::ECUBEFACE )( 2 * second + ( shadowEyePos[ second ] < 0 ? 0 : 1 ) );
	faceLut[ 5 ] = ( GrTextureCube::ECUBEFACE )( 2 * first  + ( shadowEyePos[ first  ] < 0 ? 0 : 1 ) );
#else
	faceLut[ 0 ] = ( GrTextureCube::ECUBEFACE )0;
	faceLut[ 1 ] = ( GrTextureCube::ECUBEFACE )1;
	faceLut[ 2 ] = ( GrTextureCube::ECUBEFACE )2;
	faceLut[ 3 ] = ( GrTextureCube::ECUBEFACE )3;
	faceLut[ 4 ] = ( GrTextureCube::ECUBEFACE )4;
	faceLut[ 5 ] = ( GrTextureCube::ECUBEFACE )5;
#endif
}

//----------------------------------------------------------
unsigned int
GrVirtualShadowPage::GetLastSegment( const GrVirtualShadowSegmentSet& segments )
{
	// find the first segment that's *not* 0..1 for the current face.
	unsigned int count = 0;
	unsigned int totalSegmentCount = segments.GetSegmentCount();
	for ( ; count < totalSegmentCount; ++count )
	{
		// check to see if the segment is visible.
		if ( segments.IsSegmentVisible( count ) )
		{
			// get the current AA box.
			const GrVirtualShadowSegmentSet::SAABox2D& curAABox = segments.GetSegmentAABox( count );

			// check to see if the AA box covers the indirection view.
			if ( curAABox.lowerLeft.x <= -1.0f && curAABox.lowerLeft.y <= -1.0f &&
				 curAABox.upperRight.x >= 1.0f && curAABox.upperRight.y >= 1.0f )
			{
				// we found the first segment that covers the entire indirection
				// face.  Therefore, we should advance just past it.  This will let
				// us skip segments in some cases.
				return ++count;
			}
		}
	}

	// if we got here, then all segments need to be considered.
	return count;
}

//----------------------------------------------------------
void
GrVirtualShadowPage::AllocateShadowMap( SShadowInfo& shadowInfo, const GrVirtualShadowSegmentSet& segments,
									    unsigned int segment, const GrCamera& indirCamera, unsigned int shadowMapIndex )
{
	// we ran out of shadow maps!  In this case, we'll get a low res shadow.
	if ( shadowMapIndex == _shadowMapCount )
		return;

	// get the segment's AA box.
	GrVirtualShadowSegmentSet::SAABox2D aaBox = segments.GetSegmentAABox( segment );

	// calculate the shadow's width and height.
	shadowInfo.width = _shadowMapSize >> Min( aaBox.clipLogX, 2 );
	shadowInfo.height = _shadowMapSize >> Min( aaBox.clipLogY, 2 );

	// build an off center projection for the camera.  Note that the Z
	// distribution has to be the same as the shadowCamera's.
	shadowInfo.camera.SetPos( indirCamera.GetPos() );
	shadowInfo.camera.SetRot( indirCamera.GetRot() );

	// set the shadow map index.
	shadowInfo.shadowMapIdx = shadowMapIndex;

	// get the width and height of the current view.
	float width = ( aaBox.upperRight.x - aaBox.lowerLeft.x );
	float height = ( aaBox.upperRight.y - aaBox.lowerLeft.y );

	// account for the border by extending the projection out a little bit.
	float borderWidth = float( SHADOW_BORDER_SIZE ) / float( shadowInfo.width - 2 * SHADOW_BORDER_SIZE );
	float borderHeight = float( SHADOW_BORDER_SIZE ) / float( shadowInfo.height - 2 * SHADOW_BORDER_SIZE );

	// get the indirection camera's projection so that we can create a
	// projection for th ecurrent shadow.
	const GrProjection& indirProj = indirCamera.GetProj();

	// convert the aaBox into post projection space.
	float leftT = 0.5f * aaBox.lowerLeft.x + 0.5f;
	float rightT = 0.5f * aaBox.upperRight.x + 0.5f;
	float bottomT = 0.5f * aaBox.lowerLeft.y + 0.5f;
	float topT = 0.5f * aaBox.upperRight.y + 0.5f;

	// calculate the shadow camera's left and right as a subset of the
	// indirection camera.
	float projLeft = Lerp( indirProj.GetLeft(), indirProj.GetRight(), leftT );
	float projRight = Lerp( indirProj.GetLeft(), indirProj.GetRight(), rightT );
	float projBottom = Lerp( indirProj.GetBottom(), indirProj.GetTop(), bottomT );
	float projTop = Lerp( indirProj.GetBottom(), indirProj.GetTop(), topT );

	// scale the border width to account for scale.
	borderWidth *= ( projRight - projLeft );
	borderHeight *= ( projTop - projBottom );

	// build the projection for the current shadow map.
	GrProjection proj( indirProj );
	proj.SetLeft( projLeft - borderWidth );
	proj.SetRight( projRight + borderWidth );
	proj.SetBottom( projBottom - borderHeight );
	proj.SetTop( projTop + borderHeight );

	// set the modified projection to the shadow map camera.
	shadowInfo.camera.SetProj( proj );

	// get the frustum so we can determine an appropriate shadow map size.
	shadowInfo.camera.GetFrustum();

	// what about off center projections?
	// adjust the AA box to account for the projection's scale.
#if 0
	float boxScaleX = 1.0f;
	float boxScaleY = 1.0f;
	if ( proj.IsOrtho() )
	{
		// here's where we "undo" the non-perspective shadow cube face
		// lookup.
		float projWidthScale = indirProj.GetRight() - indirProj.GetLeft();
		float projHeightScale = indirProj.GetTop() - indirProj.GetBottom();
		float invMaxScale = 1.0f / Max( projWidthScale, projHeightScale );

		// set the scale.
		boxScaleX = invMaxScale;
		boxScaleY = invMaxScale;
/*
		// apply the scale.
		aaBox.lowerLeft.x *= invMaxScale;
		aaBox.upperRight.x *= invMaxScale;
		aaBox.lowerLeft.y *= invMaxScale;
		aaBox.upperRight.y *= invMaxScale;
*/
	}
#endif
	// render segment info.
	RenderSegmentInfo( aaBox, shadowMapIndex, shadowInfo.width, shadowInfo.height );
}

//----------------------------------------------------------
void
GrVirtualShadowPage::RenderSegmentInfo( const GrVirtualShadowSegmentSet::SAABox2D& aaBox, unsigned int shadowMapIdx,
									    unsigned int width, unsigned int height )
{
	// if we got here, then we know the aa box should be rendered.
	SVec2 offsetMod;
	SVec2 offsetAdd;
	CalcShadowModAdd( offsetMod, offsetAdd, shadowMapIdx, width, height );

	// calculate mod/add for the current camera.  The idea here is
	// that the shader can do:
	//	modAdd = texCube( vsm, lightVec );
	//	xy / MA... note we have to select xy in the shader somehow.  LUT?
	//	shadowUV = modAdd.xy * xy + modAdd.zw;
	//	tex2D( shadowMap, shadowUV );

	float scaleX = 1.0f / ( aaBox.upperRight.x - aaBox.lowerLeft.x );
	float scaleY = 1.0f / ( aaBox.upperRight.y - aaBox.lowerLeft.y );

	// if we're looking down -Z, we get:
	//
	//  mod * ( ( shadowLookup.xy - bottomLeft.xy ) / widthAndHeight ) + offset.xy
	//	mod * shadowLookup.xy / widthAndHeight - mod * bottomLeft.xy / widthAndHeight + offset.xy
	//
	// or:
	//
	//	mod * ( 1.0 - ( ( shadowLookup.xy - bottomLeft.xy ) / widthAndHeight ) ) + offset.xy
	//	mod * 1.0 - mod * shadowLookup.xy / widthAndHeight + mod * bottomLeft.xy / widthAndHeight + offset.xy
	//	mod * shadowLookup.xy / widthAndHeight - mod * bottomLeft.xy / widthAndHeight + offset.xy
	//
	float modAdd[ 4 ] = {
		offsetMod.x * scaleX,
		offsetMod.y * scaleY,
		offsetAdd.x - offsetMod.x * scaleX * aaBox.lowerLeft.x,
		offsetAdd.y - offsetMod.y * scaleY * aaBox.lowerLeft.y,
	};

	// bind the shader.
	_shadowIndirShader->Bind( EGQ_LOW );
	_shadowIndirShader->SetUserParam4fv( INDIR_MOD_ADD_HANDLE, modAdd );

	// set up render states.
	GrBindVB( 0 );
	GrBindIB( 0 );
	bglMatrixMode( GL_MODELVIEW );
	bglLoadIdentity();
	bglMatrixMode( GL_PROJECTION );
	bglLoadIdentity();
	GrSetState( 0 );

	// get the coordinates of the box.
	float left = aaBox.lowerLeft.x;
	float right = aaBox.upperRight.x;
	float bottom = aaBox.lowerLeft.y;
	float top = aaBox.upperRight.y;

	// now render the geometry.
	bglBegin( GL_TRIANGLE_STRIP );
		bglTexCoord2f( 0.0f, 1.0f );
		bglVertex3f( left, top, 0.0f );

		bglTexCoord2f( 1.0f, 1.0f );
		bglVertex3f( right, top, 0.0f );

		bglTexCoord2f( 0.0f, 0.0f );
		bglVertex3f( left, bottom, 0.0f );

		bglTexCoord2f( 1.0f, 0.0f );
		bglVertex3f( right, bottom, 0.0f );
	bglEnd();
}

//----------------------------------------------------------
void
GrVirtualShadowPage::CalcLightCubeCameras( GrCamera* cameras, const GrLight& light )
{
	// generate view volumes for each of the 6 light faces.
	float range = light.GetCachedRange();
	const float fov = DegToRad( 90.0f );
	const MVec3& lightPos = light.GetPos();

	// calculate -Z orientation.
	MMat3x3 nzRot;
	nzRot.MakeZRotation( DegToRad( 180 ) );

	// calculate +Z orientation.
	MMat3x3 pzRot;
	pzRot.MakeYRotation( DegToRad( 180 ) );
	pzRot = pzRot * nzRot;

	// calculate -X orientation.
	MMat3x3 nxRot;
	nxRot.MakeYRotation( DegToRad( 90 ) );
	nxRot = nxRot * nzRot;

	// calculate +X orientation.
	MMat3x3 pxRot;
	pxRot.MakeYRotation( DegToRad( -90 ) );
	pxRot = pxRot * nzRot;

	// calculate -Y orientation.
	MMat3x3 nyRot;
	nyRot.MakeXRotation( DegToRad( -90 ) );

	// calculate +Y orientation.
	MMat3x3 pyRot;
	pyRot.MakeXRotation( DegToRad( 90 ) );

	// now build the cameras.
	cameras[ 0 ] = GrCamera( lightPos, pxRot, fov, range, 1.0f );		// +x
	cameras[ 1 ] = GrCamera( lightPos, nxRot, fov, range, 1.0f );		// -x
	cameras[ 2 ] = GrCamera( lightPos, pyRot, fov, range, 1.0f );		// +y
	cameras[ 3 ] = GrCamera( lightPos, nyRot, fov, range, 1.0f );		// -y
	cameras[ 4 ] = GrCamera( lightPos, pzRot, fov, range, 1.0f );		// +z
	cameras[ 5 ] = GrCamera( lightPos, nzRot, fov, range, 1.0f );		// -z
}

//----------------------------------------------------------
void
GrVirtualShadowPage::CalcProjCamera( GrCamera& camera, const GrLight& light )
{
	// create a matrix that goes transforms coordinates into negative Z space.
	// NOTE: negative or positive???

	// should we just get the transform from the light and combine with the
	// projection matrix?  Note that we don't have to worry about the W row
	// because it's implied by the cube map lookup.  Specifically, we should
	// set it to 0,0,0,1.

	// calculate -Z orientation.
	MMat3x3 nzRot;
	nzRot.MakeZRotation( DegToRad( 180 ) );

	// this is the light's shadow space matrix.  This is what we'll use to
	// transform the camera's segments.
	camera.SetRot( light.GetWorldTransform().GetRotate() * nzRot );
	camera.SetPos( light.GetPos() );
	camera.SetProj( light.GetProjection() );
}

//----------------------------------------------------------
void
GrVirtualShadowPage::CalcShadowModAdd( SVec2& mod, SVec2& add, unsigned int shadowMapIdx,
									   unsigned int width, unsigned int height )
{
	// get the width and height of the shadow in floating point units.
	float fInvSize = 1.0f / float( ( int )_shadowPageSize );

	// currently, we don't do any size LODs.
//	mod.x = fInvSize * float( _shadowMapSize - 2 * SHADOW_BORDER_SIZE );
//	mod.y = fInvSize * float( _shadowMapSize - 2 * SHADOW_BORDER_SIZE );
	mod.x = fInvSize * float( width - 2 * SHADOW_BORDER_SIZE );
	mod.y = fInvSize * float( height - 2 * SHADOW_BORDER_SIZE );
	float invWidthScale = ( float )( int )( _shadowMapSize / width );
	float invHeightScale = ( float )( int )( _shadowMapSize / height );

	// calculate the shadow map's X & Y.
	unsigned int shadowMapsAlongX = _shadowPageSize / _shadowMapSize;
	int x = shadowMapIdx % shadowMapsAlongX;
	int y = shadowMapIdx / shadowMapsAlongX;
	add.x = x * _texAddrStep + fInvSize * float( SHADOW_BORDER_SIZE );
	add.y = y * _texAddrStep + fInvSize * float( SHADOW_BORDER_SIZE );
}
