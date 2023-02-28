//----------------------------------------------------------
// File:		ClMain.cpp
// Author:		Shawn Presser
// Created:		12-15-08
//
// Purpose:		The entry point of the game client.
//
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

#ifdef WIN32
// windows headers.
#include <windows.h>
#endif

// GL headers.
#include <GL/GL.h>

// std c headers.
#include <stdio.h>

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/URef.h"
#include "Common/UExprMgr.h"
#include "Common/RFileMgr.h"
#include "Common/RRsrcMgr.h"
#include "Common/URand.h"
#include "Graphics/GLSubsys.h"
#include "Graphics/GrSubsys.h"
#include "Graphics/GrScene.h"
#include "Graphics/GrCamera.h"
#include "Graphics/GrModel.h"
#include "Graphics/GrUtil.h"
#include "Graphics/GrRenderUtil.h"
#include "Graphics/GrColor.h"

// PhysX headers.
#include "NxPhysics.h"

// game headers.
#include "Game/GmConfig.h"
#include "Game/GmSubsys.h"
#include "Game/GmEntityMgr.h"
#include "Game/GmEntity.h"
#include "Game/GmInputController.h"
#include "Game/GmAI.h"
#include "Game/PhSubsys.h"

// project headers.
#include "ClPlatform.h"

// TODO: remove.
#include "Editor/EdImportSourceEngine.h"

#ifdef _MSC_VER
// libraries.
#ifndef _DEBUG
// release bootstrap engine libraries.
#pragma comment( lib, "Common.lib" )
#pragma comment( lib, "FileSystem.lib" )
#pragma comment( lib, "Graphics.lib" )
#pragma comment( lib, "UberTexture.lib" )
#pragma comment( lib, "EditLib.lib" )
#else
// debug bootstrap engine libraries.
#pragma comment( lib, "CommonD.lib" )
#pragma comment( lib, "FileSystem.lib" )
#pragma comment( lib, "GraphicsD.lib" )
#pragma comment( lib, "UberTextureD.lib" )
#pragma comment( lib, "EditLibD.lib" )
#endif
// PhysX.
#pragma comment( lib, "PhysXLoader.lib" )
#pragma comment( lib, "NxCooking.lib" )
#pragma comment( lib, "NxCharacter.lib" )
#endif

// constants.
#define WINDOW_WIDTH		1024		// pixels.
#define WINDOW_HEIGHT		768			// pixels.
#define CAMERA_FOV			60.0f		// degrees.
#define CAMERA_NEAR_PLANE	0.1f		// meters.
#define CAMERA_FAR_DIST		1000.0f		// meters.
#define CAMERA_MOVE_SPEED	10.0f		// meters per second.
#define CAMERA_ROT_SPEED	0.3f		// degrees per mouse pixel.
#define CAMERA_MAX_INCLINE	85.0f		// degrees.
#define CAMERA_ORBIT_RADIUS	4.0f		// meters.

// enable this to visualize physics debug info.
#define VISUALIZE_PHYSICS	0

#define PHYSICS_ALPHA		0.90f

// forward declarations.
bool						ClStartup( ClWindow* window, int argc, char** argv, const char* windowTitle );
void						ClUpdate( ClWindow* window, unsigned int clock, unsigned int dt );
void						ClRender( ClWindow* window );
void						ClShutdown();
void						ClRenderPhysicsVisualizers( const GrCamera& cam, float alpha = 1.0f );

// window callback forward declarations.
void						ClResize( ClWindow* window, int width, int height );
void						ClClose( ClWindow* window );
void						ClKeyDown( ClWindow* window, char key );
void						ClKeyUp( ClWindow* window, char key );
void						ClMouseMove( ClWindow* window, int x, int y );
void						ClMouseDown( ClWindow* window, int button );
void						ClMouseUp( ClWindow* window, int button );

// game globals.
URand*						gRand;
GmInputController*			gGameInput;
GmCharacter*				gChar;
GrCamera*					gGameCamera;
float						gGameCameraYaw;
float						gGameCameraPitch;
unsigned int				gCharJump;
bool						gCharJumping;
int							gWindowWidth;
int							gWindowHeight;
int							gMouseX;
int							gMouseY;
bool						gDone;
bool						gKeysDown[ 256 ];

//**********************************************************
// main
//**********************************************************

//----------------------------------------------------------
#ifdef WIN32
// windows.
int WINAPI
WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
#else
// other platforms.
int
main( int argc, char** argv )
#endif
{
	// initialize 1ms timing precision.
	timeBeginPeriod( 1 );

	// seed the random number generator.
	gRand = new URand( (int)GetTickCount() );

	// initialize a platform-specific game window.
	new ClPlatform();
	ClWindowStyle* style = ClWindowStyle::MakeWindowStyle( L"Game", false, true );
	ClWindow* window = gClPlatform->MakeWindow( style, L"", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT );
	gWindowWidth = WINDOW_WIDTH;
	gWindowHeight = WINDOW_HEIGHT;

	// hide the mouse cursor.
	window->SetCursorVisible( false );

	// move the mouse cursor to the middle of the screen.
	window->WarpMousePointer( gWindowWidth/2, gWindowHeight/2 );
	gMouseX = gWindowWidth/2;
	gMouseY = gWindowHeight/2;

	if ( window )
	{
		window->SetCloseFunc( &ClClose );
		window->SetResizeFunc( &ClResize );
		window->SetKeyDownFunc( &ClKeyDown );
		window->SetKeyUpFunc( &ClKeyUp );
		window->SetMouseMoveFunc( &ClMouseMove );
		window->SetMouseDownFunc( &ClMouseDown );
		window->SetMouseUpFunc( &ClMouseUp );

		// initialize the game.
		if ( ClStartup( window, 0, 0, "" ) )
		{
			const unsigned int kFPS = 50;
			const unsigned int kFrameDuration = 1000 / kFPS;
			unsigned int startTime = timeGetTime();
			unsigned int prevTime = 0;
			unsigned int nextRenderTime = kFrameDuration;

			// enter the main game loop.
			while ( !gDone )
			{
				// determine whether we should update and render.
				unsigned int curTime = timeGetTime() - startTime;
				if ( curTime >= nextRenderTime )
				{
					// compute the time delta from the last frame.
					unsigned int dt = curTime - prevTime;
					prevTime = curTime;

					// compute when the next frame should be rendered.
					nextRenderTime = (curTime/kFrameDuration + 1)*kFrameDuration;

					// update the game.
					ClUpdate( window, curTime - startTime, dt );

					// render the graphics.
					ClRender( window );
				}

				// process window events.
				window->Update();
			}

			// shutdown the game.
			ClShutdown();
		}
	}

	// shutdown the platform-specific game window.
	delete window;
	delete style;
	delete gClPlatform;

	// shutdown 1ms timing precision.
	timeEndPeriod( 1 );

	return 0;
}

//**********************************************************
// Client functions
//**********************************************************

//----------------------------------------------------------
bool
ClStartup( ClWindow* window, int argc, char** argv, const char* windowTitle )
{
	// initialize global variables.
	gDone = false;

	// initialize the game subsystem.
	new GmSubsys();

#ifdef WIN32
	// initialize the graphics subsystem.
	// TODO: rewrite this to be cross-platform.
	StartGL( window->GetDeviceContext() );
#else
#error "Non-Windows platforms are not currently supported."
#endif

	// initialize the graphics subsystem.
	new GrSubsys( WINDOW_WIDTH, WINDOW_HEIGHT, false, true );

	// TODO: figure out why it is necessary to update the graphics subsystem
	// before loading a scene.
	gGrSubsys->Update( 1, 1 );

	// TODO: just an importer test.
	{{
		EdImportSourceEngine importSE;
		const float kScale = 0.025f;
		//importSE.ImportModel( "francis", "plane_playground", "G:\\prog\\media\\left4dead\\models\\survivors\\survivor_biker", kScale );
		importSE.ImportModel( "louis", "plane_playground", "G:\\prog\\media\\left4dead\\models\\survivors\\survivor_manager", kScale );
		importSE.ImportModel( "bill", "plane_playground", "G:\\prog\\media\\left4dead\\models\\survivors\\survivor_namvet", kScale );
		importSE.ImportModel( "zoey", "plane_playground", "G:\\prog\\media\\left4dead\\models\\survivors\\survivor_teenangst", kScale );
		//importSE.ImportModel( "boomer", "plane_playground", "G:\\prog\\media\\left4dead\\models\\infected\\boomer", kScale );
		//importSE.ImportModel( "piano", "plane_playground", "G:\\prog\\media\\left4dead\\models\\props_furniture\\piano", kScale );
		//importSE.ImportModel( "wood_board05a", "plane_playground", "G:\\prog\\media\\left4dead\\models\\props_debris\\wood_board05a", kScale );

		// import some sandbags.
		importSE.ImportModel( "sandbags_corner2", "props_fortifications", "G:\\prog\\media\\left4dead\\models\\props_fortifications\\sandbags_corner2", kScale );
		importSE.ImportModel( "sandbags_corner3", "props_fortifications", "G:\\prog\\media\\left4dead\\models\\props_fortifications\\sandbags_corner3", kScale );
		importSE.ImportModel( "sandbags_line2", "props_fortifications", "G:\\prog\\media\\left4dead\\models\\props_fortifications\\sandbags_line2", kScale );

		// boss infected.
		importSE.ImportModel( "hulk", "l4d_infected", "G:\\prog\\media\\left4dead\\models\\infected\\hulk", kScale );
		importSE.ImportModel( "boomer", "l4d_infected", "G:\\prog\\media\\left4dead\\models\\infected\\boomer", kScale );
		importSE.ImportModel( "smoker", "l4d_infected", "G:\\prog\\media\\left4dead\\models\\infected\\smoker", kScale );
		importSE.ImportModel( "hunter", "l4d_infected", "G:\\prog\\media\\left4dead\\models\\infected\\hunter", kScale );
		importSE.ImportModel( "witch", "l4d_infected", "G:\\prog\\media\\left4dead\\models\\infected\\witch", kScale );

		return false;
	}}

	// load a scene.
	gGmSubsys->LoadScene( "data/plane_playground" );

	// initialize game state.
	B_ASSERT( !gGameCamera );
	gGameCamera = new GrCamera( CHAR_SPAWN_PT, MMat3x3(), 
						GrProjection( DegToRad( CAMERA_FOV ), CAMERA_FAR_DIST, WINDOW_WIDTH / (float)WINDOW_HEIGHT,
							MPlane( MVec3( 0.0f, 0.0f, -1.0f ), MVec3( 0.0f, 0.0f, -CAMERA_NEAR_PLANE ) ) ) );

	// initialize controller inputs.
	B_ASSERT( !gGameInput );
	gGameInput = new GmInputController();
	gGameInput->GetInput( "move", EIT_VECTOR );
	gGameInput->GetInput( "rotation", EIT_VECTOR );

	// create a character controller.
	GmCharacter::SInfo info;
	info.modelFilePath = UPath( "data/plane_playground/models/cube_1x1x1.mdl" );
	info.radius = CHAR_RADIUS;
	info.height = CHAR_HEIGHT;
	info.speed = CHAR_MOVE_SPEED;
	info.position = CHAR_SPAWN_PT;
	info.inputController = gGameInput;
	gChar = gGmEntityMgr->CreateCharacter( "Pickles", info );

	// create a bunch of AI characters.
	info.inputController = 0;
	info.position.SetX( -40.0f );
	for ( unsigned int i = 0; i < AI_CHAR_COUNT; ++i )
	{
		// build the AI's name.
		tstring name( "Zombie" );
		name << ( i + 1 );

		// create it.
		info.position += MVec3( (float)i, 0.0f, 0.0f );
		gGmEntityMgr->CreateAICharacter( name, info );
	}

	// return true to indicate the game successfully initialized.
	return true;
}

//----------------------------------------------------------
void
ClUpdate( ClWindow* window, unsigned int clock, unsigned int dt )
{
	const float t = dt / 1000.0f;

	// rotate the camera by the mouse.
	if ( gMouseX != gWindowWidth/2 || gMouseY != gWindowHeight/2 )
	{
		float dx = CAMERA_ROT_SPEED * ( gWindowWidth/2 - gMouseX );
		float dy = CAMERA_ROT_SPEED * ( gWindowHeight/2 - gMouseY );
		gGameCameraYaw += dx;
		gGameCameraPitch += dy;

		// clamp the pitch so that the user can't lean up or down further 
		// than a certain threshold.
		if ( gGameCameraPitch >  CAMERA_MAX_INCLINE ) gGameCameraPitch =  CAMERA_MAX_INCLINE;
		if ( gGameCameraPitch < -CAMERA_MAX_INCLINE ) gGameCameraPitch = -CAMERA_MAX_INCLINE;

		// update the camera's rotation.
		gGameCamera->SetRot( MMat3x3( DegToRad( gGameCameraYaw ), DegToRad( gGameCameraPitch ), 0.0f ) );

		// update the player character's heading.
		gGameInput->FindInput( "rotation", EIT_VECTOR )->SetValue( MVec3( DegToRad( gGameCameraYaw ), 0.0f, 0.0f ) );

		// move the mouse back to the middle of the screen.
		window->WarpMousePointer( gWindowWidth/2, gWindowHeight/2 );
		gMouseX = gWindowWidth/2;
		gMouseY = gWindowHeight/2;
	}

	if ( !gCharJumping && gKeysDown[ ' ' ] )
	{
		gCharJumping = true;
	}

	// jumping?
	if ( gCharJumping )
	{
		gCharJump += dt;
		if ( gCharJump > 1000 )
		{
			gCharJump = 1000;
			gCharJumping = false;
		}
	}
	else if ( !gCharJumping && gCharJump > 0 )
	{
		if ( dt > gCharJump )
			gCharJump = 0;
		else
			gCharJump -= dt;
	}

	// move the character by keyboard input.
	MVec3 moveDir( 0.0f, 0.0f, 0.0f );
	if ( gKeysDown[ 'W' ] ) moveDir += gGameCamera->GetLookDir();
	if ( gKeysDown[ 'S' ] ) moveDir -= gGameCamera->GetLookDir();
	if ( gKeysDown[ 'D' ] ) moveDir += gGameCamera->GetRot().GetCol( 0 );
	if ( gKeysDown[ 'A' ] ) moveDir -= gGameCamera->GetRot().GetCol( 0 );
	if ( moveDir.MagSqr() > 0.0f )
	{
		moveDir.Normalize();
	}

	// compute the initial character displacement.
	MVec3 charDisplacement = CHAR_MOVE_SPEED*t*moveDir;

	// add the jumpjet effect.
	charDisplacement += t*MVec3( 0.0f, ( gCharJump / 1000.0f )*CHAR_JUMP_VEL, 0.0f );
	
	// add gravity.
	charDisplacement += t*GAME_GRAVITY;

	// move entities.
	gGameInput->FindInput( "move", EIT_VECTOR )->SetValue( charDisplacement );

	// update the game simulation.
	gGmSubsys->Update( dt );

	// notify entities to respond to inputs.
	gGameInput->Update();

	// set the camera position to the char controller position.
	gGameCamera->SetPos( gChar->GetPos() - CAMERA_ORBIT_RADIUS*gGameCamera->GetLookDir() );

	// update the graphics.
	gGrSubsys->Update( dt, dt );
}

//----------------------------------------------------------
void
ClRender( ClWindow* window )
{
	// draw the graphics.
	gGrSubsys->Render( gGameCamera );

	// draw other graphics.
	{{
		unsigned int mask = GrGetWriteEnable();
		GrSetWriteEnable( GR_DEPTH );
		GrClear();
		GrSetWriteEnable( mask );

		gGrRenderUtil->SetupColoredRender( *gGameCamera, true, true );

		// render the AI's destinations.
		gGmEntityMgr->DebugRender();

#if VISUALIZE_PHYSICS
		// render a visualization of the scene's physics.
		gPhSDK->setParameter( NX_VISUALIZATION_SCALE, 1.0f );
		gPhSDK->setParameter( NX_VISUALIZE_BODY_MASS_AXES, 1.0f );
		gPhSDK->setParameter( NX_VISUALIZE_COLLISION_SHAPES, 1.0f );
		gPhSDK->setParameter( NX_VISUALIZE_COLLISION_AXES, 1.0f );
		gPhSDK->setParameter( NX_VISUALIZE_CONTACT_FORCE, 3.0f );
		ClRenderPhysicsVisualizers( *gGameCamera, PHYSICS_ALPHA );
#endif
	}}

	// swap the window's back buffer (that we're drawing to) with
	// the window's front buffer (that the user currently sees).
	window->SwapBuffers();
}

//----------------------------------------------------------
void
ClShutdown()
{
	// shutdown the character.
	gChar = 0;

	// shutdown game state.
	delete gGameCamera; gGameCamera = 0;
	delete gGameInput; gGameInput = 0;

	// shutdown the game subsystem.
	delete gGmSubsys;

	// shutdown the graphics subsystem.
	delete gGrSubsys;
}

//----------------------------------------------------------
void
ClRenderPhysicsVisualizers( const GrCamera& cam, float alpha )
{
	const NxDebugRenderable *dbgData = gPhScene->getDebugRenderable();

	// draw points.
	{{
		NxU32 pointCount = dbgData->getNbPoints();
		const NxDebugPoint* point = dbgData->getPoints();
		while ( pointCount-- )
		{
			gGrRenderUtil->DrawColoredPoint( cam, 
				PhSubsys::ConvertVec3( point->p ), 
				PhSubsys::ConvertColor( point->color, alpha ), 
				2 );
			++point;
		}
	}}

	// draw lines.
	{{
		NxU32 lineCount = dbgData->getNbLines();
		const NxDebugLine* line = dbgData->getLines();
		while ( lineCount-- )
		{
			gGrRenderUtil->DrawColoredLine( 
				PhSubsys::ConvertVec3( line->p0 ), 
				PhSubsys::ConvertVec3( line->p1 ), 
				PhSubsys::ConvertColor( line->color, alpha ) );
			++line;
		}
	}}

	// draw triangles.
	{{
		NxU32 triCount = dbgData->getNbTriangles();
		const NxDebugTriangle* tri = dbgData->getTriangles();
		while ( triCount-- )
		{
			gGrRenderUtil->DrawColoredTriangle( 
				PhSubsys::ConvertVec3( tri->p0 ), 
				PhSubsys::ConvertVec3( tri->p1 ), 
				PhSubsys::ConvertVec3( tri->p2 ), 
				PhSubsys::ConvertColor( tri->color, alpha ) );
			++tri;
		}
	}}
}

//**********************************************************
// event handlers
//**********************************************************

//----------------------------------------------------------
void
ClResize( ClWindow* window, int width, int height )
{
	gWindowWidth = width;
	gWindowHeight = height;

	if ( gGameCamera )
	{
		gGameCamera->SetProjAspectRatio( width / (float)height );
	}

	if ( gGrSubsys )
	{
		gGrSubsys->Resize( width, height );
	}
}

//----------------------------------------------------------
void
ClClose( ClWindow* window )
{
	// instruct the game to shutdown.
	gDone = true;
}

//----------------------------------------------------------
void
ClKeyDown( ClWindow* window, char key )
{
	switch ( key )
	{
	case 27:
		{
			// if the user pressed escape, then quit.
			gDone = true;
		}
		break;
	case ' ':
		{
			gCharJumping = true;
		}
		break;
	}

	gKeysDown[ toupper( key ) ] = true;
}

//----------------------------------------------------------
void
ClKeyUp( ClWindow* window, char key )
{
	gKeysDown[ toupper( key ) ] = false;
}

//----------------------------------------------------------
void
ClMouseMove( ClWindow* window, int x, int y )
{
	gMouseX = x;
	gMouseY = y;
}

//----------------------------------------------------------
void
ClMouseDown( ClWindow* window, int button )
{
	if ( button == 0 )
	{
		// if the user left-clicked, create an object.
		GmObject::SInfo objInfo;

		objInfo.modelFilePath = UPath( "data/plane_playground/models/cube_1x1x1.mdl" );
		objInfo.density = 10.0f;

		//objInfo.modelFilePath = UPath( "data/plane_playground/models/face.mdl" );
		//objInfo.density = 10.0f;

		//objInfo.modelFilePath = UPath( "data/plane_playground/models/speeder.mdl" );
		//objInfo.density = 1.0f;

		// set its initial transform.
		//objInfo.transform = MMat4x4( gGameCamera->GetRot(), gChar->GetPos() + 4.0f*CHAR_RADIUS*gGameCamera->GetLookDir() );
		objInfo.transform = MMat4x4( gGameCamera->GetRot(), gChar->GetPos() + MVec3( 0.0f, 1.5f, 0.0f ) + CHAR_RADIUS*MVec3( gGameCamera->GetLookDir().GetX(), 0.0f, gGameCamera->GetLookDir().GetZ() ).Normalized() );

		// set its initial physical parameters.
		objInfo.bodyDesc.angularDamping = 0.5f;
		objInfo.bodyDesc.linearVelocity = gPhSubsys->ConvertVec3( 15.0f*gGameCamera->GetLookDir() );

		// create the object.
		gGmEntityMgr->CreateDynamicObject( "box", objInfo );
	}
}

//----------------------------------------------------------
void
ClMouseUp( ClWindow* window, int button )
{
}
