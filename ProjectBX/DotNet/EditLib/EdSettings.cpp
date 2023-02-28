//----------------------------------------------------------
// File:		EdSettings.h
// Author:		Kevin Bray
// Created:		11-24-06
//
// Purpose:		To manage editor settings.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "editlib_afx.h"

// class header.
#include "EdSettings.h"

// LibXML includes.
#include "irrXML/irrXML.h"

EdSettings* gEdSettings = 0;

// XML file header.
char xmlFileHeader[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
char xmlSettingsBegin[] = "<settings>\n";
char xmlSettingBegin[] = " <setting ";
char xmlSettingNameBegin[] = "name=\"";
char xmlSettingNameEnd[] = "\" ";
char xmlSettingValueBegin[] = "value=\"";
char xmlSettingValueEnd[] = "\"";
char xmlSettingEnd[] = "/>\n";
char xmlSettingsEnd[] = "</settings>\n";

// we're using irrXML.
using namespace irr::io;

//**********************************************************
// class EdSettings
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
EdSettings::EdSettings( const char* fileName, const char* defFileName )
: _fileName( fileName )
, _defFileName( defFileName )
, _valid( false )
{
	B_ASSERT( gEdSettings == 0 );
	gEdSettings = this;

	ReadSettings();
}

//----------------------------------------------------------
EdSettings::~EdSettings()
{
	SaveSettings();

	B_ASSERT( gEdSettings != 0 );
	gEdSettings = 0;
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
EdSettings::SetToDefault()
{
	InitSettings();
	ReadSettings();
}

//----------------------------------------------------------
void
EdSettings::SetWorkingFolder( const tstring& folder )
{
	_workingFolder = folder;
	_settings[ "workingfolder" ] = folder;
}

//----------------------------------------------------------
const UValue&
EdSettings::GetUserSetting( const char* name, const UValue& defValue )
{
	// make a copy of the string.
	tstring lcName = name;

	// make the string lower case.
	unsigned int len = ( unsigned int )lcName.length();
	for ( unsigned int i = 0; i < len; ++i )
		lcName[ i ] = tolower( lcName[ i ] );

	// find the name.
	SettingMap::iterator iter = _settings.find( lcName );
	if ( iter == _settings.end() )
	{
		UValue& temp = _settings[ lcName ];
		temp = defValue;
		return temp;
	}

	// return the setting value.
	return iter->second;
}

//----------------------------------------------------------
void
EdSettings::SetUserSetting( const char* name, const UValue& value )
{
	// g++ doesn't like calling MakeLowerCase directly with tstring( name )
	tstring temp = tstring( name );
	// set the value.
	_settings[ MakeLowerCase( temp ) ] = value;
}


//==========================================================
// private methods
//==========================================================

//----------------------------------------------------------
void
EdSettings::LoadXML()
{
	// open the file.
	FILE* fp = fopen( _fileName.c_str(), "rb" );

	// if the file couldn't be opened, initialize the settings to valid
	// defaults.
	if ( !fp )
	{
		InitSettings();
		fp = fopen( _fileName.c_str(), "rb" );
		B_ASSERT( fp );
	}

	// read in the document.
	IrrXMLReader* xmlReader = createIrrXMLReader( fp );
	while ( xmlReader->read() )
	{
		// skip if we don't have an element.
		if ( xmlReader->getNodeType() != EXN_ELEMENT )
			continue;

		// get the current node name+value.
		const char* curNodeName = xmlReader->getNodeName();
		if ( _stricmp( curNodeName, "setting" ) == 0 )
		{
			// get the name of the current setting.
			const char* settingName = xmlReader->getAttributeValue( "name" );
			if ( !settingName )
				continue;

			// get the value of the current setting.
			const char* settingValue = xmlReader->getAttributeValue( "value" );
			if ( !settingValue )
				continue;

			// store our settings.
			tstring name = settingName;
			tstring value = settingValue;
			_settings[ MakeLowerCase( name ) ] = value;
		}
	}

	// free the XML reader.
	delete xmlReader;

	// close the settings file.
	fclose( fp );
}

//----------------------------------------------------------
void
EdSettings::ReadSettings()
{
	// load the XML data.
	LoadXML();

	// now find and store our settings.
	_workingFolder		= tstring( ( const char* )_settings[ "workingfolder" ] );
	_copyrightNotice	= tstring( ( const char* )_settings[ "copyrightnotice" ] );
	_softwareUser		= ( tstring )( const char* )_settings[ "username" ];

	// mark as valid.
	_valid = true;
}

//----------------------------------------------------------
void
EdSettings::SaveSettings()
{
#define WRITE_VALUE( value )	fwrite( value, sizeof( value )-1, 1, fp );

	// open the settings file for write access.
	FILE* fp = fopen( _fileName.c_str(), "wb" );
	if ( !fp )
		return;

	// write out the XML file header.
	WRITE_VALUE( xmlFileHeader );
	WRITE_VALUE( xmlSettingsBegin );

	// now write out each individual setting.
	SettingMap::iterator iter = _settings.begin();
	for ( ; iter != _settings.end(); ++iter )
	{
		// setting begin.
		WRITE_VALUE( xmlSettingBegin );

		// setting name.
		WRITE_VALUE( xmlSettingNameBegin );
		fwrite( iter->first.c_str(), iter->first.length(), 1, fp );
		WRITE_VALUE( xmlSettingNameEnd );

		// setting value.
		WRITE_VALUE( xmlSettingValueBegin );
		fwrite( ( const char* )iter->second, strlen( ( const char* )iter->second ), 1, fp );
		WRITE_VALUE( xmlSettingValueEnd );

		// setting end.
		WRITE_VALUE( xmlSettingEnd );
	}

	// write the end of the settings block.
	WRITE_VALUE( xmlSettingsEnd );

	// close the file.
	fclose( fp );

}

//----------------------------------------------------------
void
EdSettings::InitSettings()
{
	FILE* src = 0;
	fopen_s( &src, _defFileName.c_str(), "rb" );
	if( src )
	{
		// if the default settings file exists, copy it.
		FILE* dst = 0;
		fopen_s( &dst, _fileName.c_str(), "w+b" );

		// get the size of the file.
		fpos_t pos = 0;
		fseek( src, 0, SEEK_END );
		fgetpos( src, &pos );
		fseek( src, 0, SEEK_SET );

		// copy the file and free our buffer.
		void* buffer = new char[ ( size_t )pos+1 ];
		size_t bytesRead = fread( buffer, 1, ( size_t )pos+1, src );
		fwrite( buffer, bytesRead, 1, dst );

		// cleanup.
		delete[] ( char* )buffer;
		fclose( dst );
		fclose( src );
	}
	else
	{
		// otherwise simply create a blank settings file.
		fopen_s( &src, _fileName.c_str(), "w+b" );
		fclose( src );
	}
}

//----------------------------------------------------------
tstring&
EdSettings::MakeLowerCase( tstring& str )
{
	// make the string passed in lower case.
	tstring::iterator ch = str.begin();
	while ( ch != str.end() )
	{
		*ch = tolower( *ch ) & 0xFF;
		++ch;
	}
	return str;
}
