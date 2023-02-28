//----------------------------------------------------------
// File:		Settings.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To manage editor settings.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

namespace Bootstrap
{
    //**********************************************************
    // ref class Settings
    //**********************************************************
	public ref class Settings
	{
		Settings();
	public:
		~Settings();
		!Settings();

		static Settings^	                Get();

		void				                Save();

		System::String^		                GetWorkingFolder();
		System::String^		                GetPackageFolder();
		System::String^		                GetShaderFolder();
		System::String^		                GetBrushFolder();
		System::String^		                GetCopyrightNotice();
		System::String^		                GetSoftwareUser();

		void				                SetWorkingFolder( System::String^ value );
		void				                SetBrushFolder( System::String^ value );

        // additional, named user settings.
        System::String^		                GetUserSetting( System::String^ name );
        void				                SetUserSetting( System::String^ name, System::String^ value );

	private:
		static Settings^	                _singleton;
	};
}