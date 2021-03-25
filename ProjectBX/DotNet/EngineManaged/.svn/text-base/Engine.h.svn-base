//----------------------------------------------------------
// File:		Engine.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To expose a way of interacting with the graphics engine.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class MVec3;
class GrMeshInst;
class EdUberTexRasterizer;

namespace Bootstrap 
{
	// .NET forward declarations.
	ref class Scene;
	ref class Camera;
	ref class TimeKeeper;
    value class OBox;
    value class Color;
    value class Vector3;
    value class AABox;

    //**********************************************************
    // ref class Engine
    //**********************************************************
	public ref class Engine
	{
	internal:
        Engine( System::Windows::Forms::Control^ renderOnto, System::String^ workingFolder );

	public:
		~Engine();
		!Engine();

        // delegates.
		delegate void						OutputCallback( System::String^ message );

		// starts up the engine. If 'workingFolder' is null, then 'Bootstrap.Settings.GetWorkingFolder()' is used.
        static Engine^						Startup( System::Windows::Forms::Control^ renderOnto, OutputCallback^ outputCallback, System::String^ workingFolder );

		// outputs any pending engine messages to the OutputCallback.
		static void							PumpOutputMessages();

		// gets a pointer to the engine.
		static Engine^						Get();

		// returns true if initialization was successful.
		property bool						Initialized 
		{
            bool                                get()               {   return _renderWindow != nullptr;    }
		}

		// returns the last encountered error.
		property System::String^			ErrorMessage
		{
            System::String^                     get()               {   return _lastError;                  }
		}

		// returns the active scene.
		property Bootstrap::Scene^			Scene
		{
            Bootstrap::Scene^                   get()			    {   return _scene;                      }
		}

		// returns the screen resolution.
		property System::Drawing::Size		WindowSize
		{
            System::Drawing::Size               get()		        {   return _renderWindow->Size;         }
		}

		// sets the rendering mode to wireframe.
		property bool						RenderWireframe
		{
			bool                                get();
			void                                set( bool renderWireframe );
		}

        // returns the system time delta between engine Update() calls.
        property int                        SysTimeDelta
        {
            int                                 get();
        }

        // returns the game time delta between engine Update() calls.
        property int                        GameTimeDelta
        {
            int                                 get();
        }

		// returns whether or not the engine is ready for use.
		bool								IsReady();

		bool								ImportArtScene( System::String^ targetName, System::String^ pathToArtFile,
                											bool findNormalMaps, bool uberTexture,
                											bool overwriteMaterials, bool overwriteLights );

		void								LoadBrushScript( System::String^ filePath, System::String^ outErrors );
		void								LoadBrushPaletteScript( System::String^ filePath, System::String^ outErrors );

		void								LoadMaterialScript( System::String^ filePath, System::String^ outErrors );

		// time management
		void								TimePause();
		void								TimeResume();

		// updates the engine.  (currently keeps track of delta time internally.)
		void								Update();

		// renders the scene.  Do any special-case rendering code after you call BeginRender but before you call EndRender.
		void								BeginRender( Camera^ camera );
		void								EndRender();

		// resizes the backbuffer.
		void								Resize( unsigned int width, unsigned int height );

        // clears the depth buffer.
		void								ClearDepthBuffer();

		// presents the framebuffer.  Call this after calling Render.
		void								Present();

        // initializes OpenGL states and matrices.
		void								SetupStates( Camera^ camera, bool wireframe, bool depthTest );

		// basic drawing services.
		void								DrawSphere( const Vector3& pos, float radius, Color color );
		void								DrawSphere( Vector3 pos, float radius, Color color, int slices );
		void								DrawLine( Vector3 start, Vector3 end, Color color );
		void								DrawAABox( AABox box, Color color );
		void								DrawOBox( OBox box, Color color );
		void								DrawPoint( Camera^ camera, Vector3 pos, Color color, int sizeInPixels );

		// console services.
		void								ProcessConsoleMessage( System::String^ message );

	private:
		void								HandleConsoleMessage( const std::string& wholeMessage, const std::vector< tstring >& tokens );

		static OutputCallback^				_outputCallback;
		static Engine^						_singleton;

		System::Windows::Forms::Control^	_renderWindow;
		System::String^						_lastError;
		TimeKeeper^							_timeKeeper;
		Bootstrap::Scene^					_scene;
		bool								_disposed;
	};
}
