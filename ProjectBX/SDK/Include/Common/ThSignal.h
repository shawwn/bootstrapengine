//----------------------------------------------------------
// File:        ThSignal.h
// Author:      Kevin Bray
// Created:     09-01-08
//
// Purpose:     To provide an interface for threading events.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class ThSignal
//**********************************************************
class ThSignal
{
public:
    ThSignal( bool raised, bool autoLower );
    ~ThSignal();

    // wait for the signal to be raised.
    void            Wait() const;

    // returns true if the signal is already raised.
    bool            IsRaised() const;
    bool            IsLowered() const;

    // raises a signal so that anyone waiting can move forward.
    void            Raise();

    // resets the signal to stop.
    void            Lower();

private:
    ThSignal( const ThSignal& ) { };
    ThSignal& operator = ( const ThSignal& ) { return *this; }

    void*           _signalData;
};