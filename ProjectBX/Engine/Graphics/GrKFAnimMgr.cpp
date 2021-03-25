//----------------------------------------------------------
// File:		GrKFAnimMgr.h
// Author:		Kevin Bray
// Created:		04-30-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrKFAnimMgr.h"

// project includes.
#include "GrKFAnim.h"
#include "RFileMgr.h"
#include "RFile.h"
#include "UReader.h"
#include "UWriter.h"

GrKFAnimMgr* gGrKFAnimMgr = 0;

//**********************************************************
// class GrKFAnimMgr
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrKFAnimMgr::GrKFAnimMgr()
{
	B_ASSERT( gGrKFAnimMgr == 0 );
	gGrKFAnimMgr = this;
}

//----------------------------------------------------------
GrKFAnimMgr::~GrKFAnimMgr()
{
	AnimMap::iterator iter = _animMap.begin();
	const AnimMap::iterator end = _animMap.end();
	for ( ; iter != end; ++iter )
		delete iter->second;

	gGrKFAnimMgr = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrKFAnim*
GrKFAnimMgr::GetKFAnim( const UPath& name )
{
	UPath fileName = name.GetPathString() + ".kfa";

	// try to see if the animation has already been loaded and if so
	// return a clone.
	AnimMap::iterator iter = _animMap.find( fileName );
	if ( iter != _animMap.end() )
		return ( GrKFAnim* )iter->second->Clone();

	// try to find the file specified.
	URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagRead );
	if ( !file )
		return 0;

	// get the file data.
	const void* fileBuffer = file->GetData();
	unsigned long fileSize = ( unsigned long )file->GetSize();

	// load in the animation data.
	UReader reader( fileBuffer, fileSize, false );

	// create a new animation from the file data and store in our map
	// of animations.
	GrKFAnim* newKFAnim = new GrKFAnim( reader );
	_animMap[ fileName ] = newKFAnim;

	// purge the data from the file.
	file->Purge();

	// return a clone of the stored animation.
	return ( GrKFAnim* )newKFAnim->Clone();
}

//----------------------------------------------------------
void
GrKFAnimMgr::SaveKFAnim( GrKFAnim* anim )
{
	B_ASSERT( anim != 0 );

	UPath fileName = anim->GetName() + ".kfa";

	// check to see if the animation is in the tree.  If so, free it
	// and replace it with the animation passed in.
	AnimMap::iterator iter = _animMap.find( fileName );
	if ( iter != _animMap.end() )
	{
		delete iter->second;
		iter->second = ( GrKFAnim* )anim->Clone();
	}
	else
		_animMap[ fileName ] = ( GrKFAnim* )anim->Clone();

	// write out the animation data.
	UWriter writer;
	anim->Save( writer );

	// write to a file.
	URef< RFile > file = gRFileMgr->GetFile( fileName, RFileMgr::kFlagWrite );
	file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );
}
