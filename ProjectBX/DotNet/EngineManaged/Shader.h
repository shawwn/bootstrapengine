//----------------------------------------------------------
// File:		Shader.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To load and manage a shader.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrShader;

namespace Bootstrap
{
    //**********************************************************
    // ref class Shader
    //**********************************************************
	public ref class Shader
	{
	internal:
		Shader( GrShader* shader );

		GrShader*	                        _shader;

	public:
		Shader( System::String^ name );
		~Shader();
		!Shader();

		// returns a list of shaders.
		static System::Collections::Generic::List< System::String^ >^ GetAmbientShaderList();
		static System::Collections::Generic::List< System::String^ >^ GetLightShaderList();

		// information about the shader.
		property System::String^            Name
		{
			System::String^                     get();
		};

	private:
		static System::Collections::Generic::List< System::String^ >^ GetShaderList( System::String^ path,
			System::String^ vshExt, System::String^ fshExt, System::String^ vshFilter );
	};
}
