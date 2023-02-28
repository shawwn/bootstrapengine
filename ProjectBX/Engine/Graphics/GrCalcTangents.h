//----------------------------------------------------------
// File:        GrCalcTangents.h
// Author:      Kevin Bray
// Created:     12-01-08
//
// Purpose:     To calculate tangents for a triangle.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
struct SVec3;
struct SVec2;
class MMat4x4;

// calculates tangents for a given triangle.
extern void GrCalcTangents( SVec3& tangent, SVec3& binormal, const SVec3& p0, const SVec3& p1,
                            const SVec3& p2, const SVec2& tc0, const SVec2& tc1, const SVec2& tc2,
                            bool normalize = true );

// calculate the local-space to texture-space transform.  The tangent
// and binormal must not be normalized, and p0 and tc0 are a position
// and texture-coordinate that come from a vertex.
extern void GrCalcLocalToTextureXForm( MMat4x4& matrix, const SVec3& tangent, const SVec3& binormal,
                                       const SVec3& normal, const SVec3& p0, const SVec2& tc0 );