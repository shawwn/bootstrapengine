//----------------------------------------------------------
// File:		MeshVB.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage a vertex buffer object.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrMeshVB;

// project headers.
#include "Vector2.h"
#include "Vector3.h"

// .NET namespaces.
using namespace System;

namespace Bootstrap
{
    //**********************************************************
    // enum class MeshComponents
    //**********************************************************
	public enum class MeshComponents
	{
		None                = 0x00,
		Positions           = 0x01,
		Normals             = 0x02,
		BiNormals           = 0x04,
		Tangents            = 0x08,
		Colors              = 0x10,
		TexCoords           = 0x20,
		UniqueTexCoords     = 0x40,
		All                 = 0x7F
	};

    //**********************************************************
    // ref class MeshVB
    //**********************************************************
	public ref class MeshVB
	{
	internal:
		MeshVB( GrMeshVB* meshVB );

		GrMeshVB*		                    _meshVB;

	public:
		~MeshVB();
		!MeshVB();

		property int					    VertexCount 
        { 
            int                                 get(); 
        }

		property MeshComponents			    UsedComponents 
        { 
            MeshComponents                      get(); 
        }

		property array< Vector3 >^		    Positions	
        { 
            array< Vector3 >^                   get(); 
        }

		property array< Vector3 >^		    Normals		
        { 
            array< Vector3 >^                   get(); 
        }

		property array< Vector3 >^		    BiNormals	
        { 
            array< Vector3 >^                   get(); 
        }

		property array< Vector3 >^		    Tangents	
        { 
            array< Vector3 >^                   get(); 
        }

		property array< UInt32 >^		    Colors		
        { 
            array< UInt32 >^                   get(); 
        }

		property array< Vector2 >^		    TexCoords	
        { 
            array< Vector2 >^                   get(); 
        }

	private:
		array< Vector3 >^	                _positions;
		array< Vector3 >^	                _normals;
		array< Vector3 >^	                _binormals;
		array< Vector3 >^	                _tangents;
		array< UInt32 >^	                _colors;
		array< Vector2 >^	                _texCoords;
	};
}