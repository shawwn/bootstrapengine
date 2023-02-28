//----------------------------------------------------------
// File:        GrPolygonUVAtlas.cpp
// Author:      Kevin Bray
// Created:     11-19-06
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// class header.
#include "GrPolygonUVAtlas.h"


//**********************************************************
// class GrPolygonUVAtlas
//**********************************************************

//==========================================================
// ctor & dtor
//==========================================================

//----------------------------------------------------------
GrPolygonUVAtlas::GrPolygonUVAtlas( const URect& rect )
: _rect( rect )
, _child1( 0 )
, _child2( 0 )
, _leafRect( rect )
{

}

//----------------------------------------------------------
GrPolygonUVAtlas::~GrPolygonUVAtlas()
{
    Clear();
}


//==========================================================
// public methods
//==========================================================

//----------------------------------------------------------
void
GrPolygonUVAtlas::Clear()
{
    delete _child1;
    delete _child2;
    _child1 = 0;
    _child2 = 0;
}

//----------------------------------------------------------
bool
GrPolygonUVAtlas::Insert( URect& rect, unsigned int width, unsigned height )
{
    // can we even fit in this region?
    if ( width > _rect.GetWidth() || height > _rect.GetHeight() )
        return false;

    // if we don't have any children on this node, then there is no
    // leaf rectangle.  That means we can place the rectangle here.
    if ( !_child1 )
    {
        // if _child1 is NULL, then both children must be NULL.
        B_ASSERT( !_child2 );

        // calculate the leaf rectangle.
        _leafRect.SetX( _rect.GetX() );
        _leafRect.SetY( _rect.GetY() );
        _leafRect.SetWidth( width );
        _leafRect.SetHeight( height );

        // now build our children using the remaining two rectangles.
        // note that we choose an optimal rectangular break using the
        // width > height check.  This tries to avoid any really long
        // and thin strips.
//      if ( width >= height )
//      {
            URect rect1( _rect.GetX() + width, _rect.GetY(), _rect.GetWidth() - width, height );
            URect rect2( _rect.GetX(), _rect.GetY() + height, _rect.GetWidth(), _rect.GetHeight() - height );
            _child1 = new GrPolygonUVAtlas( rect1 );
            _child2 = new GrPolygonUVAtlas( rect2 );
//      }
//      else
//      {
//          URect rect1( _rect.GetX() + width, _rect.GetY(), _rect.GetWidth() - width, _rect.GetHeight() );
//          URect rect2( _rect.GetX(), _rect.GetY() + height, width, _rect.GetHeight() - height );
//          _child1 = new GrPolygonUVAtlas( rect1 );
//          _child2 = new GrPolygonUVAtlas( rect2 );
//      }

        // return the leaf rectangle to the user.
        rect = _leafRect;

        // return true since we found a home for our node.
        return true;
    }

    // if we got here, then we have children already.  In this case,
    // simply recurse.
    B_ASSERT( _child1 != 0 && _child2 != 0 );
    if ( _child1->Insert( rect, width, height ) )
        return true;
    if ( _child2->Insert( rect, width, height ) )
        return true;

    // if we couldn't insert in any children, then we should simply
    // return false to indicate failure.
    return false;
}
