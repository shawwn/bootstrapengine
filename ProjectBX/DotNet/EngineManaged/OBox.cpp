#include "EMAfx.h"

// class header.
#include "OBox.h"

// project includes.
#include "Utils.h"
#include "AABox.h"
#include "Ray.h"

//**********************************************************
// value class OBox
//**********************************************************

//-----------------------
Bootstrap::OBox::OBox( const MOBox& box )
{
    Transform = Matrix( box.GetTransform() );
    HalfExtents = box.GetHalfExts();
}

//-----------------------
MOBox 
Bootstrap::OBox::ToMOBox()
{
    return MOBox( Transform.ToMMat4x4(), HalfExtents.ToMVec3() );
}

//-----------------------
Bootstrap::OBox::OBox( AABox aaBox )
{
    Transform = Matrix::Identity;
    HalfExtents = aaBox.HalfExtents;
}

//-----------------------
Bootstrap::OBox::OBox( Matrix transform, AABox aaBox )
{
    Transform = transform * Matrix( aaBox.Center );
    HalfExtents = aaBox.HalfExtents;
}

//-----------------------
Bootstrap::OBox::OBox( Matrix transform, Vector3 halfExtents )
{
    Transform = transform;
    HalfExtents = halfExtents;
}

//-----------------------
array< Bootstrap::Vector3 >^ 
Bootstrap::OBox::Corners::get()
{
    array< Vector3 >^ corners = gcnew array< Vector3 >( 8 );

    // this could probably be sped up.
    Vector3 center = Transform.Translate;
    Vector3 pxAxis = Transform.XAxis;
    Vector3 pyAxis = Transform.YAxis;
    Vector3 pzAxis = Transform.ZAxis;

    pxAxis *= HalfExtents.X;
    pyAxis *= HalfExtents.Y;
    pzAxis *= HalfExtents.Z;
    Vector3 nxAxis = -pxAxis;
    Vector3 nyAxis = -pyAxis;
    Vector3 nzAxis = -pzAxis;

    corners[ 0 ] = center + nxAxis + nyAxis + nzAxis;
    corners[ 1 ] = center + nxAxis + nyAxis + pzAxis;
    corners[ 2 ] = center + nxAxis + pyAxis + nzAxis;
    corners[ 3 ] = center + nxAxis + pyAxis + pzAxis;
    corners[ 4 ] = center + pxAxis + nyAxis + nzAxis;
    corners[ 5 ] = center + pxAxis + nyAxis + pzAxis;
    corners[ 6 ] = center + pxAxis + pyAxis + nzAxis;
    corners[ 7 ] = center + pxAxis + pyAxis + pzAxis;

    return corners;
}

//----------------------------------------------------------
System::String^ 
Bootstrap::OBox::ToString()
{
    return "HalfExtents " + HalfExtents + " Transform " + Transform;
}