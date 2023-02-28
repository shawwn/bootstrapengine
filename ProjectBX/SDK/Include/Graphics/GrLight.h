//----------------------------------------------------------
// File:        GrLight.h
// Author:      Kevin Bray
// Created:     10-28-04
//
// Purpose:     To store effect information and pass that into
//              shaders as necessary.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

// project includes.
#include "GrConfig.h"
#include "GrProjection.h"
#include "MMat3x3.h"
#include "MVec4.h"
#include "MVec3.h"
#include "MSphere.h"
#include "GrColor.h"
#include "UExprTerm.h"
#include "GrTexture.h"
#include "URef.h"

class MMat4x4;
class MVec3;
class UWriter;
class UReader;
class GrBSPSplit;
class GrCamera;
class GrTextureBase;
class UExprTerm;

enum ELIGHTMODE
{
    ELM_ADD,
    ELM_SUBTRACT,
};

struct SLightInfo
{
    SLightInfo();

    tstring name;
    MMat4x4 transform;
    GrProjection projection;
    URef< GrTexture > projTex;
    GrColor color;
    URef< UExprTerm > intensity;
    URef< UExprTerm > range;
    float sqrAtten;
    float linAtten;
    float constAtten;
    float haziness;
    float width;
    ELIGHTMODE mode;
    bool parallel;
    bool global;
    bool castShadow;
};

//**********************************************************
// class GrLight
//**********************************************************
class GrLight : public URefCount
{
public:
    GrLight( const UPath& id, const SLightInfo& lightInfo );
/*
    // needed parameters to create a point light.  Note that bounds is in local space,
    // not world space.
    GrLight( const tstring& name, const MMat4x4& transform, const GrColor& color, URef<UExprTerm> intensity,
        URef<UExprTerm> range, const MSphere& localBounds, float sqrAtten, float linAtten, float constAtten,
        float haziness, float width, ELIGHTMODE mode, bool castShadow );

    // needed parameters to create a point light, spot light, projected texture, or
    // parallel light.  Note that bounds is in local space, not world space.
    GrLight( const tstring& name, const MMat4x4& transform, const GrProjection& projection, URef<GrTexture> projTex,
        const GrColor& color, URef<UExprTerm> intensity, URef<UExprTerm> range, const MSphere& localBounds,
        float sqrAtten, float linAtten, float constAtten, float haziness, float width, ELIGHTMODE mode,
        bool parallel, bool castShadow );
*/
    virtual ~GrLight();

    void                    Rebuild( const SLightInfo& lightInfo );

    const UPath&            GetId() const                       {   return _id;                     }

    // clones the light.
    GrLight*                Clone( const UPath& newId ) const;

    // mutators.
    void                    SetName( const tstring& name )                  {   _name = name;               }
    void                    SetLocalTransform( const MMat4x4& transform );
    void                    SetParentTransform( const MMat4x4& transform );
    void                    SetProjection( const GrProjection& projection );
    void                    SetProjTexture( URef<GrTexture> projTexture );
    void                    SetColor( const GrColor& color )                {   _color = color;             }
    void                    SetIntensity( URef<UExprTerm> intensity )       {   _intensity = intensity;     }
    void                    SetIntensity( float intensity );
    void                    SetRange( URef<UExprTerm> range )               {   _range = range;             }
    void                    SetRange( float range );
    void                    SetParallel( bool parallel );
    void                    SetLocalBounds( const MSphere& bounds );
    void                    SetCastShadow( bool castShadow )                {   _castShadow = castShadow;   }
    void                    SetWidth( float width )                         {   _width = width;             }
    void                    SetSqrAtten( float atten )                      {   _sqrAtten = atten;          }
    void                    SetLinAtten( float atten )                      {   _linAtten = atten;          }
    void                    SetConstAtten( float atten )                    {   _constAtten = atten;        }
    void                    SetGlobal( bool global )                        {   _global = global;           }

    // accessors.
    const tstring&          GetName() const                     {   return _name;               }
    const MVec3&            GetPos() const                      {   return _position;           }
    const MVec3&            GetDir() const                      {   return _direction;          }
    const GrColor&          GetColor() const                    {   return _color;              }
    const GrColor&          GetCachedColor() const              {   return _cachedColor;        }
    float                   GetCachedIntensity() const          {   return _cachedIntensity;    }
    float                   GetCachedRange() const              {   return _cachedRange;        }
    bool                    IsParallel() const                  {   return _parallel;           }
    float                   GetSqrAtten() const                 {   return _sqrAtten;           }
    float                   GetLinAtten() const                 {   return _linAtten;           }
    float                   GetConstAtten() const               {   return _constAtten;         }
    float                   GetHaziness() const                 {   return _haziness;           }
    bool                    GetGlobal() const                   {   return _global;             }
    float                   GetWidth() const                    {   return _width;              }
    URef<GrTexture>         GetProjTexture() const              {   return _projTex;            }
    bool                    CastShadow() const                  {   return _castShadow;         }
    unsigned int            GetShadowLOD() const                {   return _shadowLOD;          }

    const MSphere&          GetLocalBounds() const              {   return _localBounds;        }
    const MSphere&          GetWorldBounds() const              {   return _worldBounds;        }

    // world to projected texture space.
    const MMat4x4&          GetTexGenMatrix() const             {   return _texgenMatrix;       }
    const MMat4x4&          GetLocalTransform() const           {   return _localTransform;     }
    const MMat4x4&          GetParentTransform() const          {   return _parentTransform;    }
    const MMat4x4&          GetWorldTransform() const           {   return _worldTransform;     }
    const GrProjection&     GetProjection() const               {   return _projection;         }

    // returns the intensity.
    URef<UExprTerm>         GetIntensity() const                {   return _intensity;          }
    URef<UExprTerm>         GetRange() const                    {   return _range;              }

    // called once per frame, just before the scene is rendered.
    void                    Update();

    // applies state necessary to render light interactions on an object.
//  void                    ApplyGlobalState() const;
//  void                    ApplyObjectState( const MMat4x4& localToWorld, const MMat4x4& worldToLocal ) const;

protected:
    void                    UpdateWorldTransform();
    void                    MakeTexgenMatrix();

    UPath                   _id;
    tstring                 _name;
    MMat4x4                 _worldTransform;
    MMat4x4                 _localTransform;
    MMat4x4                 _parentTransform;
    GrProjection            _projection;
    MMat4x4                 _texgenMatrix;
    MVec3                   _position;
    MVec3                   _direction;

    URef<GrTexture>         _projTex;
    GrColor                 _color;
    GrColor                 _cachedColor;
    URef<UExprTerm>         _intensity;
    float                   _cachedIntensity;
    URef<UExprTerm>         _range;
    float                   _cachedRange;

    MSphere                 _localBounds;
    MSphere                 _worldBounds;

    float                   _sqrAtten;
    float                   _linAtten;
    float                   _constAtten;
    float                   _haziness;

    float                   _width;
    unsigned int            _shadowLOD;
    bool                    _castShadow;
    
    bool                    _parallel;
    bool                    _global;
    ELIGHTMODE              _mode;
};
