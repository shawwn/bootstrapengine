//----------------------------------------------------------
// File:		GrPolygonBSPSplit.h
// Author:		Kevin Bray
// Created:		09-12-06
//
// Purpose:		To manage world polygons for fast picking and rendering.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "MPlane.h"
#include "GrPolygonBSPPortalSet.h"
#include "MAABox.h"

// std c++ includes.
#include <vector>
#include <set>

// forward declarations.
class GrPolygonGroup;
class GrCoplanarPolygonGroup;
class GrPolygonUberPatchGroup;
class GrPolygonUberUVMapper;
class GrPolygon;
class UReader;
class UWriter;
class GrMeshInst;
class GrFrustum;
class GrPolygonBSPArea;
class GrCamera;
class GrFrustumSet;
class GrPortal;
class MRay;
class MSphere;

//**********************************************************
// class GrPolygonBSPSplit
//**********************************************************
class GrPolygonBSPSplit
{
public:
	GrPolygonBSPSplit();
	GrPolygonBSPSplit( const GrPolygonGroup& hullPolygons, const GrPolygonGroup& propPolygons,
					   GrPolygonUberUVMapper& mapper, bool calcPVS, bool uberTexture,
					   float uberTexelsPerMeter );
	GrPolygonBSPSplit( UReader& reader, GrPolygonBSPSplit* parent = 0 );
	~GrPolygonBSPSplit();

	// for testing purposes only... temporary.
	GrPolygonBSPSplit( const MPlane& plane ) : _split( plane ), _parent( 0 ), _front( 0 ), _back( 0 ), _frontLeaf( 0 ), _backLeaf( 0 ) { }

	// generates uber mappings.
	void				GenUberMappings( const GrPolygonUberUVMapper& mapper );

	// returns a list of potentially visible leaves from the position passed in.
	void				GetVisibilityData( const MVec3& viewPos, const std::vector< unsigned int >** pvsIndices );

	// BSP node split plane.
	const MPlane&		GetPlane() const			{	return _split;			}

	// child management.
	GrPolygonBSPSplit*	GetFrontChild() const		{	return _front;			}
	GrPolygonBSPSplit*	GetBackChild() const		{	return _back;			}

	// leaf management.
	GrPolygonBSPArea*	GetFrontLeaf() const		{	return _frontLeaf;		}
	GrPolygonBSPArea*	GetBackLeaf() const			{	return _backLeaf;		}

	// gets all leaves in the tree.
	void				GetLeaves( std::vector< GrPolygonBSPArea* >& leaves );

	// finds the point where the given ray hits world geometry.  Returns false if no
	// hit was found.
	bool				GetIntersect( MVec3& hitPos, const MRay& ray, float maxDist );

	// returns all ubertexture data in the hierarchy.
	void				GetUberTexData( std::vector< GrPolygonUberPatchGroup* >& dataArray ) const;

	// save the BSP node.
	void				Save( UWriter& writer );
	void				Load( UReader& reader );

private:
	GrPolygonBSPSplit( const GrPolygonGroup& polygons, const std::vector< MPlane >& planes, const MPlane& split,
					   GrPolygonBSPSplit* parent );

	// builds the BSP node from a group of coplanar polygons.
	void				Build( GrPolygonGroup& polygons, const std::vector< MPlane >& planes, const MPlane& split );

	// chooses a "best" split plane.
	std::vector< MPlane >::iterator		ChooseSplit( std::vector< MPlane >& planes, const GrPolygonGroup& polygons ) const;

	// bounding box management.
	void				CalcBounds();

	// portal building functionality.
	void				BuildPortals();
	bool				ReducePortal( GrPolygonBSPPortalSet* portal );
	void				PushPortalToLeaves( GrPolygonBSPPortalSet* portal );

	// pushes prop geometry to leaves.
	void				PushPropsToLeaves( const GrPolygonGroup& propPolygons );

	// generates ubertexture data.
	void				GenUberTexData( float texelsPerMeter );

	// polygonal methods.
	void				GenUniquePlanes( std::vector< MPlane >& plane, const GrPolygonGroup& polygons );

	// causes leaves to fix any tjunctions created during the splitting process.
	void				FixTJuncs( const std::vector< GrPolygonBSPArea* >& leaves );

	// neighbor graph management.
	void				ConnectNeighborLeaves( const std::vector< GrPolygonBSPArea* >& leaves );
	void				CalcPVS( const std::vector< GrPolygonBSPArea* >& leaves );

	// general purpose internal methods.
	void				Clean();
	void				TempBuildMeshes();
	void				TempClean();

	typedef std::vector< GrPolygonBSPPortalSet* > PortalSetArray;
	typedef std::vector< GrMeshInst* > MeshInstArray;

	// this BSP split.
	MPlane				_split;

	// parent BSP node.
	GrPolygonBSPSplit*	_parent;

	// children.
	GrPolygonBSPSplit*	_front;
	GrPolygonBSPSplit*	_back;
	GrPolygonBSPArea*	_frontLeaf;
	GrPolygonBSPArea*	_backLeaf;

	// our bounding volume.
	MAABox				_aaBox;
};
