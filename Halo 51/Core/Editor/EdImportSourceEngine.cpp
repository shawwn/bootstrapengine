//----------------------------------------------------------
// File:        EdImportSourceEngine.cpp
// Author:      Shawn Presser
// Created:     12-21-08
// Copyright © 2008 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------

// class header.
#include "EdImportSourceEngine.h"

// Bootstrap Engine headers.
#include "Common/common_afx.h"
#include "Common/UPath.h"
#include "Common/URef.h"
#include "Common/UWriter.h"
#include "Common/RFileMgr.h"
#include "Common/RFile.h"
#include "Common/MMat3x3.h"
#include "Graphics/GrTypes.h"
#include "Graphics/GrModel.h"
#include "Graphics/GrMaterialMgr.h"
#include "Graphics/GrMaterial.h"
#include "Graphics/GrPolygonNode.h"
#include "Graphics/GrPolygonMesh.h"
#include "Graphics/GrPolygonGroup.h"
#include "Graphics/GrPolygon.h"

// EditLib headers.
#include "EditLib/EdUtil.h"

// project headers.
#include "EdImportSourceEngine_Constants.h"
#include "EdImportSourceEngine_Types.h"
#include "EdImportSourceEngine_OptimizedMeshTypes.h"

// TODO: remove.
#define HARDCODED_BASE_DIR      "G:\\prog\\media\\left4dead\\"
#define HARDCODED_WORKING_DIR   "C:\\bin\\game\\"

// forward declarations.
static void*            EdReadFile( const char* path, size_t* outSize = 0 );

//**********************************************************
// class EdImportSourceEngine
//**********************************************************

//----------------------------------------------------------
void V_StripExtension( const char *in, char *out, int outSize )
{
    // Find the last dot. If it's followed by a dot or a slash, then it's part of a 
    // directory specifier like ../../somedir/./blah.

    // scan backward for '.'
    int end = strlen( in ) - 1;
    while ( end > 0 && in[end] != '.' && !PATHSEPARATOR( in[end] ) )
    {
        --end;
    }

    if (end > 0 && !PATHSEPARATOR( in[end] ) && end < outSize)
    {
        int nChars = Min( end, outSize-1 );
        if ( out != in )
        {
            memcpy( out, in, nChars );
        }
        out[nChars] = 0;
    }
    else
    {
        // nothing found
        if ( out != in )
        {
            strncpy( out, in, outSize );
        }
    }
}

// Load the minimum quantity of verts and run fixups
inline int Studio_LoadVertexes( const vertexFileHeader_t *pTempVvdHdr, vertexFileHeader_t *pNewVvdHdr, int rootLOD, bool bNeedsTangentS )
{
    int                 i;
    int                 target;
    int                 numVertexes;
    vertexFileFixup_t   *pFixupTable;

    numVertexes = pTempVvdHdr->numLODVertexes[rootLOD];

    // copy all data up to start of vertexes
    memcpy((void*)pNewVvdHdr, (void*)pTempVvdHdr, pTempVvdHdr->vertexDataStart);

    for ( i = 0; i < rootLOD; i++)
    {
        pNewVvdHdr->numLODVertexes[i] = pNewVvdHdr->numLODVertexes[rootLOD];
    }

    // fixup data starts
    if (bNeedsTangentS)
    {
        // tangent data follows possibly reduced vertex data
        pNewVvdHdr->tangentDataStart = pNewVvdHdr->vertexDataStart + numVertexes*sizeof(mstudiovertex_t);
    }
    else
    {
        // no tangent data will be available, mark for identification
        pNewVvdHdr->tangentDataStart = 0;
    }

    if (!pNewVvdHdr->numFixups)
    {       
        // fixups not required
        // transfer vertex data
        memcpy(
            (byte *)pNewVvdHdr+pNewVvdHdr->vertexDataStart, 
            (byte *)pTempVvdHdr+pTempVvdHdr->vertexDataStart,
            numVertexes*sizeof(mstudiovertex_t) );

        if (bNeedsTangentS)
        {
            // transfer tangent data to cache memory
            memcpy(
                (byte *)pNewVvdHdr+pNewVvdHdr->tangentDataStart, 
                (byte *)pTempVvdHdr+pTempVvdHdr->tangentDataStart,
                numVertexes*sizeof(SVec4) );
        }

        return numVertexes;
    }

    // fixups required
    // re-establish mesh ordered vertexes into cache memory, according to table
    target      = 0;
    pFixupTable = (vertexFileFixup_t *)((byte *)pTempVvdHdr + pTempVvdHdr->fixupTableStart);
    for (i=0; i<pTempVvdHdr->numFixups; i++)
    {
        if (pFixupTable[i].lod < rootLOD)
        {
            // working bottom up, skip over copying higher detail lods
            continue;
        }

        // copy vertexes
        memcpy(
            (mstudiovertex_t *)((byte *)pNewVvdHdr+pNewVvdHdr->vertexDataStart) + target,
            (mstudiovertex_t *)((byte *)pTempVvdHdr+pTempVvdHdr->vertexDataStart) + pFixupTable[i].sourceVertexID,
            pFixupTable[i].numVertexes*sizeof(mstudiovertex_t) );

        if (bNeedsTangentS)
        {
            // copy tangents
            memcpy(
                (SVec4 *)((byte *)pNewVvdHdr+pNewVvdHdr->tangentDataStart) + target,
                (SVec4 *)((byte *)pTempVvdHdr+pTempVvdHdr->tangentDataStart) + pFixupTable[i].sourceVertexID,
                pFixupTable[i].numVertexes*sizeof(SVec4) );
        }

        // data is placed consecutively
        target += pFixupTable[i].numVertexes;
    }

    pNewVvdHdr->numFixups = 0;

    return target;
}

//----------------------------------------------------------
static void*
EdReadFile( const char* path, size_t* outSize )
{
    // clear the returned size if necessary.
    if ( outSize )
    {
        *outSize = 0;
    }

    // open the file.
    FILE* fp = fopen( path, "rb" );
    if ( !fp )
    {
        B_ASSERT( !"invalid file." );
        return false;
    }

    // read the file into memory.
    fseek( fp, 0, SEEK_END );
    size_t size = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    void* buf = malloc( size );
    fread( buf, 1, size, fp );
    fclose( fp );

    // return the size if necessary.
    if ( outSize )
    {
        *outSize = size;
    }

    return buf;
}

// app-specific vertex caching routine
const vertexFileHeader_t * mstudiomodel_t::CacheVertexData( void *pModelData )
{
    studiohdr_t *pActiveStudioHdr = static_cast<studiohdr_t *>(pModelData);
    B_ASSERT( pActiveStudioHdr );

    if ( pActiveStudioHdr->pVertexBase )
    {
        return (vertexFileHeader_t *)pActiveStudioHdr->pVertexBase;
    }

    // mandatory callback to make requested data resident
    // load and persist the vertex file
    char fileName[MAX_PATH];
    strcpy( fileName, "models/" );  
    strcat( fileName, pActiveStudioHdr->pszName() );
    V_StripExtension( fileName, fileName, sizeof( fileName ) );
    strcat( fileName, ".vvd" );

    // load the model.
    // TODO: DON'T HARDCODE THIS!!
    size_t vvdSize = 0;
    vertexFileHeader_t* pVvdHdr = (vertexFileHeader_t*)EdReadFile( ( tstring( HARDCODED_BASE_DIR ) + fileName ).c_str(), &vvdSize );

    // check header
    if ( pVvdHdr->id != MODEL_VERTEX_FILE_ID )
    {
        HandleError("Error Vertex File %s id %d should be %d\n", fileName, pVvdHdr->id, MODEL_VERTEX_FILE_ID);
    }
    if ( pVvdHdr->version != MODEL_VERTEX_FILE_VERSION )
    {
        HandleError("Error Vertex File %s version %d should be %d\n", fileName, pVvdHdr->version, MODEL_VERTEX_FILE_VERSION);
    }
    if ( pVvdHdr->checksum != pActiveStudioHdr->checksum )
    {
        HandleError("Error Vertex File %s checksum %d should be %d\n", fileName, pVvdHdr->checksum, pActiveStudioHdr->checksum);
    }

    // need to perform mesh relocation fixups
    // allocate a new copy
    vertexFileHeader_t *pNewVvdHdr = (vertexFileHeader_t *)malloc( vvdSize );
    if ( !pNewVvdHdr )
    {
        HandleError( "Error allocating %d bytes for Vertex File '%s'\n", vvdSize, fileName );
    }

    // load vertexes and run fixups
    Studio_LoadVertexes( pVvdHdr, pNewVvdHdr, 0, true );

    // discard original
    free( pVvdHdr );
    pVvdHdr = pNewVvdHdr;

    pActiveStudioHdr->pVertexBase = (void*)pVvdHdr;
    return pVvdHdr;
}

//**********************************************************
// class EdImportSourceEngine
//**********************************************************

//----------------------------------------------------------
EdImportSourceEngine::EdImportSourceEngine()
{
}

//----------------------------------------------------------
bool
EdImportSourceEngine::ImportModel( const char* name, const char* targetPackageName, const char* path, float scale )
{
    // just use a default material for now.
    URef< GrMaterial > defaultMaterial = gGrMaterialMgr->GetDefaultMaterial();

    // read the mdl into memory.
    void* mdlPtr = EdReadFile( ( tstring( path ) + ".mdl" ).c_str() );
    void* vvdPtr = EdReadFile( ( tstring( path ) + ".vvd" ).c_str() );
    void* vtxPtr = EdReadFile( ( tstring( path ) + ".vtx" ).c_str() );

    studiohdr_t* mdlHeader = (studiohdr_t*)mdlPtr;
    vertexFileHeader_t* vvdHeader = (vertexFileHeader_t*)vvdPtr;
    OptimizedModel::FileHeader_t* vtxHeader = (OptimizedModel::FileHeader_t*)vtxPtr;

    if ( !mdlHeader || !vtxHeader )
    {
        B_ASSERT( false );
        return false;
    }

    // determine package path.
    UPath workingDir( HARDCODED_WORKING_DIR ); // TODO: don't hardcode this.
    UPath targetPath( "data" );
    targetPath += targetPackageName;

    tstring materialTxt;

    // try to load the existing materials file.
    UPath materialFileName = targetPath + GR_MATERIAL_FILENAME;

    // read the text in the existing material file.
    URef< RFile > mtlFile = gRFileMgr->GetFile( workingDir + materialFileName, RFileMgr::kFlagRead );
    if ( !mtlFile )
    {
        // material file output.
        materialTxt =
            "//----------------------------------------------------------\n"
            "// File:       "GR_MATERIAL_FILENAME"\n"
            "// Author:     BootEdit DAE Importer (: Kevin Bray's proxy :)\n"
            "// ";
        materialTxt += "// Copyright (C) 2004 Bootstrap Games.  All rights reserved.\n"
            "//----------------------------------------------------------\n"
            ;
    }
    else
    {
        // initialize the material data to what is currently in the file.
        materialTxt = ( char* )mtlFile->GetData();

        // free the file's data.
        mtlFile->Purge();
    }

    // start writing data to the material file.
    mtlFile = gRFileMgr->GetFile( materialFileName, RFileMgr::kFlagWrite );

    GrPolygonNode* polyNode = new GrPolygonNode( "__GlobalRoot", MMat4x4( MMat3x3(), MVec3( 0.0f, 0.0f, 0.0f ), MVec3( scale, scale, scale ) ) );

    unsigned int degenerateTriCount = 0;

    for ( int bodyPartIdx = 0; bodyPartIdx < mdlHeader->numbodyparts; ++bodyPartIdx )
    {
        OptimizedModel::BodyPartHeader_t* pVtxBodyPart = vtxHeader->pBodyPart( bodyPartIdx );
        mstudiobodyparts_t* bodyParts = mdlHeader->pBodypart( bodyPartIdx );

        const char* name = bodyParts->pszName();

        GrPolygonGroup polygons;
        for ( int modelIdx = 0; modelIdx < bodyParts->nummodels; ++modelIdx )
        {
            OptimizedModel::ModelHeader_t* vtxModel = pVtxBodyPart->pModel( modelIdx );
            mstudiomodel_t* model = bodyParts->pModel( modelIdx );

            // get the specified lod, assuming lod 0
            int nLod = 0;
            OptimizedModel::ModelLODHeader_t *vtxLOD = vtxModel->pLOD( nLod );

            GrPolygonGroup curPolygons;
            for ( int meshIdx = 0; meshIdx < model->nummeshes; ++meshIdx )
            {
                OptimizedModel::MeshHeader_t* vtxMesh = vtxLOD->pMesh( meshIdx );
                mstudiomesh_t* mesh = model->pMesh( meshIdx );
                mstudiotexture_t* texture = mdlHeader->pTexture( mesh->material );

                const char* textureName = texture->pszName();

                const mstudio_meshvertexdata_t* vertData = mesh->GetVertexData( (void*)mdlHeader );

                bool hasTangentData = vertData->HasTangentData();
                unsigned int components = GrPolygon::kNormal | GrPolygon::kUV;

                if ( hasTangentData )
                {
                    components |= GrPolygon::kBiNormal | GrPolygon::kTangent;
                }

                URef< GrMaterial > engineMaterial = defaultMaterial;

                if ( strlen( textureName ) > 0 )
                {
                    // build the name of the part's material.
                    UPath materialName( EdFixupTexturePath( targetPath, textureName ) );

                    // if that material hasn't been imported, then import it.
                    tstring matErrors;
                    engineMaterial = gGrMaterialMgr->GetMaterial( materialName, matErrors );
                    if ( !engineMaterial )
                    {
                        tstring texFilePath( EdFixupTexturePath( targetPath, textureName ).GetPathString() );

                        // write the name of the material.
                        materialTxt += "\n";
                        materialTxt += materialName.GetPathString();

                        // write out the shader.
                        materialTxt += "\n{\n\t{\n";
                        materialTxt += "\t\tShader\t\t\tblinn\n";

                        // color map.
                        if ( gRFileMgr->Exists( texFilePath + "_c.tga" ) )
                        {
                            materialTxt += "\t\t";
                            materialTxt += "DiffuseMap\t\t";
                            materialTxt += texFilePath + "_c.tga";
                            materialTxt += "\n";
                        }

                        // normal map.
                        if ( gRFileMgr->Exists( texFilePath + "_n.tga" ) )
                        {
                            materialTxt += "\t\t";
                            materialTxt += "NormalMap\t\t";
                            materialTxt += texFilePath + "_n.tga";
                            materialTxt += "\n";
                        }

                        materialTxt += "\t}\n}\n";

                        // write out the material file.
                        mtlFile->WriteData( 0, materialTxt.c_str(), ( unsigned int )materialTxt.length() );

                        // try to load the new material.
                        tstring matErrors;
                        engineMaterial = gGrMaterialMgr->GetMaterial( materialName, matErrors );

                        // if the material still couldn't be loaded, use the default material.
                        if ( !engineMaterial )
                        {
                            B_ASSERT( !"failed to create a material." );
                            engineMaterial = defaultMaterial;
                        }
                    }
                }

                for ( int stripGroupIdx = 0; stripGroupIdx < vtxMesh->numStripGroups; ++stripGroupIdx )
                {
                    OptimizedModel::StripGroupHeader_t* stripGroup = vtxMesh->pStripGroup( stripGroupIdx );

                    for ( int stripIdx = 0; stripIdx < stripGroup->numStrips; stripIdx++ )
                    {
                        OptimizedModel::StripHeader_t *strip = stripGroup->pStrip( stripIdx );

                        if ( strip->flags & OptimizedModel::STRIP_IS_TRISTRIP )
                        {
                            // all tris expected to be discrete tri lists
                            // must fixme if stripping ever occurs
                            HandleError( "unexpected strips found\n" );
                            return false;
                        }

                        for ( int i = 0; i < strip->numIndices; i += 3 )
                        {
                            int idx = strip->indexOffset + i;

                            SPolygonVertex verts[ 3 ];

                            for ( int j = 0; j < 3; ++j )
                            {
                                unsigned short iVertIndir = *stripGroup->pIndex( idx + j );
                                unsigned short iVert = stripGroup->pVertex( iVertIndir )->origMeshVertID;

                                verts[ j ].position = *vertData->Position( iVert );
                                verts[ j ].normal = *vertData->Normal( iVert );
                                //verts[ j ].uv = *vertData->Texcoord( iVert );
                                verts[ j ].uv = vertData->Vertex( iVert )->m_vecTexCoord;

                                // TODO: for some reason, V needs to be negated.
                                verts[ j ].uv.y = -verts[ j ].uv.y;

                                if ( hasTangentData )
                                {
                                    SVec4* tangentS = vertData->TangentS( iVert );
                                    verts[ j ].tangent = *(SVec3*)tangentS;
                                    verts[ j ].binormal = verts[ j ].normal.Cross( verts[ j ].tangent );
                                    verts[ j ].binormal *= tangentS->w;
                                    verts[ j ].binormal.Normalize(); // TODO: verify that this is okay.

                                    // verify that the incoming binormals and tangents are valid.
                                    B_ASSERT( verts[ j ].tangent.MagSqr() > 0.9f && verts[ j ].tangent.MagSqr() < 1.1f );
                                    B_ASSERT( verts[ j ].binormal.MagSqr() > 0.9f && verts[ j ].binormal.MagSqr() < 1.1f );
                                    tangentS = tangentS;
                                }

                                // swap the Y and Z axes.
                                Swap( verts[ j ].position[ 1 ], verts[ j ].position[ 2 ] );
                                Swap( verts[ j ].normal[ 1 ], verts[ j ].normal[ 2 ] );
                                Swap( verts[ j ].binormal[ 1 ], verts[ j ].binormal[ 2 ] );
                                Swap( verts[ j ].tangent[ 1 ], verts[ j ].tangent[ 2 ] );
                            }

                            // if the triangle is degenerate, skip it.
                            SVec3 t0( verts[ 0 ].position - verts[ 1 ].position );
                            SVec3 t1( verts[ 2 ].position - verts[ 1 ].position );
                            SVec3 normal( t0.Cross( t1 ) );
                            if ( EdIsTriDegenerate( verts[ 0 ].position, verts[ 1 ].position, verts[ 2 ].position, FLT_EPSILON ) )
                            {
                                ++degenerateTriCount;
                                continue;
                            }

                            GrPolygon poly( components, verts[ 0 ], verts[ 1 ], verts[ 2 ], engineMaterial );
                            curPolygons.AddPolygon( poly );
                        }
                    }
                }

                // if the mesh doesn't contain its own tangents and binormals, then generate them.
                if ( !hasTangentData )
                {
                    curPolygons.GenTSBasis();
                }
                polygons.AddGroup( curPolygons );
            }

            // a Bootstrap polygon mesh is analogous to a Source Engine model.
            GrPolygonMesh* polyMesh = new GrPolygonMesh( model->pszName(), polygons );
            polyNode->AddMesh( polyMesh );
        }

        bodyParts = bodyParts;
    }

    //B_ASSERT( degenerateTriCount == 0 );
    if ( degenerateTriCount )
    {
        PrintF( "WARNING: Model contains %u degenerate triangles.\n", degenerateTriCount );
    }

    polyNode->Update();

    // write out the material file.
    //mtlFile->WriteData( 0, materialTxt.c_str(), ( unsigned int )materialTxt.length() );

    //  now construct a GrModel from the polygon mesh.
    URef< GrModel > model = new GrModel( "scene", polyNode );

    // determine the target model file name.
    UPath relativeModelFilename = targetPath;
    relativeModelFilename += "models";
    relativeModelFilename += name;

    // determine the file name + extension.
    relativeModelFilename = ( relativeModelFilename.GetPathString() + ".mdl" );

    // save out the scene model.
    {{
        UWriter writer;
        model->Save( writer );
        URef< RFile > file = gRFileMgr->GetFile( workingDir + relativeModelFilename, RFileMgr::kFlagWrite );
        file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );
    }}

    return true;
}
