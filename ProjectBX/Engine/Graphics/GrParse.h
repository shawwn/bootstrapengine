//----------------------------------------------------------
// File:		GrParse.h
// Author:		Kevin Bray
// Created:		08-07-05
//
// Purpose:		To provide parsers to load graphical resources.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrColor.h"

// forward declarations.
class BscArrayTable;
class BscVarTable;
struct SBscToken;

extern bool					ParseMaterial( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
extern bool					ParseLight( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
extern bool					ParseSceneDef( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );

// default value constants...
#define GR_PARSE_AMBIENT_MOD_DEF		GrColor( 1.0f )
#define GR_PARSE_AMBIENT_ADD_DEF		GrColor( 0.0f )
#define GR_PARSE_EMISSIVE_MOD_DEF		GrColor( 1.0f )
#define GR_PARSE_EMISSIVE_ADD_DEF		GrColor( 0.0f )
#define GR_PARSE_BUMP_MOD_DEF			GrColor( 0.006f )
#define GR_PARSE_BUMP_ADD_DEF			GrColor(-0.003f )
#define GR_PARSE_DIFFUSE_MOD_DEF		GrColor( 1.0f )
#define GR_PARSE_DIFFUSE_ADD_DEF		GrColor( 0.0f )
#define GR_PARSE_SPECULAR_MOD_DEF		GrColor( 2.0f )
#define GR_PARSE_SPECULAR_ADD_DEF		GrColor( 0.0f )
