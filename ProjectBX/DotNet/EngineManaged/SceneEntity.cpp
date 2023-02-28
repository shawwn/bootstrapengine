#include "EMAfx.h"

// class header.
#include "SceneEntity.h"

// project headers.
#include "Matrix.h"

// engine headers.
#include "MMat4x4.h"
#include "MMat3x3.h"
#include "MVec3.h"

//**********************************************************
// ref class SceneEntity
//**********************************************************

//----------------------------------------------------------
Bootstrap::SceneEntity::SceneEntity( const MMat4x4& matrix, bool scalable )
: _scalable( scalable )
{
    // get the rotation angles.
    Vector3 rot = Matrix( matrix ).Rotate.EulerAngles;

    // convert them to degrees and store 'em.
    _rotation = Vector3( RadToDeg( rot.X ), RadToDeg( rot.Y ), RadToDeg( rot.Z ) );
}

//----------------------------------------------------------
Bootstrap::SceneEntity::SceneEntity( Vector3 position, Vector3 rotation, Vector3 scale, bool scalable )
: _scalable( scalable )
, _rotation( rotation )
{
}

//----------------------------------------------------------
Bootstrap::Matrix 
Bootstrap::SceneEntity::Transform::get()
{
    return GetTransform();
}

//----------------------------------------------------------
void 
Bootstrap::SceneEntity::Transform::set( Matrix mat )
{
    // get the rotation angles.
    Vector3 rot = mat.Rotate.EulerAngles;

    // convert them to degrees and store 'em.
    _rotation = Vector3( RadToDeg( rot.X ), RadToDeg( rot.Y ), RadToDeg( rot.Z ) );

    // apply the transform.
    ApplyTransform( mat );
    TransformChanged( this );
    PositionChanged( this );
    RotationChanged( this );
    ScaleChanged( this );
}

//----------------------------------------------------------
Bootstrap::Vector3
Bootstrap::SceneEntity::Position::get()
{
    return Transform.Translate;
}
//----------------------------------------------------------
void
Bootstrap::SceneEntity::Position::set( Vector3 value )
{
    Matrix mat = Transform; 
    mat.Translate = value; 
    Transform = mat;
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::SceneEntity::Rotation::get()
{
    return _rotation;
}

//----------------------------------------------------------
void 
Bootstrap::SceneEntity::Rotation::set( Vector3 value )
{
    // set the transform with that rotation.
    SetTransform( Position, value, Scale );
}

//----------------------------------------------------------
Bootstrap::Vector3 
Bootstrap::SceneEntity::Scale::get()
{
    return Transform.Scale;
}

//----------------------------------------------------------
void 
Bootstrap::SceneEntity::Scale::set( Vector3 value )
{
    if ( !_scalable )
        return;
    Matrix mat = Transform;
    mat.Scale = value;
    Transform = mat;
}

//----------------------------------------------------------
void 
Bootstrap::SceneEntity::SetTransform( Vector3 position, Vector3 rotation, Vector3 scale )
{
    _rotation = rotation;

    Matrix trans( Matrix3x3( DegToRad( _rotation.Y ), DegToRad( _rotation.X ), DegToRad( _rotation.Z ) ),
        position,
        scale );
    ApplyTransform( trans );
    TransformChanged( this );
    PositionChanged( this );
    RotationChanged( this );
    ScaleChanged( this );
}
