#include "common_afx.h"

// class header.
#include "MQuat.h"

// project includes.
#include "MVec3.h"
#include "MMat3x3.h"
#include "MMat4x4.h"


//**********************************************************
// class MQuat
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
MQuat::MQuat()
: MVec4( 0.0f, 0.0f, 0.0f, 1.0f )
{

}

//----------------------------------------------------------
MQuat::MQuat( float x, float y, float z, float w )
: MVec4( x, y, z, w )
{

}

//----------------------------------------------------------
MQuat::MQuat( const MVec3& axis, float angle )
: MVec4( Sin( 0.5f*angle ) * axis.Normalized(), Cos( 0.5f*angle ) )
{

}

//----------------------------------------------------------
MQuat::MQuat( const MQuat& quat )
: MVec4( quat )
{

}

//----------------------------------------------------------
MQuat::MQuat( const MMat3x3& rot )
{
    MakeFromMat3x3( rot );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
MQuat&
MQuat::operator = ( const MQuat& quat )
{
    MVec4::operator = ( quat );
    return *this;
}

//----------------------------------------------------------
MQuat&
MQuat::operator = ( const MMat3x3& rot )
{
    MakeFromMat3x3( rot );
    return *this;
}

//----------------------------------------------------------
void
MQuat::MakeRotFast( MMat3x3& rot ) const
{
    // calculate necessary coefficients.
    float x2 = GetX() + GetX();
    float y2 = GetY() + GetY();
    float z2 = GetZ() + GetZ();
    float xx = GetX() * x2;
    float xy = GetX() * y2;
    float xz = GetX() * z2;
    float yy = GetY() * y2;
    float yz = GetY() * z2;
    float zz = GetZ() * z2;
    float wx = GetW() * x2;
    float wy = GetW() * y2;
    float wz = GetW() * z2;

    // build our matrix and return it.
    rot( 0, 0 ) = 1.0f - (yy + zz);
    rot( 0, 1 ) = xy - wz;
    rot( 0, 2 ) = xz + wy;

    rot( 1, 0 ) = xy + wz;
    rot( 1, 1 ) = 1.0f - (xx + zz);
    rot( 1, 2 ) = yz - wx;

    rot( 2, 0 ) = xz - wy;
    rot( 2, 1 ) = yz + wx;
    rot( 2, 2 ) = 1.0f - (xx + yy);
}

//----------------------------------------------------------
MMat3x3
MQuat::MakeRot() const
{
    // calculate necessary coefficients.
    float x2 = GetX() + GetX();
    float y2 = GetY() + GetY();
    float z2 = GetZ() + GetZ();
    float xx = GetX() * x2;
    float xy = GetX() * y2;
    float xz = GetX() * z2;
    float yy = GetY() * y2;
    float yz = GetY() * z2;
    float zz = GetZ() * z2;
    float wx = GetW() * x2;
    float wy = GetW() * y2;
    float wz = GetW() * z2;

    // build our matrix and return it.
    return MMat3x3( 1.0f - (yy + zz), xy - wz, xz + wy,
                    xy + wz, 1.0f - (xx + zz), yz - wx,
                    xz - wy, yz + wx, 1.0f - (xx + yy) );
}

//----------------------------------------------------------
void
MQuat::SetMatrixRot( MMat4x4& mat ) const
{
    // calculate necessary coefficients.
    float x2 = GetX() + GetX();
    float y2 = GetY() + GetY();
    float z2 = GetZ() + GetZ();
    float xx = GetX() * x2;
    float xy = GetX() * y2;
    float xz = GetX() * z2;
    float yy = GetY() * y2;
    float yz = GetY() * z2;
    float zz = GetZ() * z2;
    float wx = GetW() * x2;
    float wy = GetW() * y2;
    float wz = GetW() * z2;

    // build our matrix and return it.
    mat( 0, 0 ) = 1.0f - (yy + zz);
    mat( 0, 1 ) = xy - wz;
    mat( 0, 2 ) = xz + wy;

    mat( 1, 0 ) = xy + wz;
    mat( 1, 1 ) = 1.0f - (xx + zz);
    mat( 1, 2 ) = yz - wx;

    mat( 2, 0 ) = xz - wy;
    mat( 2, 1 ) = yz + wx;
    mat( 2, 2 ) = 1.0f - (xx + yy);
}

//----------------------------------------------------------
MQuat
MQuat::operator * ( const MQuat& rhs ) const
{
    // w*w' - dot(v, v');
    float w = GetW()*rhs.GetW() - ( GetX()*rhs.GetX() + GetY()*rhs.GetY() + GetZ()*rhs.GetZ() );

    // v.Cross( v' ) + wv' + w'v;
    float x = GetY()*rhs.GetZ() - GetZ()*rhs.GetY();
    float y = GetZ()*rhs.GetX() - GetX()*rhs.GetZ();
    float z = GetX()*rhs.GetY() - GetY()*rhs.GetX();
    x += GetW() * rhs.GetX();
    x += rhs.GetW() * GetX();
    y += GetW() * rhs.GetY();
    y += rhs.GetW() * GetY();
    z += GetW() * rhs.GetZ();
    z += rhs.GetW() * GetZ();

    // return our new quaternion.
    return MQuat( x, y, z, w );
}

//----------------------------------------------------------
MQuat&
MQuat::operator *= ( const MQuat& rhs )
{
    // w*w' - dot(v, v');
    float w = GetW()*rhs.GetW() - ( GetX()*rhs.GetX() + GetY()*rhs.GetY() + GetZ()*rhs.GetZ() );

    // v.Cross( v' ) + wv' + w'v;
    float x = GetY()*rhs.GetZ() - GetZ()*rhs.GetY();
    float y = GetZ()*rhs.GetX() - GetX()*rhs.GetZ();
    float z = GetX()*rhs.GetY() - GetY()*rhs.GetX();
    x += GetW() * rhs.GetX();
    x += rhs.GetW() * GetX();
    y += GetW() * rhs.GetY();
    y += rhs.GetW() * GetY();
    z += GetW() * rhs.GetZ();
    z += rhs.GetW() * GetZ();

    // assign.
    SetX( x );
    SetY( y );
    SetZ( z );
    SetW( w );

    return *this;
}

//----------------------------------------------------------
void
MQuat::Invert()
{
    SetX( -GetX() );
    SetY( -GetY() );
    SetZ( -GetZ() );
}

//----------------------------------------------------------
MQuat
MQuat::Inverted() const
{
    return MQuat( -GetX(), -GetY(), -GetZ(), GetW() );
}

//----------------------------------------------------------
void
MQuat::Normalize()
{
    float invMag = 1.0f/Sqrt( GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ() + GetW()*GetW() );
    SetX( invMag*GetX() );
    SetY( invMag*GetY() );
    SetZ( invMag*GetZ() );
    SetW( invMag*GetW() );
}

//----------------------------------------------------------
MQuat
MQuat::Normalized() const
{
    float invMag = 1.0f/Sqrt( GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ() + GetW()*GetW() );
    return MQuat( invMag*GetX(), invMag*GetY(), invMag*GetZ(), invMag*GetW() );
}

//----------------------------------------------------------
void
MQuat::SlerpToFast( const MQuat& to, float t )
{
    // TODO: Rewrite!  Clean this the hell up!
    float to1[4];
    float omega, cosom, sinom, scale0, scale1;

    // calc cosine.
    cosom = GetX() * to.GetX() + GetY() * to.GetY() + GetZ() * to.GetZ() + GetW() * to.GetW();

    // adjust signs (if necessary).
    if ( cosom < 0.0 )
    {
        cosom = -cosom;
        to1[0] = -to.GetX();
        to1[1] = -to.GetY();
        to1[2] = -to.GetZ();
        to1[3] = -to.GetW();
    }
    else
    {
        to1[0] = to.GetX();
        to1[1] = to.GetY();
        to1[2] = to.GetZ();
        to1[3] = to.GetW();
    }

    // calculate coefficients.
    if ( (1.0f - cosom) > 0.001f )
    {
        // standard case (slerp).
        omega = ACos( cosom );
        sinom = Sin( omega );
        scale0 = Sin( ( 1.0f - t ) * omega ) / sinom;
        scale1 = Sin( t * omega ) / sinom;
    }
    else
    {
        // "from" and "to" quaternions are very close 
        //  ... so we can do a linear interpolation.
        scale0 = 1.0f - t;
        scale1 = t;
    }

    // calculate final values.
    SetX( scale0 * GetX() + scale1 * to1[0] );
    SetY( scale0 * GetY() + scale1 * to1[1] );
    SetZ( scale0 * GetZ() + scale1 * to1[2] );
    SetW( scale0 * GetW() + scale1 * to1[3] );
}

//----------------------------------------------------------
MQuat
MQuat::SlerpTo( const MQuat& to, float t ) const
{
    // TODO: Rewrite!  Clean this the hell up!
    float to1[4];
    float omega, cosom, sinom, scale0, scale1;

    // calc cosine
    cosom = GetX() * to.GetX() + GetY() * to.GetY() + GetZ() * to.GetZ() + GetW() * to.GetW();

    // adjust signs (if necessary)
    if ( cosom < 0.0 )
    {
        cosom = -cosom;
        to1[0] = -to.GetX();
        to1[1] = -to.GetY();
        to1[2] = -to.GetZ();
        to1[3] = -to.GetW();
    }
    else
    {
        to1[0] = to.GetX();
        to1[1] = to.GetY();
        to1[2] = to.GetZ();
        to1[3] = to.GetW();
    }

    // calculate coefficients
    if ( (1.0 - cosom) > 0.001f )
    {
        // standard case (slerp)
        omega = ACos( cosom );
        sinom = Sin( omega );
        scale0 = Sin( ( 1.0f - t ) * omega ) / sinom;
        scale1 = Sin( t * omega ) / sinom;
    }
    else
    {
        // "from" and "to" quaternions are very close 
        //  ... so we can do a linear interpolation
        scale0 = 1.0f - t;
        scale1 = t;
    }

    // calculate final values
    return MQuat(   scale0 * GetX() + scale1 * to1[0],
                    scale0 * GetY() + scale1 * to1[1],
                    scale0 * GetZ() + scale1 * to1[2],
                    scale0 * GetW() + scale1 * to1[3] );
}

//----------------------------------------------------------
void
MQuat::SlerpFromIdentFast( float t )
{
    // TODO: Rewrite!  Clean this the hell up!
    float to1[4];
    float omega, cosom, sinom, scale0, scale1;

    // calc cosine
    cosom = GetW();

    // adjust signs (if necessary)
    if ( cosom < 0.0 )
    {
        cosom = -cosom;
        to1[0] = -GetX();
        to1[1] = -GetY();
        to1[2] = -GetZ();
        to1[3] = -GetW();
    }
    else
    {
        to1[0] = GetX();
        to1[1] = GetY();
        to1[2] = GetZ();
        to1[3] = GetW();
    }

    // calculate coefficients
    if ( (1.0 - cosom) > 0.001f )
    {
        // standard case (slerp)
        omega = ACos( cosom );
        sinom = Sin( omega );
        scale0 = Sin( ( 1.0f - t ) * omega ) / sinom;
        scale1 = Sin( t * omega ) / sinom;
    }
    else
    {
        // "from" and "to" quaternions are very close 
        //  ... so we can do a linear interpolation
        scale0 = 1.0f - t;
        scale1 = t;
    }

    // calculate final values
    SetX( scale1 * to1[0] );
    SetY( scale1 * to1[1] );
    SetZ( scale1 * to1[2] );
    SetW( scale0 + scale1 * to1[3] );
}

//----------------------------------------------------------
MQuat
MQuat::SlerpFromIdent( float t )
{
    // TODO: Rewrite!  Clean this the hell up!
    float to1[4];
    float omega, cosom, sinom, scale0, scale1;

    // calc cosine
    cosom = GetW();

    // adjust signs (if necessary)
    if ( cosom < 0.0 )
    {
        cosom = -cosom;
        to1[0] = -GetX();
        to1[1] = -GetY();
        to1[2] = -GetZ();
        to1[3] = -GetW();
    }
    else
    {
        to1[0] = GetX();
        to1[1] = GetY();
        to1[2] = GetZ();
        to1[3] = GetW();
    }

    // calculate coefficients
    if ( (1.0 - cosom) > 0.001f )
    {
        // standard case (slerp)
        omega = ACos( cosom );
        sinom = Sin( omega );
        scale0 = Sin( ( 1.0f - t ) * omega ) / sinom;
        scale1 = Sin( t * omega ) / sinom;
    }
    else
    {
        // "from" and "to" quaternions are very close 
        //  ... so we can do a linear interpolation
        scale0 = 1.0f - t;
        scale1 = t;
    }

    // calculate final values
    return MQuat(   scale1 * to1[0],
                    scale1 * to1[1],
                    scale1 * to1[2],
                    scale0 + scale1 * to1[3] );
}

//----------------------------------------------------------
void
MQuat::BlendFromIdentFast( float t )
{
    // calc cosine
    float cosom = GetW();
    if ( Abs( cosom ) > 0.9995 )
    {
        SetX( Lerp( 0.0f, GetX(), t ) );
        SetY( Lerp( 0.0f, GetY(), t ) );
        SetZ( Lerp( 0.0f, GetZ(), t ) );
        SetW( Lerp( 0.0f, GetW(), t ) );
        return;
    }

    // standard case (blend)
    float omega = ACos( cosom );
    float sinom = Sin( omega );
    float scale1 = Sin( t * omega ) / sinom;

    // calculate final values
    SetX( scale1 * GetX() );
    SetY( scale1 * GetY() );
    SetZ( scale1 * GetZ() );
    SetW( Cos( t * omega ) );
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
MQuat::MakeFromMat3x3( const MMat3x3& rot )
{
    float  tr, s, q[4];
    int    i, j, k;

    int nxt[3] = {1, 2, 0};
    tr = rot(0, 0) + rot(1, 1) + rot(2, 2);

    // check the diagonal.
    if ( tr > 0.0f )
    {
        // positive diagonal.
        s = Sqrt( tr + 1.0f );
        SetW( s / 2.0f );
        s = 0.5f / s;
        SetX( (rot( 2, 1 ) - rot( 1, 2 )) * s );
        SetY( (rot( 0, 2 ) - rot( 2, 0 )) * s );
        SetZ( (rot( 1, 0 ) - rot( 0, 1 )) * s );
    }
    else
    {
        // negative diagonal.
        i = 0;
        if ( rot( 1, 1 ) > rot( 0, 0 ) )
            i = 1;
        if ( rot( 2, 2 ) > rot( i, i ) )
            i = 2;
        j = nxt[i];
        k = nxt[j];

        s = Sqrt( (rot( i, i ) - (rot( j, j ) + rot( k, k ))) + 1.0f );
        q[i] = s * 0.5f;
        if ( s != 0.0f )
            s = 0.5f / s;

        q[3] = (rot( k, j ) - rot( j, k )) * s;
        q[j] = (rot( j, i ) + rot( i, j )) * s;
        q[k] = (rot( k, i ) + rot( i, k )) * s;

        SetX( q[0] );
        SetY( q[1] );
        SetZ( q[2] );
        SetW( q[3] );
    }
}
