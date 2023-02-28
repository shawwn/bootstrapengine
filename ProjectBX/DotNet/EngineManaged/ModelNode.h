//----------------------------------------------------------
// File:        ModelNode.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To manage a node in a model.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "SceneEntity.h"

// forward declarations.
class GrModelNode;
class GrModel;

namespace Bootstrap
{
    // .NET forward declarations.
    ref class MeshInst;
    ref class Mesh;
    ref class Model;
    ref class Light;
    value class Matrix;
    value class AABox;

    //**********************************************************
    // ref class ModelNode
    //**********************************************************
    public ref class ModelNode : public SceneEntity
    {
    internal:
        ModelNode( GrModelNode* modelNode );

        // model node.
        GrModelNode*                        _modelNode;

    public:
        ~ModelNode();
        !ModelNode();

        property System::String^            Name           
        { 
            System::String^                     get();        
        }

        property Model^                     Owner
        { 
            Model^                              get(); 
        }

        // clones a node hierarchy.  'parent' can be null.
        ModelNode^                          Clone( Model^ newOwner, bool deep, ModelNode^ parent );

        // the local transform.
        property Matrix                     LocalTransform
        {
            Matrix                              get();
            void                                set( Matrix mat );
        }

        // the world transform.
        property Matrix                     WorldTransform
        {
            Matrix                              get();
            void                                set( Matrix mat );
        }

        // child management.
        property int                        ChildCount              
        { 
            int                                 get();          
        }

        ModelNode^                          GetChild( int idx );

        ModelNode^                          FindNode( System::String^ name, bool ignoreCase );

        // parent.
        property ModelNode^                 Parent 
        { 
            ModelNode^                          get(); 
        }

        // mesh instance management.
        property int                        MeshInstCount           
        { 
            int                                 get();          
        }

        MeshInst^                           GetMeshInst( int idx );

        // calculates an AABB from geometry that includes the origin.
        property AABox                      Bounds 
        { 
            AABox                               get();          
        }

        // calculates the optimal AABB from geometry.
        property AABox                      BoundsExact             
        { 
            AABox                               get();          
        }

        // builds a list of all unique meshes in the hierarchy.
        property array< Mesh^ >^            MeshList 
        { 
            array< Mesh^ >^                     get(); 
        }

        // optimize meshes on this node.
        void                                Optimize();

        // comparator.
        virtual bool                        Equals( System::Object^ obj ) override;

    protected:
        // inherited from SceneEntity.
        virtual void                        ApplyTransform( Matrix mat ) override;
        virtual Matrix                      GetTransform() sealed override;
    };
}
