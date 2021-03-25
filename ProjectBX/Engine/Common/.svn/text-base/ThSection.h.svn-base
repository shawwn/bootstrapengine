//----------------------------------------------------------
// File:		ThSection.h
// Author:		Kevin Bray
// Created:		09-01-08
//
// Purpose:		To provide an interface for dealing with critical
//				sections.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class ThSection
//**********************************************************
class ThSection
{
public:
	ThSection();
	~ThSection();

	// section management.  Only the thread that called Enter
	// can execute through a section of code until Leave is called.
	void				Enter();
	bool				TryEnter();
	void				Leave();

private:
	ThSection( const ThSection& ) { }
	ThSection& operator = ( const ThSection& ) { return *this; }

	void*				_section;
};

class ThTakeSection
{
public:
	ThTakeSection( ThSection& section )
	: _section( section )
	{
		_section.Enter();
	}

	~ThTakeSection()
	{
		_section.Leave();
	}

private:
	ThSection&		_section;
};
