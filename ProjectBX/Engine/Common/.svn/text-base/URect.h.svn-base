//----------------------------------------------------------
// File:		URect.h
// Author:		Kevin Bray
// Created:		03-27-06
//
// Purpose:		To manage a rectangle
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

//**********************************************************
// class URect
//**********************************************************
class URect
{
public:
	URect( unsigned int width, unsigned int height );
	URect( unsigned int x, unsigned int y, unsigned int width, unsigned int height );
	~URect() { }

	void				SetX( unsigned int x )				{	_x = x;			}
	void				SetY( unsigned int y )				{	_y = y;			}
	void				SetWidth( unsigned int w )			{	_width = w;		}
	void				SetHeight( unsigned int h )			{	_height = h;	}

	unsigned int		GetX() const						{	return _x;		}
	unsigned int		GetY() const						{	return _y;		}
	unsigned int		GetWidth() const					{	return _width;	}
	unsigned int		GetHeight() const					{	return _height;	}

private:
	unsigned int		_x;
	unsigned int		_y;
	unsigned int		_width;
	unsigned int		_height;
};
