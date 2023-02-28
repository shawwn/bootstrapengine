//----------------------------------------------------------
// File:        GLSubsys.h
// Author:      Kevin Bray
// Created:     10-05-04
//
// Purpose:     OpenGL subsystem initialization.
//
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#pragma once

#include "gl.h"
#include "glext.h"
#include "wglext.h"
#include "enums.h"

#define _MONITOR_GL_STATS

#ifdef _DEBUG
 #if 0
  #define CHECK_GL()        _DbgCheckGL( __FILE__, __LINE__ )
 #else
  #define CHECK_GL()
 #endif
#else
 #define CHECK_GL()
#endif

//enum EGLPROFILE
//{
//  EGLP_2_0,
//  EGLP_2_1,
//  EGLP_3_0,
//};

extern void         StartGL( HDC device );
extern void         StopGL();

extern EGLPROFILE   GetGLProfile();

extern bool         IsGLGammaSupported();
extern void         SetGLGamma( float gamma );
extern void         SetGLGamma( unsigned short* red, unsigned short* green, unsigned short* blue );

extern bool         IsExtSupported( const char* ext );

extern void         _DbgCheckGL( const char *file, long line );

#ifdef _MONITOR_GL_STATS

extern void         ClearGLStats();

extern unsigned int _bglDrawCount;
extern unsigned int _bglDrawRangeCount;
extern unsigned int _bglBindTextureCount;
extern unsigned int _bglSetUniformCount;
extern unsigned int _bglBindBufferCount;

#endif

extern const char* glExtString;
extern const char* wglExtString;
extern const char* glShaderString;
extern const char* glRendererString;
extern const char* glVendorString;
extern const char* glVersionString;
extern const char* glFormattedDriverString;

// WGL...
extern const char* bwglGetExtensionsStringARB( HDC device );
extern BOOL bwglSwapIntervalEXT( int interval );
extern int bwglGetSwapIntervalEXT();

// OpenGL 1.1
extern void bglAccum( GLenum op, GLfloat value );
extern void bglAlphaFunc( GLenum func, GLclampf ref );
extern GLboolean bglAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences );
extern void bglArrayElement( GLint i );
extern void bglBegin( GLenum mode );
extern void bglBindTexture( GLenum target, GLuint texture );
extern void bglBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap );
extern void bglBlendFunc( GLenum sfactor, GLenum dfactor );
extern void bglCallList( GLuint list );
extern void bglCallLists( GLsizei n, GLenum type, const GLvoid *lists );
extern void bglClear( GLbitfield mask );
extern void bglClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern void bglClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
extern void bglClearDepth( GLclampd depth );
extern void bglClearIndex( GLfloat c );
extern void bglClearStencil( GLint s );
extern void bglClipPlane( GLenum plane, const GLdouble *equation );
extern void bglColor3b( GLbyte red, GLbyte green, GLbyte blue );
extern void bglColor3bv( const GLbyte *v );
extern void bglColor3d( GLdouble red, GLdouble green, GLdouble blue );
extern void bglColor3dv( const GLdouble *v );
extern void bglColor3f( GLfloat red, GLfloat green, GLfloat blue );
extern void bglColor3fv( const GLfloat *v );
extern void bglColor3i( GLint red, GLint green, GLint blue );
extern void bglColor3iv( const GLint *v );
extern void bglColor3s( GLshort red, GLshort green, GLshort blue );
extern void bglColor3sv( const GLshort *v );
extern void bglColor3ub( GLubyte red, GLubyte green, GLubyte blue );
extern void bglColor3ubv( const GLubyte *v );
extern void bglColor3ui( GLuint red, GLuint green, GLuint blue );
extern void bglColor3uiv( const GLuint *v );
extern void bglColor3us( GLushort red, GLushort green, GLushort blue );
extern void bglColor3usv( const GLushort *v );
extern void bglColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
extern void bglColor4bv( const GLbyte *v );
extern void bglColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
extern void bglColor4dv( const GLdouble *v );
extern void bglColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
extern void bglColor4fv( const GLfloat *v );
extern void bglColor4i( GLint red, GLint green, GLint blue, GLint alpha );
extern void bglColor4iv( const GLint *v );
extern void bglColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha );
extern void bglColor4sv( const GLshort *v );
extern void bglColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
extern void bglColor4ubv( const GLubyte *v );
extern void bglColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha );
extern void bglColor4uiv( const GLuint *v );
extern void bglColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha );
extern void bglColor4usv( const GLushort *v );
extern void bglColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
extern void bglColorMaterial( GLenum face, GLenum mode );
extern void bglColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
extern void bglCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
extern void bglCopyTexImage1D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
extern void bglCopyTexImage2D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
extern void bglCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
extern void bglCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
extern void bglCullFace( GLenum mode );
extern void bglDeleteLists( GLuint list, GLsizei range );
extern void bglDeleteTextures( GLsizei n, const GLuint *textures );
extern void bglDepthFunc( GLenum func );
extern void bglDepthMask( GLboolean flag );
extern void bglDepthRange( GLclampd zNear, GLclampd zFar );
extern void bglDisable( GLenum cap );
extern void bglDisableClientState( GLenum array );
extern void bglDrawArrays( GLenum mode, GLint first, GLsizei count );
extern void bglDrawBuffer( GLenum mode );
extern void bglDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );
extern void bglDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
extern void bglEdgeFlag( GLboolean flag );
extern void bglEdgeFlagPointer( GLsizei stride, const GLvoid *pointer );
extern void bglEdgeFlagv( const GLboolean *flag );
extern void bglEnable( GLenum cap );
extern void bglEnableClientState( GLenum array );
extern void bglEnd( void );
extern void bglEndList( void );
extern void bglEvalCoord1d( GLdouble u );
extern void bglEvalCoord1dv( const GLdouble *u );
extern void bglEvalCoord1f( GLfloat u );
extern void bglEvalCoord1fv( const GLfloat *u );
extern void bglEvalCoord2d( GLdouble u, GLdouble v );
extern void bglEvalCoord2dv( const GLdouble *u );
extern void bglEvalCoord2f( GLfloat u, GLfloat v );
extern void bglEvalCoord2fv( const GLfloat *u );
extern void bglEvalMesh1( GLenum mode, GLint i1, GLint i2 );
extern void bglEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
extern void bglEvalPoint1( GLint i );
extern void bglEvalPoint2( GLint i, GLint j );
extern void bglFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer );
extern void bglFinish( void );
extern void bglFlush( void );
extern void bglFogf( GLenum pname, GLfloat param );
extern void bglFogfv( GLenum pname, const GLfloat *params );
extern void bglFogi( GLenum pname, GLint param );
extern void bglFogiv( GLenum pname, const GLint *params );
extern void bglFrontFace( GLenum mode );
extern void bglFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern GLuint bglGenLists( GLsizei range );
extern void bglGenTextures( GLsizei n, GLuint *textures );
extern void bglGetBooleanv( GLenum pname, GLboolean *params );
extern void bglGetClipPlane( GLenum plane, GLdouble *equation );
extern void bglGetDoublev( GLenum pname, GLdouble *params );
extern GLenum bglGetError( void );
extern void bglGetFloatv( GLenum pname, GLfloat *params );
extern void bglGetIntegerv( GLenum pname, GLint *params );
extern void bglGetLightfv( GLenum light, GLenum pname, GLfloat *params );
extern void bglGetLightiv( GLenum light, GLenum pname, GLint *params );
extern void bglGetMapdv( GLenum target, GLenum query, GLdouble *v );
extern void bglGetMapfv( GLenum target, GLenum query, GLfloat *v );
extern void bglGetMapiv( GLenum target, GLenum query, GLint *v );
extern void bglGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );
extern void bglGetMaterialiv( GLenum face, GLenum pname, GLint *params );
extern void bglGetPixelMapfv( GLenum map, GLfloat *values );
extern void bglGetPixelMapuiv( GLenum map, GLuint *values );
extern void bglGetPixelMapusv( GLenum map, GLushort *values );
extern void bglGetPointerv( GLenum pname, GLvoid* *params );
extern void bglGetPolygonStipple( GLubyte *mask );
extern const char *bglGetString ( GLenum name );
extern void bglGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
extern void bglGetTexEnviv( GLenum target, GLenum pname, GLint *params );
extern void bglGetTexGendv( GLenum coord, GLenum pname, GLdouble *params );
extern void bglGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );
extern void bglGetTexGeniv( GLenum coord, GLenum pname, GLint *params );
extern void bglGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels );
extern void bglGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
extern void bglGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );
extern void bglGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params );
extern void bglGetTexParameteriv( GLenum target, GLenum pname, GLint *params );
extern void bglHint( GLenum target, GLenum mode );
extern void bglIndexMask( GLuint mask );
extern void bglIndexPointer( GLenum type, GLsizei stride, const GLvoid *pointer );
extern void bglIndexd( GLdouble c );
extern void bglIndexdv( const GLdouble *c );
extern void bglIndexf( GLfloat c );
extern void bglIndexfv( const GLfloat *c );
extern void bglIndexi( GLint c );
extern void bglIndexiv( const GLint *c );
extern void bglIndexs( GLshort c );
extern void bglIndexsv( const GLshort *c );
extern void bglIndexub( GLubyte c );
extern void bglIndexubv( const GLubyte *c );
extern void bglInitNames( void );
extern void bglInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer );
extern GLboolean bglIsEnabled( GLenum cap );
extern GLboolean bglIsList( GLuint list );
extern GLboolean bglIsTexture( GLuint texture );
extern void bglLightModelf( GLenum pname, GLfloat param );
extern void bglLightModelfv( GLenum pname, const GLfloat *params );
extern void bglLightModeli( GLenum pname, GLint param );
extern void bglLightModeliv( GLenum pname, const GLint *params );
extern void bglLightf( GLenum light, GLenum pname, GLfloat param );
extern void bglLightfv( GLenum light, GLenum pname, const GLfloat *params );
extern void bglLighti( GLenum light, GLenum pname, GLint param );
extern void bglLightiv( GLenum light, GLenum pname, const GLint *params );
extern void bglLineStipple( GLint factor, GLushort pattern );
extern void bglLineWidth( GLfloat width );
extern void bglListBase( GLuint base );
extern void bglLoadIdentity( void );
extern void bglLoadMatrixd( const GLdouble *m );
extern void bglLoadMatrixf( const GLfloat *m );
extern void bglLoadName( GLuint name );
extern void bglLogicOp( GLenum opcode );
extern void bglMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
extern void bglMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );
extern void bglMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
extern void bglMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );
extern void bglMapGrid1d( GLint un, GLdouble u1, GLdouble u2 );
extern void bglMapGrid1f( GLint un, GLfloat u1, GLfloat u2 );
extern void bglMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
extern void bglMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
extern void bglMaterialf( GLenum face, GLenum pname, GLfloat param );
extern void bglMaterialfv( GLenum face, GLenum pname, const GLfloat *params );
extern void bglMateriali( GLenum face, GLenum pname, GLint param );
extern void bglMaterialiv( GLenum face, GLenum pname, const GLint *params );
extern void bglMatrixMode( GLenum mode );
extern void bglMultMatrixd( const GLdouble *m );
extern void bglMultMatrixf( const GLfloat *m );
extern void bglNewList( GLuint list, GLenum mode );
extern void bglNormal3b( GLbyte nx, GLbyte ny, GLbyte nz );
extern void bglNormal3bv( const GLbyte *v );
extern void bglNormal3d( GLdouble nx, GLdouble ny, GLdouble nz );
extern void bglNormal3dv( const GLdouble *v );
extern void bglNormal3f( GLfloat nx, GLfloat ny, GLfloat nz );
extern void bglNormal3fv( const GLfloat *v );
extern void bglNormal3i( GLint nx, GLint ny, GLint nz );
extern void bglNormal3iv( const GLint *v );
extern void bglNormal3s( GLshort nx, GLshort ny, GLshort nz );
extern void bglNormal3sv( const GLshort *v );
extern void bglNormalPointer( GLenum type, GLsizei stride, const GLvoid *pointer );
extern void bglOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
extern void bglPassThrough( GLfloat token );
extern void bglPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values );
extern void bglPixelMapuiv( GLenum map, GLsizei mapsize, const GLuint *values );
extern void bglPixelMapusv( GLenum map, GLsizei mapsize, const GLushort *values );
extern void bglPixelStoref( GLenum pname, GLfloat param );
extern void bglPixelStorei( GLenum pname, GLint param );
extern void bglPixelTransferf( GLenum pname, GLfloat param );
extern void bglPixelTransferi( GLenum pname, GLint param );
extern void bglPixelZoom( GLfloat xfactor, GLfloat yfactor );
extern void bglPointSize( GLfloat size );
extern void bglPolygonMode( GLenum face, GLenum mode );
extern void bglPolygonOffset( GLfloat factor, GLfloat units );
extern void bglPolygonStipple( const GLubyte *mask );
extern void bglPopAttrib( void );
extern void bglPopClientAttrib( void );
extern void bglPopMatrix( void );
extern void bglPopName( void );
extern void bglPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities );
extern void bglPushAttrib( GLbitfield mask );
extern void bglPushClientAttrib( GLbitfield mask );
extern void bglPushMatrix( void );
extern void bglPushName( GLuint name );
extern void bglRasterPos2d( GLdouble x, GLdouble y );
extern void bglRasterPos2dv( const GLdouble *v );
extern void bglRasterPos2f( GLfloat x, GLfloat y );
extern void bglRasterPos2fv( const GLfloat *v );
extern void bglRasterPos2i( GLint x, GLint y );
extern void bglRasterPos2iv( const GLint *v );
extern void bglRasterPos2s( GLshort x, GLshort y );
extern void bglRasterPos2sv( const GLshort *v );
extern void bglRasterPos3d( GLdouble x, GLdouble y, GLdouble z );
extern void bglRasterPos3dv( const GLdouble *v );
extern void bglRasterPos3f( GLfloat x, GLfloat y, GLfloat z );
extern void bglRasterPos3fv( const GLfloat *v );
extern void bglRasterPos3i( GLint x, GLint y, GLint z );
extern void bglRasterPos3iv( const GLint *v );
extern void bglRasterPos3s( GLshort x, GLshort y, GLshort z );
extern void bglRasterPos3sv( const GLshort *v );
extern void bglRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void bglRasterPos4dv( const GLdouble *v );
extern void bglRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void bglRasterPos4fv( const GLfloat *v );
extern void bglRasterPos4i( GLint x, GLint y, GLint z, GLint w );
extern void bglRasterPos4iv( const GLint *v );
extern void bglRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w );
extern void bglRasterPos4sv( const GLshort *v );
extern void bglReadBuffer( GLenum mode );
extern void bglReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
extern void bglRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
extern void bglRectdv( const GLdouble *v1, const GLdouble *v2 );
extern void bglRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
extern void bglRectfv( const GLfloat *v1, const GLfloat *v2 );
extern void bglRecti( GLint x1, GLint y1, GLint x2, GLint y2 );
extern void bglRectiv( const GLint *v1, const GLint *v2 );
extern void bglRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
extern void bglRectsv( const GLshort *v1, const GLshort *v2 );
extern GLint bglRenderMode( GLenum mode );
extern void bglRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
extern void bglRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
extern void bglScaled( GLdouble x, GLdouble y, GLdouble z );
extern void bglScalef( GLfloat x, GLfloat y, GLfloat z );
extern void bglScissor( GLint x, GLint y, GLsizei width, GLsizei height );
extern void bglSelectBuffer( GLsizei size, GLuint *buffer );
extern void bglShadeModel( GLenum mode );
extern void bglStencilFunc( GLenum func, GLint ref, GLuint mask );
extern void bglStencilMask( GLuint mask );
extern void bglStencilOp( GLenum fail, GLenum zfail, GLenum zpass );
extern void bglTexCoord1d( GLdouble s );
extern void bglTexCoord1dv( const GLdouble *v );
extern void bglTexCoord1f( GLfloat s );
extern void bglTexCoord1fv( const GLfloat *v );
extern void bglTexCoord1i( GLint s );
extern void bglTexCoord1iv( const GLint *v );
extern void bglTexCoord1s( GLshort s );
extern void bglTexCoord1sv( const GLshort *v );
extern void bglTexCoord2d( GLdouble s, GLdouble t );
extern void bglTexCoord2dv( const GLdouble *v );
extern void bglTexCoord2f( GLfloat s, GLfloat t );
extern void bglTexCoord2fv( const GLfloat *v );
extern void bglTexCoord2i( GLint s, GLint t );
extern void bglTexCoord2iv( const GLint *v );
extern void bglTexCoord2s( GLshort s, GLshort t );
extern void bglTexCoord2sv( const GLshort *v );
extern void bglTexCoord3d( GLdouble s, GLdouble t, GLdouble r );
extern void bglTexCoord3dv( const GLdouble *v );
extern void bglTexCoord3f( GLfloat s, GLfloat t, GLfloat r );
extern void bglTexCoord3fv( const GLfloat *v );
extern void bglTexCoord3i( GLint s, GLint t, GLint r );
extern void bglTexCoord3iv( const GLint *v );
extern void bglTexCoord3s( GLshort s, GLshort t, GLshort r );
extern void bglTexCoord3sv( const GLshort *v );
extern void bglTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
extern void bglTexCoord4dv( const GLdouble *v );
extern void bglTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
extern void bglTexCoord4fv( const GLfloat *v );
extern void bglTexCoord4i( GLint s, GLint t, GLint r, GLint q );
extern void bglTexCoord4iv( const GLint *v );
extern void bglTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q );
extern void bglTexCoord4sv( const GLshort *v );
extern void bglTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
extern void bglTexEnvf( GLenum target, GLenum pname, GLfloat param );
extern void bglTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
extern void bglTexEnvi( GLenum target, GLenum pname, GLint param );
extern void bglTexEnviv( GLenum target, GLenum pname, const GLint *params );
extern void bglTexGend( GLenum coord, GLenum pname, GLdouble param );
extern void bglTexGendv( GLenum coord, GLenum pname, const GLdouble *params );
extern void bglTexGenf( GLenum coord, GLenum pname, GLfloat param );
extern void bglTexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
extern void bglTexGeni( GLenum coord, GLenum pname, GLint param );
extern void bglTexGeniv( GLenum coord, GLenum pname, const GLint *params );
extern void bglTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
extern void bglTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
extern void bglTexParameterf( GLenum target, GLenum pname, GLfloat param );
extern void bglTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
extern void bglTexParameteri( GLenum target, GLenum pname, GLint param );
extern void bglTexParameteriv( GLenum target, GLenum pname, const GLint *params );
extern void bglTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );
extern void bglTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
extern void bglTranslated( GLdouble x, GLdouble y, GLdouble z );
extern void bglTranslatef( GLfloat x, GLfloat y, GLfloat z );
extern void bglVertex2d( GLdouble x, GLdouble y );
extern void bglVertex2dv( const GLdouble *v );
extern void bglVertex2f( GLfloat x, GLfloat y );
extern void bglVertex2fv( const GLfloat *v );
extern void bglVertex2i( GLint x, GLint y );
extern void bglVertex2iv( const GLint *v );
extern void bglVertex2s( GLshort x, GLshort y );
extern void bglVertex2sv( const GLshort *v );
extern void bglVertex3d( GLdouble x, GLdouble y, GLdouble z );
extern void bglVertex3dv( const GLdouble *v );
extern void bglVertex3f( GLfloat x, GLfloat y, GLfloat z );
extern void bglVertex3fv( const GLfloat *v );
extern void bglVertex3i( GLint x, GLint y, GLint z );
extern void bglVertex3iv( const GLint *v );
extern void bglVertex3s( GLshort x, GLshort y, GLshort z );
extern void bglVertex3sv( const GLshort *v );
extern void bglVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
extern void bglVertex4dv( const GLdouble *v );
extern void bglVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
extern void bglVertex4fv( const GLfloat *v );
extern void bglVertex4i( GLint x, GLint y, GLint z, GLint w );
extern void bglVertex4iv( const GLint *v );
extern void bglVertex4s( GLshort x, GLshort y, GLshort z, GLshort w );
extern void bglVertex4sv( const GLshort *v );
extern void bglVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
extern void bglViewport( GLint x, GLint y, GLsizei width, GLsizei height );

// OpenGL 1.5
// 1.2
extern void bglBlendColor( GLclampf, GLclampf, GLclampf, GLclampf );
extern void bglBlendEquation( GLenum equation );
extern void bglDrawRangeElements( GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid * );
extern void bglColorTable( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void bglColorTableParameterfv( GLenum, GLenum, const GLfloat * );
extern void bglColorTableParameteriv( GLenum, GLenum, const GLint * );
extern void bglCopyColorTable( GLenum, GLenum, GLint, GLint, GLsizei );
extern void bglGetColorTable( GLenum, GLenum, GLenum, GLvoid * );
extern void bglGetColorTableParameterfv( GLenum, GLenum, GLfloat * );
extern void bglGetColorTableParameteriv( GLenum, GLenum, GLint * );
extern void bglColorSubTable( GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void bglCopyColorSubTable( GLenum, GLsizei, GLint, GLint, GLsizei );
extern void bglConvolutionFilter1D( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
extern void bglConvolutionFilter2D( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void bglConvolutionParameterf( GLenum, GLenum, GLfloat );
extern void bglConvolutionParameterfv( GLenum, GLenum, const GLfloat * );
extern void bglConvolutionParameteri( GLenum, GLenum, GLint );
extern void bglConvolutionParameteriv( GLenum, GLenum, const GLint * );
extern void bglCopyConvolutionFilter1D( GLenum, GLenum, GLint, GLint, GLsizei );
extern void bglCopyConvolutionFilter2D( GLenum, GLenum, GLint, GLint, GLsizei, GLsizei );
extern void bglGetConvolutionFilter( GLenum, GLenum, GLenum, GLvoid * );
extern void bglGetConvolutionParameterfv( GLenum, GLenum, GLfloat * );
extern void bglGetConvolutionParameteriv( GLenum, GLenum, GLint * );
extern void bglGetSeparableFilter( GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid * );
extern void bglSeparableFilter2D( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid * );
extern void bglGetHistogram( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
extern void bglGetHistogramParameterfv( GLenum, GLenum, GLfloat * );
extern void bglGetHistogramParameteriv( GLenum, GLenum, GLint * );
extern void bglGetMinmax( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
extern void bglGetMinmaxParameterfv( GLenum, GLenum, GLfloat * );
extern void bglGetMinmaxParameteriv( GLenum, GLenum, GLint * );
extern void bglHistogram( GLenum, GLsizei, GLenum, GLboolean );
extern void bglMinmax( GLenum, GLenum, GLboolean );
extern void bglResetHistogram( GLenum );
extern void bglResetMinmax( GLenum );
extern void bglTexImage3D( GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid * );
extern void bglTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
extern void bglCopyTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei );

// 1.3
extern void bglActiveTexture( GLenum texUnit );
extern void bglClientActiveTexture( GLenum texUnitStream );
extern void bglMultiTexCoord1d( GLenum, GLdouble );
extern void bglMultiTexCoord1dv( GLenum, const GLdouble * );
extern void bglMultiTexCoord1f( GLenum, GLfloat );
extern void bglMultiTexCoord1fv( GLenum, const GLfloat * );
extern void bglMultiTexCoord1i( GLenum, GLint );
extern void bglMultiTexCoord1iv( GLenum, const GLint * );
extern void bglMultiTexCoord1s( GLenum, GLshort );
extern void bglMultiTexCoord1sv( GLenum, const GLshort * );
extern void bglMultiTexCoord2d( GLenum, GLdouble, GLdouble );
extern void bglMultiTexCoord2dv( GLenum, const GLdouble * );
extern void bglMultiTexCoord2f( GLenum, GLfloat, GLfloat );
extern void bglMultiTexCoord2fv( GLenum, const GLfloat * );
extern void bglMultiTexCoord2i( GLenum, GLint, GLint );
extern void bglMultiTexCoord2iv( GLenum, const GLint * );
extern void bglMultiTexCoord2s( GLenum, GLshort, GLshort );
extern void bglMultiTexCoord2sv( GLenum, const GLshort * );
extern void bglMultiTexCoord3d( GLenum, GLdouble, GLdouble, GLdouble );
extern void bglMultiTexCoord3dv( GLenum, const GLdouble * );
extern void bglMultiTexCoord3f( GLenum, GLfloat, GLfloat, GLfloat );
extern void bglMultiTexCoord3fv( GLenum, const GLfloat * );
extern void bglMultiTexCoord3i( GLenum, GLint, GLint, GLint );
extern void bglMultiTexCoord3iv( GLenum, const GLint * );
extern void bglMultiTexCoord3s( GLenum, GLshort, GLshort, GLshort );
extern void bglMultiTexCoord3sv( GLenum, const GLshort * );
extern void bglMultiTexCoord4d( GLenum, GLdouble, GLdouble, GLdouble, GLdouble );
extern void bglMultiTexCoord4dv( GLenum, const GLdouble * );
extern void bglMultiTexCoord4f( GLenum, GLfloat, GLfloat, GLfloat, GLfloat );
extern void bglMultiTexCoord4fv( GLenum, const GLfloat * );
extern void bglMultiTexCoord4i( GLenum, GLint, GLint, GLint, GLint );
extern void bglMultiTexCoord4iv( GLenum, const GLint * );
extern void bglMultiTexCoord4s( GLenum, GLshort, GLshort, GLshort, GLshort );
extern void bglMultiTexCoord4sv( GLenum, const GLshort * );
extern void bglLoadTransposeMatrixf( const GLfloat * );
extern void bglLoadTransposeMatrixd( const GLdouble * );
extern void bglMultTransposeMatrixf( const GLfloat * );
extern void bglMultTransposeMatrixd( const GLdouble * );
extern void bglSampleCoverage( GLclampf, GLboolean );
extern void bglCompressedTexImage3D( GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid * );
extern void bglCompressedTexImage2D( GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid * );
extern void bglCompressedTexImage1D( GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid * );
extern void bglCompressedTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid * );
extern void bglCompressedTexSubImage2D( GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid * );
extern void bglCompressedTexSubImage1D( GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid * );
extern void bglGetCompressedTexImage( GLenum, GLint, GLvoid * );

// 1.4
extern void bglBlendFuncSeparate( GLenum, GLenum, GLenum, GLenum );
extern void bglFogCoordf( GLfloat );
extern void bglFogCoordfv( const GLfloat * );
extern void bglFogCoordd( GLdouble );
extern void bglFogCoorddv( const GLdouble * );
extern void bglFogCoordPointer( GLenum, GLsizei, const GLvoid * );
extern void bglMultiDrawArrays( GLenum, GLint *, GLsizei *, GLsizei );
extern void bglMultiDrawElements( GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei );
extern void bglPointParameterf( GLenum, GLfloat );
extern void bglPointParameterfv( GLenum, const GLfloat * );
extern void bglPointParameteri( GLenum, GLint );
extern void bglPointParameteriv( GLenum, const GLint * );
extern void bglSecondaryColor3b( GLbyte, GLbyte, GLbyte );
extern void bglSecondaryColor3bv( const GLbyte * );
extern void bglSecondaryColor3d( GLdouble, GLdouble, GLdouble );
extern void bglSecondaryColor3dv( const GLdouble * );
extern void bglSecondaryColor3f( GLfloat, GLfloat, GLfloat );
extern void bglSecondaryColor3fv( const GLfloat * );
extern void bglSecondaryColor3i( GLint, GLint, GLint );
extern void bglSecondaryColor3iv( const GLint * );
extern void bglSecondaryColor3s( GLshort, GLshort, GLshort );
extern void bglSecondaryColor3sv( const GLshort * );
extern void bglSecondaryColor3ub( GLubyte, GLubyte, GLubyte );
extern void bglSecondaryColor3ubv( const GLubyte * );
extern void bglSecondaryColor3ui( GLuint, GLuint, GLuint );
extern void bglSecondaryColor3uiv( const GLuint * );
extern void bglSecondaryColor3us( GLushort, GLushort, GLushort );
extern void bglSecondaryColor3usv( const GLushort * );
extern void bglSecondaryColorPointer( GLint, GLenum, GLsizei, const GLvoid * );
extern void bglWindowPos2d( GLdouble, GLdouble );
extern void bglWindowPos2dv( const GLdouble * );
extern void bglWindowPos2f( GLfloat, GLfloat );
extern void bglWindowPos2fv( const GLfloat * );
extern void bglWindowPos2i( GLint, GLint );
extern void bglWindowPos2iv( const GLint * );
extern void bglWindowPos2s( GLshort, GLshort );
extern void bglWindowPos2sv( const GLshort * );
extern void bglWindowPos3d( GLdouble, GLdouble, GLdouble );
extern void bglWindowPos3dv( const GLdouble * );
extern void bglWindowPos3f( GLfloat, GLfloat, GLfloat );
extern void bglWindowPos3fv( const GLfloat * );
extern void bglWindowPos3i( GLint, GLint, GLint );
extern void bglWindowPos3iv( const GLint * );
extern void bglWindowPos3s( GLshort, GLshort, GLshort );
extern void bglWindowPos3sv( const GLshort * );

// 1.5
extern void bglGenQueries( GLsizei, GLuint * );
extern void bglDeleteQueries( GLsizei, const GLuint * );
extern GLboolean bglIsQuery( GLuint );
extern void bglBeginQuery( GLenum, GLuint );
extern void bglEndQuery( GLenum );
extern void bglGetQueryiv( GLenum, GLenum, GLint * );
extern void bglGetQueryObjectiv( GLuint, GLenum, GLint * );
extern void bglGetQueryObjectuiv( GLuint, GLenum, GLuint * );
extern void bglBindBuffer( GLenum, GLuint );
extern void bglDeleteBuffers( GLsizei, const GLuint * );
extern void bglGenBuffers( GLsizei, GLuint * );
extern GLboolean bglIsBuffer( GLuint );
extern void bglBufferData( GLenum, GLsizeiptr, const GLvoid *, GLenum );
extern void bglBufferSubData( GLenum, GLintptr, GLsizeiptr, const GLvoid * );
extern void bglGetBufferSubData( GLenum, GLintptr, GLsizeiptr, GLvoid * );
extern GLvoid *bglMapBuffer ( GLenum, GLenum );
extern GLboolean bglUnmapBuffer( GLenum );
extern void bglGetBufferParameteriv( GLenum, GLenum, GLint * );
extern void bglGetBufferPointerv( GLenum, GLenum, GLvoid* * );

// 2.0
extern void bglBlendEquationSeparate( GLenum, GLenum );
extern void bglDrawBuffers( GLsizei, const GLenum * );
extern void bglStencilOpSeparate( GLenum, GLenum, GLenum, GLenum );
extern void bglStencilFuncSeparate( GLenum, GLenum, GLint, GLuint );
extern void bglStencilMaskSeparate( GLenum, GLuint );
extern void bglAttachShader( GLuint, GLuint );
extern void bglBindAttribLocation( GLuint, GLuint, const GLchar * );
extern void bglCompileShader( GLuint );
extern GLuint bglCreateProgram( void );
extern GLuint bglCreateShader( GLenum );
extern void bglDeleteProgram( GLuint );
extern void bglDeleteShader( GLuint );
extern void bglDetachShader( GLuint, GLuint );
extern void bglDisableVertexAttribArray( GLuint );
extern void bglEnableVertexAttribArray( GLuint );
extern void bglGetActiveAttrib( GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar * );
extern void bglGetActiveUniform( GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar * );
extern void bglGetAttachedShaders( GLuint, GLsizei, GLsizei *, GLuint * );
extern GLint bglGetAttribLocation( GLuint, const GLchar * );
extern void bglGetProgramiv( GLuint, GLenum, GLint * );
extern void bglGetProgramInfoLog( GLuint, GLsizei, GLsizei *, GLchar * );
extern void bglGetShaderiv( GLuint, GLenum, GLint * );
extern void bglGetShaderInfoLog( GLuint, GLsizei, GLsizei *, GLchar * );
extern void bglGetShaderSource( GLuint, GLsizei, GLsizei *, GLchar * );
extern GLint bglGetUniformLocation( GLuint, const GLchar * );
extern void bglGetUniformfv( GLuint, GLint, GLfloat * );
extern void bglGetUniformiv( GLuint, GLint, GLint * );
extern void bglGetVertexAttribdv( GLuint, GLenum, GLdouble * );
extern void bglGetVertexAttribfv( GLuint, GLenum, GLfloat * );
extern void bglGetVertexAttribiv( GLuint, GLenum, GLint * );
extern void bglGetVertexAttribPointerv( GLuint, GLenum, GLvoid* * );
extern GLboolean bglIsProgram( GLuint );
extern GLboolean bglIsShader( GLuint );
extern void bglLinkProgram( GLuint );
extern void bglShaderSource( GLuint, GLsizei, const GLchar* *, const GLint * );
extern void bglUseProgram( GLuint );
extern void bglUniform1f( GLint, GLfloat );
extern void bglUniform2f( GLint, GLfloat, GLfloat );
extern void bglUniform3f( GLint, GLfloat, GLfloat, GLfloat );
extern void bglUniform4f( GLint, GLfloat, GLfloat, GLfloat, GLfloat );
extern void bglUniform1i( GLint, GLint );
extern void bglUniform2i( GLint, GLint, GLint );
extern void bglUniform3i( GLint, GLint, GLint, GLint );
extern void bglUniform4i( GLint, GLint, GLint, GLint, GLint );
extern void bglUniform1fv( GLint, GLsizei, const GLfloat * );
extern void bglUniform2fv( GLint, GLsizei, const GLfloat * );
extern void bglUniform3fv( GLint, GLsizei, const GLfloat * );
extern void bglUniform4fv( GLint, GLsizei, const GLfloat * );
extern void bglUniform1iv( GLint, GLsizei, const GLint * );
extern void bglUniform2iv( GLint, GLsizei, const GLint * );
extern void bglUniform3iv( GLint, GLsizei, const GLint * );
extern void bglUniform4iv( GLint, GLsizei, const GLint * );
extern void bglUniformMatrix2fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix3fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix4fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglValidateProgram( GLuint );
extern void bglVertexAttrib1d( GLuint, GLdouble );
extern void bglVertexAttrib1dv( GLuint, const GLdouble * );
extern void bglVertexAttrib1f( GLuint, GLfloat );
extern void bglVertexAttrib1fv( GLuint, const GLfloat * );
extern void bglVertexAttrib1s( GLuint, GLshort );
extern void bglVertexAttrib1sv( GLuint, const GLshort * );
extern void bglVertexAttrib2d( GLuint, GLdouble, GLdouble );
extern void bglVertexAttrib2dv( GLuint, const GLdouble * );
extern void bglVertexAttrib2f( GLuint, GLfloat, GLfloat );
extern void bglVertexAttrib2fv( GLuint, const GLfloat * );
extern void bglVertexAttrib2s( GLuint, GLshort, GLshort );
extern void bglVertexAttrib2sv( GLuint, const GLshort * );
extern void bglVertexAttrib3d( GLuint, GLdouble, GLdouble, GLdouble );
extern void bglVertexAttrib3dv( GLuint, const GLdouble * );
extern void bglVertexAttrib3f( GLuint, GLfloat, GLfloat, GLfloat );
extern void bglVertexAttrib3fv( GLuint, const GLfloat * );
extern void bglVertexAttrib3s( GLuint, GLshort, GLshort, GLshort );
extern void bglVertexAttrib3sv( GLuint, const GLshort * );
extern void bglVertexAttrib4Nbv( GLuint, const GLbyte * );
extern void bglVertexAttrib4Niv( GLuint, const GLint * );
extern void bglVertexAttrib4Nsv( GLuint, const GLshort * );
extern void bglVertexAttrib4Nub( GLuint, GLubyte, GLubyte, GLubyte, GLubyte );
extern void bglVertexAttrib4Nubv( GLuint, const GLubyte * );
extern void bglVertexAttrib4Nuiv( GLuint, const GLuint * );
extern void bglVertexAttrib4Nusv( GLuint, const GLushort * );
extern void bglVertexAttrib4bv( GLuint, const GLbyte * );
extern void bglVertexAttrib4d( GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
extern void bglVertexAttrib4dv( GLuint, const GLdouble * );
extern void bglVertexAttrib4f( GLuint, GLfloat, GLfloat, GLfloat, GLfloat );
extern void bglVertexAttrib4fv( GLuint, const GLfloat * );
extern void bglVertexAttrib4iv( GLuint, const GLint * );
extern void bglVertexAttrib4s( GLuint, GLshort, GLshort, GLshort, GLshort );
extern void bglVertexAttrib4sv( GLuint, const GLshort * );
extern void bglVertexAttrib4ubv( GLuint, const GLubyte * );
extern void bglVertexAttrib4uiv( GLuint, const GLuint * );
extern void bglVertexAttrib4usv( GLuint, const GLushort * );
extern void bglVertexAttribPointer( GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid * );

// 2.1
extern void bglUniformMatrix2x3fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix3x2fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix2x4fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix4x2fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix3x4fv( GLint, GLsizei, GLboolean, const GLfloat * );
extern void bglUniformMatrix4x3fv( GLint, GLsizei, GLboolean, const GLfloat * );
/*
// Generic extensions req'd.
// ARB_draw_buffers
extern void bglDrawBuffersARB( GLsizei, const GLenum * );

// GL_ARB_vertex_shader
// < inherited from GL_ARB_vertex_program >
extern void bglVertexAttrib1dARB( GLuint, GLdouble );
extern void bglVertexAttrib1dvARB( GLuint, const GLdouble * );
extern void bglVertexAttrib1fARB( GLuint, GLfloat );
extern void bglVertexAttrib1fvARB( GLuint, const GLfloat * );
extern void bglVertexAttrib1sARB( GLuint, GLshort );
extern void bglVertexAttrib1svARB( GLuint, const GLshort * );
extern void bglVertexAttrib2dARB( GLuint, GLdouble, GLdouble );
extern void bglVertexAttrib2dvARB( GLuint, const GLdouble * );
extern void bglVertexAttrib2fARB( GLuint, GLfloat, GLfloat );
extern void bglVertexAttrib2fvARB( GLuint, const GLfloat * );
extern void bglVertexAttrib2sARB( GLuint, GLshort, GLshort );
extern void bglVertexAttrib2svARB( GLuint, const GLshort * );
extern void bglVertexAttrib3dARB( GLuint, GLdouble, GLdouble, GLdouble );
extern void bglVertexAttrib3dvARB( GLuint, const GLdouble * );
extern void bglVertexAttrib3fARB( GLuint, GLfloat, GLfloat, GLfloat );
extern void bglVertexAttrib3fvARB( GLuint, const GLfloat * );
extern void bglVertexAttrib3sARB( GLuint, GLshort, GLshort, GLshort );
extern void bglVertexAttrib3svARB( GLuint, const GLshort * );
extern void bglVertexAttrib4NbvARB( GLuint, const GLbyte * );
extern void bglVertexAttrib4NivARB( GLuint, const GLint * );
extern void bglVertexAttrib4NsvARB( GLuint, const GLshort * );
extern void bglVertexAttrib4NubARB( GLuint, GLubyte, GLubyte, GLubyte, GLubyte );
extern void bglVertexAttrib4NubvARB( GLuint, const GLubyte * );
extern void bglVertexAttrib4NuivARB( GLuint, const GLuint * );
extern void bglVertexAttrib4NusvARB( GLuint, const GLushort * );
extern void bglVertexAttrib4bvARB( GLuint, const GLbyte * );
extern void bglVertexAttrib4dARB( GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
extern void bglVertexAttrib4dvARB( GLuint, const GLdouble * );
extern void bglVertexAttrib4fARB( GLuint, GLfloat, GLfloat, GLfloat, GLfloat );
extern void bglVertexAttrib4fvARB( GLuint, const GLfloat * );
extern void bglVertexAttrib4ivARB( GLuint, const GLint * );
extern void bglVertexAttrib4sARB( GLuint, GLshort, GLshort, GLshort, GLshort );
extern void bglVertexAttrib4svARB( GLuint, const GLshort * );
extern void bglVertexAttrib4ubvARB( GLuint, const GLubyte * );
extern void bglVertexAttrib4uivARB( GLuint, const GLuint * );
extern void bglVertexAttrib4usvARB( GLuint, const GLushort * );
extern void bglVertexAttribPointerARB( GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid * );
extern void bglEnableVertexAttribArrayARB( GLuint );
extern void bglDisableVertexAttribArrayARB( GLuint );

extern void bglProgramStringARB( GLenum, GLenum, GLsizei, const GLvoid * );
extern void bglBindProgramARB( GLenum, GLuint );
extern void bglDeleteProgramsARB( GLsizei, const GLuint * );
extern void bglGenProgramsARB( GLsizei, GLuint * );
extern void bglProgramEnvParameter4dARB( GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
extern void bglProgramEnvParameter4dvARB( GLenum, GLuint, const GLdouble * );
extern void bglProgramEnvParameter4fARB( GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat );
extern void bglProgramEnvParameter4fvARB( GLenum, GLuint, const GLfloat * );
extern void bglProgramLocalParameter4dARB( GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
extern void bglProgramLocalParameter4dvARB( GLenum, GLuint, const GLdouble * );
extern void bglProgramLocalParameter4fARB( GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat );
extern void bglProgramLocalParameter4fvARB( GLenum, GLuint, const GLfloat * );
extern void bglGetProgramEnvParameterdvARB( GLenum, GLuint, GLdouble * );
extern void bglGetProgramEnvParameterfvARB( GLenum, GLuint, GLfloat * );
extern void bglGetProgramLocalParameterdvARB( GLenum, GLuint, GLdouble * );
extern void bglGetProgramLocalParameterfvARB( GLenum, GLuint, GLfloat * );
extern void bglGetProgramivARB( GLenum, GLenum, GLint * );
extern void bglGetProgramStringARB( GLenum, GLenum, GLvoid * );
extern void bglGetVertexAttribdvARB( GLuint, GLenum, GLdouble * );
extern void bglGetVertexAttribfvARB( GLuint, GLenum, GLfloat * );
extern void bglGetVertexAttribivARB( GLuint, GLenum, GLint * );
extern void bglGetVertexAttribPointervARB( GLuint, GLenum, GLvoid* * );

// < /inherited from GL_ARB_vertex_program >
extern void bglBindAttribLocationARB( GLhandleARB, GLuint, const GLcharARB * );
extern void bglGetActiveAttribARB( GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB * );
extern GLint bglGetAttribLocationARB( GLhandleARB, const GLcharARB * );
*/
// GL_ARB_multisample
extern void bglSampleCoverageARB( GLclampf, GLboolean );

// GL_ARB_color_buffer_float
//extern void bglClampColorARB( GLenum target, GLenum clamp );

// GL_EXT_framebuffer_object
extern GLboolean bglIsRenderbufferEXT( GLuint renderbuffer );
extern void bglBindRenderbufferEXT( GLenum target, GLuint renderbuffer );
extern void bglDeleteRenderbuffersEXT( GLsizei n, const GLuint *renderbuffers );
extern void bglGenRenderbuffersEXT( GLsizei n, GLuint *renderbuffers );
extern void bglRenderbufferStorageEXT( GLenum target, GLenum internalformat, GLsizei width, GLsizei height );
extern void bglGetRenderbufferParameterivEXT( GLenum target, GLenum pname, GLint* params );
extern GLboolean bglIsFramebufferEXT( GLuint framebuffer );
extern void bglBindFramebufferEXT( GLenum target, GLuint framebuffer );
extern void bglDeleteFramebuffersEXT( GLsizei n, const GLuint *framebuffers );
extern void bglGenFramebuffersEXT( GLsizei n, GLuint *framebuffers );
extern GLenum bglCheckFramebufferStatusEXT( GLenum target );
extern void bglFramebufferTexture1DEXT( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
extern void bglFramebufferTexture2DEXT( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );
extern void bglFramebufferTexture3DEXT( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset );
extern void bglFramebufferRenderbufferEXT( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer );
extern void bglGetFramebufferAttachmentParameterivEXT( GLenum target, GLenum attachment, GLenum pname, GLint *params );
extern void bglGenerateMipmapEXT( GLenum target );

// GL_EXT_depth_bounds_test
extern void bglDepthBoundsEXT( GLclampd zmin, GLclampd zmax );

// optional extension support.
// GL_ARB_texture_rectangle
extern bool ARB_texture_rectangle;

// GL_ARB_texture_non_power_of_two
extern bool ARB_texture_non_power_of_two;

// GL_NV_fence
extern void bglGenFencesNV( GLsizei n, GLuint* fences );
extern void bglDeleteFencesNV( GLsizei n, const GLuint* fences );
extern void bglSetFenceNV( GLuint fence, GLenum condition );
extern GLboolean bglTestFenceNV( GLuint fence );
extern void bglFinishFenceNV( GLuint fence );
extern GLboolean bglIsFenceNV( GLuint fence );
extern void bglGetFenceivNV( GLuint fence, GLenum pname, GLint* params );
