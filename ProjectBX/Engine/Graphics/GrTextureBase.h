//----------------------------------------------------------
// File:		GrTextureBase.h
// Author:		Kevin Bray
// Created:		10-01-05
//
// Purpose:		To maintain an OpenGL texture object.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrConfig.h"
#include "URefCount.h"
#include "UPath.h"

class GrImage;

//**********************************************************
// class GrTextureBase
//**********************************************************
class GrTextureBase : public URefCount
{
public:
	// possible texture formats.  Note that if a new format is added,
	// modifications must be made to _internalFormatMap, in the GrTexture.cpp
	// and GrTextureCube.cpp.
	enum EFORMAT
	{
		EF_DXT1,
		EF_DXT5,
		EF_DXT5R,
		EF_RGB16,
		EF_RGB24,
		EF_RGB15_A1,	// 5:5:5:1
		EF_RGBA32,
		EF_RGB48F,
		EF_RGBA64F,
		EF_RGBA128F,
		EF_R8,
		EF_RG16,
		EF_L8,
		EF_I8,
		EF_I16F,
		EF_A8,
		EF_LA16,
		EF_LA32F,
		EF_NORMALMAP,	// expands at best to G=X, A=Y. assumes Z is up in TS.
		EF_HIGHDEFNORMALMAP,
		EF_DEPTH,
		EF_DEPTHSTENCIL,
		EF_GUESS,
		EF_COUNT,
	};

	// recommended filtering mode.
	enum EFILTERMODE
	{
		EFM_ANISOTROPIC,
		EFM_TRILINEAR,
		EFM_BILINEAR,
		EFM_NEAREST,
		EFM_COUNT,
	};

	enum ETEXTURETYPE
	{
		ETT_2D,
		ETT_3D,
		ETT_CUBE,
		ETT_RECT,
	};

	// flags
	typedef unsigned int Flags;
	static const unsigned int kNoMipMap = 0x0001;
	static const unsigned int kNoCompress = 0x0008;
	static const unsigned int kBorder = 0x0010;

	GrTextureBase( const UPath& name, unsigned int bindTarget, EFORMAT format, Flags flags, 
				   ETEXTURETYPE type );
	virtual ~GrTextureBase();

	// accessors.
	ETEXTURETYPE		GetType() const						{	return _type;				}
	const UPath&		GetName() const						{	return _name;				}
	EFORMAT				GetFormat() const					{	return _format;				}
	unsigned int		GetFlags() const					{	return _flags;				}		

	unsigned int		GetRedBitCount() const				{	return _redBits;			}
	unsigned int		GetGreenBitCount() const			{	return _greenBits;			}
	unsigned int		GetBlueBitCount() const				{	return _blueBits;			}
	unsigned int		GetAlphaBitCount() const			{	return _alphaBits;			}
	unsigned int		GetLuminanceBitCount() const		{	return _luminanceBits;		}

	void				SetUnloadable( bool unloadable )	{	_unloadable = unloadable;	}
	bool				GetUnloadable() const				{	return _unloadable;			}

	// texture state modifiers.  Note that these will not take effect until
	// a context reset occurs or until the texture is recreated.
	void				SetFlags( Flags flags );

	unsigned int		GetBindTarget() const		{	return _bindTarget;		}

	// binds the texture to the OpenGL texture unit specified [0..15]
	void				Bind( unsigned int texUnit );
	void				Bind( unsigned int texUnit, unsigned int glMinFilter, unsigned int glMagFilter, unsigned int glAddressS,
							  unsigned int glAddressT, float maxAnisotropy = 1.0f, int mipBias = 0 );

	// ensures the texture is not bound to any texture units.
	void				Unbind();

	// This should be called after rendering to all of the faces of a texture.
	// This way, mipmaps will be automatically generated in hardware.  Note
	// that this function will cause the texture to be bound to the actively
	// selected texture unit.  Note that the texture cannot be part of a bound
	// framebuffer object at the time of this function call.  Also note that
	// mipmapping must be enabled for this texture object.
	void				AutoGenMipmaps();

	// recreates the texture.
	void				Recreate();

	// context reset notification.
	void				PreContextReset();
	void				PostContextReset();

	// This is provided in case the OpenGL texture handle is desired.  Use of
	// this function is not recommended.  Straight OpenGL function calls are
	// prohibited in the engine except for in the driver, and this funcion
	// will not be available on all platforms (Win32/Linux/Mac only).
	unsigned int		GetGLHandle() const			{	return _glHandle;		}

protected:
	void				Create();
	bool				WasCreated() const			{	return _created;		}
	unsigned int		GetTexHandle() const		{	return _glHandle;		}
	unsigned int		GetGLFormat( GrImage* image );
	GrImage*			CreateImage( const UPath& name );

	// functionality that gets overridden by the implementation class.
	virtual void		UploadImage( bool mipmap ) = 0;

private:
	UPath				_name;
	unsigned int		_glHandle;
	const EFORMAT		_format;
	unsigned int		_bindTarget;
	unsigned int		_flags;
	unsigned int		_glMinFilter;
	unsigned int		_glMagFilter;
	unsigned int		_glAddressS;
	unsigned int		_glAddressT;
	unsigned int		_redBits;
	unsigned int		_greenBits;
	unsigned int		_blueBits;
	unsigned int		_alphaBits;
	unsigned int		_luminanceBits;
	int					_mipBias;
	float				_maxAnisotropy;
	const ETEXTURETYPE	_type;
	bool				_unloadable;
	bool				_created;
};
