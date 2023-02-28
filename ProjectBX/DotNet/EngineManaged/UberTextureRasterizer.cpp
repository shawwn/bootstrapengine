#include "EMAfx.h"

// class header.
#include "UberTextureRasterizer.h"

// project headers.
#include "Utils.h"
#include "Brush.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "MeshVB.h"
#include "MeshInst.h"

// edlib headers.
#include "EdUberTexRasterizer.h"
#include "EdImportOBJ.h"
#include "EdBrush.h"

// engine headers.
#include "GrMeshVB.h"
#include "EdSimplePolygonGroup.h"

// .NET namespaces.
using namespace System;
using namespace System::Runtime::InteropServices;

// declare a function pointer that's compatible with what C# interop
// gives us.
typedef void ( __stdcall *INTEROPCALLBACK )( int totalTris, int trisProcessed, int raysCast );

// This gets function pointer gets forwarded calls from our callback.
static INTEROPCALLBACK _callback;

//----------------------------------------------------------
void CastCallbackProc( unsigned int totalTris, unsigned int trisProcessed,
                       unsigned int raysCast )
{
    if ( _callback )
        _callback( ( int )totalTris, ( int )trisProcessed, ( int )raysCast );
}

//**********************************************************
// ref class UberTextureRasterizer
//**********************************************************

//----------------------------------------------------------
Bootstrap::UberTextureRasterizer::UberTextureRasterizer( EdUberTexRasterizer* rasterizer )
    : _rasterizer( rasterizer )
{
}

//----------------------------------------------------------
Bootstrap::UberTextureRasterizer::UberTextureRasterizer()
    : _rasterizer( new EdUberTexRasterizer() )
{
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::Clear()
{
    _rasterizer->Clear();
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::Bake()
{
    _rasterizer->Bake();
}

//----------------------------------------------------------
Bootstrap::Brush^ 
Bootstrap::UberTextureRasterizer::Brush::get() 
{
    return _currentBrush;
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::Brush::set( Bootstrap::Brush^ val )
{
    _currentBrush = val;
    _rasterizer->SetBrush( _currentBrush != nullptr ?_currentBrush->_brush : 0 );
}

//----------------------------------------------------------
Bootstrap::MeshInst^
Bootstrap::UberTextureRasterizer::RenderTarget::get()
{
    return _currentRenderTarget;
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::RenderTarget::set( MeshInst^ target )
{
    _currentRenderTarget = target;
    _rasterizer->SetRenderTarget( _currentRenderTarget != nullptr ? _currentRenderTarget->_meshInst : 0 );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::CastGeometry( CastProgressCallback^ callback, System::String^ objFilePath,
                                                float maxRayDist, bool normal, bool bump, float bumpScale,
                                                bool smooth )
{
/*
    // create a polygon group to contain the high-poly data.
    EdSimplePolygonGroup highPolyData;

    // read in the obj file specified.
    EdImportOBJ objLoader;
    objLoader.LoadOBJ( highPolyData, StrToTStr( objFilePath ).c_str() );

    // now cast the bump data.
    if ( highPolyData.GetPolygonCount() > 0 )
    {
        // build vertex normals if necessary.
        if ( smooth && !highPolyData.HasVertexNormals() )
            highPolyData.GenVertexNormals();

        // get a function pointer for the delegate.
        EdUberGeometryCaster::Callback progressCallback = 0;
        if ( callback != nullptr )
        {
            _callback = ( INTEROPCALLBACK )Marshal::GetFunctionPointerForDelegate( callback ).ToPointer();
            progressCallback = CastCallbackProc;
        }
        else
            _callback = 0;

        // now cast bump/normal data.
        EdUberGeometryCaster caster;
        caster.CastBumpData( progressCallback, highPolyData, maxRayDist, normal, bump, bumpScale );
    }
*/
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::SetAutoBake( bool bake )
{
    _rasterizer->SetAutoBake( bake );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::SetVertexConstant( unsigned int index, Vector4 vec )
{
    _rasterizer->SetVertexConstant( index, vec.ToSVec4() );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::SetVertexConstants( unsigned int index, Matrix matrix )
{
    _rasterizer->SetVertexConstants( index, matrix.ToMMat4x4() );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::SetFragmentConstant( unsigned int index, Vector4 vec )
{
    _rasterizer->SetFragmentConstant( index, vec.ToSVec4() );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::SetFragmentConstants( unsigned int index, Matrix matrix )
{
    _rasterizer->SetFragmentConstants( index, matrix.ToMMat4x4() );
}

//----------------------------------------------------------
Bootstrap::VertexProgram^ 
Bootstrap::UberTextureRasterizer::VertexProgram::get() 
{
    return _currentVertexProgram;
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::VertexProgram::set( Bootstrap::VertexProgram^ val )
{
    _currentVertexProgram = val;
    _rasterizer->SetVertexProgram( _currentVertexProgram->_program );
}

//----------------------------------------------------------
Bootstrap::FragmentProgram^ 
Bootstrap::UberTextureRasterizer::FragmentProgram::get() 
{
    return _currentFragmentProgram;
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::FragmentProgram::set( Bootstrap::FragmentProgram^ val )
{
    _currentFragmentProgram = val;
    _rasterizer->SetFragmentProgram( _currentFragmentProgram->_program );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::RenderTriangleList(   MeshVB^ meshVB, array< unsigned short >^ indices, 
                                                        unsigned int indexOffset, unsigned int indexCount )
{
    SVec3* positions = meshVB->_meshVB->GetVertices();
    SVec2* texCoords = meshVB->_meshVB->GetTexCoords();

    if( !positions || !texCoords )
        throw gcnew System::ArgumentException( "The meshVB must contain both positions and texcoords." );

    if( indexOffset + indexCount > (unsigned int)indices->Length )
        throw gcnew System::ArgumentException( "Tried to draw more triangles than exists in [indices]." );

    pin_ptr< unsigned short > indicesPtr = &indices[ indexOffset ];

    _rasterizer->RenderTriangleList( positions, texCoords, indicesPtr, indexCount/3 );
}

//----------------------------------------------------------
void 
Bootstrap::UberTextureRasterizer::RenderTriangle( Vector3 position0, Vector2 texCoord0,
                                                  Vector3 position1, Vector2 texCoord1,
                                                  Vector3 position2, Vector2 texCoord2 )
{
    // setup the vertex data.
    SPolygonVertex vertices[ 3 ];
    vertices[ 0 ].position  = position0.ToSVec3();
    vertices[ 0 ].uv        = texCoord0.ToSVec2();

    vertices[ 1 ].position  = position1.ToSVec3();
    vertices[ 1 ].uv        = texCoord1.ToSVec2();

    vertices[ 2 ].position  = position2.ToSVec3();
    vertices[ 2 ].uv        = texCoord2.ToSVec2();

    // now render the triangle.
    _rasterizer->RenderTriangle( vertices );
}