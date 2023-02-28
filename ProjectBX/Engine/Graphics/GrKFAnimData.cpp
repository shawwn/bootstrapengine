//----------------------------------------------------------
// File:		GrKFAnimData.cpp
// Author:		Kevin Bray
// Created:		04-29-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrKFAnimData.h"


//**********************************************************
// class GrKFAnimData
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrKFAnimData::GrKFAnimData( const tstring& targetName, float duration, const UFastArray< SRotKeyFrame >& rotKeys,
						    const UFastArray< SScaleKeyFrame >& scaleKeys, const UFastArray< STranslateKeyFrame >& translateKeys )
: _targetName( targetName )
, _rotateKFArray( rotKeys )
, _scaleKFArray( scaleKeys )
, _translateKFArray( translateKeys )
, _duration( duration )
{
	// ensure that there are keyframes at the end and the
	// beginnings of the animations.
	FixupData();
}

//----------------------------------------------------------
GrKFAnimData::GrKFAnimData( UReader& reader )
: _targetName( "" )
, _rotateKFArray()
, _scaleKFArray()
, _translateKFArray()
, _duration( 0.0f )
{
	Load( reader );
}

//----------------------------------------------------------
GrKFAnimData::~GrKFAnimData()
{

}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
const GrKFAnimData::SRotKeyFrame&
GrKFAnimData::GetRotKF( unsigned int idx )
{
	B_ASSERT( idx < GetRotKFCount() );
	return _rotateKFArray[ idx ];
}

//----------------------------------------------------------
const GrKFAnimData::SScaleKeyFrame&
GrKFAnimData::GetScaleKF( unsigned int idx )
{
	B_ASSERT( idx < GetScaleKFCount() );
	return _scaleKFArray[ idx ];
}

//----------------------------------------------------------
const GrKFAnimData::STranslateKeyFrame&
GrKFAnimData::GetTranslateKF( unsigned int idx )
{
	B_ASSERT( idx < GetTranslateKFCount() );
	return _translateKFArray[ idx ];
}

//----------------------------------------------------------
void
GrKFAnimData::Save( UWriter& writer )
{
	// save out the header and version.
	writer.WriteUInt( *( ( unsigned int *)"KEYF" ) );
	writer.WriteUInt( 0x00010000U );

	// read in our target's name.
	writer.WriteString( _targetName );

	// read in the duration.
	writer.WriteFloat( _duration );

	// save out rotational keyframes.
	writer.WriteUInt( _rotateKFArray.GetElemCount() );
	for ( unsigned int i = 0; i < _rotateKFArray.GetElemCount(); ++i )
	{
		writer.WriteUInt( _rotateKFArray[ i ].time );
		_rotateKFArray[ i ].rotate.Save( writer );
	}

	// save out scale keyframes.
	writer.WriteUInt( _scaleKFArray.GetElemCount() );
	for ( unsigned int i = 0; i < _scaleKFArray.GetElemCount(); ++i )
	{
		writer.WriteUInt( _scaleKFArray[ i ].time );
		_scaleKFArray[ i ].scaleXYZ.Save( writer );
		_scaleKFArray[ i ].inTan.Save( writer );
		_scaleKFArray[ i ].outTan.Save( writer );
	}

	// save out translation keyframes.
	writer.WriteUInt( _translateKFArray.GetElemCount() );
	for ( unsigned int i = 0; i < _translateKFArray.GetElemCount(); ++i )
	{
		writer.WriteUInt( _translateKFArray[ i ].time );
		_translateKFArray[ i ].translate.Save( writer );
		_translateKFArray[ i ].inTan.Save( writer );
		_translateKFArray[ i ].outTan.Save( writer );
	}
}

//----------------------------------------------------------
void
GrKFAnimData::Load( UReader& reader )
{
	// clear out any current data.
	Clean();

	// read in the data.
	unsigned int tag  = reader.ReadUInt();
	unsigned int ver = reader.ReadUInt();
	B_ASSERT( tag == *( ( unsigned int* )"KEYF" ) );
	B_ASSERT( ver == 0x00010000U );

	// read in our target's name.
	_targetName = reader.ReadString();

	// read in the duration.
	_duration = reader.ReadFloat();

	// read in rotational keyframes.
	unsigned int rotateKFCount = reader.ReadUInt();
	for ( unsigned int i = 0; i < rotateKFCount; ++i )
	{
		SRotKeyFrame& rotKF = _rotateKFArray.Push();
		rotKF.time = reader.ReadUInt();
		rotKF.rotate.Load( reader );
	}

	// read in scale keyframes.
	unsigned int scaleKFCount = reader.ReadUInt();
	for ( unsigned int i = 0; i < scaleKFCount; ++i )
	{
		SScaleKeyFrame& scaleKF = _scaleKFArray.Push();
		scaleKF.time = reader.ReadUInt();
		scaleKF.scaleXYZ.Load( reader );
		scaleKF.inTan.Load( reader );
		scaleKF.outTan.Load( reader );
	}

	// read in scale keyframes.
	unsigned int translateKFCount = reader.ReadUInt();
	for ( unsigned int i = 0; i < translateKFCount; ++i )
	{
		STranslateKeyFrame& translateKF = _translateKFArray.Push();
		translateKF.time = reader.ReadUInt();
		translateKF.translate.Load( reader );
		translateKF.inTan.Load( reader );
		translateKF.outTan.Load( reader );
	}
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
GrKFAnimData::FixupData()
{
	unsigned int fixedDuration = ( unsigned int )( _duration * 1024.0f );
	if ( _rotateKFArray.GetElemCount() > 0 )
	{
		if ( _rotateKFArray[ 0 ].time > 0 )
		{
			if ( _rotateKFArray[ 0 ].time > 5 )
				_rotateKFArray.Insert( 0, _rotateKFArray[ 0 ] );
			_rotateKFArray[ 0 ].time = 0;
		}
		if ( _rotateKFArray[ _rotateKFArray.GetElemCount()-1 ].time < fixedDuration )
		{
			if ( ( _duration - _rotateKFArray[ _rotateKFArray.GetElemCount()-1 ].time ) > 5 )
				_rotateKFArray.Push( _rotateKFArray[0] );
			_rotateKFArray[ _rotateKFArray.GetElemCount()-1 ].time = fixedDuration;
		}
	}

	// TODO: Perhaps do something more advanced here?
	if ( _scaleKFArray.GetElemCount() > 0 )
	{
		if ( _scaleKFArray[ 0 ].time > 0 )
		{
//			if ( _scaleKFArray[ 0 ].time > 5 )
//			{
//				_scaleKFArray.Insert( 0, _scaleKFArray[ 0 ] );
//				_scaleKFArray[ 0 ].inTan = _scaleKFArray[ 0 ].scaleXYZ;
//				_scaleKFArray[ 0 ].outTan = _scaleKFArray[ 0 ].scaleXYZ;
//			}
			_scaleKFArray[ 0 ].time = 0;
		}
		if ( _scaleKFArray[ _scaleKFArray.GetElemCount()-1 ].time < fixedDuration )
		{
//			if ( ( _duration - _scaleKFArray[ _scaleKFArray.GetElemCount()-1 ].time ) > 5 )
//			{
//				SScaleKeyFrame& scaleKF = _scaleKFArray.Push();
//				scaleKF.scaleXYZ = _scaleKFArray[_scaleKFArray.GetElemCount()-2].scaleXYZ;
//				scaleKF.inTan = scaleKF.scaleXYZ;
//				scaleKF.outTan = scaleKF.scaleXYZ;
//			}
			_scaleKFArray[ _scaleKFArray.GetElemCount()-1 ].time = fixedDuration;
		}
	}

	if ( _translateKFArray.GetElemCount() > 0 )
	{
		if ( _translateKFArray[ 0 ].time > 0 )
		{
//			if ( _translateKFArray[ 0 ].time > 5 )
//			{
//				_translateKFArray.Insert( 0, _translateKFArray[ 0 ] );
//				_translateKFArray[ 0 ].inTan = _translateKFArray[ 0 ].translate;
//				_translateKFArray[ 0 ].outTan = _translateKFArray[ 0 ].translate;
//			}
			_translateKFArray[ 0 ].time = 0;
		}
		if ( _translateKFArray[ _translateKFArray.GetElemCount()-1 ].time < _duration )
		{
//			if ( ( _duration - _translateKFArray[ _translateKFArray.GetElemCount()-1 ].time ) > 5 )
//			{
//				STranslateKeyFrame& translateKF = _translateKFArray.Push();
//				translateKF.translate = _translateKFArray[0].translate;
//				translateKF.inTan = translateKF.translate;
//				translateKF.outTan = translateKF.translate;
//			}
			_translateKFArray[ _translateKFArray.GetElemCount()-1 ].time = fixedDuration;
		}
	}
}

//----------------------------------------------------------
void
GrKFAnimData::Clean()
{
	_rotateKFArray.Clear();
	_scaleKFArray.Clear();
	_translateKFArray.Clear();
}
