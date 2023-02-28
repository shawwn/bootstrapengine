//----------------------------------------------------------
// File:        UFixed.h
// Author:      Kevin Bray
// Created:     12-18-05
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#include "common_afx.h"

// class header
#include "UFixed.h"

DECLARE_ALIGNED_16( double, kConvertScale[ 1 ] ) = { 4096.0 };
DECLARE_ALIGNED_16( double, kInvConvertScale[ 1 ] ) = { 1.0 / 4096.0 };

//----------------------------------------------------------
fixed_t __fastcall FloatToFixed( float x )
{
    fixed_t temp;
    __asm
    {
        movd        xmm0, x
        cvtss2sd    xmm0, xmm0
        mulsd       xmm0, kConvertScale
        cvtsd2si    eax, xmm0
        mov         temp, eax
    }
    return temp;
}

//----------------------------------------------------------
fixed_t __fastcall IntToFixed( int x )
{
    return x << 12;
}

//----------------------------------------------------------
float __fastcall FixedToFloat( fixed_t fixed )
{
    float temp[ 1 ];
    __asm
    {
        cvtsi2sd    xmm0, fixed
        mulsd       xmm0, kInvConvertScale
        cvtsd2ss    xmm0, xmm0
        movd        temp, xmm0
    }
    return temp[ 0 ];
}

//----------------------------------------------------------
int __fastcall FixedToInt( fixed_t fixed )
{
    return fixed >> 12;
}

//----------------------------------------------------------
fixed_t __fastcall FixedMul( fixed_t a, fixed_t b )
{
    fixed_t temp;
    __asm
    {
        mov         eax, b
        imul        a
        shrd        eax, edx, 12
        mov         temp, eax
    }
    return temp;
}

//----------------------------------------------------------
fixed_t __fastcall FixedDiv( fixed_t a, fixed_t b )
{
    fixed_t temp;
    __asm
    {
        mov         eax, a
        mov         edx, eax
        sar         edx, 20
        shl         eax, 12
        idiv        b
        mov         temp, eax
    }
    return temp;
}

//----------------------------------------------------------
fixed_t __fastcall FixedInv( fixed_t a )
{
    fixed_t temp;
    __asm
    {
        mov         eax, 16777216
        xor         edx, edx
        idiv        a
        mov         temp, eax
    }
    return temp;
}

//----------------------------------------------------------
fixed_t __fastcall FixedCeil( fixed_t a )
{
    fixed_t temp;
    __asm
    {
        mov         eax, a
        add         eax, 4095
        and         eax, ~4095
        mov         temp, eax
    }
    return temp;
}

//----------------------------------------------------------
fixed_t __fastcall FixedFloor( fixed_t a )
{
    fixed_t temp;
    __asm
    {
        mov         eax, a
        and         eax, ~4095
        mov         temp, eax
    }
    return temp;
}
