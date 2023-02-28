//----------------------------------------------------------
// File:        UTime.h
// Author:      Kevin Bray
// Created:     04-08-06
//
// Purpose:     To represent a 32.32 fixed point time value.  This has a
//              maximum limit of approx 136 years.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class UTime
//**********************************************************
class UTime
{
public:
    UTime( unsigned int seconds, unsigned int fraction );
    ~UTime();

    UTime               operator - ( const UTime& rhs ) const;
    void                Advance( unsigned int fraction );
    void                Advance( unsigned int seconds, unsigned int fraction );

private:
    __int64             _time;          // 32.32 fixed point.
};
