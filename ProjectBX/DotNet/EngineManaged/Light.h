//----------------------------------------------------------
// File:        Light.h
// Author:      Shawn Presser
// Created:     09-23-08
//
// Purpose:     To store effect information and pass that into
//              shaders as necessary.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// base class header.
#include "SceneEntity.h"

// forward declarations.
class GrLight;

namespace Bootstrap
{
    // .NET forward declarations.
    value class Color;

    //**********************************************************
    // ref class Light
    //**********************************************************
    public ref class Light : public SceneEntity
    {
        Light( System::String^ id, System::String^ localName );

    internal:
        Light( GrLight* impl );

        GrLight*                            _light;

    public:
        static Light^                       Create( System::String^ localName );

        ~Light();
        !Light();

        property System::String^            ID
        {
            System::String^                     get();
        }

        property System::String^            Name
        {
            System::String^                     get();
            void                                set( System::String^ name );
        }

        property Bootstrap::Color           Color
        {
            Bootstrap::Color                    get();
            void                                set( Bootstrap::Color col );
        }

        property float                      Intensity
        {
            float                               get();
            void                                set( float value );
        }

        property float                      Range
        {
            float                               get();
            void                                set( float value );
        }

        property float                      Softness
        {
            float                               get();
            void                                set( float value );
        }

        property bool                       CastShadow
        {
            bool                                get();
            void                                set( bool value );
        }

        property bool                       Parallel
        {
            bool                                get();
            void                                set( bool value );
        }

        property bool                       Global
        {
            bool                                get();
            void                                set( bool value );
        }

        property System::String^            Mask
        {
            System::String^                     get();
            void                                set( System::String^ name );
        }

        property float                      ProjSpreadAngle
        {
            float                               get();
            void                                set( float value );
        }

        property float                      ProjWidth
        {
            float                               get();
            void                                set( float value );
        }

        property float                      ProjHeight
        {
            float                               get();
            void                                set( float value );
        }

        property float                      AttenConst
        {
            float                               get();
            void                                set( float value );
        }

        property float                      AttenLinear
        {
            float                               get();
            void                                set( float value );
        }

        property float                      AttenQuadratic
        {
            float                               get();
            void                                set( float value );
        }

        // clones the light.
        Light^                              Clone( System::String^ instName );

        virtual bool                        Equals( System::Object^ obj ) override
        {
            // if the object is null, it is obviously not equal.
            if( obj == nullptr )
                return false;

            // if the object isn't of the same type, it is not equal.
            if( obj->GetType() != this->GetType() )
                return false;

            // if the light does not reference the same internal pointer, it is not equal.
            Light^ light = ( Light^ )obj;
            return ( _light == light->_light );
        }

    protected:
        // inherited from SceneEntity.
        virtual void                        ApplyTransform( Matrix mat ) sealed override;
        virtual Matrix                      GetTransform() sealed override;

    private:
        void                                Acquire( GrLight* light );

        bool                                _inScene;
        static System::Random               _random;
    };
}
