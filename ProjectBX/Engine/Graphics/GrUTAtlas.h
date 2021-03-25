//----------------------------------------------------------
// File:		GrUTAtlas.h
// Author:		Kevin Bray
// Created:		11-17-2008
//
// Purpose:		To provide tile locating services.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class UBitReader;

//**********************************************************
// class GrUTAtlas
//**********************************************************
class GrUTAtlas
{
public:
	GrUTAtlas( unsigned int* tileUsage, unsigned int baseChunk );
	~GrUTAtlas();

	// get the tile's address.  If the tile is found, then it's chunk
	// is return.  If the tile is not found, then the return value is
	// ~0.
	unsigned int		FindTileChunk( unsigned short mip, unsigned short x, unsigned short y ) const;

	// returns the number of tiles for the given node.
	unsigned int		GetMaxTileCount() const;

	// tree traversal.
	bool				IsLeaf() const;
	bool				IsEmpty() const;
	GrUTAtlas*			GetChild( unsigned int idx ) const;
	unsigned int		GetChunk() const;

private:
	// note that this function signature contains a bool to avoid
	// conflicting with the public constructor.
	GrUTAtlas( unsigned int* tileUsage, unsigned int maxTileCount, unsigned int depth );
	GrUTAtlas( UBitReader& bitReader, unsigned int maxTileCount, unsigned int depth );

	// get the tile's address.  If the tile is found, then it's chunk
	// is return.  If the tile is not found, then the return value is
	// ~0.
	unsigned int		FindTileChunkInternal( unsigned short mip, unsigned short x, unsigned short y ) const;

	// builds the hierarchy.
	void				BuildTreeFast( unsigned int* tileUsage, unsigned int maxTileCount );
	void				CalcTileAddress( unsigned int& startAddress, unsigned int sizeInTiles );

	unsigned int		_chunk;
	unsigned short		_flags;
	unsigned short		_invDepth;
	GrUTAtlas*			_children[ 4 ];
};
