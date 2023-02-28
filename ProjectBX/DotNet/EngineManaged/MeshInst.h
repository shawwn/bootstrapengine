//----------------------------------------------------------
// File:        MeshInst.h
// Author:      Shawn Presser
// Created:     10-15-08
//
// Purpose:     To manage a mesh instance.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "SceneEntity.h"
#include "Mesh.h"

// forward declarations.
class GrMeshInst;

// .NET namespaces.
using namespace System::Collections::Generic;

namespace Bootstrap
{
    // .NET forward declarations.
    ref class Mesh;
    ref class Material;
    value class RangeInfo;
    value class OBox;

    //***********************
    // enum VertexComponents
    //***********************
    public enum VertexComponents
    {
        None            = 0x00,
        Position        = 0x01,
        Normal          = 0x02,
        Binormal        = 0x04,
        Tangent         = 0x08,
        DiffuseColor    = 0x10,
        Texcoord        = 0x20,
        Ambient         = ( Position | Normal | Binormal | Tangent | Texcoord ),
        All             = 0x3F
    };

    //***********************
    // ref class MeshInst
    //***********************
    public ref class MeshInst : public SceneEntity
    {
        MeshInst( GrMeshInst* meshInst );

    internal:
        // creates a MeshInst reference.
        static MeshInst^                    Create( GrMeshInst* meshInst );

        GrMeshInst*                         _meshInst;

    public:
        ~MeshInst();
        !MeshInst();

        // clones a mesh instance.  If deep is true, then the mesh and materials are also cloned.
        // Generally, this should be false.
        MeshInst^                           Clone( bool deep );

        // assignment for the mesh instance data.
        void                                SetMesh( Bootstrap::Mesh^ mesh, IList< Material^ >^ materials );
        void                                SetMaterial( int range, Material^ material );

        property int                        RangeCount
        {     
            int                                 get();
        }

        property Bootstrap::Mesh^           Mesh
        {   
            Bootstrap::Mesh^                    get();
        }

        Material^                           GetMaterial( int rangeIdx );
        
        property Matrix                     Transform
        {   
            Matrix                              get();
            void                                set( Matrix mat );
        }

        // spatial information.
        property Sphere                     Sphere                  
        {   
            Bootstrap::Sphere                   get();              
        }

        property Bootstrap::OBox            OBox                            
        {   
            Bootstrap::OBox                     get();              
        }

        Bootstrap::Sphere                   GetRangeBoundingSphere( int rangeIdx );
        Bootstrap::OBox                     GetRangeOBox( int rangeIdx );

        // TODO: create a SkinInst .NET wrapper.
        // property SkinInst^               SkinInst
        // {   
        //    SkinInst^                        get();
        // }

        void                                RenderRange( int range, VertexComponents vertexComponents, unsigned int frameTime );

        virtual bool                        Equals( System::Object^ obj ) override;
        virtual int                         GetHashCode() override;

    protected:      
        // inherited from SceneEntity.
        virtual void                        ApplyTransform( Matrix mat ) sealed override;
        virtual Matrix                      GetTransform() sealed override;

    private:
        static Dictionary< System::UInt64, MeshInst^ >^ _meshInstCache;
    };
}

