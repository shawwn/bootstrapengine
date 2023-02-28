//----------------------------------------------------------
// File:        MeshIB.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "MeshIB.h"

// project includes.
#include "Utils.h"

// engine includes.
#include "GrMeshIB.h"

//**********************************************************
// ref class MeshIB
//**********************************************************

//----------------------------------------------------------
Bootstrap::MeshIB::MeshIB( GrMeshIB* meshIB )
    :  _meshIB( meshIB )
{
    B_ASSERT( _meshIB != 0 );

    _meshIB->IncRef();
}

//----------------------------------------------------------
Bootstrap::MeshIB::~MeshIB()
{
    this->!MeshIB();
}

//----------------------------------------------------------
Bootstrap::MeshIB::!MeshIB()
{
    _meshIB->DecRef();
}

//----------------------------------------------------------
int 
Bootstrap::MeshIB::IndexCount::get()
{
    return _meshIB->GetIndexCount();
}

//----------------------------------------------------------
array< UInt16 >^ 
Bootstrap::MeshIB::Indices::get()
{
    // cache the indices.
    if( _indices == nullptr )
    {
        int count = _meshIB->GetIndexCount();
        _indices = gcnew array< UInt16 >( count );
        unsigned short* index = _meshIB->GetIndices();
        for( int i = 0; i < count; ++i, ++index )
            _indices[i] = *index;
    }

    return _indices;
}