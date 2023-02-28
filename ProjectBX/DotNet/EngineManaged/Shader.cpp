//----------------------------------------------------------
// File:        Shader.cpp
// Author:      Shawn Presser
// Created:     09-23-08
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "EMAfx.h"

// class header.
#include "Shader.h"

// project headers.
#include "Utils.h"
#include "Settings.h"

// engine headers.
#include "GrShaderMgr.h"
#include "GrShader.h"
#include "GrConfig.h"

// .NET namespaces.
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

//**********************************************************
// ref class Shader
//**********************************************************

//----------------------------------------------------------
Bootstrap::Shader::Shader( GrShader* shader )
: _shader( shader )
{
    B_ASSERT( shader != 0 )
    _shader->IncRef();
}

//----------------------------------------------------------
Bootstrap::Shader::Shader( String^ name )
: _shader( 0 )
{

}

//----------------------------------------------------------
Bootstrap::Shader::~Shader()
{
    this->!Shader();
}

//----------------------------------------------------------
Bootstrap::Shader::!Shader()
{
    _shader->DecRef();
}

//----------------------------------------------------------
List< String^ >^ 
Bootstrap::Shader::GetAmbientShaderList()
{
    // get the shaders and add in the default ambient shader.
    List< String^ >^ ambientShaders = GetShaderList( Settings::Get()->GetShaderFolder(), ".vsh", ".fsh", "*_amb.vsh" );
    ambientShaders->Insert( 0, GR_SHADER_DEFAULT_AMBIENT );

    // return the list of ambient shaders.
    return ambientShaders;
}

//----------------------------------------------------------
List< String^ >^ 
Bootstrap::Shader::GetLightShaderList()
{
    // get the ambient shaders.
    List< String^ >^ exclusionList = GetAmbientShaderList();

    // get all shaders.
    List< String^ >^ shaderList = GetShaderList( Settings::Get()->GetShaderFolder(), ".vsh", ".fsh", "*.vsh" );

    // add the default light shader.
    shaderList->Insert( 0, GR_SHADER_DEFAULT_LIGHT );

    // remove ambient shaders from the shaderList.
    for each ( String^ name in exclusionList )
        shaderList->Remove( name );

    // return the shader list.
    return shaderList;
}

//----------------------------------------------------------
String^ 
Bootstrap::Shader::Name::get()
{
    return TStrToStr( _shader->GetName().GetPathString() );
}

//----------------------------------------------------------
List< String^ >^ 
Bootstrap::Shader::GetShaderList( String^ path, String^ vshExt, String^ fshExt, String^ vshFilter )
{
    // get a file listing for the shaders folder.
    DirectoryInfo^ dirInfo = gcnew DirectoryInfo( path );
    cli::array< FileInfo^ >^ files = dirInfo->GetFiles( vshFilter );
    List< String^ >^ shaders = gcnew List< String^ >();

    // now iterate through every file and make sure there is a matching pixel shader.
    for each( FileInfo^ file in files )
    {
        String^ shaderName = file->FullName->Substring(0, file->FullName->Length - file->Extension->Length);
        FileInfo^ fragShader = gcnew FileInfo( shaderName + fshExt );
        if ( fragShader->Exists )
        {
            int index = shaderName->LastIndexOf( "\\" );
            shaders->Add( shaderName->Substring( index + 1 ) );
        }
    }

    // return the shaders.
    return shaders;
}