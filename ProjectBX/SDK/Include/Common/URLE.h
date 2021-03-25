//----------------------------------------------------------
// File:		URLE.h
// Author:		Kevin Bray
// Created:		11-17-2008
//
// Purpose:		To encode and decode RLE compressed data.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class URLE
//**********************************************************
class URLE
{
public:
	URLE();
	~URLE();

	// compression routines.
	unsigned int	CalcMaxCompSize( unsigned int srcSize );
	unsigned int	Compress( unsigned char* dst, const unsigned char* src, unsigned int srcSize );

	// decompression routines.
	void			Decompress( unsigned char* dst, unsigned int dstSize, const unsigned char* src );
};
