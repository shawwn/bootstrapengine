#include "GMAfx.h"

// class header.
#include "Time.h"

// standard headers.
#include <windows.h>
#include <mmsystem.h>
#include <cassert>

//-----------------------
// engine forward declarations.
namespace Bootstrap
{

    //-----------------------
    TimeKeeper::TimeKeeper()
    : _speed( 65536 )
    , _absTime( 0 )
    , _sysTimeDelta( 0 )
    , _gameTimeDelta( 0 )
    , _paused( false )
    {
        _absTime = timeGetTime();
    }

    //-----------------------
    TimeKeeper::~TimeKeeper()
    {

    }

    //-----------------------
    void TimeKeeper::Update()
    {
        // calculate the new system time delta.
        DWORD time = timeGetTime();
        _sysTimeDelta = time - _absTime;
        _absTime = time;

        // add in the speed multiplier.
        if ( !_paused )
        {
            unsigned __int64 gameTimeDelta = _sysTimeDelta * _speed;
            gameTimeDelta >>= 16;
            _gameTimeDelta = ( unsigned int )gameTimeDelta;
        }
        else
        {
            _gameTimeDelta = 0;
        }
    }

    //-----------------------
    void TimeKeeper::SetGameTimeSpeed( float speed )
    {
        _speed = ( unsigned int )( ( double )speed * 65536.0 );
    }
}