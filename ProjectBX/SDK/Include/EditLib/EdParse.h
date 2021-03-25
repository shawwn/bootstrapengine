//----------------------------------------------------------
// File:		EdParse.h
// Author:		Kevin Bray
// Created:		05-17-08
//
// Purpose:		To parse brushes from a file.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// forward declarations.
class BscArrayTable;
class BscVarTable;
struct SBscToken;

extern bool	ParseBrush( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
extern bool ParseBrushPalette( const SBscToken*& stream, const BscArrayTable& arrayTable, const BscVarTable& varTable, tstring& errors );
