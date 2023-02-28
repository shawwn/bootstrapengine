// Copyright (c) 2007 Bootstrap Studios LLC.  All rights reserved.
#pragma once

// TGA file header.
#pragma pack ( push )
#pragma pack ( 1 )
struct STGAHeaderInfo
{
	unsigned char	idlen;    // length of optional identification sequence
	unsigned char	cmtype;   // indicates whether a palette is present
	unsigned char	imtype;   // image data type ( e.g., uncompressed RGB )
	unsigned short	cmorg;    // first palette index, if present
	unsigned short	cmcnt;    // number of palette entries, if present
	unsigned char	cmsize;   // number of bits per palette entry
	unsigned short	imxorg;   // horiz pixel coordinate of lower left of image
	unsigned short	imyorg;   // vert pixel coordinate of lower left of image
	unsigned short	imwidth;  // image width in pixels
	unsigned short	imheight; // image height in pixels
	unsigned char	imdepth;  // image color depth ( bits per pixel )
	unsigned char	imdesc;   // image attribute flags
};
#pragma pack ( pop )

// TGA class.
class TGAImage
{
public:
	TGAImage( const void* data );
	~TGAImage();

	unsigned int		GetWidth() const		{	return _width;			}
	unsigned int		GetHeight() const		{	return _height;			}
	unsigned char*		GetBits() const			{	return _bits;			}
	unsigned int		GetBytesPerPel() const	{	return _bytesPerPel;	}

	void				Swizzle();

private:
	void ReadRLE( const unsigned char* src );
	void ReadRaw( const unsigned char* src );

	unsigned int		_width;
	unsigned int		_height;
	unsigned int		_bytesPerPel;
	unsigned char*		_bits;
};