// Time.h
#pragma once

//-----------------------
// engine forward declarations.
namespace Bootstrap
{
	//-----------------------
	public ref class TimeKeeper
	{
	public:
		TimeKeeper();
		~TimeKeeper();

		//-----------------------
		void Update();

		//-----------------------
		void Pause()					{ _paused = true;			}

		//-----------------------
		unsigned int GetSysTimeDelta()	{ return _sysTimeDelta;		}

		//-----------------------
		unsigned int GetGameTimeDelta() { return _gameTimeDelta;	}

		//-----------------------
		// sets the game time speed multiplier.  Note that
		// this is in 16:16 fixed point.
		void SetGameTimeSpeed( float speed );

	private:
		static TimeKeeper^ _singleton;

		// speed in 16:16 fixed point.
		unsigned int _speed;
		unsigned int _absTime;
		unsigned int _sysTimeDelta;
		unsigned int _gameTimeDelta;
		bool _paused;
	};
}