#include "editlib_afx.h"

// class header.
#include "EdImportDAEAnims.h"

// collada includes.
#pragma warning ( disable : 4996 )
#include "FCollada.h"
#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDAnimation.h"
#include "FCDocument/FCDAnimationChannel.h"
#pragma warning ( default : 4996 )


//**********************************************************
// class EdImportDAEAnims
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdImportDAEAnims::EdImportDAEAnims()
{

}

//----------------------------------------------------------
EdImportDAEAnims::~EdImportDAEAnims()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
GrKFAnim*
EdImportDAEAnims::ImportAnim( FCDocument* doc, const UPath& outputPath )
{
/*
	// get the animation library from the DAE file.
	FCDAnimationLibrary* animLib = doc->GetAnimationLibrary();

	// go through all animations in the lib.
	unsigned int entityCount = ( unsigned int )animLib->GetEntityCount();
	for ( unsigned int i = 0; i < entityCount; ++i )
	{
		// get the current animation.
		FCDAnimation* curAnim = animLib->GetEntity( i );

		// go through curves in the animation.
		unsigned int animChannelCount = ( unsigned int )curAnim->GetChannelCount();
		for ( unsigned int j = 0; j < animChannelCount; ++j )
		{
			// get the current animation channel.
			FCDAnimationChannel* curChannel = curAnim->GetChannel( j );

			// read in the target qualifier.
			const fm::string& channelTarget = curChannel->GetTargetPointer();
			const fm::string& channelTargetQualifier = curChannel->GetTargetQualifier();

			// now sample the curve information.
			
		}
	}
*/
	return 0;
}
