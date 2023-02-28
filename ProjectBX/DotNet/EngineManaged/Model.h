//----------------------------------------------------------
// File:		Model.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage a model consisting of multiple meshes.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "SceneEntity.h"
#include "AABox.h"

// forward declarations.
class GrModel;

// .NET namespaces.
using namespace System::Collections::Generic;

namespace Bootstrap
{
	// forward declarations.
	ref class MeshInst;
	ref class ModelNode;
	ref class Light;

    //**********************************************************
    // ref class Model
    //**********************************************************
	public ref class Model : public SceneEntity
	{
	internal:
		Model( GrModel* model );

		// model.
		GrModel*		                    _model;

	public:
		static Model^	                    Create( System::String^ filePath, System::String^ instName, bool clone );

		~Model();
		!Model();

        property AABox                      Bounds              
        { 
            AABox                               get();  
        }

		property System::String^            FileName 
        { 
            System::String^                     get();  
        }

		property System::String^            InstanceName        
        { 
            System::String^                     get();  
        }

        property bool                       Pickable 
        { 
            bool                                get(); 
            void                                set( bool value ); 
        }

        // clones the model.  If deep is true, then the geometry is cloned as well and
        // the object is treated as an original, not a clone.
        Model^                              Clone( System::String^ instName, bool deep );

        // clones the model and the file it originated from.
		Model^			                    CloneFile( System::String^ fileName, System::String^ instName );

		property bool                       InSkybox
		{
			bool                                get();
		}

        property Model^                     Parent 
        { 
            Model^                              get();  
        }

        property ModelNode^                 ParentNode 
        { 
            ModelNode^                          get();  
        }

        // TODO: create a .NET AnimPlayer wrapper.
        // animation.  GetOrCreateAnimPlayer will create an animation
        // player if one does not exist.
        //property AnimPlayer^                TheAnimPlayer 
        //{ 
        //    AnimPlayer^                         get();  
        //}
        //AnimPlayer^                         GetOrCreateAnimPlayer();

        // TODO: create a .NET KFAnim wrapper.
        // ambient animation.
        //property KFAnim^                       DefaultAnim         
        //{ 
        //    KFAnim^                                get();
        //    void                                   set( KFAnim^ anim ); 
        //}

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

        // node manipulation.
		property ModelNode^                 RootNode 
        { 
            ModelNode^                          get();  
        }

		property int                        ChildCount          
        { 
            int                                 get();  
        }

		property int                        LightCount          
        { 
            int                                 get();  
        }

        // adds a child model.  'attachTo' can be null.
        void                                AddChild( Model^ model, ModelNode^ attachTo );
		Model^                              GetChild( int idx );
        bool                                RemoveChild( Model^ model );
        void                                RemoveChildren();

		// adds a light.  'attachTo' can be null.
		void								AddLight( Light^ light, ModelNode^ attachTo );
		Light^								GetLight( int idx );
		ModelNode^							GetLightParent( int idx );
		bool								RemoveLight( Light^ light );
		void								RemoveLights();

		void			                    AddToSkybox();

        // recursively finds a model by its instance name or its file name.
        Model^                              FindModelByInstName( System::String^ instName );
        array< Model^ >^                    FindModelsByFileName( System::String^ fileName );

        // retrieves all of the mesh instances from the model.
		property array< MeshInst^ >^        MeshInsts           
        { 
            array< MeshInst^ >^                 get();  
        }

        // debug functionality.
        void                                DbgDrawHierarchy();

        // load and save the model.  This does not save children.
		void			                    Save();

        // comparator.
		virtual bool                        Equals( System::Object^ obj ) override;

	protected:
		// inherited from SceneEntity.
		virtual void	                    ApplyTransform( Matrix mat ) override;
		virtual Matrix	                    GetTransform() sealed override;
	};
}
