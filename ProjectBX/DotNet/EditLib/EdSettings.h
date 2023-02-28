//----------------------------------------------------------
// File:        EdSettings.h
// Author:      Kevin Bray
// Created:     11-24-06
//
// Purpose:     To manage editor settings.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "UValue.h"

// std c++ includes.
#include <map>

//**********************************************************
// class EdSettings
//**********************************************************
class EdSettings
{
public:
    EdSettings( const char* fileName, const char* defFileName );
    virtual ~EdSettings();

    bool                IsValid() const             {   return _valid;              }

    void                SetToDefault();

    // sets the working folder.
    void                SetWorkingFolder( const tstring& folder );

    // saves any changes.  Note that this is called when the settings are
    // destroyed.  However, it's good practice to call this during important
    // events just in case the unthinkable happens (a crash).
    void                SaveSettings();

    // predefined settings.
    const tstring&      GetWorkingFolder() const    {   return _workingFolder;      }
    const tstring&      GetCopyrightNotice() const  {   return _copyrightNotice;    }
    const tstring&      GetSoftwareUser()           {   return _softwareUser;       }

    // additional, named user settings.
    const UValue&       GetUserSetting( const char* name, const UValue& defValue = UValue() );
    void                SetUserSetting( const char* name, const UValue& value );

private:
    typedef std::map< tstring, UValue > SettingMap;

    void                LoadXML();
    void                ReadSettings();
    void                InitSettings();
    tstring&            MakeLowerCase( tstring& str );

    SettingMap          _settings;
    tstring             _workingFolder;
    tstring             _copyrightNotice;
    tstring             _softwareUser;
    tstring             _fileName;
    tstring             _defFileName;
    bool                _valid;
};
extern EdSettings* gEdSettings;
