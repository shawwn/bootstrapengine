//----------------------------------------------------------
// File:		Material.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To describe how an object should be rendered.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class GrMaterial;

namespace Bootstrap
{
	// .NET forward declarations.
	ref class MaterialPass;

    //**********************************************************
    // ref class Material
    //**********************************************************
	public ref class Material
	{
	internal:
		Material( GrMaterial* material );

		GrMaterial*		                    _material;

	public:
		Material( System::String^ name );

		~Material();
		!Material();

		static array< System::String^ >^    GetLoadedMaterialNames();

		property System::String^            Name
		{
			System::String^                     get();
		}

		property System::String^            ShortName
		{
			System::String^                     get();
		}

		// stipple support.
		property bool                       Stipple
		{
			bool                                get();
			void                                set( bool value );
		}

		// pass management.
		property int	                    PassCount
		{
			int                                 get();
		}
		MaterialPass^	                    GetPass( int passIdx );
		void			                    AddPass();

		// post process pass management.
		property int	                    PostPassCount
		{
			int                                 get();
		}
		MaterialPass^	                    GetPostPass( int passIdx );
		void			                    AddPostPass();

		// deformer management.
		// TODO: Add support for deformers.

	private:
		void			                    CachePasses();
		void			                    CachePostPasses();

		array< MaterialPass^ >^		        _passes;
		array< MaterialPass^ >^		        _postPasses;
	};
}
