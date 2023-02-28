//----------------------------------------------------------
// File:        GrSubsys.h
// Author:      Kevin Bray
// Created:     12-28-04
//
// Purpose:     To manage the primary rendering context and all
//              graphics singletons.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrConfig.h"

// std c++ includes.
#include <map>

// forward declarations.
class GrCamera;

//**********************************************************
// class GrSubsys
//**********************************************************
class GrSubsys
{
public:
    GrSubsys( unsigned int screenWidth, unsigned int screenHeight, bool uberTexCompress, bool uberTexPaint );
    ~GrSubsys();

    // graphics pipeline configuration...
    //
    // NOTE: All functions require the graphics subsystem to be reset
    //       before they take effect ( unless otherwise noted ).

    // screen resolution change.
//  void            ResolutionChanged( unsigned int width, unsigned int height );

    unsigned int    GetScreenWidth() const                      {   return _screenWidth;            }
    unsigned int    GetScreenHeight() const                     {   return _screenHeight;           }

    // quality settings.  These don't take effect until the next time the
    // context is reset.
    void            SetTexturingQuality( EGRQUALITY quality )   {   _textureQuality = quality;      }
    void            SetShadingQuality( EGRQUALITY quality )     {   _shadingQuality = quality;      }
    void            SetHDRQuality( EGRQUALITY quality )         {   _hdrQuality = quality;          }
    void            SetHDREnable( bool enabled )                {   _hdrEnable = enabled;           }
    void            SetUberDXTCompression( bool enabled )       {   _uberDXTCompression = enabled;  }
    void            SetUberOverlaySupport( bool enabled )       {   _uberOverlaySupport = enabled;  }

    EGRQUALITY      GetTexturingQuality() const                 {   return _textureQuality;         }
    EGRQUALITY      GetShadingQuality() const                   {   return _shadingQuality;         }
    EGRQUALITY      GetHDRQuality() const                       {   return _hdrQuality;             }
    bool            GetHDREnable() const                        {   return _hdrEnable;              }
    bool            GetUberDXTCompression() const               {   return _uberDXTCompression;     }
    bool            GetUberOverlaySupport() const               {   return _uberOverlaySupport;     }

    // color controls.
    void            SetGamma( float gamma );
    void            SetBrightness( float brightness );

    // takes a screenshot.
    void            TakeScreenshot( const char* fileName );

    // updates the graphics system.  This must be called once per frame
    // before anything is rendered.
    void            Update( unsigned int timeDelta, unsigned int sysTimeDelta );
    void            Render( const GrCamera* camera );

    // resize: resize resizes the back buffer and resets a handful of
    // subsystems.
    void            Resize( unsigned int width, unsigned int height );

    // reset: frees and reloads all graphics resources.  Also applies all
    // changes to settings.
    void            Reset( unsigned int width, unsigned int height );
    void            Reset();

    // context reset notification.
    void            PreContextReset();
    void            PostContextReset( unsigned int width, unsigned int height );

    // issues a text based command.
    bool            IssueCommand( const tstring& command );
    void            AddCommandHandler( const char* command, bool ( *handler )( const tstring& ) );

private:
    void            UpdateColorTable();
    void            CreateFrameBuffer();

    typedef bool ( *HandlerProc )( const tstring& );
    typedef std::map< tstring, HandlerProc > CommandMap;

    CommandMap      _commands;
    unsigned int    _screenWidth;
    unsigned int    _screenHeight;
    float           _gamma;
    float           _brightness;
    unsigned char   _colorTable[ 256 ];
    EGRQUALITY      _textureQuality;
    EGRQUALITY      _shadingQuality;
    EGRQUALITY      _hdrQuality;
    bool            _parallaxEnable;
    bool            _hdrEnable;
    bool            _colorTableDirty;
    bool            _uberDXTCompression;
    bool            _uberOverlaySupport;
};
extern GrSubsys *gGrSubsys;
