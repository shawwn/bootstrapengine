//----------------------------------------------------------
// File:		GmConfig.h
// Author:		Shawn Presser
// Created:		12-20-08
//
// Purpose:		To configure the game at compile-time.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// constants.
#define CHAR_HEIGHT			2.0f		// meters.
#define CHAR_RADIUS			0.5f		// meters.
#define CHAR_MOVE_SPEED		5.0f		// meters per second.
#define CHAR_JUMP_VEL		20.0f
#define CHAR_SPAWN_PT		MVec3( 0.0f, 10.0f, 0.0f ) // meters.
#define GAME_GRAVITY		MVec3( 0.0f, -9.81f, 0.0f ) // meters per second per second.
#define AI_CHAR_COUNT		50
