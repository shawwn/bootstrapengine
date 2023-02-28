#include "graphics_afx.h"

// header.
#include "GrDebug.h"

// project includes.
#include "GrTypes.h"
#include "UWriter.h"
#include "RFileMgr.h"
#include "RFile.h"

char kObjHeader[] =
"# File exported by the Bootstrap Engine for debugging purposes.\n"
"# www.bootstrapstudios.com\n";

//**********************************************************
// functions implementations.
//**********************************************************

//----------------------------------------------------------
void
GrSaveTGA( const char* filePath, const void* data, unsigned int width,
           unsigned int height, unsigned int bytesPerPel )
{
    // make sure the file path and data passed in are valid.
    B_ASSERT( filePath != 0 && data != 0 );

    // we currently only support saving greyscale, RGB, and RGBA images.
    B_ASSERT( bytesPerPel == 1 || bytesPerPel == 3 || bytesPerPel == 4 );

    // write out a TGA file of the depth color map.
    UWriter writer;
    writer.WriteChar( 0 );
    writer.WriteChar( 0 );
    writer.WriteChar( ( bytesPerPel == 1 ) ? 3 : 2 );
    writer.WriteChar( 0 ); writer.WriteChar( 0 );
    writer.WriteChar( 0 ); writer.WriteChar( 0 );
    writer.WriteChar( 0 ); 
    writer.WriteChar( 0 ); writer.WriteChar( 0 );
    writer.WriteChar( 0 ); writer.WriteChar( 0 );
    writer.WriteChar( width & 0xFF ); writer.WriteChar( width >> 8 );
    writer.WriteChar( height & 0xFF ); writer.WriteChar( height >> 8 );
    writer.WriteChar( 8*bytesPerPel );
    writer.WriteChar( 0 );

    // now write out the image data.
    writer.WriteBytes( data, width * height * bytesPerPel );
    URef< RFile > file = gRFileMgr->GetFile( filePath, RFileMgr::kFlagCreateAlways | RFileMgr::kFlagWrite );

    // write the data to the file.
    file->WriteData( 0, writer.GetBufferPtr(), writer.GetBytesWritten() );
}

//----------------------------------------------------------
void GrSaveOBJ( const char* filePath, const char* modelName, const SVec3* positions,
                unsigned int positionCount, const unsigned short* indices,
                unsigned int indexCount )
{
    // make sure the file path and data passed in are valid.
    B_ASSERT( filePath != 0 && positions != 0 );

    // save out an OBJ file.
    URef< RFile > file = gRFileMgr->GetFile( filePath, RFileMgr::kFlagCreateAlways | RFileMgr::kFlagWrite );

    // keep track of the file offset.
    __int64 fileOffset = 0;

    // buffer for writing text.
    int len = 0;

    unsigned int fileBufSize = 262144;
    unsigned int fileBufOffset = 0;
    char* fileBuf = ( char* )TempAlloc( 262144 );

    // write out the file header.
    file->WriteData( fileOffset, kObjHeader, sizeof( kObjHeader ) - 1 );
    fileOffset += sizeof( kObjHeader ) - 1;

    // write out the model's name.
    len = sprintf_s( fileBuf, fileBufSize - fileBufOffset, "g %s\n", modelName );
    file->WriteData( fileOffset, fileBuf, len );
    fileOffset += len;

    // write out vertices.
    for ( unsigned int i = 0; i < positionCount; ++i )
    {
        // write the text for the current vertex.
        const SVec3& curPos = positions[ i ];
        if ( ( _scprintf( "v %f %f %f\n", curPos.x, curPos.y, curPos.z ) + 1 ) > ( int )( fileBufSize - fileBufOffset ) )
        {
            // flush the file buffer and redo the operation.
            file->WriteData( fileOffset, fileBuf, fileBufOffset );
            fileOffset += fileBufOffset;
            fileBufOffset = 0;
        }

        // try to write the data to the buffer again now that the
        // buffer has been flushed.
        len = sprintf_s( fileBuf + fileBufOffset, fileBufSize - fileBufOffset, "v %f %f %f\n", curPos.x, curPos.y, curPos.z );

        // advance the file buffer's offset.
        fileBufOffset += len;
    }

    // write out faces.
    if ( indices != 0 )
    {
        // write out each triangle.
        for ( unsigned int i = 0; i < indexCount; i += 3 )
        {
            // write the text for the current triangle.
            unsigned int i0 = indices[ i + 0 ] + 1;
            unsigned int i1 = indices[ i + 1 ] + 1;
            unsigned int i2 = indices[ i + 2 ] + 1;
            if ( ( _scprintf( "f %d %d %d\n", i0, i1, i2 ) + 1 ) > ( int )( fileBufSize - fileBufOffset ) )
            {
                // flush the file buffer and redo the operation.
                file->WriteData( fileOffset, fileBuf, fileBufOffset );
                fileOffset += fileBufOffset;
                fileBufOffset = 0;
            }

            // write out the string.
            len = sprintf_s( fileBuf + fileBufOffset, fileBufSize - fileBufOffset, "f %d %d %d\n", i0, i1, i2 );

            // advance the file buffer's offset.
            fileBufOffset += len;
        }
    }
    else
    {
        // write out each triangle.
        indexCount = positionCount / 3;
        for ( unsigned int i = 0; i < indexCount; i += 3 )
        {
            if ( ( _scprintf( "f %d %d %d\n", i + 1, i + 2, i + 3 ) + 1 ) > ( int )( fileBufSize - fileBufOffset ) )
            {
                // flush the file buffer and redo the operation.
                file->WriteData( fileOffset, fileBuf, fileBufOffset );
                fileOffset += fileBufOffset;
                fileBufOffset = 0;
            }

            // try to write the data to the buffer again now that the
            // buffer has been flushed.
            len = sprintf_s( fileBuf + fileBufOffset, fileBufSize - fileBufOffset, "f %d %d %d\n", i + 1, i + 2, i + 3 );

            // advance the file buffer's offset.
            fileBufOffset += len;
        }
    }

    // flush any remaining data.
    if ( fileBufOffset > 0 )
        file->WriteData( fileOffset, fileBuf, fileBufOffset );

    // free the file buffer.
    TempFree( fileBuf );
}
