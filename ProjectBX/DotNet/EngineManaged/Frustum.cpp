//----------------------------------------------------------
// File:        Frustum.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Frustum.h"

// library includes.
#include "OBox.h"

// engine includes.
#include "MAABox.h"
#include "MVec3.h"
#include "MPlane.h"
#include "GrTypes.h"
#include "GrFrustum.h"

//**********************************************************
// ref class Frustum
//**********************************************************

//----------------------------------------------------------
Bootstrap::Frustum::Frustum( const GrFrustum& frustum )
: _frustum( new GrFrustum( frustum ) )
{
}

//----------------------------------------------------------
Bootstrap::Frustum::Frustum( Vector3 lookPoint, Vector3 lookDir, Vector3 nearBottomLeft, Vector3 nearTopLeft,
                             Vector3 nearTopRight, Vector3 nearBottomRight, Vector3 farBottomLeft, Vector3 farTopLeft,
                             Vector3 farTopRight, Vector3 farBottomRight, float nearDist, float farDist )
: _frustum( 0 )
{
    _frustum = new GrFrustum( lookPoint, lookDir, nearBottomLeft, nearTopLeft, 
                              nearTopRight, nearBottomRight, farBottomLeft, farTopLeft,
                              farTopRight, farBottomRight, nearDist, farDist );
}

//----------------------------------------------------------
Bootstrap::Frustum::~Frustum()
{
    this->!Frustum();
}

//----------------------------------------------------------
Bootstrap::Frustum::!Frustum()
{
    delete _frustum;
}

//----------------------------------------------------------
array< Bootstrap::Plane >^  
Bootstrap::Frustum::CullPlanes::get()   
{ 
    // construct the cull planes.
    const GrFrustum::PlaneArray& planes = _frustum->GetCullPlanes();

    // construct the result array.
    array< Plane >^ result = gcnew array< Plane >( planes.GetElemCount() );

    // loop through each cull plane, adding it to the result array.
    for ( unsigned int i = 0; i < planes.GetElemCount(); ++i )
        result[ i ] = Plane( planes[ i ] );

    // return the result array.
    return result;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::Position::get()
{
    return _frustum->GetPos();
}

//----------------------------------------------------------
void 
Bootstrap::Frustum::AddCullPlane( Plane plane )
{
    _frustum->AddCullPlane( plane.ToMPlane() );
}

//----------------------------------------------------------
Bootstrap::Plane 
Bootstrap::Frustum::GetCullPlane( CullPlane plane )
{
    switch( plane )
    {
    case CullPlane::Left:
        return Plane( _frustum->GetCullPlane( GrFrustum::ECP_LEFT ) );
    case CullPlane::Right:
        return Plane( _frustum->GetCullPlane( GrFrustum::ECP_RIGHT ) );
    case CullPlane::Top:
        return Plane( _frustum->GetCullPlane( GrFrustum::ECP_TOP ) );
    case CullPlane::Bottom:
        return Plane( _frustum->GetCullPlane( GrFrustum::ECP_BOTTOM ) );
    case CullPlane::Near:
        return Plane( _frustum->GetCullPlane( GrFrustum::ECP_NEAR ) );
    case CullPlane::Far:
        return Plane( _frustum->GetCullPlane( GrFrustum::ECP_FAR ) );
    default:
        throw gcnew System::ArgumentException( "invalid CullPlane." );
    }
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::Frustum::GetCrossSection( Plane plane )
{
    UFastArray< MVec3 > hull;
    _frustum->GetCrossSection( hull, plane.ToMPlane() );
    array< Vector3 >^ result = gcnew array< Vector3 >( hull.GetElemCount() );
    for( unsigned int i = 0; i < hull.GetElemCount(); ++i )
        result[ i ] = hull[ i ];
    return result;
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::Frustum::GetPerpCrossSection( float dist )
{
    UFastArray< MVec3 > hull;
    _frustum->GetPerpCrossSection( hull, dist );
    array< Vector3 >^ result = gcnew array< Vector3 >( hull.GetElemCount() );
    for( unsigned int i = 0; i < hull.GetElemCount(); ++i )
        result[ i ] = hull[ i ];
    return result;
}

//----------------------------------------------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::Frustum::BuildHull()
{
    SVec3 verts[ 18 ];
    _frustum->BuildHull( verts );
    array< Vector3 >^ result = gcnew array< Vector3 >( 18 );
    for( unsigned int i = 0; i < 18; ++i )
        result[ i ] = Vector3( verts[i] );
    return result;
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsHull::get()
{
    return _frustum->IsHull();
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsSphereInside( Vector3 pt, float radius )
{
    return _frustum->IsSphereInside( pt, radius );
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsPointInside( Vector3 pt )
{
    return _frustum->IsPointInside( pt );
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsAABoxInside( AABox box )
{
    return _frustum->IsAABoxInside( box.ToMAABox() );
}

//----------------------------------------------------------
bool
Bootstrap::Frustum::IsOBoxInside( OBox box )
{
    return _frustum->IsOBoxInside( box.ToMOBox() );
}

//----------------------------------------------------------
int 
Bootstrap::Frustum::FindRleativeLoc( Plane plane )
{
    return _frustum->FindRelativeLoc( plane.ToMPlane() );
}

//----------------------------------------------------------
int 
Bootstrap::Frustum::FindRelativeLoc( Vector3 pt0, Vector3 pt1 )
{
    return _frustum->FindRelativeLoc( pt0, pt1 );
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsFrustumInside( Frustum^ frustum )
{
    return _frustum->IsFrustumInside( *frustum->_frustum );
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsTriangleInside( Vector3 p0, Vector3 p1, Vector3 p2 )
{
    return _frustum->IsTriangleInside( p0.ToSVec3(), p1.ToSVec3(), p2.ToSVec3() );
}

//----------------------------------------------------------
bool 
Bootstrap::Frustum::IsRayInside( const MRay& ray, float epsilon )
{
    return _frustum->IsRayInside( ray, epsilon );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::FarBottomLeft::get() 
{ 
    return _frustum->GetFarBottomLeft(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::FarTopLeft::get() 
{ 
    return _frustum->GetFarTopLeft(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::FarTopRight::get()
{ 
    return _frustum->GetFarTopRight(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::FarBottomRight::get() 
{ 
    return _frustum->GetFarBottomRight(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::NearBottomLeft::get() 
{ 
    return _frustum->GetFarBottomLeft(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::NearTopLeft::get() 
{ 
    return _frustum->GetFarTopLeft(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::NearTopRight::get() 
{ 
    return _frustum->GetFarTopRight(); 
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::Frustum::NearBottomRight::get()  
{ 
    return _frustum->GetFarBottomRight(); 
}
