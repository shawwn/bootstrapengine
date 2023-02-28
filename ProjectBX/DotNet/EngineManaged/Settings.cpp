#include "EMAfx.h"

// class header.
#include "Settings.h"

// project includes.
#include "Utils.h"

// edlib includes.
#include "EdSettings.h"

// engine includes.
#include "GrConfig.h"

//**********************************************************
// ref class Settings
//**********************************************************

//----------------------------------------------------------
Bootstrap::Settings::Settings()
{
    new EdSettings( "settings.xml", "defaults.xml" );
}

//----------------------------------------------------------
Bootstrap::Settings::~Settings()
{
    this->!Settings();
}

//----------------------------------------------------------
Bootstrap::Settings::!Settings()
{
    delete gEdSettings;
}

//----------------------------------------------------------
Bootstrap::Settings^ 
Bootstrap::Settings::Get()
{
    if ( !_singleton )
        _singleton = gcnew Settings();
    return _singleton;
}

//----------------------------------------------------------
void 
Bootstrap::Settings::Save()
{
    gEdSettings->SaveSettings();
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetWorkingFolder()
{
    return TStrToStr( gEdSettings->GetWorkingFolder() );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetPackageFolder()
{
    return TStrToStr( gEdSettings->GetWorkingFolder() + "/data" );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetShaderFolder()
{
    return TStrToStr( gEdSettings->GetWorkingFolder() + "/"GR_PATH_SHADERS );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetBrushFolder()
{
    return TStrToStr( gEdSettings->GetUserSetting( "brushfolder" ) );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetCopyrightNotice()
{
    return TStrToStr( gEdSettings->GetCopyrightNotice() );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetSoftwareUser()
{
    return TStrToStr( gEdSettings->GetSoftwareUser() );
}

//----------------------------------------------------------
void 
Bootstrap::Settings::SetWorkingFolder( System::String^ value )
{
    gEdSettings->SetWorkingFolder( StrToTStr( value ) );
}

//----------------------------------------------------------
void 
Bootstrap::Settings::SetBrushFolder( System::String^ value )
{
    gEdSettings->SetUserSetting( "brushfolder", StrToTStr( value ) );
}

//----------------------------------------------------------
System::String^ 
Bootstrap::Settings::GetUserSetting( System::String^ name )
{
    return TStrToStr( gEdSettings->GetUserSetting( StrToTStr( name ).c_str() ) );
}

//----------------------------------------------------------
void 
Bootstrap::Settings::SetUserSetting( System::String^ name, System::String^ value )
{
    gEdSettings->SetUserSetting( StrToTStr( name ).c_str(), StrToTStr( value ) );
}