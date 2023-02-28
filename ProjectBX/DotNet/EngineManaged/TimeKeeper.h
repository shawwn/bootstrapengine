//----------------------------------------------------------
// File:		TimeKeeper.h
// Author:		Shawn Presser
// Created:		09-23-08
//
// Purpose:		To track system and game time.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

namespace Bootstrap
{
    //**********************************************************
    // ref class TimeKeeper
    //**********************************************************
	public ref class TimeKeeper
	{
	public:
		TimeKeeper();
		~TimeKeeper();
		!TimeKeeper();

		void                                Update();

		void                                Pause()					        {   _paused = true;			    }

		void                                Resume()					    {   _paused = false;			}

		unsigned int                        GetSysTimeDelta()	            {   return _sysTimeDelta;		}

		unsigned int                        GetGameTimeDelta()              {   return _gameTimeDelta;	    }

		// sets the game time speed multiplier.  Note that
		// this is in 16:16 fixed point.
		void                                SetGameTimeSpeed( float speed );

	private:
		static TimeKeeper^                  _singleton;

		// speed in 16:16 fixed point.
		unsigned int                        _speed;
		unsigned int                        _absTime;
		unsigned int                        _sysTimeDelta;
		unsigned int                        _gameTimeDelta;
		bool                                _paused;
	};
}