#pragma once

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Graphics/GrTypes.h"

// project headers.
#include "EdImportSourceEngine_Constants.h"

//**********************************************************
// Source Engine types.
//**********************************************************

// TODO: remove.
typedef unsigned char byte;

// forward decls.
struct mstudiomodel_t;
void    V_StripExtension( const char *in, char *out, int outSize );

// 16 bytes
struct mstudioboneweight_t
{
    float   weight[MAX_NUM_BONES_PER_VERT];
    char    bone[MAX_NUM_BONES_PER_VERT]; 
    byte    numbones;

//  byte    material;
//  short   firstref;
//  short   lastref;
};

// NOTE: This is exactly 48 bytes
struct mstudiovertex_t
{
    mstudioboneweight_t m_BoneWeights;
    SVec3               m_vecPosition;
    SVec3               m_vecNormal;
    SVec2               m_vecTexCoord;

    mstudiovertex_t() {}

private:
    // No copy constructors allowed
    mstudiovertex_t(const mstudiovertex_t& vOther);
};

// skin info
struct mstudiotexture_t
{
    int                     sznameindex;
    inline char * const     pszName( void ) const { return ((char *)this) + sznameindex; }
    int                     flags;
    int                     used;
    int                     unused1;
    //mutable IMaterial     *material;  // fixme: this needs to go away . .isn't used by the engine, but is used by studiomdl
    //mutable void          *clientmaterial;    // gary, replace with client material pointer if used
    
    int                     unused[10];
};

struct vertexFileHeader_t
{
    int     id;                             // MODEL_VERTEX_FILE_ID
    int     version;                        // MODEL_VERTEX_FILE_VERSION
    long    checksum;                       // same as studiohdr_t, ensures sync
    int     numLODs;                        // num of valid lods
    int     numLODVertexes[MAX_NUM_LODS];   // num verts for desired root lod
    int     numFixups;                      // num of vertexFileFixup_t
    int     fixupTableStart;                // offset from base to fixup table
    int     vertexDataStart;                // offset from base to vertex block
    int     tangentDataStart;               // offset from base to tangent block

public:

    // Accessor to fat vertex data
    const mstudiovertex_t *GetVertexData() const
    {
        if ( ( id == MODEL_VERTEX_FILE_ID ) && ( vertexDataStart != 0 ) )
            return ( mstudiovertex_t * ) ( vertexDataStart + (byte *)this );
        else
            return NULL;
    }
    // Accessor to (fat) tangent vertex data (tangents aren't stored in compressed data)
    const SVec4 *GetTangentData() const
    {
        if ( ( id == MODEL_VERTEX_FILE_ID ) && ( tangentDataStart != 0 ) )
            return ( SVec4 * ) ( tangentDataStart + (byte *)this );
        else
            return NULL;
    }

    /*
    // Accessor to thin vertex data
    const  thinModelVertices_t *GetThinVertexData() const
    {
        if ( ( id == MODEL_VERTEX_FILE_THIN_ID ) && ( vertexDataStart != 0 ) )
            return ( thinModelVertices_t * ) ( vertexDataStart + (byte *)this );
        else
            return NULL;
    }
    */
};

// demand loaded sequence groups
struct mstudiomodelgroup_t
{
    int                 szlabelindex;   // textual name
    inline char * const pszLabel( void ) const { return ((char *)this) + szlabelindex; }
    int                 sznameindex;    // file name
    inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
};

struct mstudio_modelvertexdata_t
{
    SVec3               *Position( int i ) const;
    mstudiovertex_t     *Vertex( int i ) const;
    SVec3               *Normal( int i ) const;
    SVec4               *TangentS( int i ) const;
    SVec2               *Texcoord( int i ) const;
    mstudioboneweight_t *BoneWeights( int i ) const;
    bool                HasTangentData( void ) const;
    int                 GetGlobalVertexIndex( int i ) const;
    int                 GetGlobalTangentIndex( int i ) const;

    // base of external vertex data stores
    const void          *pVertexData;
    const void          *pTangentData;
};

struct mstudio_meshvertexdata_t
{
    SVec3               *Position( int i ) const;
    SVec3               *Normal( int i ) const;
    SVec4               *TangentS( int i ) const;
    SVec2               *Texcoord( int i ) const;
    mstudioboneweight_t *BoneWeights( int i ) const;
    mstudiovertex_t     *Vertex( int i ) const;
    bool                HasTangentData( void ) const;
    int                 GetModelVertexIndex( int i ) const;
    int                 GetGlobalVertexIndex( int i ) const;

    // indirection to this mesh's model's vertex data
    const mstudio_modelvertexdata_t *modelvertexdata;

    // used for fixup calcs when culling top level lods
    // expected number of mesh verts at desired lod
    int                 numLODVertexes[MAX_NUM_LODS];
};

struct mstudiomesh_t
{
    int                 material;

    int                 modelindex;
    mstudiomodel_t *pModel() const; 

    int                 numvertices;        // number of unique vertices/normals/texcoords
    int                 vertexoffset;       // vertex mstudiovertex_t

    // Access thin/fat mesh vertex data (only one will return a non-NULL result)
    const mstudio_meshvertexdata_t  *GetVertexData(     void *pModelData = NULL );
    //const thinModelVertices_t     *GetThinVertexData( void *pModelData = NULL );

    int                 numflexes;          // vertex animation
    int                 flexindex;
    //inline mstudioflex_t *pFlex( int i ) const { return (mstudioflex_t *)(((byte *)this) + flexindex) + i; };

    // special codes for material operations
    int                 materialtype;
    int                 materialparam;

    // a unique ordinal for this mesh
    int                 meshid;

    SVec3               center;

    mstudio_meshvertexdata_t vertexdata;

    int                 unused[8]; // remove as appropriate

    mstudiomesh_t(){}
private:
    // No copy constructors allowed
    mstudiomesh_t(const mstudiomesh_t& vOther);
};

// studio models
struct mstudiomodel_t
{
    inline const char * pszName( void ) const { return name; }
    char                name[64];

    int                 type;

    float               boundingradius;

    int                 nummeshes;  
    int                 meshindex;
    inline mstudiomesh_t *pMesh( int i ) const { return (mstudiomesh_t *)(((byte *)this) + meshindex) + i; };

    // cache purposes
    int                 numvertices;        // number of unique vertices/normals/texcoords
    int                 vertexindex;        // vertex Vector
    int                 tangentsindex;      // tangents Vector

    // These functions are defined in application-specific code:
    const vertexFileHeader_t            *CacheVertexData(           void *pModelData );

    // Access thin/fat mesh vertex data (only one will return a non-NULL result)
    const mstudio_modelvertexdata_t     *GetVertexData(     void *pModelData = NULL );
    //const thinModelVertices_t         *GetThinVertexData( void *pModelData = NULL );

    int                 numattachments;
    int                 attachmentindex;

    int                 numeyeballs;
    int                 eyeballindex;
    //inline  mstudioeyeball_t *pEyeball( int i ) { return (mstudioeyeball_t *)(((byte *)this) + eyeballindex) + i; };

    mstudio_modelvertexdata_t vertexdata;

    int                 unused[8];      // remove as appropriate
};

// body part index
struct mstudiobodyparts_t
{
    int                 sznameindex;
    inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
    int                 nummodels;
    int                 base;
    int                 modelindex; // index into models array
    inline mstudiomodel_t *pModel( int i ) const { return (mstudiomodel_t *)(((byte *)this) + modelindex) + i; };
};

struct studiohdr_t
{
    int                 id;
    int                 version;

    long                checksum;       // this has to be the same in the phy and vtx files to load!

    inline const char * pszName( void ) const { return name; }
    char                name[64];
    int                 length;


    SVec3               eyeposition;    // ideal eye position

    SVec3               illumposition;  // illumination center

    SVec3               hull_min;       // ideal movement hull size
    SVec3               hull_max;           

    SVec3               view_bbmin;     // clipping bounding box
    SVec3               view_bbmax;     

    int                 flags;

    int                 numbones;           // bones
    int                 boneindex;
    //inline mstudiobone_t *pBone( int i ) const { Assert( i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
    //int                   RemapSeqBone( int iSequence, int iLocalBone ) const;    // maps local sequence bone to global bone
    //int                   RemapAnimBone( int iAnim, int iLocalBone ) const;       // maps local animations bone to global bone

    int                 numbonecontrollers;     // bone controllers
    int                 bonecontrollerindex;
    //inline mstudiobonecontroller_t *pBonecontroller( int i ) const { Assert( i >= 0 && i < numbonecontrollers); return (mstudiobonecontroller_t *)(((byte *)this) + bonecontrollerindex) + i; };

    int                 numhitboxsets;
    int                 hitboxsetindex;

    /*
    // Look up hitbox set by index
    mstudiohitboxset_t  *pHitboxSet( int i ) const 
    { 
        Assert( i >= 0 && i < numhitboxsets); 
        return (mstudiohitboxset_t *)(((byte *)this) + hitboxsetindex ) + i; 
    };

    // Calls through to hitbox to determine size of specified set
    inline mstudiobbox_t *pHitbox( int i, int set ) const 
    { 
        mstudiohitboxset_t const *s = pHitboxSet( set );
        if ( !s )
            return NULL;

        return s->pHitbox( i );
    };

    // Calls through to set to get hitbox count for set
    inline int          iHitboxCount( int set ) const
    {
        mstudiohitboxset_t const *s = pHitboxSet( set );
        if ( !s )
            return 0;

        return s->numhitboxes;
    };
    */

    // file local animations? and sequences
    //private:
    int                 numlocalanim;           // animations/poses
    int                 localanimindex;     // animation descriptions
    //inline mstudioanimdesc_t *pLocalAnimdesc( int i ) const { if (i < 0 || i >= numlocalanim) i = 0; return (mstudioanimdesc_t *)(((byte *)this) + localanimindex) + i; };

    int                 numlocalseq;                // sequences
    int                 localseqindex;
    //inline mstudioseqdesc_t *pLocalSeqdesc( int i ) const { if (i < 0 || i >= numlocalseq) i = 0; return (mstudioseqdesc_t *)(((byte *)this) + localseqindex) + i; };

    //public:
    //bool              SequencesAvailable() const;
    //int                   GetNumSeq() const;
    //mstudioanimdesc_t &pAnimdesc( int i ) const;
    //mstudioseqdesc_t  &pSeqdesc( int i ) const;
    //int                   iRelativeAnim( int baseseq, int relanim ) const;    // maps seq local anim reference to global anim index
    //int                   iRelativeSeq( int baseseq, int relseq ) const;      // maps seq local seq reference to global seq index

    //private:
    mutable int         activitylistversion;    // initialization flag - have the sequences been indexed?
    mutable int         eventsindexed;
    //public:
    //int                   GetSequenceActivity( int iSequence );
    //void              SetSequenceActivity( int iSequence, int iActivity );
    //int                   GetActivityListVersion( void ) const;
    //void              SetActivityListVersion( int version ) const;
    //int                   GetEventListVersion( void ) const;
    //void              SetEventListVersion( int version ) const;

    // raw textures
    int                 numtextures;
    int                 textureindex;
    inline mstudiotexture_t *pTexture( int i ) const { B_ASSERT( i >= 0 && i < numtextures ); return (mstudiotexture_t *)(((byte *)this) + textureindex) + i; }; 


    // raw textures search paths
    int                 numcdtextures;
    int                 cdtextureindex;
    inline char         *pCdtexture( int i ) const { return (((char *)this) + *((int *)(((byte *)this) + cdtextureindex) + i)); };

    // replaceable textures tables
    int                 numskinref;
    int                 numskinfamilies;
    int                 skinindex;
    //inline short      *pSkinref( int i ) const { return (short *)(((byte *)this) + skinindex) + i; };

    int                 numbodyparts;       
    int                 bodypartindex;
    inline mstudiobodyparts_t   *pBodypart( int i ) const { return (mstudiobodyparts_t *)(((byte *)this) + bodypartindex) + i; };

    // queryable attachable points
    //private:
    int                 numlocalattachments;
    int                 localattachmentindex;
    //inline mstudioattachment_t    *pLocalAttachment( int i ) const { Assert( i >= 0 && i < numlocalattachments); return (mstudioattachment_t *)(((byte *)this) + localattachmentindex) + i; };
    //public:
    //int                   GetNumAttachments( void ) const;
    //const mstudioattachment_t &pAttachment( int i ) const;
    //int                   GetAttachmentBone( int i ) const;
    // used on my tools in hlmv, not persistant
    //void              SetAttachmentBone( int iAttachment, int iBone );

    // animation node to animation node transition graph
    //private:
    int                 numlocalnodes;
    int                 localnodeindex;
    int                 localnodenameindex;
    //inline char           *pszLocalNodeName( int iNode ) const { Assert( iNode >= 0 && iNode < numlocalnodes); return (((char *)this) + *((int *)(((byte *)this) + localnodenameindex) + iNode)); }
    //inline byte           *pLocalTransition( int i ) const { Assert( i >= 0 && i < (numlocalnodes * numlocalnodes)); return (byte *)(((byte *)this) + localnodeindex) + i; };

    //public:
    //int                   EntryNode( int iSequence ) const;
    //int                   ExitNode( int iSequence ) const;
    //char              *pszNodeName( int iNode ) const;
    //int                   GetTransition( int iFrom, int iTo ) const;

    int                 numflexdesc;
    int                 flexdescindex;
    //inline mstudioflexdesc_t *pFlexdesc( int i ) const { Assert( i >= 0 && i < numflexdesc); return (mstudioflexdesc_t *)(((byte *)this) + flexdescindex) + i; };

    int                 numflexcontrollers;
    int                 flexcontrollerindex;
    //inline mstudioflexcontroller_t *pFlexcontroller( LocalFlexController_t i ) const { Assert( i >= 0 && i < numflexcontrollers); return (mstudioflexcontroller_t *)(((byte *)this) + flexcontrollerindex) + i; };

    int                 numflexrules;
    int                 flexruleindex;
    //inline mstudioflexrule_t *pFlexRule( int i ) const { Assert( i >= 0 && i < numflexrules); return (mstudioflexrule_t *)(((byte *)this) + flexruleindex) + i; };

    int                 numikchains;
    int                 ikchainindex;
    //inline mstudioikchain_t *pIKChain( int i ) const { Assert( i >= 0 && i < numikchains); return (mstudioikchain_t *)(((byte *)this) + ikchainindex) + i; };

    int                 nummouths;
    int                 mouthindex;
    //inline mstudiomouth_t *pMouth( int i ) const { Assert( i >= 0 && i < nummouths); return (mstudiomouth_t *)(((byte *)this) + mouthindex) + i; };

    //private:
    int                 numlocalposeparameters;
    int                 localposeparamindex;
    //inline mstudioposeparamdesc_t *pLocalPoseParameter( int i ) const { Assert( i >= 0 && i < numlocalposeparameters); return (mstudioposeparamdesc_t *)(((byte *)this) + localposeparamindex) + i; };
    //public:
    //int                   GetNumPoseParameters( void ) const;
    //const mstudioposeparamdesc_t &pPoseParameter( int i ) const;
    //int                   GetSharedPoseParameter( int iSequence, int iLocalPose ) const;

    int                 surfacepropindex;
    //inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropindex; }

    // Key values
    int                 keyvalueindex;
    int                 keyvaluesize;
    //inline const char * KeyValueText( void ) const { return keyvaluesize != 0 ? ((char *)this) + keyvalueindex : NULL; }

    int                 numlocalikautoplaylocks;
    int                 localikautoplaylockindex;
    //inline mstudioiklock_t *pLocalIKAutoplayLock( int i ) const { Assert( i >= 0 && i < numlocalikautoplaylocks); return (mstudioiklock_t *)(((byte *)this) + localikautoplaylockindex) + i; };

    //int                   GetNumIKAutoplayLocks( void ) const;
    //const mstudioiklock_t &pIKAutoplayLock( int i ) const;
    //int                   CountAutoplaySequences() const;
    //int                   CopyAutoplaySequences( unsigned short *pOut, int outCount ) const;
    //int                   GetAutoplayList( unsigned short **pOut ) const;

    // The collision model mass that jay wanted
    float               mass;
    int                 contents;

    // external animations, models, etc.
    int                 numincludemodels;
    int                 includemodelindex;
    inline mstudiomodelgroup_t *pModelGroup( int i ) const { B_ASSERT( i >= 0 && i < numincludemodels); return (mstudiomodelgroup_t *)(((byte *)this) + includemodelindex) + i; };
    // implementation specific call to get a named model
    //const studiohdr_t *FindModel( void **cache, char const *modelname ) const;

    // implementation specific back pointer to virtual data
    mutable void        *virtualModel;
    //virtualmodel_t        *GetVirtualModel( void ) const;

    // for demand loaded animation blocks
    int                 szanimblocknameindex;   
    //inline char * const pszAnimBlockName( void ) const { return ((char *)this) + szanimblocknameindex; }
    int                 numanimblocks;
    int                 animblockindex;
    //inline mstudioanimblock_t *pAnimBlock( int i ) const { Assert( i > 0 && i < numanimblocks); return (mstudioanimblock_t *)(((byte *)this) + animblockindex) + i; };
    mutable void        *animblockModel;
    //byte *                GetAnimBlock( int i ) const;

    int                 bonetablebynameindex;
    //inline const byte *GetBoneTableSortedByName() const { return (byte *)this + bonetablebynameindex; }

    // used by tools only that don't cache, but persist mdl's peer data
    // engine uses virtualModel to back link to cache pointers
    void                *pVertexBase;
    void                *pIndexBase;

    // if STUDIOHDR_FLAGS_CONSTANT_DIRECTIONAL_LIGHT_DOT is set,
    // this value is used to calculate directional components of lighting 
    // on static props
    byte                constdirectionallightdot;

    // set during load of mdl data to track *desired* lod configuration (not actual)
    // the *actual* clamped root lod is found in studiohwdata
    // this is stored here as a global store to ensure the staged loading matches the rendering
    byte                rootLOD;

    // set in the mdl data to specify that lod configuration should only allow first numAllowRootLODs
    // to be set as root LOD:
    //  numAllowedRootLODs = 0  means no restriction, any lod can be set as root lod.
    //  numAllowedRootLODs = N  means that lod0 - lod(N-1) can be set as root lod, but not lodN or lower.
    byte                numAllowedRootLODs;

    byte                unused[1];

    int                 unused4; // zero out if version < 47

    int                 numflexcontrollerui;
    int                 flexcontrolleruiindex;
    //mstudioflexcontrollerui_t *pFlexControllerUI( int i ) const { Assert( i >= 0 && i < numflexcontrollerui); return (mstudioflexcontrollerui_t *)(((byte *)this) + flexcontrolleruiindex) + i; }

    int                 unused3[2];

    // FIXME: Remove when we up the model version. Move all fields of studiohdr2_t into studiohdr_t.
    int                 studiohdr2index;
    //studiohdr2_t*     pStudioHdr2() const { return (studiohdr2_t *)( ( (byte *)this ) + studiohdr2index ); }

    // Src bone transforms are transformations that will convert .dmx or .smd-based animations into .mdl-based animations
    //int                   NumSrcBoneTransforms() const { return studiohdr2index ? pStudioHdr2()->numsrcbonetransform : 0; }
    //const mstudiosrcbonetransform_t* SrcBoneTransform( int i ) const { Assert( i >= 0 && i < NumSrcBoneTransforms()); return (mstudiosrcbonetransform_t *)(((byte *)this) + pStudioHdr2()->srcbonetransformindex) + i; }

    //inline int            IllumPositionAttachmentIndex() const { return studiohdr2index ? pStudioHdr2()->IllumPositionAttachmentIndex() : 0; }

    //inline float      MaxEyeDeflection() const { return studiohdr2index ? pStudioHdr2()->MaxEyeDeflection() : 0.866f; } // default to cos(30) if not set

    //inline mstudiolinearbone_t *pLinearBones() const { return studiohdr2index ? pStudioHdr2()->pLinearBones() : NULL; }

    // NOTE: No room to add stuff? Up the .mdl file format version 
    // [and move all fields in studiohdr2_t into studiohdr_t and kill studiohdr2_t],
    // or add your stuff to studiohdr2_t. See NumSrcBoneTransforms/SrcBoneTransform for the pattern to use.
    int                 unused2[1];

    studiohdr_t() {}

private:
    // No copy constructors allowed
    studiohdr_t(const studiohdr_t& vOther);

    friend struct virtualmodel_t;
};

// apply sequentially to lod sorted vertex and tangent pools to re-establish mesh order
struct vertexFileFixup_t
{
    int     lod;                // used to skip culled root lod
    int     sourceVertexID;     // absolute index from start of vertex/tangent blocks
    int     numVertexes;
};

//**********************************************************
// Source Engine implementations.
//**********************************************************

//**********************************************************
// mstudiomodel_t
//**********************************************************

// model vertex data accessor (defined here so vertexFileHeader_t can be used)
inline const mstudio_modelvertexdata_t * mstudiomodel_t::GetVertexData( void *pModelData )
{
    const vertexFileHeader_t * pVertexHdr = CacheVertexData( pModelData );
    if ( !pVertexHdr )
        return NULL;

    vertexdata.pVertexData  = pVertexHdr->GetVertexData();
    vertexdata.pTangentData = pVertexHdr->GetTangentData();

    if ( !vertexdata.pVertexData )
        return NULL;

    return &vertexdata;
}

//**********************************************************
// mstudio_modelvertexdata_t
//**********************************************************

inline mstudiovertex_t *mstudio_modelvertexdata_t::Vertex( int i ) const 
{
    return (mstudiovertex_t *)pVertexData + GetGlobalVertexIndex( i );
}

inline SVec3 *mstudio_modelvertexdata_t::Position( int i ) const 
{
    mstudiovertex_t* vert = Vertex(i);
    return &vert->m_vecPosition;
}

inline SVec3 *mstudio_modelvertexdata_t::Normal( int i ) const 
{ 
    return &Vertex(i)->m_vecNormal;
}

inline SVec4 *mstudio_modelvertexdata_t::TangentS( int i ) const 
{
    // NOTE: The tangents vector is 16-bytes in a separate array
    // because it only exists on the high end, and if I leave it out
    // of the mstudiovertex_t, the vertex is 64-bytes (good for low end)
    return (SVec4 *)pTangentData + GetGlobalTangentIndex( i );
}

inline SVec2 *mstudio_modelvertexdata_t::Texcoord( int i ) const 
{ 
    return &Vertex(i)->m_vecTexCoord;
}

inline mstudioboneweight_t *mstudio_modelvertexdata_t::BoneWeights( int i ) const 
{
    return &Vertex(i)->m_BoneWeights;
}

inline bool mstudio_modelvertexdata_t::HasTangentData( void ) const 
{
    return (pTangentData != NULL);
}

inline int mstudio_modelvertexdata_t::GetGlobalVertexIndex( int i ) const
{
    mstudiomodel_t *modelptr = (mstudiomodel_t *)((byte *)this - offsetof(mstudiomodel_t, vertexdata));
    B_ASSERT( ( modelptr->vertexindex % sizeof( mstudiovertex_t ) ) == 0 );
    return ( i + ( modelptr->vertexindex / sizeof( mstudiovertex_t ) ) );
}

inline int mstudio_modelvertexdata_t::GetGlobalTangentIndex( int i ) const
{
    mstudiomodel_t *modelptr = (mstudiomodel_t *)((byte *)this - offsetof(mstudiomodel_t, vertexdata));
    B_ASSERT( ( modelptr->tangentsindex % sizeof( SVec4 ) ) == 0 );
    return ( i + ( modelptr->tangentsindex / sizeof( SVec4 ) ) );
}

//**********************************************************
// mstudiomesh_t
//**********************************************************

inline mstudiomodel_t *mstudiomesh_t::pModel() const 
{ 
    return (mstudiomodel_t *)(((byte *)this) + modelindex); 
}

inline bool mstudio_meshvertexdata_t::HasTangentData( void ) const
{
    return modelvertexdata->HasTangentData();
}

inline const mstudio_meshvertexdata_t *mstudiomesh_t::GetVertexData( void *pModelData )
{
    // get this mesh's model's vertex data (allow for mstudiomodel_t::GetVertexData
    // returning NULL if the data has been converted to 'thin' vertices)
    this->pModel()->GetVertexData( pModelData );
    vertexdata.modelvertexdata = &( this->pModel()->vertexdata );

    if ( !vertexdata.modelvertexdata->pVertexData )
        return NULL;

    return &vertexdata;
}

/*
inline const thinModelVertices_t * mstudiomesh_t::GetThinVertexData( void *pModelData )
{
    // get this mesh's model's thin vertex data
    return this->pModel()->GetThinVertexData( pModelData );
}
*/

inline int mstudio_meshvertexdata_t::GetModelVertexIndex( int i ) const
{
    mstudiomesh_t *meshptr = (mstudiomesh_t *)((byte *)this - offsetof(mstudiomesh_t,vertexdata)); 
    return meshptr->vertexoffset + i;
}

inline int mstudio_meshvertexdata_t::GetGlobalVertexIndex( int i ) const
{
    return modelvertexdata->GetGlobalVertexIndex( GetModelVertexIndex( i ) );
}

inline SVec3 *mstudio_meshvertexdata_t::Position( int i ) const 
{
    return modelvertexdata->Position( GetModelVertexIndex( i ) ); 
};

inline SVec3 *mstudio_meshvertexdata_t::Normal( int i ) const 
{
    return modelvertexdata->Normal( GetModelVertexIndex( i ) ); 
};

inline SVec4 *mstudio_meshvertexdata_t::TangentS( int i ) const
{
    return modelvertexdata->TangentS( GetModelVertexIndex( i ) );
}

inline SVec2 *mstudio_meshvertexdata_t::Texcoord( int i ) const 
{
    return modelvertexdata->Texcoord( GetModelVertexIndex( i ) ); 
};

inline mstudioboneweight_t *mstudio_meshvertexdata_t::BoneWeights( int i ) const 
{
    return modelvertexdata->BoneWeights( GetModelVertexIndex( i ) ); 
};

inline mstudiovertex_t *mstudio_meshvertexdata_t::Vertex( int i ) const
{
    return modelvertexdata->Vertex( GetModelVertexIndex( i ) );
}
