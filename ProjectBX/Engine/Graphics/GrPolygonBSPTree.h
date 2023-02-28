//----------------------------------------------------------
// File:		GrPolygonBSPTree.h
// Author:		Kevin Bray
// Created:		09-30-06
//
// Purpose:		To manage a tree of BSP nodes.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// std c++ includes.
#include <vector>

class GrPolygonGroup;
class GrPolygonBSPSplit;
class GrPolygonBSPArea;
class GrPolygonUberPatchGroup;
class GrPolygonUberUVMapper;
class UReader;
class UWriter;
class GrFrustum;
class GrColor;
class MSphere;
class MVec3;
class MRay;

//**********************************************************
// class GrPolygonBSPTree
//**********************************************************
class GrPolygonBSPTree
{
public:
	typedef std::vector< GrPolygonBSPArea* > AreaVector;

	GrPolygonBSPTree();
	GrPolygonBSPTree( const GrPolygonGroup& hullPolygons, const GrPolygonGroup& propPolygons,
					  GrPolygonUberUVMapper& mapper, bool calcPVS = true,
					  bool uberTexture = true, float uberTexelsPerMeter = 1024 );
	GrPolygonBSPTree( UReader& reader );
	~GrPolygonBSPTree();

	// gets all leaves in the tree.
	unsigned int			GetLeafCount() const	{	return ( unsigned int )_leaves.size();	}
	AreaVector&				GetLeaves()				{	return _leaves;							}
	const AreaVector&		GetLeaves()	const		{	return _leaves;							}

	// finds the point where the given ray hits world geometry.  Returns false if no
	// hit was found.
	bool					GetIntersect( MVec3& hitPos, const MRay& ray, float maxDist );

	// builds a new BSP tree from the polygons passed in.
	void					Build( const GrPolygonGroup& hullPolygons, const GrPolygonGroup& propPolygons,
								   GrPolygonUberUVMapper& mapper, bool calcPVS, bool uberTexture,
								   float uberTexelsPerMeter );

	// packs ubertexture patches into an ubertexture.
	void					GetUberTexData( std::vector< GrPolygonUberPatchGroup* >& dataArray ) const;

	// returns the root BSP node.
	GrPolygonBSPSplit*		GetRoot() const			{	return _root;							}

	// load/save.
	void					Load( UReader& reader );
	void					Save( UWriter& writer );

private:
	void					BuildLeafVector();
	void					Clean();

	GrPolygonBSPSplit*		_root;
	AreaVector				_leaves;
	mutable unsigned int	_frameId;
	bool					_cached;
};
