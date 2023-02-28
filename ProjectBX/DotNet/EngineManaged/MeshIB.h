//----------------------------------------------------------
// File:		MeshIB.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage an index buffer object.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrMeshIB;

using namespace System;

namespace Bootstrap
{
    //**********************************************************
    // ref class MeshIB
    //**********************************************************
	public ref class MeshIB
	{
	internal:
		MeshIB( GrMeshIB* meshIB );

	public:
		~MeshIB();
		!MeshIB();

		property int				        IndexCount 
        { 
            int                                 get(); 
        }
		property array< UInt16 >^	        Indices 
        { 
            array< UInt16 >^                    get(); 
        }

	private:
		array< UInt16 >^	                _indices;

	internal:
		// model.
		GrMeshIB*			                _meshIB;
	};
}