//----------------------------------------------------------
// File:		Mesh.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		Contains mesh data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "Vector3.h"
#include "AABox.h"

// forward declarations.
class GrMesh;

// using namespaces.
using System::Runtime::InteropServices::OutAttribute;

namespace Bootstrap
{
	// forward declarations.
	ref class MeshVB;
	ref class MeshIB;

    //**********************************************************
    // value class TriInfo
    //**********************************************************
	public value class TriInfo
	{
	public:
		unsigned int                        VertIdx0;
		unsigned int                        VertIdx1;
		unsigned int                        VertIdx2;
		unsigned int                        Range;
	};

    //**********************************************************
    // value class RangeInfo
    //**********************************************************
	public value class RangeInfo
	{
	public:
		System::String^		                Name;
		unsigned int		                Start;		// starting vertex or index.
		unsigned int		                Count;		// number of vertices or indices.
		unsigned int		                MinIndex;	// only used for indexed meshes.
		unsigned int		                MaxIndex;
		AABox				                AlignedBoundingBox;
	};

    //**********************************************************
    // ref class Mesh
    //**********************************************************
	public ref class Mesh 
	{
	internal:
		Mesh( GrMesh* mesh );

		GrMesh*		                        _mesh;

	public:
		~Mesh();
		!Mesh();

		property System::String^            Name 
        { 
            System::String^                     get(); 
        }

		property MeshVB^                    VertexBuffer 
        { 
            MeshVB^                             get(); 
        }
        
		property MeshIB^                    IndexBuffer 
        { 
            MeshIB^                             get(); 
        }

		property int                        VertexCount 
        { 
            int                                 get(); 
        }

		property int                        IndexCount 
        { 
            int                                 get(); 
        }

		property int                        RangeCount 
        { 
            int                                 get(); 
        }

		bool                                GetRange( [Out] RangeInfo% rangeInfo, int rangeIdx );

		bool                                GetTriInfo( [Out] TriInfo% triInfo, int triIdx );

    private:
        MeshVB^		                        _meshVB;
        MeshIB^		                        _meshIB;
	};
}

