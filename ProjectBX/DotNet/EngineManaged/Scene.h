//----------------------------------------------------------
// File:        Scene.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To manage the 3D scene.  Essentially a spatially relational
//              database.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project headers.
#include "Mesh.h"
#include "Vector3.h"

// forward declarations.
class GrMeshInst;

// .NET namespaces.
using namespace System::Collections::Generic;

namespace Bootstrap
{
    // .NET forward declarations.
    ref class SceneEntity;
    ref class Model;
    ref class Light;
    ref class MeshInst;
    ref class Camera;
    value class Color;
    value class Ray;

    //**********************************************************
    // ref class Scene
    //**********************************************************
    public ref class Scene 
    {
    internal:
        Scene();

    public:
        ~Scene();
        !Scene();

        //**********************************************************
        // enum class Entities
        //**********************************************************
        // NOTE: This enumeration MUST match EdPick.h and GrScene.h.
        enum class Entities
        {
            None        = 0x00,
            BSP         = 0x01,
            Models      = 0x02,
            ModelNodes  = 0x04,
            Lights      = 0x08,
            All         = BSP | Models | ModelNodes | Lights,
        };

        //**********************************************************
        // value class PickResult
        //**********************************************************
        // holds the results of a raycast.
        value class PickResult
        {
        public:
            SceneEntity^                    PickedEntity;
            MeshInst^                       PickedMeshInst;
            Entities                        PickedType;
            unsigned int                    TriangleIndex;
            TriInfo                         TriangleInfo;
            Vector3                         HitLocation;
            Vector3                         HitNormal;
            float                           U;
            float                           V;
            float                           W;
        };

        property Model^                     Root
        {
            Model^                              get()                   {   return _root;           }
        }

        property Bootstrap::Color           ClearColor
        {
            Bootstrap::Color                    get();
            void                                set( Bootstrap::Color value );
        }

        property IList< Light^ >^           Lights
        {
            IList< Light^ >^                    get()                   {   return GetLights();     }
        }

        property Vector3                    WorldUp
        {
            Vector3                             get();
        }

        property bool                       ZUp
        {
            bool                                get();
        }

        // returns the scene name, e.g. "data/metalroomnew"
        property System::String^            Name
        {
            System::String^                     get();
        }

        // returns the short scene name, e.g. "metalroomnew"
        property System::String^            ShortName
        {
            System::String^                     get();
        }

        property bool                       Loaded 
        { 
            bool                                get()                   {   return _loaded;         } 
        }

        property Model^                     SceneModel
        {
            Model^                              get();
        }

        property int                        LightCount
        {
            int                                 get();
        }

        // intersects a ray with the scene.
        PickResult                          Pick( Ray ray, float distance, Entities whichEntities );

        // light management.
        void                                AddLight( Light^ light );
        Light^                              FindLight( System::String^ id );
        bool                                RemoveLight( System::String^ id );
        bool                                RemoveLight( Light^ light );
        Light^                              GetLight( int idx );

        IList< MeshInst^ >^                 GetVisibleMeshInsts( Camera^ camera );

        IList< MeshInst^ >^                 GetBSPMeshInsts();

        // imports a model into a package.
        bool                                ImportArtModel( System::String^ name, System::String^ targetPackageName,
                                                            System::String^ pathToArtFile, bool findNormalMaps, bool importAnimations );

        // misc. scene methods.
        bool                                Load( System::String^ name );
        void                                Save();
        void                                Clear();

    private:
        IList< Light^ >^                    GetLights();

        // root model.
        Model^                              _root;
        bool                                _loaded;
    };  
}
