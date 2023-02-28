#pragma once

// little-endian "IDSV"
#define MODEL_VERTEX_FILE_ID        (('V'<<24)+('S'<<16)+('D'<<8)+'I')
#define MODEL_VERTEX_FILE_VERSION   4
// this id (IDCV) is used once the vertex data has been compressed.
#define MODEL_VERTEX_FILE_THIN_ID   (('V'<<24)+('C'<<16)+('D'<<8)+'I')

#define MAX_NUM_BONES_PER_VERT 3
#define MAX_NUM_LODS 8

#if defined( _WIN32 ) || defined( WIN32 )
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#else   //_WIN32
#define PATHSEPARATOR(c) ((c) == '/')
#endif  //_WIN32
