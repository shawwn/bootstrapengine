//----------------------------------------------------------
// File:        UFixed.h
// Author:      Kevin Bray
// Created:     12-18-05
//
// Purpose:     To represent a 20.12 fixed point number.
//
// Copyright © 2005 Bootstrap Studios
//----------------------------------------------------------
#pragma once

//**********************************************************
// fixed point 20.12 type.
//**********************************************************
typedef int fixed_t;

//----------------------------------------------------------
fixed_t __fastcall FloatToFixed( float x );
fixed_t __fastcall IntToFixed( int x );
float __fastcall FixedToFloat( fixed_t fixed );
int __fastcall FixedToInt( fixed_t fixed );
//fixed_t __fastcall FixedAdd( fixed_t a, fixed_t b );
//fixed_t __fastcall FixedSub( fixed_t a, fixed_t b );
fixed_t __fastcall FixedMul( fixed_t a, fixed_t b );
fixed_t __fastcall FixedDiv( fixed_t a, fixed_t b );
fixed_t __fastcall FixedInv( fixed_t a );
fixed_t __fastcall FixedCeil( fixed_t a );
fixed_t __fastcall FixedFloor( fixed_t a );

/*
// these are faster than floating point.
#define FIXED_BEGIN( value )            __asm mov eax, value

#define FIXED_ADD( a )                  __asm add eax, a

#define FIXED_SUB( a )                  __asm sub eax, a

#define FIXED_MUL( a )                  __asm imul a \
                                        __asm sal edx, 16 \
                                        __asm shr eax, 16 \
                                        __asm or eax, edx

#define FIXED_NEG()                     __asm neg eax;

// These are currently *slower* than floating point!
#define FIXED_INV()                     __asm mov edx, 1 \
                                        __asm mov ebx, eax \
                                        __asm xor eax, eax \
                                        __asm idiv ebx

#define FIXED_DIV( b )                  __asm push eax \
                                        __asm mov edx, 1 \
                                        __asm xor eax, eax \
                                        __asm idiv b \
                                        __asm pop ebx \
                                        __asm imul ebx \
                                        __asm sal edx, 16 \
                                        __asm shr eax, 16 \
                                        __asm or eax, edx

#define FIXED_STORE( target )           __asm mov target, eax

#define FIXED_STORE_PTR( target )       __asm mov edx, target \
                                        __asm mov [edx], eax

#define FIXED_PUSH()                    __asm push eax

#define FIXED_POP()                     __asm pop eax
*/
