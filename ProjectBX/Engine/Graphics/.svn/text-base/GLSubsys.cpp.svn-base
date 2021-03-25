//----------------------------------------------------------
// File:		MVec3.cpp
// Author:		Kevin Bray
// Created:		10-05-04
// Copyright © 2004 Bootstrap Studios.  All rights reserved.
//----------------------------------------------------------
#include "graphics_afx.h"

// project includes.
#include "GLSubsys.h"

// OpenGL includes.
#include <gl/glu.h>

// std c++ includes.
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>

// windows includes.
#include "WinGDI.h"

using std::string;
using std::vector;

const char *glExtString = 0;
const char *wglExtString = 0;
const char* glShaderString = 0;
const char* glRendererString = 0;
const char* glVendorString = 0;
const char* glVersionString = 0;
const char* glFormattedDriverString = 0;

static unsigned short _deviceGammaTable[ 768 ];
static bool _deviceGammaSupported = false;

static EGLPROFILE profile;

#ifdef _MONITOR_GL_STATS

unsigned int _bglDrawCount = 0;
unsigned int _bglDrawRangeCount = 0;
unsigned int _bglBindTextureCount = 0;
unsigned int _bglSetUniformCount = 0;
unsigned int _bglBindBufferCount = 0;

#define INCR_STAT( x )		( ++x )
#else
#define INCR_STAT( x )
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4996 )
#endif

//----------------------------------------------------------
// Windows specific extensions.
PFNWGLGETEXTENSIONSSTRINGARBPROC	wglGetExtensionsStringARB = 0;
PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT = 0;
PFNWGLGETSWAPINTERVALEXTPROC		wglGetSwapIntervalEXT = 0;

// OpenGL interface.
// 1.2
PFNGLBLENDCOLORPROC					glBlendColor = 0;
PFNGLBLENDEQUATIONPROC				glBlendEquation = 0;	
PFNGLDRAWRANGEELEMENTSPROC			glDrawRangeElements	= 0;
PFNGLCOLORTABLEPROC					glColorTable = 0;
PFNGLCOLORTABLEPARAMETERFVPROC		glColorTableParameterfv = 0;
PFNGLCOLORTABLEPARAMETERIVPROC		glColorTableParameteriv = 0;
PFNGLCOPYCOLORTABLEPROC				glCopyColorTable = 0;
PFNGLGETCOLORTABLEPROC				glGetColorTable = 0;
PFNGLGETCOLORTABLEPARAMETERFVPROC	glGetColorTableParameterfv = 0;
PFNGLGETCOLORTABLEPARAMETERIVPROC	glGetColorTableParameteriv = 0;
PFNGLCOLORSUBTABLEPROC				glColorSubTable = 0;
PFNGLCOPYCOLORSUBTABLEPROC			glCopyColorSubTable = 0;
PFNGLCONVOLUTIONFILTER1DPROC		glConvolutionFilter1D = 0;
PFNGLCONVOLUTIONFILTER2DPROC		glConvolutionFilter2D = 0;
PFNGLCONVOLUTIONPARAMETERFPROC		glConvolutionParameterf = 0;
PFNGLCONVOLUTIONPARAMETERFVPROC		glConvolutionParameterfv = 0;
PFNGLCONVOLUTIONPARAMETERIPROC		glConvolutionParameteri = 0;
PFNGLCONVOLUTIONPARAMETERIVPROC		glConvolutionParameteriv = 0;
PFNGLCOPYCONVOLUTIONFILTER1DPROC	glCopyConvolutionFilter1D = 0;
PFNGLCOPYCONVOLUTIONFILTER2DPROC	glCopyConvolutionFilter2D = 0;
PFNGLGETCONVOLUTIONFILTERPROC		glGetConvolutionFilter = 0;
PFNGLGETCONVOLUTIONPARAMETERFVPROC	glGetConvolutionParameterfv = 0;
PFNGLGETCONVOLUTIONPARAMETERIVPROC	glGetConvolutionParameteriv = 0;
PFNGLGETSEPARABLEFILTERPROC			glGetSeparableFilter = 0;
PFNGLSEPARABLEFILTER2DPROC			glSeparableFilter2D = 0;
PFNGLGETHISTOGRAMPROC				glGetHistogram = 0;
PFNGLGETHISTOGRAMPARAMETERFVPROC	glGetHistogramParameterfv = 0;
PFNGLGETHISTOGRAMPARAMETERIVPROC	glGetHistogramParameteriv = 0;
PFNGLGETMINMAXPROC					glGetMinmax = 0;
PFNGLGETMINMAXPARAMETERFVPROC		glGetMinmaxParameterfv = 0;
PFNGLGETMINMAXPARAMETERIVPROC		glGetMinmaxParameteriv = 0;
PFNGLHISTOGRAMPROC					glHistogram = 0;
PFNGLMINMAXPROC						glMinmax = 0;
PFNGLRESETHISTOGRAMPROC				glResetHistogram = 0;
PFNGLRESETMINMAXPROC				glResetMinmax = 0;
PFNGLTEXIMAGE3DPROC					glTexImage3D = 0;
PFNGLTEXSUBIMAGE3DPROC				glTexSubImage3D = 0;
PFNGLCOPYTEXSUBIMAGE3DPROC			glCopyTexSubImage3D = 0;

// 1.3
PFNGLACTIVETEXTUREPROC				glActiveTexture = 0;
PFNGLCLIENTACTIVETEXTUREPROC		glClientActiveTexture = 0;
PFNGLMULTITEXCOORD1DPROC			glMultiTexCoord1d = 0;
PFNGLMULTITEXCOORD1DVPROC			glMultiTexCoord1dv = 0;
PFNGLMULTITEXCOORD1FPROC			glMultiTexCoord1f = 0;
PFNGLMULTITEXCOORD1FVPROC			glMultiTexCoord1fv = 0;
PFNGLMULTITEXCOORD1IPROC			glMultiTexCoord1i = 0;
PFNGLMULTITEXCOORD1IVPROC			glMultiTexCoord1iv = 0;
PFNGLMULTITEXCOORD1SPROC			glMultiTexCoord1s = 0;
PFNGLMULTITEXCOORD1SVPROC			glMultiTexCoord1sv = 0;
PFNGLMULTITEXCOORD2DPROC			glMultiTexCoord2d = 0;
PFNGLMULTITEXCOORD2DVPROC			glMultiTexCoord2dv = 0;
PFNGLMULTITEXCOORD2FPROC			glMultiTexCoord2f = 0;
PFNGLMULTITEXCOORD2FVPROC			glMultiTexCoord2fv = 0;
PFNGLMULTITEXCOORD2IPROC			glMultiTexCoord2i = 0;
PFNGLMULTITEXCOORD2IVPROC			glMultiTexCoord2iv = 0;
PFNGLMULTITEXCOORD2SPROC			glMultiTexCoord2s = 0;
PFNGLMULTITEXCOORD2SVPROC			glMultiTexCoord2sv = 0;
PFNGLMULTITEXCOORD3DPROC			glMultiTexCoord3d = 0;
PFNGLMULTITEXCOORD3DVPROC			glMultiTexCoord3dv = 0;
PFNGLMULTITEXCOORD3FPROC			glMultiTexCoord3f = 0;
PFNGLMULTITEXCOORD3FVPROC			glMultiTexCoord3fv = 0;
PFNGLMULTITEXCOORD3IPROC			glMultiTexCoord3i = 0;
PFNGLMULTITEXCOORD3IVPROC			glMultiTexCoord3iv = 0;
PFNGLMULTITEXCOORD3SPROC			glMultiTexCoord3s = 0;
PFNGLMULTITEXCOORD3SVPROC			glMultiTexCoord3sv = 0;
PFNGLMULTITEXCOORD4DPROC			glMultiTexCoord4d = 0;
PFNGLMULTITEXCOORD4DVPROC			glMultiTexCoord4dv = 0;
PFNGLMULTITEXCOORD4FPROC			glMultiTexCoord4f = 0;
PFNGLMULTITEXCOORD4FVPROC			glMultiTexCoord4fv = 0;
PFNGLMULTITEXCOORD4IPROC			glMultiTexCoord4i = 0;
PFNGLMULTITEXCOORD4IVPROC			glMultiTexCoord4iv = 0;
PFNGLMULTITEXCOORD4SPROC			glMultiTexCoord4s = 0;
PFNGLMULTITEXCOORD4SVPROC			glMultiTexCoord4sv = 0;
PFNGLLOADTRANSPOSEMATRIXFPROC		glLoadTransposeMatrixf = 0;
PFNGLLOADTRANSPOSEMATRIXDPROC		glLoadTransposeMatrixd = 0;
PFNGLMULTTRANSPOSEMATRIXFPROC		glMultTransposeMatrixf = 0;
PFNGLMULTTRANSPOSEMATRIXDPROC		glMultTransposeMatrixd = 0;
PFNGLSAMPLECOVERAGEPROC				glSampleCoverage = 0;
PFNGLCOMPRESSEDTEXIMAGE3DPROC		glCompressedTexImage3D = 0;
PFNGLCOMPRESSEDTEXIMAGE2DPROC		glCompressedTexImage2D = 0;
PFNGLCOMPRESSEDTEXIMAGE1DPROC		glCompressedTexImage1D = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC	glCompressedTexSubImage3D = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC	glCompressedTexSubImage2D = 0;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC	glCompressedTexSubImage1D = 0;
PFNGLGETCOMPRESSEDTEXIMAGEPROC		glGetCompressedTexImage = 0;

// 1.4
PFNGLBLENDFUNCSEPARATEPROC			glBlendFuncSeparate = 0;
PFNGLFOGCOORDFPROC					glFogCoordf = 0;
PFNGLFOGCOORDFVPROC					glFogCoordfv = 0;
PFNGLFOGCOORDDPROC					glFogCoordd = 0;
PFNGLFOGCOORDDVPROC					glFogCoorddv = 0;
PFNGLFOGCOORDPOINTERPROC			glFogCoordPointer = 0;
PFNGLMULTIDRAWARRAYSPROC			glMultiDrawArrays = 0;
PFNGLMULTIDRAWELEMENTSPROC			glMultiDrawElements = 0;
PFNGLPOINTPARAMETERFPROC			glPointParameterf = 0;
PFNGLPOINTPARAMETERFVPROC			glPointParameterfv = 0;
PFNGLPOINTPARAMETERIPROC			glPointParameteri = 0;
PFNGLPOINTPARAMETERIVPROC			glPointParameteriv = 0;
PFNGLSECONDARYCOLOR3BPROC			glSecondaryColor3b = 0;
PFNGLSECONDARYCOLOR3BVPROC			glSecondaryColor3bv = 0;
PFNGLSECONDARYCOLOR3DPROC			glSecondaryColor3d = 0;
PFNGLSECONDARYCOLOR3DVPROC			glSecondaryColor3dv = 0;
PFNGLSECONDARYCOLOR3FPROC			glSecondaryColor3f = 0;
PFNGLSECONDARYCOLOR3FVPROC			glSecondaryColor3fv = 0;
PFNGLSECONDARYCOLOR3IPROC			glSecondaryColor3i = 0;
PFNGLSECONDARYCOLOR3IVPROC			glSecondaryColor3iv = 0;
PFNGLSECONDARYCOLOR3SPROC			glSecondaryColor3s = 0;
PFNGLSECONDARYCOLOR3SVPROC			glSecondaryColor3sv = 0;
PFNGLSECONDARYCOLOR3UBPROC			glSecondaryColor3ub = 0;
PFNGLSECONDARYCOLOR3UBVPROC			glSecondaryColor3ubv = 0;
PFNGLSECONDARYCOLOR3UIPROC			glSecondaryColor3ui = 0;
PFNGLSECONDARYCOLOR3UIVPROC			glSecondaryColor3uiv = 0;
PFNGLSECONDARYCOLOR3USPROC			glSecondaryColor3us = 0;
PFNGLSECONDARYCOLOR3USVPROC			glSecondaryColor3usv = 0;
PFNGLSECONDARYCOLORPOINTERPROC		glSecondaryColorPointer = 0;
PFNGLWINDOWPOS2DPROC				glWindowPos2d = 0;
PFNGLWINDOWPOS2DVPROC				glWindowPos2dv = 0;
PFNGLWINDOWPOS2FPROC				glWindowPos2f = 0;
PFNGLWINDOWPOS2FVPROC				glWindowPos2fv = 0;
PFNGLWINDOWPOS2IPROC				glWindowPos2i = 0;
PFNGLWINDOWPOS2IVPROC				glWindowPos2iv = 0;
PFNGLWINDOWPOS2SPROC				glWindowPos2s = 0;
PFNGLWINDOWPOS2SVPROC				glWindowPos2sv = 0;
PFNGLWINDOWPOS3DPROC				glWindowPos3d = 0;
PFNGLWINDOWPOS3DVPROC				glWindowPos3dv = 0;
PFNGLWINDOWPOS3FPROC				glWindowPos3f = 0;
PFNGLWINDOWPOS3FVPROC				glWindowPos3fv = 0;
PFNGLWINDOWPOS3IPROC				glWindowPos3i = 0;
PFNGLWINDOWPOS3IVPROC				glWindowPos3iv = 0;
PFNGLWINDOWPOS3SPROC				glWindowPos3s = 0;
PFNGLWINDOWPOS3SVPROC				glWindowPos3sv = 0;

// 1.5
PFNGLGENQUERIESPROC					glGenQueries = 0;
PFNGLDELETEQUERIESPROC				glDeleteQueries = 0;
PFNGLISQUERYPROC					glIsQuery = 0;
PFNGLBEGINQUERYPROC					glBeginQuery = 0;
PFNGLENDQUERYPROC					glEndQuery = 0;
PFNGLGETQUERYIVPROC					glGetQueryiv = 0;
PFNGLGETQUERYOBJECTIVPROC			glGetQueryObjectiv = 0;
PFNGLGETQUERYOBJECTUIVPROC			glGetQueryObjectuiv = 0;
PFNGLBINDBUFFERPROC					glBindBuffer = 0;
PFNGLDELETEBUFFERSPROC				glDeleteBuffers = 0;
PFNGLGENBUFFERSPROC					glGenBuffers = 0;
PFNGLISBUFFERPROC					glIsBuffer = 0;
PFNGLBUFFERDATAPROC					glBufferData = 0;
PFNGLBUFFERSUBDATAPROC				glBufferSubData = 0;
PFNGLGETBUFFERSUBDATAPROC			glGetBufferSubData = 0;
PFNGLMAPBUFFERPROC					glMapBuffer = 0;
PFNGLUNMAPBUFFERPROC				glUnmapBuffer = 0;
PFNGLGETBUFFERPARAMETERIVPROC		glGetBufferParameteriv = 0;
PFNGLGETBUFFERPOINTERVPROC			glGetBufferPointerv = 0;

// 2.0
PFNGLBLENDEQUATIONSEPARATEPROC		glBlendEquationSeparate = 0;
PFNGLDRAWBUFFERSPROC				glDrawBuffers = 0;
PFNGLSTENCILOPSEPARATEPROC			glStencilOpSeparate = 0;
PFNGLSTENCILFUNCSEPARATEPROC		glStencilFuncSeparate = 0;
PFNGLSTENCILMASKSEPARATEPROC		glStencilMaskSeparate = 0;
PFNGLATTACHSHADERPROC				glAttachShader = 0;
PFNGLBINDATTRIBLOCATIONPROC			glBindAttribLocation = 0;
PFNGLCOMPILESHADERPROC				glCompileShader = 0;
PFNGLCREATEPROGRAMPROC				glCreateProgram = 0;
PFNGLCREATESHADERPROC				glCreateShader = 0;
PFNGLDELETEPROGRAMPROC				glDeleteProgram = 0;
PFNGLDELETESHADERPROC				glDeleteShader = 0;
PFNGLDETACHSHADERPROC				glDetachShader = 0;
PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray = 0;
PFNGLENABLEVERTEXATTRIBARRAYPROC	glEnableVertexAttribArray = 0;
PFNGLGETACTIVEATTRIBPROC			glGetActiveAttrib = 0;
PFNGLGETACTIVEUNIFORMPROC			glGetActiveUniform = 0;
PFNGLGETATTACHEDSHADERSPROC			glGetAttachedShaders = 0;
PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation = 0;
PFNGLGETPROGRAMIVPROC				glGetProgramiv = 0;
PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog = 0;
PFNGLGETSHADERIVPROC				glGetShaderiv = 0;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog = 0;
PFNGLGETSHADERSOURCEPROC			glGetShaderSource = 0;
PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation = 0;
PFNGLGETUNIFORMFVPROC				glGetUniformfv = 0;
PFNGLGETUNIFORMIVPROC				glGetUniformiv = 0;
PFNGLGETVERTEXATTRIBDVPROC			glGetVertexAttribdv = 0;
PFNGLGETVERTEXATTRIBFVPROC			glGetVertexAttribfv = 0;
PFNGLGETVERTEXATTRIBIVPROC			glGetVertexAttribiv = 0;
PFNGLGETVERTEXATTRIBPOINTERVPROC	glGetVertexAttribPointerv = 0;
PFNGLISPROGRAMPROC					glIsProgram = 0;
PFNGLISSHADERPROC					glIsShader = 0;
PFNGLLINKPROGRAMPROC				glLinkProgram = 0;
PFNGLSHADERSOURCEPROC				glShaderSource = 0;
PFNGLUSEPROGRAMPROC 				glUseProgram = 0;
PFNGLUNIFORM1FPROC					glUniform1f = 0;
PFNGLUNIFORM2FPROC					glUniform2f = 0;
PFNGLUNIFORM3FPROC					glUniform3f = 0;
PFNGLUNIFORM4FPROC					glUniform4f = 0;
PFNGLUNIFORM1IPROC					glUniform1i = 0;
PFNGLUNIFORM2IPROC					glUniform2i = 0;
PFNGLUNIFORM3IPROC					glUniform3i = 0;
PFNGLUNIFORM4IPROC					glUniform4i = 0;
PFNGLUNIFORM1FVPROC					glUniform1fv = 0;
PFNGLUNIFORM2FVPROC					glUniform2fv = 0;
PFNGLUNIFORM3FVPROC					glUniform3fv = 0;
PFNGLUNIFORM4FVPROC					glUniform4fv = 0;
PFNGLUNIFORM1IVPROC					glUniform1iv = 0;
PFNGLUNIFORM2IVPROC					glUniform2iv = 0;
PFNGLUNIFORM3IVPROC					glUniform3iv = 0;
PFNGLUNIFORM4IVPROC					glUniform4iv = 0;
PFNGLUNIFORMMATRIX2FVPROC			glUniformMatrix2fv = 0;
PFNGLUNIFORMMATRIX3FVPROC			glUniformMatrix3fv = 0;
PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv = 0;
PFNGLVALIDATEPROGRAMPROC			glValidateProgram = 0;
PFNGLVERTEXATTRIB1DPROC				glVertexAttrib1d = 0;
PFNGLVERTEXATTRIB1DVPROC			glVertexAttrib1dv = 0;
PFNGLVERTEXATTRIB1FPROC				glVertexAttrib1f = 0;
PFNGLVERTEXATTRIB1FVPROC			glVertexAttrib1fv = 0;
PFNGLVERTEXATTRIB1SPROC				glVertexAttrib1s = 0;
PFNGLVERTEXATTRIB1SVPROC			glVertexAttrib1sv = 0;
PFNGLVERTEXATTRIB2DPROC				glVertexAttrib2d = 0;
PFNGLVERTEXATTRIB2DVPROC			glVertexAttrib2dv = 0;
PFNGLVERTEXATTRIB2FPROC				glVertexAttrib2f = 0;
PFNGLVERTEXATTRIB2FVPROC			glVertexAttrib2fv = 0;
PFNGLVERTEXATTRIB2SPROC				glVertexAttrib2s = 0;
PFNGLVERTEXATTRIB2SVPROC			glVertexAttrib2sv = 0;
PFNGLVERTEXATTRIB3DPROC				glVertexAttrib3d = 0;
PFNGLVERTEXATTRIB3DVPROC			glVertexAttrib3dv = 0;
PFNGLVERTEXATTRIB3FPROC				glVertexAttrib3f = 0;
PFNGLVERTEXATTRIB3FVPROC			glVertexAttrib3fv = 0;
PFNGLVERTEXATTRIB3SPROC				glVertexAttrib3s = 0;
PFNGLVERTEXATTRIB3SVPROC			glVertexAttrib3sv = 0;
PFNGLVERTEXATTRIB4NBVPROC			glVertexAttrib4Nbv = 0;
PFNGLVERTEXATTRIB4NIVPROC			glVertexAttrib4Niv = 0;
PFNGLVERTEXATTRIB4NSVPROC			glVertexAttrib4Nsv = 0;
PFNGLVERTEXATTRIB4NUBPROC			glVertexAttrib4Nub = 0;
PFNGLVERTEXATTRIB4NUBVPROC			glVertexAttrib4Nubv = 0;
PFNGLVERTEXATTRIB4NUIVPROC			glVertexAttrib4Nuiv = 0;
PFNGLVERTEXATTRIB4NUSVPROC			glVertexAttrib4Nusv = 0;
PFNGLVERTEXATTRIB4BVPROC			glVertexAttrib4bv = 0;
PFNGLVERTEXATTRIB4DPROC				glVertexAttrib4d = 0;
PFNGLVERTEXATTRIB4DVPROC			glVertexAttrib4dv = 0;
PFNGLVERTEXATTRIB4FPROC				glVertexAttrib4f = 0;
PFNGLVERTEXATTRIB4FVPROC			glVertexAttrib4fv = 0;
PFNGLVERTEXATTRIB4IVPROC			glVertexAttrib4iv = 0;
PFNGLVERTEXATTRIB4SPROC				glVertexAttrib4s = 0;
PFNGLVERTEXATTRIB4SVPROC			glVertexAttrib4sv = 0;
PFNGLVERTEXATTRIB4UBVPROC			glVertexAttrib4ubv = 0;
PFNGLVERTEXATTRIB4UIVPROC			glVertexAttrib4uiv = 0;
PFNGLVERTEXATTRIB4USVPROC			glVertexAttrib4usv = 0;
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer = 0;

// 2.1
PFNGLUNIFORMMATRIX2X3FVPROC			glUniformMatrix2x3fv = 0;
PFNGLUNIFORMMATRIX3X2FVPROC			glUniformMatrix3x2fv = 0;
PFNGLUNIFORMMATRIX2X4FVPROC			glUniformMatrix2x4fv = 0;
PFNGLUNIFORMMATRIX4X2FVPROC			glUniformMatrix4x2fv = 0;
PFNGLUNIFORMMATRIX3X4FVPROC			glUniformMatrix3x4fv = 0;
PFNGLUNIFORMMATRIX4X3FVPROC			glUniformMatrix4x3fv = 0;

// Generic extensions req'd.
// GL_ARB_shader_objects
PFNGLDELETEOBJECTARBPROC			glDeleteObjectARB = 0;
PFNGLGETHANDLEARBPROC				glGetHandleARB = 0;
PFNGLDETACHOBJECTARBPROC			glDetachObjectARB = 0;
PFNGLCREATESHADEROBJECTARBPROC		glCreateShaderObjectARB = 0;
PFNGLSHADERSOURCEARBPROC			glShaderSourceARB = 0;
PFNGLCOMPILESHADERARBPROC			glCompileShaderARB = 0;
PFNGLCREATEPROGRAMOBJECTARBPROC		glCreateProgramObjectARB = 0;
PFNGLATTACHOBJECTARBPROC			glAttachObjectARB = 0;
PFNGLLINKPROGRAMARBPROC				glLinkProgramARB = 0;
PFNGLUSEPROGRAMOBJECTARBPROC		glUseProgramObjectARB = 0;
PFNGLVALIDATEPROGRAMARBPROC			glValidateProgramARB = 0;
PFNGLUNIFORM1FARBPROC				glUniform1fARB = 0;
PFNGLUNIFORM2FARBPROC				glUniform2fARB = 0;
PFNGLUNIFORM3FARBPROC				glUniform3fARB = 0;
PFNGLUNIFORM4FARBPROC				glUniform4fARB = 0;
PFNGLUNIFORM1IARBPROC				glUniform1iARB = 0;
PFNGLUNIFORM2IARBPROC				glUniform2iARB = 0;
PFNGLUNIFORM3IARBPROC				glUniform3iARB = 0;
PFNGLUNIFORM4IARBPROC				glUniform4iARB = 0;
PFNGLUNIFORM1FVARBPROC				glUniform1fvARB = 0;
PFNGLUNIFORM2FVARBPROC				glUniform2fvARB = 0;
PFNGLUNIFORM3FVARBPROC				glUniform3fvARB = 0;
PFNGLUNIFORM4FVARBPROC				glUniform4fvARB = 0;
PFNGLUNIFORM1IVARBPROC				glUniform1ivARB = 0;
PFNGLUNIFORM2IVARBPROC				glUniform2ivARB = 0;
PFNGLUNIFORM3IVARBPROC				glUniform3ivARB = 0;
PFNGLUNIFORM4IVARBPROC				glUniform4ivARB = 0;
PFNGLUNIFORMMATRIX2FVARBPROC		glUniformMatrix2fvARB = 0;
PFNGLUNIFORMMATRIX3FVARBPROC		glUniformMatrix3fvARB = 0;
PFNGLUNIFORMMATRIX4FVARBPROC		glUniformMatrix4fvARB = 0;
PFNGLGETOBJECTPARAMETERFVARBPROC	glGetObjectParameterfvARB = 0;
PFNGLGETOBJECTPARAMETERIVARBPROC	glGetObjectParameterivARB = 0;
PFNGLGETINFOLOGARBPROC				glGetInfoLogARB = 0;
PFNGLGETATTACHEDOBJECTSARBPROC		glGetAttachedObjectsARB = 0;
PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocationARB = 0;
PFNGLGETACTIVEUNIFORMARBPROC		glGetActiveUniformARB = 0;
PFNGLGETUNIFORMFVARBPROC			glGetUniformfvARB = 0;
PFNGLGETUNIFORMIVARBPROC			glGetUniformivARB = 0;
PFNGLGETSHADERSOURCEARBPROC			glGetShaderSourceARB = 0;

// ARB_draw_buffers
PFNGLDRAWBUFFERSARBPROC				glDrawBuffersARB = 0;

// GL_ARB_vertex_program
PFNGLVERTEXATTRIB1DARBPROC			glVertexAttrib1dARB = 0;
PFNGLVERTEXATTRIB1DVARBPROC			glVertexAttrib1dvARB = 0;
PFNGLVERTEXATTRIB1FARBPROC			glVertexAttrib1fARB = 0;
PFNGLVERTEXATTRIB1FVARBPROC			glVertexAttrib1fvARB = 0;
PFNGLVERTEXATTRIB1SARBPROC			glVertexAttrib1sARB = 0;
PFNGLVERTEXATTRIB1SVARBPROC			glVertexAttrib1svARB = 0;
PFNGLVERTEXATTRIB2DARBPROC			glVertexAttrib2dARB = 0;
PFNGLVERTEXATTRIB2DVARBPROC			glVertexAttrib2dvARB = 0;
PFNGLVERTEXATTRIB2FARBPROC			glVertexAttrib2fARB = 0;
PFNGLVERTEXATTRIB2FVARBPROC			glVertexAttrib2fvARB = 0;
PFNGLVERTEXATTRIB2SARBPROC			glVertexAttrib2sARB = 0;
PFNGLVERTEXATTRIB2SVARBPROC			glVertexAttrib2svARB = 0;
PFNGLVERTEXATTRIB3DARBPROC			glVertexAttrib3dARB = 0;
PFNGLVERTEXATTRIB3DVARBPROC			glVertexAttrib3dvARB = 0;
PFNGLVERTEXATTRIB3FARBPROC			glVertexAttrib3fARB = 0;
PFNGLVERTEXATTRIB3FVARBPROC			glVertexAttrib3fvARB = 0;
PFNGLVERTEXATTRIB3SARBPROC			glVertexAttrib3sARB = 0;
PFNGLVERTEXATTRIB3SVARBPROC			glVertexAttrib3svARB = 0;
PFNGLVERTEXATTRIB4NBVARBPROC		glVertexAttrib4NbvARB = 0;
PFNGLVERTEXATTRIB4NIVARBPROC		glVertexAttrib4NivARB = 0;
PFNGLVERTEXATTRIB4NSVARBPROC		glVertexAttrib4NsvARB = 0;
PFNGLVERTEXATTRIB4NUBARBPROC		glVertexAttrib4NubARB = 0;
PFNGLVERTEXATTRIB4NUBVARBPROC		glVertexAttrib4NubvARB = 0;
PFNGLVERTEXATTRIB4NUIVARBPROC		glVertexAttrib4NuivARB = 0;
PFNGLVERTEXATTRIB4NUSVARBPROC		glVertexAttrib4NusvARB = 0;
PFNGLVERTEXATTRIB4BVARBPROC			glVertexAttrib4bvARB = 0;
PFNGLVERTEXATTRIB4DARBPROC			glVertexAttrib4dARB = 0;
PFNGLVERTEXATTRIB4DVARBPROC			glVertexAttrib4dvARB = 0;
PFNGLVERTEXATTRIB4FARBPROC			glVertexAttrib4fARB = 0;
PFNGLVERTEXATTRIB4FVARBPROC			glVertexAttrib4fvARB = 0;
PFNGLVERTEXATTRIB4IVARBPROC			glVertexAttrib4ivARB = 0;
PFNGLVERTEXATTRIB4SARBPROC			glVertexAttrib4sARB = 0;
PFNGLVERTEXATTRIB4SVARBPROC			glVertexAttrib4svARB = 0;
PFNGLVERTEXATTRIB4UBVARBPROC		glVertexAttrib4ubvARB = 0;
PFNGLVERTEXATTRIB4UIVARBPROC		glVertexAttrib4uivARB = 0;
PFNGLVERTEXATTRIB4USVARBPROC		glVertexAttrib4usvARB = 0;
PFNGLVERTEXATTRIBPOINTERARBPROC		glVertexAttribPointerARB = 0;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC	glEnableVertexAttribArrayARB = 0;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	glDisableVertexAttribArrayARB = 0;
PFNGLGETVERTEXATTRIBDVARBPROC		glGetVertexAttribdvARB = 0;
PFNGLGETVERTEXATTRIBFVARBPROC		glGetVertexAttribfvARB = 0;
PFNGLGETVERTEXATTRIBIVARBPROC		glGetVertexAttribivARB = 0;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC	glGetVertexAttribPointervARB = 0;

PFNGLBINDPROGRAMARBPROC				glBindProgramARB = 0;
PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB = 0;
PFNGLGENPROGRAMSARBPROC				glGenProgramsARB = 0;
PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB = 0;
PFNGLPROGRAMENVPARAMETER4FVARBPROC	glProgramEnvParameter4fvARB = 0;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	glProgramLocalParameter4fvARB = 0;
PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB = 0;

// < /inherited from GL_ARB_vertex_program >
PFNGLBINDATTRIBLOCATIONARBPROC		glBindAttribLocationARB = 0;
PFNGLGETACTIVEATTRIBARBPROC			glGetActiveAttribARB = 0;
PFNGLGETATTRIBLOCATIONARBPROC		glGetAttribLocationARB = 0;

// GL_EXT_framebuffer_object
PFNGLISRENDERBUFFEREXTPROC			glIsRenderbufferEXT = 0;
PFNGLBINDRENDERBUFFEREXTPROC		glBindRenderbufferEXT = 0;
PFNGLDELETERENDERBUFFERSEXTPROC		glDeleteRenderbuffersEXT = 0;
PFNGLGENRENDERBUFFERSEXTPROC		glGenRenderbuffersEXT = 0;
PFNGLRENDERBUFFERSTORAGEEXTPROC		glRenderbufferStorageEXT = 0;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivEXT = 0;
PFNGLISFRAMEBUFFEREXTPROC			glIsFramebufferEXT = 0;
PFNGLBINDFRAMEBUFFEREXTPROC			glBindFramebufferEXT = 0;
PFNGLDELETEFRAMEBUFFERSEXTPROC		glDeleteFramebuffersEXT = 0;
PFNGLGENFRAMEBUFFERSEXTPROC			glGenFramebuffersEXT = 0;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC	glCheckFramebufferStatusEXT = 0;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC	glFramebufferTexture1DEXT = 0;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC	glFramebufferTexture2DEXT = 0;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC	glFramebufferTexture3DEXT = 0;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC	glFramebufferRenderbufferEXT = 0;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	glGetFramebufferAttachmentParameterivEXT = 0;
PFNGLGENERATEMIPMAPEXTPROC			glGenerateMipmapEXT = 0;

// GL_ARB_multisample
PFNGLSAMPLECOVERAGEARBPROC			glSampleCoverageARB = 0;

// GL_ARB_color_buffer_float
PFNGLCLAMPCOLORARBPROC				glClampColorARB = 0;

// GL_EXT_depth_bounds_test
PFNGLDEPTHBOUNDSEXTPROC				glDepthBoundsEXT = 0;

// GL_NV_primitive_restart
PFNGLPRIMITIVERESTARTNVPROC			glPrimitiveRestartNV = 0;
PFNGLPRIMITIVERESTARTINDEXNVPROC	glPrimitiveRestartIndexNV = 0;

// GL_NV_fence
PFNGLGENFENCESNVPROC                glGenFencesNV = 0;
PFNGLDELETEFENCESNVPROC             glDeleteFencesNV = 0;
PFNGLSETFENCENVPROC                 glSetFenceNV = 0;
PFNGLTESTFENCENVPROC                glTestFenceNV = 0;
PFNGLFINISHFENCENVPROC              glFinishFenceNV = 0;
PFNGLISFENCENVPROC                  glIsFenceNV = 0;
PFNGLGETFENCEIVNVPROC               glGetFenceivNV = 0;

// WGL_ARB_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVARBPROC	wglGetPixelFormatAttribivARB = 0;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC	wglGetPixelFormatAttribfvARB = 0;
PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB = 0;

// WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC			wglCreatePbufferARB = 0;
PFNWGLGETPBUFFERDCARBPROC			wglGetPbufferDCARB = 0;
PFNWGLRELEASEPBUFFERDCARBPROC		wglReleasePbufferDCARB = 0;
PFNWGLDESTROYPBUFFERARBPROC			wglDestroyPbufferARB = 0;
PFNWGLQUERYPBUFFERARBPROC			wglQueryPbufferARB = 0;

// WGL_ARB_render_texture
PFNWGLBINDTEXIMAGEARBPROC			wglBindTexImageARB = 0;
PFNWGLRELEASETEXIMAGEARBPROC		wglReleaseTexImageARB = 0;
PFNWGLSETPBUFFERATTRIBARBPROC		wglSetPbufferAttribARB = 0;

// optional extensions.
// GL_ARB_texture_rectangle
bool ARB_texture_rectangle = false;

// GL_ARB_texture_non_power_of_two
bool ARB_texture_non_power_of_two = false;

//----------------------------------------------------------
static
void
SplitString( const std::string& str, std::vector< std::string >& vec)
{
	std::string::size_type start= str.find_first_not_of( " " );
	while ( start != std::string::npos ) 
	{
		std::string::size_type end = str.find( ' ', start );
		vec.push_back( str.substr( start, end - start ) );
		start = str.find_first_not_of( " ", end );
	}
}


//----------------------------------------------------------
static
void
CheckExts( const char *neededExtsParam )
{
	B_ASSERT( glExtString != 0 );
	B_ASSERT( wglExtString != 0 );

	vector< string > needed;
	SplitString( neededExtsParam, needed);
	vector< string > exts;
	SplitString( glExtString, exts );
	SplitString( wglExtString, exts );
	std::sort( needed.begin(), needed.end() );
	std::sort( exts.begin(), exts.end() );
	
	vector< string > notFound;
	std::set_difference( needed.begin(), needed.end(), 
		exts.begin(), exts.end(), 
		std::back_insert_iterator< vector< string > >( notFound ) );

//	// make a writable string.
//	unsigned int memSize = ( unsigned int )strlen( neededExtsParam ) + 1;
//	char *neededExts = new char[ memSize + 1 ];
//	MemCopy( neededExts, neededExtsParam, memSize );
//
//	// scan for extensions.
//	vector< char * > notFound;
//	char* context = 0;
//	for ( char *token = strtok_s( neededExts, " \0", &context ); token != 0; token = strtok_s( NULL, " \0", &context ) )
//	{
//		if ( strstr( glExtString, token ) == 0 && strstr( wglExtString, token ) == 0 )
//		{
//			notFound.push_back( token );
//		}
//	}
//
	// throw an error if necessary extensions were not found.
	if ( notFound.size() != 0 )
	{
		tstring errStr = "Video card does not meet minimum requirements.  The following extensions were not found...\n";
		for ( unsigned int i = 0; i < notFound.size(); ++i )
		{
			errStr += "\t";
			errStr += notFound[ i ];
			errStr += "\n";
		}

		//delete[] neededExts;
		B_ERROR( errStr );
	}
//	delete[] neededExts;
}

//----------------------------------------------------------
unsigned int
GetExtCount( const char* glExtString )
{
	unsigned int count = 1;
	const char* cur = strchr( glExtString, ' ' );
	while ( cur )
	{
		cur = strchr( cur + 1, ' ' );
		++count;
	}
	return count;
}

//----------------------------------------------------------
char*
BuildDriverInfoString( const char* glRendererString, const char* glVendorString, const char* glVersionString, const char* glExtString, const char* wglExtString )
{
	// get the length of the extension string.
	size_t glRendererLen = strlen( glRendererString );
	size_t glVendorLen = strlen( glVendorString );
	size_t glVersionLen = strlen( glVersionString );
	size_t glExtLen = strlen( glExtString );
	size_t wglExtLen = strlen( wglExtString );

	// get the extension count.
	unsigned int glExtCount = GetExtCount( glExtString );
	unsigned int wglExtCount = GetExtCount( wglExtString );

	// calculate the length of the OpenGL strings.
	size_t totalGLStrLen = glRendererLen + glExtLen + wglExtLen;

	// calculate the length of the strings we're adding.
	size_t localLen = strlen( "OpenGL Version: \n" ) + strlen( "Vendor: " ) + strlen( "Renderer: \n" ) + 
		strlen( "WGL Extensions:\n" ) + strlen( "Extensions:\n" );
	localLen += 2 * glExtCount;		// add space for tabs and new-lines.
	localLen += 2 * wglExtCount;	// add space for tabs and new-lines.

	// allocate space for the driver string.
	char* driverString = ( char* )malloc( localLen + totalGLStrLen + 1);

	// now build the formatted string:
	char* dst = driverString;
	dst += sprintf( dst, "Vendor: %s\n", glVendorString );
	dst += sprintf( dst, "Renderer: %s\n", glRendererString );
	dst += sprintf( dst, "OpenGL Version: %s\n", glVersionString );

	// add the WGL extension tag.
	dst += sprintf( dst, "WGL Extensions: \n" );

	// add the WGL extnsions.
	char* temp = strcpy( ( char* )malloc( wglExtLen + 1 ), wglExtString );
	for ( char* ext = strtok( temp, " " ); ext != 0; ext = strtok( 0, " " ) )
		dst += sprintf( dst, "\t%s\n", ext );
	free( temp );

	// add the GL extension tag.
	dst += sprintf( dst, "GL Extensions: \n" );
	
	// add the GL extensions.
	temp = strcpy( ( char* )malloc( glExtLen + 1 ), glExtString );
	for ( char* ext = strtok( temp, " " ); ext != 0; ext = strtok( 0, " " ) )
		dst += sprintf( dst, "\t%s\n", ext );
	free( temp );

	// all done.
	return driverString;
}

//----------------------------------------------------------
void
StartGL( HDC device )
{
	// check the OpenGL version string.
	const char *version = ( const char * )glGetString( GL_VERSION );
	string ver = version;
	size_t midPt = ver.find_first_of( "." );
	int majorVer = atoi( ver.substr( 0, midPt ).c_str() );
	int minorVer = atoi( ver.substr( midPt+1, midPt+2 ).c_str() );
	while ( minorVer > 10 ) minorVer /= 10;

	// make sure we have version 2.0 or better.
	if ( majorVer < 2 )
	{
		B_ERROR( "Video card does not meet minimum requirements (OpenGL 2.0 is required)!" );
	}

	if ( majorVer == 2 )
	{
		if ( minorVer == 0 )
			profile	= EGLP_2_0;
		else if ( minorVer >= 1 )
			profile = EGLP_2_1;
	}
	else if ( majorVer >= 3 )
	{
		profile = EGLP_3_0;
	}

	// get the gamma table from the device passed in.
	_deviceGammaSupported = GetDeviceGammaRamp( device, _deviceGammaTable ) != 0;

	// WGL extensions...
	wglGetExtensionsStringARB =		( PFNWGLGETEXTENSIONSSTRINGARBPROC	 )wglGetProcAddress( "wglGetExtensionsStringARB" );
	wglSwapIntervalEXT =			( PFNWGLSWAPINTERVALEXTPROC )wglGetProcAddress( "wglSwapIntervalEXT" );
	wglGetSwapIntervalEXT =			( PFNWGLGETSWAPINTERVALEXTPROC )wglGetProcAddress( "wglGetSwapIntervalEXT" );

	bwglSwapIntervalEXT( 0 );

	// check for required extensions...
	glExtString = ( const char * )glGetString( GL_EXTENSIONS );
	::wglExtString = bwglGetExtensionsStringARB( device );

	// check for the necessary extensions.
	CheckExts( "GL_ARB_shader_objects "
			   "GL_ARB_vertex_shader "
			   "GL_ARB_fragment_shader "
			   "GL_ARB_transpose_matrix "
			   "GL_EXT_texture_filter_anisotropic "
//			   "GL_EXT_texture_mirror_clamp "
			   "GL_EXT_texture_lod_bias "
			   "GL_EXT_framebuffer_object "
			   "WGL_ARB_pixel_format "
			   "WGL_ARB_pbuffer "
			   "WGL_ARB_render_texture "
			   "WGL_ATI_pixel_format_float "
			  );

	// set flags for optional extensions.
	ARB_texture_rectangle = ( strstr( glExtString, "GL_ARB_texture_rectangle" ) != 0 );
	ARB_texture_non_power_of_two = ( strstr( glExtString, "GL_ARB_texture_non_power_of_two" ) != 0 );

	// get the shading language version.
	glShaderString = ( const char* )glGetString( GL_SHADING_LANGUAGE_VERSION_ARB );
	glRendererString = ( const char* )glGetString( GL_RENDERER );
	glVendorString = ( const char* )glGetString( GL_VENDOR );
	glVersionString = ( const char* )glGetString( GL_VERSION );

	// build the driver string.
	glFormattedDriverString = BuildDriverInfoString( glRendererString, glVendorString, glVersionString, glExtString, wglExtString );

	// output the formatted driver string to the console.
	PrintF( glFormattedDriverString );

	//---------------------------------------------------------
	// Required extensions
	//---------------------------------------------------------
	// initialize OpenGL 1.5
	// 1.2
	glBlendColor =					( PFNGLBLENDCOLORPROC				 )wglGetProcAddress( "glBlendColor" );
	glBlendEquation =				( PFNGLBLENDEQUATIONPROC			 )wglGetProcAddress( "glBlendEquation" );
	glDrawRangeElements =			( PFNGLDRAWRANGEELEMENTSPROC		 )wglGetProcAddress( "glDrawRangeElements" );
	glColorTable =					( PFNGLCOLORTABLEPROC				 )wglGetProcAddress( "glColorTable" );
	glColorTableParameterfv =		( PFNGLCOLORTABLEPARAMETERFVPROC	 )wglGetProcAddress( "glColorTableParameterfv" );
	glColorTableParameteriv =		( PFNGLCOLORTABLEPARAMETERIVPROC	 )wglGetProcAddress( "glColorTableParameteriv" );
	glCopyColorTable =				( PFNGLCOPYCOLORTABLEPROC			 )wglGetProcAddress( "glCopyColorTable" );
	glGetColorTable =				( PFNGLGETCOLORTABLEPROC			 )wglGetProcAddress( "glGetColorTable" );
	glGetColorTableParameterfv =	( PFNGLGETCOLORTABLEPARAMETERFVPROC	 )wglGetProcAddress( "glGetColorTableParameterfv" );
	glGetColorTableParameteriv =	( PFNGLGETCOLORTABLEPARAMETERIVPROC	 )wglGetProcAddress( "glGetColorTableParameteriv" );
	glColorSubTable =				( PFNGLCOLORSUBTABLEPROC			 )wglGetProcAddress( "glColorSubTable" );
	glCopyColorSubTable =			( PFNGLCOPYCOLORSUBTABLEPROC		 )wglGetProcAddress( "glCopyColorSubTable" );
	glConvolutionFilter1D =			( PFNGLCONVOLUTIONFILTER1DPROC		 )wglGetProcAddress( "glConvolutionFilter1D" );
	glConvolutionFilter2D =			( PFNGLCONVOLUTIONFILTER2DPROC		 )wglGetProcAddress( "glConvolutionFilter2D" );
	glConvolutionParameterf =		( PFNGLCONVOLUTIONPARAMETERFPROC	 )wglGetProcAddress( "glConvolutionParameterf" );
	glConvolutionParameterfv =		( PFNGLCONVOLUTIONPARAMETERFVPROC	 )wglGetProcAddress( "glConvolutionParameterfv" );
	glConvolutionParameteri =		( PFNGLCONVOLUTIONPARAMETERIPROC	 )wglGetProcAddress( "glConvolutionParameteri" );
	glConvolutionParameteriv =		( PFNGLCONVOLUTIONPARAMETERIVPROC	 )wglGetProcAddress( "glConvolutionParameteriv" );
	glCopyConvolutionFilter1D =		( PFNGLCOPYCONVOLUTIONFILTER1DPROC	 )wglGetProcAddress( "glCopyConvolutionFilter1D" );
	glCopyConvolutionFilter2D =		( PFNGLCOPYCONVOLUTIONFILTER2DPROC	 )wglGetProcAddress( "glCopyConvolutionFilter2D" );
	glGetConvolutionFilter =		( PFNGLGETCONVOLUTIONFILTERPROC		 )wglGetProcAddress( "glGetConvolutionFilter" );
	glGetConvolutionParameterfv =	( PFNGLGETCONVOLUTIONPARAMETERFVPROC )wglGetProcAddress( "glGetConvolutionParameterfv" );
	glGetConvolutionParameteriv =	( PFNGLGETCONVOLUTIONPARAMETERIVPROC )wglGetProcAddress( "glGetConvolutionParameteriv" );
	glGetSeparableFilter =			( PFNGLGETSEPARABLEFILTERPROC		 )wglGetProcAddress( "glGetSeparableFilter" );
	glSeparableFilter2D =			( PFNGLSEPARABLEFILTER2DPROC		 )wglGetProcAddress( "glSeparableFilter2D" );
	glGetHistogram =				( PFNGLGETHISTOGRAMPROC				 )wglGetProcAddress( "glGetHistogram" );
	glGetHistogramParameterfv =		( PFNGLGETHISTOGRAMPARAMETERFVPROC	 )wglGetProcAddress( "glGetHistogramParameterfv" );
	glGetHistogramParameteriv =		( PFNGLGETHISTOGRAMPARAMETERIVPROC	 )wglGetProcAddress( "glGetHistogramParameteriv" );
	glGetMinmax =					( PFNGLGETMINMAXPROC				 )wglGetProcAddress( "glGetMinmax" );
	glGetMinmaxParameterfv =		( PFNGLGETMINMAXPARAMETERFVPROC		 )wglGetProcAddress( "glGetMinmaxParameterfv" );
	glGetMinmaxParameteriv =		( PFNGLGETMINMAXPARAMETERIVPROC		 )wglGetProcAddress( "glGetMinmaxParameteriv" );
	glHistogram =					( PFNGLHISTOGRAMPROC				 )wglGetProcAddress( "glHistogram" );
	glMinmax =						( PFNGLMINMAXPROC					 )wglGetProcAddress( "glMinmax" );
	glResetHistogram =				( PFNGLRESETHISTOGRAMPROC			 )wglGetProcAddress( "glResetHistogram" );
	glResetMinmax =					( PFNGLRESETMINMAXPROC				 )wglGetProcAddress( "glResetMinmax" );
	glTexImage3D =					( PFNGLTEXIMAGE3DPROC				 )wglGetProcAddress( "glTexImage3D" );
	glTexSubImage3D =				( PFNGLTEXSUBIMAGE3DPROC			 )wglGetProcAddress( "glTexSubImage3D" );
	glCopyTexSubImage3D =			( PFNGLCOPYTEXSUBIMAGE3DPROC		 )wglGetProcAddress( "glCopyTexSubImage3D" );

	// 1.3
	glActiveTexture =				( PFNGLACTIVETEXTUREPROC			 )wglGetProcAddress( "glActiveTexture" );
	glClientActiveTexture =			( PFNGLCLIENTACTIVETEXTUREPROC		 )wglGetProcAddress( "glClientActiveTexture" );
	glMultiTexCoord1d =				( PFNGLMULTITEXCOORD1DPROC			 )wglGetProcAddress( "glMultiTexCoord1d" );
	glMultiTexCoord1dv =			( PFNGLMULTITEXCOORD1DVPROC			 )wglGetProcAddress( "glMultiTexCoord1dv" );
	glMultiTexCoord1f =				( PFNGLMULTITEXCOORD1FPROC			 )wglGetProcAddress( "glMultiTexCoord1f" );
	glMultiTexCoord1fv =			( PFNGLMULTITEXCOORD1FVPROC			 )wglGetProcAddress( "glMultiTexCoord1fv" );
	glMultiTexCoord1i =				( PFNGLMULTITEXCOORD1IPROC			 )wglGetProcAddress( "glMultiTexCoord1i" );
	glMultiTexCoord1iv =			( PFNGLMULTITEXCOORD1IVPROC			 )wglGetProcAddress( "glMultiTexCoord1iv" );
	glMultiTexCoord1s =				( PFNGLMULTITEXCOORD1SPROC			 )wglGetProcAddress( "glMultiTexCoord1s" );
	glMultiTexCoord1sv =			( PFNGLMULTITEXCOORD1SVPROC			 )wglGetProcAddress( "glMultiTexCoord1sv" );
	glMultiTexCoord2d =				( PFNGLMULTITEXCOORD2DPROC			 )wglGetProcAddress( "glMultiTexCoord2d" );
	glMultiTexCoord2dv =			( PFNGLMULTITEXCOORD2DVPROC			 )wglGetProcAddress( "glMultiTexCoord2dv" );
	glMultiTexCoord2f =				( PFNGLMULTITEXCOORD2FPROC			 )wglGetProcAddress( "glMultiTexCoord2f" );
	glMultiTexCoord2fv =			( PFNGLMULTITEXCOORD2FVPROC			 )wglGetProcAddress( "glMultiTexCoord2fv" );
	glMultiTexCoord2i =				( PFNGLMULTITEXCOORD2IPROC			 )wglGetProcAddress( "glMultiTexCoord2i" );
	glMultiTexCoord2iv =			( PFNGLMULTITEXCOORD2IVPROC			 )wglGetProcAddress( "glMultiTexCoord2iv" );
	glMultiTexCoord2s =				( PFNGLMULTITEXCOORD2SPROC			 )wglGetProcAddress( "glMultiTexCoord2s" );
	glMultiTexCoord2sv =			( PFNGLMULTITEXCOORD2SVPROC			 )wglGetProcAddress( "glMultiTexCoord2sv" );
	glMultiTexCoord3d =				( PFNGLMULTITEXCOORD3DPROC			 )wglGetProcAddress( "glMultiTexCoord3d" );
	glMultiTexCoord3dv =			( PFNGLMULTITEXCOORD3DVPROC			 )wglGetProcAddress( "glMultiTexCoord3dv" );
	glMultiTexCoord3f =				( PFNGLMULTITEXCOORD3FPROC			 )wglGetProcAddress( "glMultiTexCoord3f" );
	glMultiTexCoord3fv =			( PFNGLMULTITEXCOORD3FVPROC			 )wglGetProcAddress( "glMultiTexCoord3fv" );
	glMultiTexCoord3i =				( PFNGLMULTITEXCOORD3IPROC			 )wglGetProcAddress( "glMultiTexCoord3i" );
	glMultiTexCoord3iv =			( PFNGLMULTITEXCOORD3IVPROC			 )wglGetProcAddress( "glMultiTexCoord3iv" );
	glMultiTexCoord3s =				( PFNGLMULTITEXCOORD3SPROC			 )wglGetProcAddress( "glMultiTexCoord3s" );
	glMultiTexCoord3sv =			( PFNGLMULTITEXCOORD3SVPROC			 )wglGetProcAddress( "glMultiTexCoord3sv" );
	glMultiTexCoord4d =				( PFNGLMULTITEXCOORD4DPROC			 )wglGetProcAddress( "glMultiTexCoord4d" );
	glMultiTexCoord4dv =			( PFNGLMULTITEXCOORD4DVPROC			 )wglGetProcAddress( "glMultiTexCoord4dv" );
	glMultiTexCoord4f =				( PFNGLMULTITEXCOORD4FPROC			 )wglGetProcAddress( "glMultiTexCoord4f" );
	glMultiTexCoord4fv =			( PFNGLMULTITEXCOORD4FVPROC			 )wglGetProcAddress( "glMultiTexCoord4fv" );
	glMultiTexCoord4i =				( PFNGLMULTITEXCOORD4IPROC			 )wglGetProcAddress( "glMultiTexCoord4i" );
	glMultiTexCoord4iv =			( PFNGLMULTITEXCOORD4IVPROC			 )wglGetProcAddress( "glMultiTexCoord4iv" );
	glMultiTexCoord4s =				( PFNGLMULTITEXCOORD4SPROC			 )wglGetProcAddress( "glMultiTexCoord4s" );
	glMultiTexCoord4sv =			( PFNGLMULTITEXCOORD4SVPROC			 )wglGetProcAddress( "glMultiTexCoord4sv" );
	glLoadTransposeMatrixf =		( PFNGLLOADTRANSPOSEMATRIXFPROC		 )wglGetProcAddress( "glLoadTransposeMatrixf" );
	glLoadTransposeMatrixd =		( PFNGLLOADTRANSPOSEMATRIXDPROC		 )wglGetProcAddress( "glLoadTransposeMatrixd" );
	glMultTransposeMatrixf =		( PFNGLMULTTRANSPOSEMATRIXFPROC		 )wglGetProcAddress( "glMultTransposeMatrixf" );
	glMultTransposeMatrixd =		( PFNGLMULTTRANSPOSEMATRIXDPROC		 )wglGetProcAddress( "glMultTransposeMatrixd" );
	glSampleCoverage =				( PFNGLSAMPLECOVERAGEPROC			 )wglGetProcAddress( "glSampleCoverage" );
	glCompressedTexImage3D =		( PFNGLCOMPRESSEDTEXIMAGE3DPROC		 )wglGetProcAddress( "glCompressedTexImage3D" );
	glCompressedTexImage2D =		( PFNGLCOMPRESSEDTEXIMAGE2DPROC		 )wglGetProcAddress( "glCompressedTexImage2D" );
	glCompressedTexImage1D =		( PFNGLCOMPRESSEDTEXIMAGE1DPROC		 )wglGetProcAddress( "glCompressedTexImage1D" );
	glCompressedTexSubImage3D =		( PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC	 )wglGetProcAddress( "glCompressedTexSubImage3D" );
	glCompressedTexSubImage2D =		( PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC	 )wglGetProcAddress( "glCompressedTexSubImage2D" );
	glCompressedTexSubImage1D =		( PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC	 )wglGetProcAddress( "glCompressedTexSubImage1D" );
	glGetCompressedTexImage =		( PFNGLGETCOMPRESSEDTEXIMAGEPROC	 )wglGetProcAddress( "glGetCompressedTexImage" );

	// 1.4
	glBlendFuncSeparate =			( PFNGLBLENDFUNCSEPARATEPROC		 )wglGetProcAddress( "glBlendFuncSeparate" );
	glFogCoordf =					( PFNGLFOGCOORDFPROC				 )wglGetProcAddress( "glFogCoordf" );
	glFogCoordfv =					( PFNGLFOGCOORDFVPROC				 )wglGetProcAddress( "glFogCoordfv" );
	glFogCoordd =					( PFNGLFOGCOORDDPROC				 )wglGetProcAddress( "glFogCoordd" );
	glFogCoorddv =					( PFNGLFOGCOORDDVPROC				 )wglGetProcAddress( "glFogCoorddv" );
	glFogCoordPointer =				( PFNGLFOGCOORDPOINTERPROC			 )wglGetProcAddress( "glFogCoordPointer" );
	glMultiDrawArrays =				( PFNGLMULTIDRAWARRAYSPROC			 )wglGetProcAddress( "glMultiDrawArrays" );
	glMultiDrawElements =			( PFNGLMULTIDRAWELEMENTSPROC		 )wglGetProcAddress( "glMultiDrawElements" );
	glPointParameterf =				( PFNGLPOINTPARAMETERFPROC			 )wglGetProcAddress( "glPointParameterf" );
	glPointParameterfv =			( PFNGLPOINTPARAMETERFVPROC			 )wglGetProcAddress( "glPointParameterfv" );
	glPointParameteri =				( PFNGLPOINTPARAMETERIPROC			 )wglGetProcAddress( "glPointParameteri" );
	glPointParameteriv =			( PFNGLPOINTPARAMETERIVPROC			 )wglGetProcAddress( "glPointParameteriv" );
	glSecondaryColor3b =			( PFNGLSECONDARYCOLOR3BPROC			 )wglGetProcAddress( "glSecondaryColor3b" );
	glSecondaryColor3bv =			( PFNGLSECONDARYCOLOR3BVPROC		 )wglGetProcAddress( "glSecondaryColor3bv" );
	glSecondaryColor3d =			( PFNGLSECONDARYCOLOR3DPROC			 )wglGetProcAddress( "glSecondaryColor3d" );
	glSecondaryColor3dv =			( PFNGLSECONDARYCOLOR3DVPROC		 )wglGetProcAddress( "glSecondaryColor3dv" );
	glSecondaryColor3f =			( PFNGLSECONDARYCOLOR3FPROC			 )wglGetProcAddress( "glSecondaryColor3f" );
	glSecondaryColor3fv =			( PFNGLSECONDARYCOLOR3FVPROC		 )wglGetProcAddress( "glSecondaryColor3fv" );
	glSecondaryColor3i =			( PFNGLSECONDARYCOLOR3IPROC			 )wglGetProcAddress( "glSecondaryColor3i" );
	glSecondaryColor3iv =			( PFNGLSECONDARYCOLOR3IVPROC		 )wglGetProcAddress( "glSecondaryColor3iv" );
	glSecondaryColor3s =			( PFNGLSECONDARYCOLOR3SPROC			 )wglGetProcAddress( "glSecondaryColor3s" );
	glSecondaryColor3sv =			( PFNGLSECONDARYCOLOR3SVPROC		 )wglGetProcAddress( "glSecondaryColor3sv" );
	glSecondaryColor3ub =			( PFNGLSECONDARYCOLOR3UBPROC		 )wglGetProcAddress( "glSecondaryColor3ub" );
	glSecondaryColor3ubv =			( PFNGLSECONDARYCOLOR3UBVPROC		 )wglGetProcAddress( "glSecondaryColor3ubv" );
	glSecondaryColor3ui =			( PFNGLSECONDARYCOLOR3UIPROC		 )wglGetProcAddress( "glSecondaryColor3ui" );
	glSecondaryColor3uiv =			( PFNGLSECONDARYCOLOR3UIVPROC		 )wglGetProcAddress( "glSecondaryColor3uiv" );
	glSecondaryColor3us =			( PFNGLSECONDARYCOLOR3USPROC		 )wglGetProcAddress( "glSecondaryColor3us" );
	glSecondaryColor3usv =			( PFNGLSECONDARYCOLOR3USVPROC		 )wglGetProcAddress( "glSecondaryColor3usv" );
	glSecondaryColorPointer =		( PFNGLSECONDARYCOLORPOINTERPROC	 )wglGetProcAddress( "glSecondaryColorPointer" );
	glWindowPos2d =					( PFNGLWINDOWPOS2DPROC				 )wglGetProcAddress( "glWindowPos2d" );
	glWindowPos2dv =				( PFNGLWINDOWPOS2DVPROC				 )wglGetProcAddress( "glWindowPos2dv" );
	glWindowPos2f =					( PFNGLWINDOWPOS2FPROC				 )wglGetProcAddress( "glWindowPos2f" );
	glWindowPos2fv =				( PFNGLWINDOWPOS2FVPROC				 )wglGetProcAddress( "glWindowPos2fv" );
	glWindowPos2i =					( PFNGLWINDOWPOS2IPROC				 )wglGetProcAddress( "glWindowPos2i" );
	glWindowPos2iv =				( PFNGLWINDOWPOS2IVPROC				 )wglGetProcAddress( "glWindowPos2iv" );
	glWindowPos2s =					( PFNGLWINDOWPOS2SPROC				 )wglGetProcAddress( "glWindowPos2s" );
	glWindowPos2sv =				( PFNGLWINDOWPOS2SVPROC				 )wglGetProcAddress( "glWindowPos2sv" );
	glWindowPos3d =					( PFNGLWINDOWPOS3DPROC				 )wglGetProcAddress( "glWindowPos3d" );
	glWindowPos3dv =				( PFNGLWINDOWPOS3DVPROC				 )wglGetProcAddress( "glWindowPos3dv" );
	glWindowPos3f =					( PFNGLWINDOWPOS3FPROC				 )wglGetProcAddress( "glWindowPos3f" );
	glWindowPos3fv =				( PFNGLWINDOWPOS3FVPROC				 )wglGetProcAddress( "glWindowPos3fv" );
	glWindowPos3i =					( PFNGLWINDOWPOS3IPROC				 )wglGetProcAddress( "glWindowPos3i" );
	glWindowPos3iv =				( PFNGLWINDOWPOS3IVPROC				 )wglGetProcAddress( "glWindowPos3iv" );
	glWindowPos3s =					( PFNGLWINDOWPOS3SPROC				 )wglGetProcAddress( "glWindowPos3s" );
	glWindowPos3sv =				( PFNGLWINDOWPOS3SVPROC				 )wglGetProcAddress( "glWindowPos3sv" );

	// 1.5
	glGenQueries =					( PFNGLGENQUERIESPROC				 )wglGetProcAddress( "glGenQueries" );
	glDeleteQueries =				( PFNGLDELETEQUERIESPROC			 )wglGetProcAddress( "glDeleteQueries" );
	glIsQuery =						( PFNGLISQUERYPROC					 )wglGetProcAddress( "glIsQuery" );
	glBeginQuery =					( PFNGLBEGINQUERYPROC				 )wglGetProcAddress( "glBeginQuery" );
	glEndQuery =					( PFNGLENDQUERYPROC					 )wglGetProcAddress( "glEndQuery" );
	glGetQueryiv =					( PFNGLGETQUERYIVPROC				 )wglGetProcAddress( "glGetQueryiv" );
	glGetQueryObjectiv =			( PFNGLGETQUERYOBJECTIVPROC			 )wglGetProcAddress( "glGetQueryObjectiv" );
	glGetQueryObjectuiv =			( PFNGLGETQUERYOBJECTUIVPROC		 )wglGetProcAddress( "glGetQueryObjectuiv" );
	glBindBuffer =					( PFNGLBINDBUFFERPROC				 )wglGetProcAddress( "glBindBuffer" );
	glDeleteBuffers =				( PFNGLDELETEBUFFERSPROC			 )wglGetProcAddress( "glDeleteBuffers" );
	glGenBuffers =					( PFNGLGENBUFFERSPROC				 )wglGetProcAddress( "glGenBuffers" );
	glIsBuffer =					( PFNGLISBUFFERPROC					 )wglGetProcAddress( "glIsBuffer" );
	glBufferData =					( PFNGLBUFFERDATAPROC				 )wglGetProcAddress( "glBufferData" );
	glBufferSubData =				( PFNGLBUFFERSUBDATAPROC			 )wglGetProcAddress( "glBufferSubData" );
	glGetBufferSubData =			( PFNGLGETBUFFERSUBDATAPROC			 )wglGetProcAddress( "glGetBufferSubData" );
	glMapBuffer =					( PFNGLMAPBUFFERPROC				 )wglGetProcAddress( "glMapBuffer" );
	glUnmapBuffer =					( PFNGLUNMAPBUFFERPROC				 )wglGetProcAddress( "glUnmapBuffer" );
	glGetBufferParameteriv =		( PFNGLGETBUFFERPARAMETERIVPROC		 )wglGetProcAddress( "glGetBufferParameteriv" );
	glGetBufferPointerv =			( PFNGLGETBUFFERPOINTERVPROC		 )wglGetProcAddress( "glGetBufferPointerv" );

	// 2.0
	glBlendEquationSeparate =		( PFNGLBLENDEQUATIONSEPARATEPROC	 )wglGetProcAddress( "glBlendEquationSeparate" );
	glDrawBuffers =					( PFNGLDRAWBUFFERSPROC				 )wglGetProcAddress( "glDrawBuffers" );
	glStencilOpSeparate =			( PFNGLSTENCILOPSEPARATEPROC		 )wglGetProcAddress( "glStencilOpSeparate" );
	glStencilFuncSeparate =			( PFNGLSTENCILFUNCSEPARATEPROC		 )wglGetProcAddress( "glStencilFuncSeparate" );
	glStencilMaskSeparate =			( PFNGLSTENCILMASKSEPARATEPROC		 )wglGetProcAddress( "glStencilMaskSeparate" );
	glAttachShader =				( PFNGLATTACHSHADERPROC				 )wglGetProcAddress( "glAttachShader" );
	glBindAttribLocation =			( PFNGLBINDATTRIBLOCATIONPROC		 )wglGetProcAddress( "glBindAttribLocation" );
	glCompileShader =				( PFNGLCOMPILESHADERPROC			 )wglGetProcAddress( "glCompileShader" );
	glCreateProgram =				( PFNGLCREATEPROGRAMPROC			 )wglGetProcAddress( "glCreateProgram" );
	glCreateShader =				( PFNGLCREATESHADERPROC				 )wglGetProcAddress( "glCreateShader" );
	glDeleteProgram =				( PFNGLDELETEPROGRAMPROC			 )wglGetProcAddress( "glDeleteProgram" );
	glDeleteShader =				( PFNGLDELETESHADERPROC				 )wglGetProcAddress( "glDeleteShader" );
	glDetachShader =				( PFNGLDETACHSHADERPROC				 )wglGetProcAddress( "glDetachShader" );
	glDisableVertexAttribArray =	( PFNGLDISABLEVERTEXATTRIBARRAYPROC	 )wglGetProcAddress( "glDisableVertexAttribArray" );
	glEnableVertexAttribArray =		( PFNGLENABLEVERTEXATTRIBARRAYPROC	 )wglGetProcAddress( "glEnableVertexAttribArray" );
	glGetActiveAttrib =				( PFNGLGETACTIVEATTRIBPROC			 )wglGetProcAddress( "glGetActiveAttrib" );
	glGetActiveUniform =			( PFNGLGETACTIVEUNIFORMPROC			 )wglGetProcAddress( "glGetActiveUniform" );
	glGetAttachedShaders =			( PFNGLGETATTACHEDSHADERSPROC		 )wglGetProcAddress( "glGetAttachedShaders" );
	glGetAttribLocation =			( PFNGLGETATTRIBLOCATIONPROC		 )wglGetProcAddress( "glGetAttribLocation" );
	glGetProgramiv =				( PFNGLGETPROGRAMIVPROC				 )wglGetProcAddress( "glGetProgramiv" );
	glGetProgramInfoLog =			( PFNGLGETPROGRAMINFOLOGPROC		 )wglGetProcAddress( "glGetProgramInfoLog" );
	glGetShaderiv =					( PFNGLGETSHADERIVPROC				 )wglGetProcAddress( "glGetShaderiv" );
	glGetShaderInfoLog =			( PFNGLGETSHADERINFOLOGPROC			 )wglGetProcAddress( "glGetShaderInfoLog" );
	glGetShaderSource =				( PFNGLGETSHADERSOURCEPROC			 )wglGetProcAddress( "glGetShaderSource" );
	glGetUniformLocation =			( PFNGLGETUNIFORMLOCATIONPROC		 )wglGetProcAddress( "glGetUniformLocation" );
	glGetUniformfv =				( PFNGLGETUNIFORMFVPROC				 )wglGetProcAddress( "glGetUniformfv" );
	glGetUniformiv =				( PFNGLGETUNIFORMIVPROC				 )wglGetProcAddress( "glGetUniformiv" );
	glGetVertexAttribdv =			( PFNGLGETVERTEXATTRIBDVPROC		 )wglGetProcAddress( "glGetVertexAttribdv" );
	glGetVertexAttribfv =			( PFNGLGETVERTEXATTRIBFVPROC		 )wglGetProcAddress( "glGetVertexAttribfv" );
	glGetVertexAttribiv =			( PFNGLGETVERTEXATTRIBIVPROC		 )wglGetProcAddress( "glGetVertexAttribiv" );
	glGetVertexAttribPointerv =		( PFNGLGETVERTEXATTRIBPOINTERVPROC	 )wglGetProcAddress( "glGetVertexAttribPointerv" );
	glIsProgram =					( PFNGLISPROGRAMPROC				 )wglGetProcAddress( "glIsProgram" );
	glIsShader =					( PFNGLISSHADERPROC					 )wglGetProcAddress( "glIsShader" );
	glLinkProgram =					( PFNGLLINKPROGRAMPROC				 )wglGetProcAddress( "glLinkProgram" );
	glShaderSource =				( PFNGLSHADERSOURCEPROC				 )wglGetProcAddress( "glShaderSource" );
	glUseProgram =					( PFNGLUSEPROGRAMPROC 				 )wglGetProcAddress( "glUseProgram" );
	glUniform1f =					( PFNGLUNIFORM1FPROC				 )wglGetProcAddress( "glUniform1f" );
	glUniform2f =					( PFNGLUNIFORM2FPROC				 )wglGetProcAddress( "glUniform2f" );
	glUniform3f =					( PFNGLUNIFORM3FPROC				 )wglGetProcAddress( "glUniform3f" );
	glUniform4f =					( PFNGLUNIFORM4FPROC				 )wglGetProcAddress( "glUniform4f" );
	glUniform1i =					( PFNGLUNIFORM1IPROC				 )wglGetProcAddress( "glUniform1i" );
	glUniform2i =					( PFNGLUNIFORM2IPROC				 )wglGetProcAddress( "glUniform2i" );
	glUniform3i =					( PFNGLUNIFORM3IPROC				 )wglGetProcAddress( "glUniform3i" );
	glUniform4i =					( PFNGLUNIFORM4IPROC				 )wglGetProcAddress( "glUniform4i" );
	glUniform1fv =					( PFNGLUNIFORM1FVPROC				 )wglGetProcAddress( "glUniform1fv" );
	glUniform2fv =					( PFNGLUNIFORM2FVPROC				 )wglGetProcAddress( "glUniform2fv" );
	glUniform3fv =					( PFNGLUNIFORM3FVPROC				 )wglGetProcAddress( "glUniform3fv" );
	glUniform4fv =					( PFNGLUNIFORM4FVPROC				 )wglGetProcAddress( "glUniform4fv" );
	glUniform1iv =					( PFNGLUNIFORM1IVPROC				 )wglGetProcAddress( "glUniform1iv" );
	glUniform2iv =					( PFNGLUNIFORM2IVPROC				 )wglGetProcAddress( "glUniform2iv" );
	glUniform3iv =					( PFNGLUNIFORM3IVPROC				 )wglGetProcAddress( "glUniform3iv" );
	glUniform4iv =					( PFNGLUNIFORM4IVPROC				 )wglGetProcAddress( "glUniform4iv" );
	glUniformMatrix2fv =			( PFNGLUNIFORMMATRIX2FVPROC			 )wglGetProcAddress( "glUniformMatrix2fv" );
	glUniformMatrix3fv =			( PFNGLUNIFORMMATRIX3FVPROC			 )wglGetProcAddress( "glUniformMatrix3fv" );
	glUniformMatrix4fv =			( PFNGLUNIFORMMATRIX4FVPROC			 )wglGetProcAddress( "glUniformMatrix4fv" );
	glValidateProgram =				( PFNGLVALIDATEPROGRAMPROC			 )wglGetProcAddress( "glValidateProgram" );
	glVertexAttrib1d =				( PFNGLVERTEXATTRIB1DPROC			 )wglGetProcAddress( "glVertexAttrib1d" );
	glVertexAttrib1dv =				( PFNGLVERTEXATTRIB1DVPROC			 )wglGetProcAddress( "glVertexAttrib1dv" );
	glVertexAttrib1f =				( PFNGLVERTEXATTRIB1FPROC			 )wglGetProcAddress( "glVertexAttrib1f" );
	glVertexAttrib1fv =				( PFNGLVERTEXATTRIB1FVPROC			 )wglGetProcAddress( "glVertexAttrib1fv" );
	glVertexAttrib1s =				( PFNGLVERTEXATTRIB1SPROC			 )wglGetProcAddress( "glVertexAttrib1s" );
	glVertexAttrib1sv =				( PFNGLVERTEXATTRIB1SVPROC			 )wglGetProcAddress( "glVertexAttrib1sv" );
	glVertexAttrib2d =				( PFNGLVERTEXATTRIB2DPROC			 )wglGetProcAddress( "glVertexAttrib2d" );
	glVertexAttrib2dv =				( PFNGLVERTEXATTRIB2DVPROC			 )wglGetProcAddress( "glVertexAttrib2dv" );
	glVertexAttrib2f =				( PFNGLVERTEXATTRIB2FPROC			 )wglGetProcAddress( "glVertexAttrib2f" );
	glVertexAttrib2fv =				( PFNGLVERTEXATTRIB2FVPROC			 )wglGetProcAddress( "glVertexAttrib2fv" );
	glVertexAttrib2s =				( PFNGLVERTEXATTRIB2SPROC			 )wglGetProcAddress( "glVertexAttrib2s" );
	glVertexAttrib2sv =				( PFNGLVERTEXATTRIB2SVPROC			 )wglGetProcAddress( "glVertexAttrib2sv" );
	glVertexAttrib3d =				( PFNGLVERTEXATTRIB3DPROC			 )wglGetProcAddress( "glVertexAttrib3d" );
	glVertexAttrib3dv =				( PFNGLVERTEXATTRIB3DVPROC			 )wglGetProcAddress( "glVertexAttrib3dv" );
	glVertexAttrib3f =				( PFNGLVERTEXATTRIB3FPROC			 )wglGetProcAddress( "glVertexAttrib3f" );
	glVertexAttrib3fv =				( PFNGLVERTEXATTRIB3FVPROC			 )wglGetProcAddress( "glVertexAttrib3fv" );
	glVertexAttrib3s =				( PFNGLVERTEXATTRIB3SPROC			 )wglGetProcAddress( "glVertexAttrib3s" );
	glVertexAttrib3sv =				( PFNGLVERTEXATTRIB3SVPROC			 )wglGetProcAddress( "glVertexAttrib3sv" );
	glVertexAttrib4Nbv =			( PFNGLVERTEXATTRIB4NBVPROC			 )wglGetProcAddress( "glVertexAttrib4Nbv" );
	glVertexAttrib4Niv =			( PFNGLVERTEXATTRIB4NIVPROC			 )wglGetProcAddress( "glVertexAttrib4Niv" );
	glVertexAttrib4Nsv =			( PFNGLVERTEXATTRIB4NSVPROC			 )wglGetProcAddress( "glVertexAttrib4Nsv" );
	glVertexAttrib4Nub =			( PFNGLVERTEXATTRIB4NUBPROC			 )wglGetProcAddress( "glVertexAttrib4Nub" );
	glVertexAttrib4Nubv =			( PFNGLVERTEXATTRIB4NUBVPROC		 )wglGetProcAddress( "glVertexAttrib4Nubv" );
	glVertexAttrib4Nuiv =			( PFNGLVERTEXATTRIB4NUIVPROC		 )wglGetProcAddress( "glVertexAttrib4Nuiv" );
	glVertexAttrib4Nusv =			( PFNGLVERTEXATTRIB4NUSVPROC		 )wglGetProcAddress( "glVertexAttrib4Nusv" );
	glVertexAttrib4bv =				( PFNGLVERTEXATTRIB4BVPROC			 )wglGetProcAddress( "glVertexAttrib4bv" );
	glVertexAttrib4d =				( PFNGLVERTEXATTRIB4DPROC			 )wglGetProcAddress( "glVertexAttrib4d" );
	glVertexAttrib4dv =				( PFNGLVERTEXATTRIB4DVPROC			 )wglGetProcAddress( "glVertexAttrib4dv" );
	glVertexAttrib4f =				( PFNGLVERTEXATTRIB4FPROC			 )wglGetProcAddress( "glVertexAttrib4f" );
	glVertexAttrib4fv =				( PFNGLVERTEXATTRIB4FVPROC			 )wglGetProcAddress( "glVertexAttrib4fv" );
	glVertexAttrib4iv =				( PFNGLVERTEXATTRIB4IVPROC			 )wglGetProcAddress( "glVertexAttrib4iv" );
	glVertexAttrib4s =				( PFNGLVERTEXATTRIB4SPROC			 )wglGetProcAddress( "glVertexAttrib4s" );
	glVertexAttrib4sv =				( PFNGLVERTEXATTRIB4SVPROC			 )wglGetProcAddress( "glVertexAttrib4sv" );
	glVertexAttrib4ubv =			( PFNGLVERTEXATTRIB4UBVPROC			 )wglGetProcAddress( "glVertexAttrib4ubv" );
	glVertexAttrib4uiv =			( PFNGLVERTEXATTRIB4UIVPROC			 )wglGetProcAddress( "glVertexAttrib4uiv" );
	glVertexAttrib4usv =			( PFNGLVERTEXATTRIB4USVPROC			 )wglGetProcAddress( "glVertexAttrib4usv" );
	glVertexAttribPointer =			( PFNGLVERTEXATTRIBPOINTERPROC		 )wglGetProcAddress( "glVertexAttribPointer" );

	// 2.1
	glUniformMatrix2x3fv =			( PFNGLUNIFORMMATRIX2X3FVPROC		 )wglGetProcAddress( "glUniformMatrix2x3fv" );
	glUniformMatrix3x2fv =			( PFNGLUNIFORMMATRIX3X2FVPROC		 )wglGetProcAddress( "glUniformMatrix3x2fv" );
	glUniformMatrix2x4fv =			( PFNGLUNIFORMMATRIX2X4FVPROC		 )wglGetProcAddress( "glUniformMatrix2x4fv" );
	glUniformMatrix4x2fv =			( PFNGLUNIFORMMATRIX4X2FVPROC		 )wglGetProcAddress( "glUniformMatrix4x2fv" );
	glUniformMatrix3x4fv =			( PFNGLUNIFORMMATRIX3X4FVPROC		 )wglGetProcAddress( "glUniformMatrix3x4fv" );
	glUniformMatrix4x3fv =			( PFNGLUNIFORMMATRIX4X3FVPROC		 )wglGetProcAddress( "glUniformMatrix4x3fv" );

	// GL_ARB_shader_objects
	glDeleteObjectARB =				( PFNGLDELETEOBJECTARBPROC			 )wglGetProcAddress( "glDeleteObjectARB" );
	glGetHandleARB =				( PFNGLGETHANDLEARBPROC				 )wglGetProcAddress( "glGetHandleARB" );
	glDetachObjectARB =				( PFNGLDETACHOBJECTARBPROC			 )wglGetProcAddress( "glDetachObjectARB" );
	glCreateShaderObjectARB =		( PFNGLCREATESHADEROBJECTARBPROC	 )wglGetProcAddress( "glCreateShaderObjectARB" );
	glShaderSourceARB =				( PFNGLSHADERSOURCEARBPROC			 )wglGetProcAddress( "glShaderSourceARB" );
	glCompileShaderARB =			( PFNGLCOMPILESHADERARBPROC			 )wglGetProcAddress( "glCompileShaderARB" );
	glCreateProgramObjectARB =		( PFNGLCREATEPROGRAMOBJECTARBPROC	 )wglGetProcAddress( "glCreateProgramObjectARB" );
	glAttachObjectARB =				( PFNGLATTACHOBJECTARBPROC			 )wglGetProcAddress( "glAttachObjectARB" );
	glLinkProgramARB =				( PFNGLLINKPROGRAMARBPROC			 )wglGetProcAddress( "glLinkProgramARB" );
	glUseProgramObjectARB =			( PFNGLUSEPROGRAMOBJECTARBPROC		 )wglGetProcAddress( "glUseProgramObjectARB" );
	glValidateProgramARB =			( PFNGLVALIDATEPROGRAMARBPROC		 )wglGetProcAddress( "glValidateProgramARB" );
	glUniform1fARB =				( PFNGLUNIFORM1FARBPROC				 )wglGetProcAddress( "glUniform1fARB" );
	glUniform2fARB =				( PFNGLUNIFORM2FARBPROC				 )wglGetProcAddress( "glUniform2fARB" );
	glUniform3fARB =				( PFNGLUNIFORM3FARBPROC				 )wglGetProcAddress( "glUniform3fARB" );
	glUniform4fARB =				( PFNGLUNIFORM4FARBPROC				 )wglGetProcAddress( "glUniform4fARB" );
	glUniform1iARB =				( PFNGLUNIFORM1IARBPROC				 )wglGetProcAddress( "glUniform1iARB" );
	glUniform2iARB =				( PFNGLUNIFORM2IARBPROC				 )wglGetProcAddress( "glUniform2iARB" );
	glUniform3iARB =				( PFNGLUNIFORM3IARBPROC				 )wglGetProcAddress( "glUniform3iARB" );
	glUniform4iARB =				( PFNGLUNIFORM4IARBPROC				 )wglGetProcAddress( "glUniform4iARB" );
	glUniform1fvARB =				( PFNGLUNIFORM1FVARBPROC			 )wglGetProcAddress( "glUniform1fvARB" );
	glUniform2fvARB =				( PFNGLUNIFORM2FVARBPROC			 )wglGetProcAddress( "glUniform2fvARB" );
	glUniform3fvARB =				( PFNGLUNIFORM3FVARBPROC			 )wglGetProcAddress( "glUniform3fvARB" );
	glUniform4fvARB =				( PFNGLUNIFORM4FVARBPROC			 )wglGetProcAddress( "glUniform4fvARB" );
	glUniform1ivARB =				( PFNGLUNIFORM1IVARBPROC			 )wglGetProcAddress( "glUniform1ivARB" );
	glUniform2ivARB =				( PFNGLUNIFORM2IVARBPROC			 )wglGetProcAddress( "glUniform2ivARB" );
	glUniform3ivARB =				( PFNGLUNIFORM3IVARBPROC			 )wglGetProcAddress( "glUniform3ivARB" );
	glUniform4ivARB =				( PFNGLUNIFORM4IVARBPROC			 )wglGetProcAddress( "glUniform4ivARB" );
	glUniformMatrix2fvARB =			( PFNGLUNIFORMMATRIX2FVARBPROC		 )wglGetProcAddress( "glUniformMatrix2fvARB" );
	glUniformMatrix3fvARB =			( PFNGLUNIFORMMATRIX3FVARBPROC		 )wglGetProcAddress( "glUniformMatrix3fvARB" );
	glUniformMatrix4fvARB =			( PFNGLUNIFORMMATRIX4FVARBPROC		 )wglGetProcAddress( "glUniformMatrix4fvARB" );
	glGetObjectParameterfvARB =		( PFNGLGETOBJECTPARAMETERFVARBPROC	 )wglGetProcAddress( "glGetObjectParameterfvARB" );
	glGetObjectParameterivARB =		( PFNGLGETOBJECTPARAMETERIVARBPROC	 )wglGetProcAddress( "glGetObjectParameterivARB" );
	glGetInfoLogARB =				( PFNGLGETINFOLOGARBPROC			 )wglGetProcAddress( "glGetInfoLogARB" );
	glGetAttachedObjectsARB =		( PFNGLGETATTACHEDOBJECTSARBPROC	 )wglGetProcAddress( "glGetAttachedObjectsARB" );
	glGetUniformLocationARB =		( PFNGLGETUNIFORMLOCATIONARBPROC	 )wglGetProcAddress( "glGetUniformLocationARB" );
	glGetActiveUniformARB =			( PFNGLGETACTIVEUNIFORMARBPROC		 )wglGetProcAddress( "glGetActiveUniformARB" );
	glGetUniformfvARB =				( PFNGLGETUNIFORMFVARBPROC			 )wglGetProcAddress( "glGetUniformfvARB" );
	glGetUniformivARB =				( PFNGLGETUNIFORMIVARBPROC			 )wglGetProcAddress( "glGetUniformivARB" );
	glGetShaderSourceARB =			( PFNGLGETSHADERSOURCEARBPROC		 )wglGetProcAddress( "glGetShaderSourceARB" );

	// ATI_draw_buffers
	glDrawBuffersARB =				( PFNGLDRAWBUFFERSARBPROC			 )wglGetProcAddress( "glDrawBuffersARB" );
	
	// GL_ARB_vertex_shader
	glVertexAttrib1dARB =			( PFNGLVERTEXATTRIB1DARBPROC		 )wglGetProcAddress( "glVertexAttrib1dARB" );
	glVertexAttrib1dvARB =			( PFNGLVERTEXATTRIB1DVARBPROC		 )wglGetProcAddress( "glVertexAttrib1dvARB" );
	glVertexAttrib1fARB =			( PFNGLVERTEXATTRIB1FARBPROC		 )wglGetProcAddress( "glVertexAttrib1fARB" );
	glVertexAttrib1fvARB =			( PFNGLVERTEXATTRIB1FVARBPROC		 )wglGetProcAddress( "glVertexAttrib1fvARB" );
	glVertexAttrib1sARB =			( PFNGLVERTEXATTRIB1SARBPROC		 )wglGetProcAddress( "glVertexAttrib1sARB" );
	glVertexAttrib1svARB =			( PFNGLVERTEXATTRIB1SVARBPROC		 )wglGetProcAddress( "glVertexAttrib1svARB" );
	glVertexAttrib2dARB =			( PFNGLVERTEXATTRIB2DARBPROC		 )wglGetProcAddress( "glVertexAttrib2dARB" );
	glVertexAttrib2dvARB =			( PFNGLVERTEXATTRIB2DVARBPROC		 )wglGetProcAddress( "glVertexAttrib2dvARB" );
	glVertexAttrib2fARB =			( PFNGLVERTEXATTRIB2FARBPROC		 )wglGetProcAddress( "glVertexAttrib2fARB" );
	glVertexAttrib2fvARB =			( PFNGLVERTEXATTRIB2FVARBPROC		 )wglGetProcAddress( "glVertexAttrib2fvARB" );
	glVertexAttrib2sARB =			( PFNGLVERTEXATTRIB2SARBPROC		 )wglGetProcAddress( "glVertexAttrib2sARB" );
	glVertexAttrib2svARB =			( PFNGLVERTEXATTRIB2SVARBPROC		 )wglGetProcAddress( "glVertexAttrib2svARB" );
	glVertexAttrib3dARB =			( PFNGLVERTEXATTRIB3DARBPROC		 )wglGetProcAddress( "glVertexAttrib3dARB" );
	glVertexAttrib3dvARB =			( PFNGLVERTEXATTRIB3DVARBPROC		 )wglGetProcAddress( "glVertexAttrib3dvARB" );
	glVertexAttrib3fARB =			( PFNGLVERTEXATTRIB3FARBPROC		 )wglGetProcAddress( "glVertexAttrib3fARB" );
	glVertexAttrib3fvARB =			( PFNGLVERTEXATTRIB3FVARBPROC		 )wglGetProcAddress( "glVertexAttrib3fvARB" );
	glVertexAttrib3sARB =			( PFNGLVERTEXATTRIB3SARBPROC		 )wglGetProcAddress( "glVertexAttrib3sARB" );
	glVertexAttrib3svARB =			( PFNGLVERTEXATTRIB3SVARBPROC		 )wglGetProcAddress( "glVertexAttrib3svARB" );
	glVertexAttrib4NbvARB =			( PFNGLVERTEXATTRIB4NBVARBPROC		 )wglGetProcAddress( "glVertexAttrib4NbvARB" );
	glVertexAttrib4NivARB =			( PFNGLVERTEXATTRIB4NIVARBPROC		 )wglGetProcAddress( "glVertexAttrib4NivARB" );
	glVertexAttrib4NsvARB =			( PFNGLVERTEXATTRIB4NSVARBPROC		 )wglGetProcAddress( "glVertexAttrib4NsvARB" );
	glVertexAttrib4NubARB =			( PFNGLVERTEXATTRIB4NUBARBPROC		 )wglGetProcAddress( "glVertexAttrib4NubARB" );
	glVertexAttrib4NubvARB =		( PFNGLVERTEXATTRIB4NUBVARBPROC		 )wglGetProcAddress( "glVertexAttrib4NubvARB" );
	glVertexAttrib4NuivARB =		( PFNGLVERTEXATTRIB4NUIVARBPROC		 )wglGetProcAddress( "glVertexAttrib4NuivARB" );
	glVertexAttrib4NusvARB =		( PFNGLVERTEXATTRIB4NUSVARBPROC		 )wglGetProcAddress( "glVertexAttrib4NusvARB" );
	glVertexAttrib4bvARB =			( PFNGLVERTEXATTRIB4BVARBPROC		 )wglGetProcAddress( "glVertexAttrib4bvARB" );
	glVertexAttrib4dARB =			( PFNGLVERTEXATTRIB4DARBPROC		 )wglGetProcAddress( "glVertexAttrib4dARB" );
	glVertexAttrib4dvARB =			( PFNGLVERTEXATTRIB4DVARBPROC		 )wglGetProcAddress( "glVertexAttrib4dvARB" );
	glVertexAttrib4fARB =			( PFNGLVERTEXATTRIB4FARBPROC		 )wglGetProcAddress( "glVertexAttrib4fARB" );
	glVertexAttrib4fvARB =			( PFNGLVERTEXATTRIB4FVARBPROC		 )wglGetProcAddress( "glVertexAttrib4fvARB" );
	glVertexAttrib4ivARB =			( PFNGLVERTEXATTRIB4IVARBPROC		 )wglGetProcAddress( "glVertexAttrib4ivARB" );
	glVertexAttrib4sARB =			( PFNGLVERTEXATTRIB4SARBPROC		 )wglGetProcAddress( "glVertexAttrib4sARB" );
	glVertexAttrib4svARB =			( PFNGLVERTEXATTRIB4SVARBPROC		 )wglGetProcAddress( "glVertexAttrib4svARB" );
	glVertexAttrib4ubvARB =			( PFNGLVERTEXATTRIB4UBVARBPROC		 )wglGetProcAddress( "glVertexAttrib4ubvARB" );
	glVertexAttrib4uivARB =			( PFNGLVERTEXATTRIB4UIVARBPROC		 )wglGetProcAddress( "glVertexAttrib4uivARB" );
	glVertexAttrib4usvARB =			( PFNGLVERTEXATTRIB4USVARBPROC		 )wglGetProcAddress( "glVertexAttrib4usvARB" );
	glVertexAttribPointerARB =		( PFNGLVERTEXATTRIBPOINTERARBPROC	 )wglGetProcAddress( "glVertexAttribPointerARB" );
	glEnableVertexAttribArrayARB =	( PFNGLENABLEVERTEXATTRIBARRAYARBPROC	 )wglGetProcAddress( "glEnableVertexAttribArrayARB" );
	glDisableVertexAttribArrayARB =	( PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	 )wglGetProcAddress( "glDisableVertexAttribArrayARB" );
	glGetVertexAttribdvARB =		( PFNGLGETVERTEXATTRIBDVARBPROC		 )wglGetProcAddress( "glGetVertexAttribdvARB" );
	glGetVertexAttribfvARB =		( PFNGLGETVERTEXATTRIBFVARBPROC		 )wglGetProcAddress( "glGetVertexAttribfvARB" );
	glGetVertexAttribivARB =		( PFNGLGETVERTEXATTRIBIVARBPROC		 )wglGetProcAddress( "glGetVertexAttribivARB" );
	glGetVertexAttribPointervARB =	( PFNGLGETVERTEXATTRIBPOINTERVARBPROC	 )wglGetProcAddress( "glGetVertexAttribPointervARB" );
	glBindAttribLocationARB =		( PFNGLBINDATTRIBLOCATIONARBPROC	 )wglGetProcAddress( "glBindAttribLocationARB" );
	glGetActiveAttribARB =			( PFNGLGETACTIVEATTRIBARBPROC		 )wglGetProcAddress( "glGetActiveAttribARB" );
	glGetAttribLocationARB =		( PFNGLGETATTRIBLOCATIONARBPROC		 )wglGetProcAddress( "glGetAttribLocationARB" );

	// GL_ARB_vertex/fragment_program
	glBindProgramARB =				( PFNGLBINDPROGRAMARBPROC			 )wglGetProcAddress( "glBindProgramARB" );
	glDeleteProgramsARB =			( PFNGLDELETEPROGRAMSARBPROC		 )wglGetProcAddress( "glDeleteProgramsARB" );
	glGenProgramsARB =				( PFNGLGENPROGRAMSARBPROC			 )wglGetProcAddress( "glGenProgramsARB" );
	glProgramEnvParameter4fARB =	( PFNGLPROGRAMENVPARAMETER4FARBPROC )wglGetProcAddress( "glProgramEnvParameter4fARB" );
	glProgramEnvParameter4fvARB =	( PFNGLPROGRAMENVPARAMETER4FVARBPROC )wglGetProcAddress( "glProgramEnvParameter4fvARB" );
	glProgramLocalParameter4fvARB = ( PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	 )wglGetProcAddress( "glProgramLocalParameter4fvARB" );
	glProgramStringARB =			( PFNGLPROGRAMSTRINGARBPROC			 )wglGetProcAddress( "glProgramStringARB" );

	// GL_ARB_multisample
	glSampleCoverageARB	=			( PFNGLSAMPLECOVERAGEARBPROC		 )wglGetProcAddress( "glSampleCoverageARB" );

	// GL_ARB_color_buffer_float
//	glClampColorARB =				( PFNGLCLAMPCOLORARBPROC			 )wglGetProcAddress( "glClampColorARB" );

	// GL_EXT_framebuffer_object
	glIsRenderbufferEXT =			( PFNGLISRENDERBUFFEREXTPROC		 )wglGetProcAddress( "glIsRenderbufferEXT" );
	glBindRenderbufferEXT =			( PFNGLBINDRENDERBUFFEREXTPROC		 )wglGetProcAddress( "glBindRenderbufferEXT" );
	glDeleteRenderbuffersEXT =		( PFNGLDELETERENDERBUFFERSEXTPROC	 )wglGetProcAddress( "glDeleteRenderbuffersEXT" );
	glGenRenderbuffersEXT =			( PFNGLGENRENDERBUFFERSEXTPROC		 )wglGetProcAddress( "glGenRenderbuffersEXT" );
	glRenderbufferStorageEXT =		( PFNGLRENDERBUFFERSTORAGEEXTPROC	 )wglGetProcAddress( "glRenderbufferStorageEXT" );
	glGetRenderbufferParameterivEXT = ( PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	 )wglGetProcAddress( "glGetRenderbufferParameterivEXT" );
	glIsFramebufferEXT =			( PFNGLISFRAMEBUFFEREXTPROC			 )wglGetProcAddress( "glIsFramebufferEXT" );
	glBindFramebufferEXT =			( PFNGLBINDFRAMEBUFFEREXTPROC		)wglGetProcAddress( "glBindFramebufferEXT" );
	glDeleteFramebuffersEXT =		( PFNGLDELETEFRAMEBUFFERSEXTPROC	)wglGetProcAddress( "glDeleteFramebuffersEXT" );
	glGenFramebuffersEXT =			( PFNGLGENFRAMEBUFFERSEXTPROC		)wglGetProcAddress( "glGenFramebuffersEXT" );
	glCheckFramebufferStatusEXT =	( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress( "glCheckFramebufferStatusEXT" );
	glFramebufferTexture1DEXT =		( PFNGLFRAMEBUFFERTEXTURE1DEXTPROC	)wglGetProcAddress( "glFramebufferTexture1DEXT" );
	glFramebufferTexture2DEXT =		( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC	)wglGetProcAddress( "glFramebufferTexture2DEXT" );
	glFramebufferTexture3DEXT =		( PFNGLFRAMEBUFFERTEXTURE3DEXTPROC	)wglGetProcAddress( "glFramebufferTexture3DEXT" );
	glFramebufferRenderbufferEXT =	( PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC	 )wglGetProcAddress( "glFramebufferRenderbufferEXT" );
	glGetFramebufferAttachmentParameterivEXT = ( PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC	 )wglGetProcAddress( "glGetFramebufferAttachmentParameterivEXT" );
	glGenerateMipmapEXT =			( PFNGLGENERATEMIPMAPEXTPROC		)wglGetProcAddress( "glGenerateMipmapEXT" );

	// GL_EXT_depth_bounds_test
	glDepthBoundsEXT =				( PFNGLDEPTHBOUNDSEXTPROC			)wglGetProcAddress( "glDepthBoundsEXT" );

	//---------------------------------------------------------
	// Optional extensions
	//---------------------------------------------------------
	// GL_NV_primitive_restart
	if ( strstr( glExtString, "GL_NV_primitive_restart" ) )
	{
		glPrimitiveRestartNV =			( PFNGLPRIMITIVERESTARTNVPROC		 )wglGetProcAddress( "glPrimitiveRestartNV" );
		glPrimitiveRestartIndexNV = 	( PFNGLPRIMITIVERESTARTINDEXNVPROC	 )wglGetProcAddress( "glPrimitiveRestartIndexNV" );

		// turn on primitive restarts.
		glEnableClientState( GL_PRIMITIVE_RESTART_NV );
		glPrimitiveRestartIndexNV( 0xFFFF );
	}

    // GL_NV_fence
    if ( strstr( glExtString, "GL_NV_fence" ) )
    {
        glGenFencesNV =                 ( PFNGLGENFENCESNVPROC )   wglGetProcAddress( "glGenFencesNV" );
        glDeleteFencesNV =              ( PFNGLDELETEFENCESNVPROC )wglGetProcAddress( "glDeleteFencesNV" );
        glSetFenceNV =                  ( PFNGLSETFENCENVPROC )    wglGetProcAddress( "glSetFenceNV" );
        glTestFenceNV =                 ( PFNGLTESTFENCENVPROC )   wglGetProcAddress( "glTestFenceNV" );
        glFinishFenceNV =               ( PFNGLFINISHFENCENVPROC ) wglGetProcAddress( "glFinishFenceNV" );
        glIsFenceNV =                   ( PFNGLISFENCENVPROC )     wglGetProcAddress( "glIsFenceNV" );
        glGetFenceivNV =                ( PFNGLGETFENCEIVNVPROC )  wglGetProcAddress( "glGetFenceivNV" );
    }

	//---------------------------------------------------------
	// Windows specific extensions
	//---------------------------------------------------------
	// WGL_ARB_pixel_format
	wglGetPixelFormatAttribivARB =	( PFNWGLGETPIXELFORMATATTRIBIVARBPROC	 )wglGetProcAddress( "wglGetPixelFormatAttribivARB" );
	wglGetPixelFormatAttribfvARB =	( PFNWGLGETPIXELFORMATATTRIBFVARBPROC	 )wglGetProcAddress( "wglGetPixelFormatAttribfvARB" );
	wglChoosePixelFormatARB =		( PFNWGLCHOOSEPIXELFORMATARBPROC	 )wglGetProcAddress( "wglChoosePixelFormatARB" );

	// WGL_ARB_pbuffer
	wglCreatePbufferARB =			( PFNWGLCREATEPBUFFERARBPROC		 )wglGetProcAddress( "wglCreatePbufferARB" );
	wglGetPbufferDCARB =			( PFNWGLGETPBUFFERDCARBPROC			 )wglGetProcAddress( "wglGetPbufferDCARB" );
	wglReleasePbufferDCARB =		( PFNWGLRELEASEPBUFFERDCARBPROC		 )wglGetProcAddress( "wglReleasePbufferDCARB" );
	wglDestroyPbufferARB =			( PFNWGLDESTROYPBUFFERARBPROC		 )wglGetProcAddress( "wglDestroyPbufferARB" );
	wglQueryPbufferARB =			( PFNWGLQUERYPBUFFERARBPROC			 )wglGetProcAddress( "wglQueryPbufferARB" );

	// WGL_ARB_render_texture
	wglBindTexImageARB =			( PFNWGLBINDTEXIMAGEARBPROC			 )wglGetProcAddress( "wglBindTexImageARB" );
	wglReleaseTexImageARB =			( PFNWGLRELEASETEXIMAGEARBPROC		 )wglGetProcAddress( "wglReleaseTexImageARB" );
	wglSetPbufferAttribARB =		( PFNWGLSETPBUFFERATTRIBARBPROC		 )wglGetProcAddress( "wglSetPbufferAttribARB" );

	// setup GL hints and all of that shizzle.
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint( GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST );
	glHint( GL_TEXTURE_COMPRESSION_HINT_ARB, GL_NICEST );
	CHECK_GL();

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	CHECK_GL();

	// make sure color clamping is disabled.
//	if ( glClampColorARB )
//	{
//		glClampColorARB( GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE );
//		glClampColorARB( GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE );
//		glClampColorARB( GL_CLAMP_READ_COLOR_ARB, GL_FALSE );
//		CHECK_GL();
//	}
}

//----------------------------------------------------------
void
StopGL()
{
	// restore the initial gamma ramp.
	SetGLGamma( _deviceGammaTable, _deviceGammaTable + 256, _deviceGammaTable + 512 );
	_deviceGammaSupported = false;

	free( ( void* )glFormattedDriverString );
	glFormattedDriverString = 0;
}

//----------------------------------------------------------
EGLPROFILE
GetGLProfile()
{
	return profile;
}

//----------------------------------------------------------
bool
IsGLGammaSupported()
{
	return _deviceGammaSupported;
}

//----------------------------------------------------------
void
SetGLGamma( float gamma )
{
	if ( !_deviceGammaSupported )
		return;

	// build a gamma table.
	unsigned short gammaTable[ 256 ];
	unsigned short* cur = gammaTable;

	// build the gamma table for each color component.
	int curColor = 0;
	float invGamma = 1.0f / gamma;
	for ( unsigned int j = 0; j < 256; ++j, ++cur )
	{
		// get the current color.
		curColor = ( unsigned int )( 65535.0f * Pow( j / 255.0f, invGamma ) + 0.5f );

		// clamp and store.
		*cur = ( unsigned short )Clamp( curColor, 0, 65535 );
	}

	// set the gamma table.
	SetGLGamma( gammaTable, gammaTable, gammaTable );
}

//----------------------------------------------------------
void
SetGLGamma( unsigned short* red, unsigned short* green, unsigned short* blue )
{
	if ( !_deviceGammaSupported )
		return;

	PrintF( "Setting the gamma ramp...\n" );

	// coalesce to a single list.
	unsigned short gammaTable[ 768 ];
	unsigned short* cur = gammaTable;

	// copy each list.
	for ( unsigned int i = 0; i < 256; ++i, ++cur )
		*cur = red[ i ];

	// copy each list.
	for ( unsigned int i = 0; i < 256; ++i, ++cur )
		*cur = green[ i ];

	// copy each list.
	for ( unsigned int i = 0; i < 256; ++i, ++cur )
		*cur = blue[ i ];

	// update the device's gamma.
	HDC deviceDC = wglGetCurrentDC();
	BOOL ret = SetDeviceGammaRamp( deviceDC, gammaTable );
	if ( !ret )
	{
		PrintF( "Gamma ramp change failed.\n" );
	}
	else
	{
		PrintF( "Gamma ramp change succeeded.\n" );
	}
}

//----------------------------------------------------------
bool
IsExtSupported( const char* ext )
{
	return ( strstr( glExtString, ext ) != 0 );
}

//----------------------------------------------------------
void
_DbgCheckGL( const char *file, long line )
{
	static GLenum err;
	err = glGetError();
	if ( err != 0 )
	{
		// turn the line number into a string.
		char lineBuf[ 20 ];
		MemSet( lineBuf, 0, sizeof( lineBuf ) );
		_ltoa_s( line, lineBuf, 20, 10 );

		// compile a more complete error message.
		const GLubyte *str = gluErrorString( err );
		tstring errorMsg = "OpenGL Error occured in file \"";
		errorMsg += file;
		errorMsg += ", line ";
		errorMsg += lineBuf;
		errorMsg += ": ";
		errorMsg += ( str != 0 ? ( char * )str : "No OpenGL error given!" );
//		MessageBox( NULL, errorMsg.c_str(), "OpenGL error...", MB_OK | MB_ICONERROR );

		// assert and then throw an error.
		B_ERROR( ( const char * )errorMsg.c_str() );
	}
}

//----------------------------------------------------------
#ifdef _MONITOR_GL_STATS
void
ClearGLStats()
{
	_bglDrawCount = 0;
	_bglDrawRangeCount = 0;
	_bglBindTextureCount = 0;
	_bglSetUniformCount = 0;
	_bglBindBufferCount = 0;
}
#endif

//==========================================================
// wrapper functions.
//==========================================================

const char* bwglGetExtensionsStringARB( HDC device )
{
	return ( const char* )wglGetExtensionsStringARB( device );
}

BOOL bwglSwapIntervalEXT( int interval )
{
	if ( wglSwapIntervalEXT )
		return wglSwapIntervalEXT( interval );
	return TRUE;
}

int bwglGetSwapIntervalEXT()
{
	if ( wglGetSwapIntervalEXT )
		return wglGetSwapIntervalEXT();
	return 1;
}

void bglAccum( GLenum op, GLfloat value )
{
	glAccum( op, value );
}

void bglAlphaFunc( GLenum func, GLclampf ref )
{
	glAlphaFunc( func, ref );
}

GLboolean bglAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences )
{
	return glAreTexturesResident( n, textures, residences );
}

void bglArrayElement( GLint i )
{
	glArrayElement( i );
}

void bglBegin( GLenum mode )
{
	glBegin( mode );
}

void bglBindTexture( GLenum target, GLuint texture )
{
	INCR_STAT( _bglBindTextureCount );
	glBindTexture( target, texture );
}

void bglBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap )
{
	glBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
}

void bglBlendFunc( GLenum sfactor, GLenum dfactor )
{
	glBlendFunc( sfactor, dfactor );
}

void bglCallList( GLuint list )
{
	glCallList( list );
}

void bglCallLists( GLsizei n, GLenum type, const GLvoid *lists )
{
	glCallLists( n, type, lists );
}

void bglClear( GLbitfield mask )
{
	glClear( mask );
}

void bglClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	glClearAccum( red, green, blue, alpha );
}

void bglClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	glClearColor( red, green, blue, alpha );
}

void bglClearDepth( GLclampd depth )
{
	glClearDepth( depth );
}

void bglClearIndex( GLfloat c )
{
	glClearIndex( c );
}

void bglClearStencil( GLint s )
{
	glClearStencil( s );
}

void bglClipPlane( GLenum plane, const GLdouble *equation )
{
	glClipPlane( plane, equation );
}

void bglColor3b( GLbyte red, GLbyte green, GLbyte blue )
{
	glColor3b( red, green, blue );
}

void bglColor3bv( const GLbyte *v )
{
	glColor3bv( v );
}

void bglColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
	glColor3d( red, green, blue );
}

void bglColor3dv( const GLdouble *v )
{
	glColor3dv( v );
}

void bglColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
	glColor3f( red, green, blue );
}

void bglColor3fv( const GLfloat *v )
{
	glColor3fv( v );
}

void bglColor3i( GLint red, GLint green, GLint blue )
{
	glColor3i( red, green, blue );
}

void bglColor3iv( const GLint *v )
{
	glColor3iv( v );
}

void bglColor3s( GLshort red, GLshort green, GLshort blue )
{
	glColor3s( red, green, blue );
}

void bglColor3sv( const GLshort *v )
{
	glColor3sv( v );
}

void bglColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
	glColor3ub( red, green, blue );
}

void bglColor3ubv( const GLubyte *v )
{
	glColor3ubv( v );
}

void bglColor3ui( GLuint red, GLuint green, GLuint blue )
{
	glColor3ui( red, green, blue );
}

void bglColor3uiv( const GLuint *v )
{
	glColor3uiv( v );
}

void bglColor3us( GLushort red, GLushort green, GLushort blue )
{
	glColor3us( red, green, blue );
}

void bglColor3usv( const GLushort *v )
{
	glColor3usv( v );
}

void bglColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
	glColor4b( red, green, blue, alpha );
}

void bglColor4bv( const GLbyte *v )
{
	glColor4bv( v );
}

void bglColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
	glColor4d( red, green, blue, alpha );
}

void bglColor4dv( const GLdouble *v )
{
	glColor4dv( v );
}

void bglColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
	glColor4f( red, green, blue, alpha );
}

void bglColor4fv( const GLfloat *v )
{
	glColor4fv( v );
}

void bglColor4i( GLint red, GLint green, GLint blue, GLint alpha )
{
	glColor4i( red, green, blue, alpha );
}

void bglColor4iv( const GLint *v )
{
	glColor4iv( v );
}

void bglColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
	glColor4s( red, green, blue, alpha );
}

void bglColor4sv( const GLshort *v )
{
	glColor4sv( v );
}


void bglColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
	glColor4ub( red, green, blue, alpha );
}

void bglColor4ubv( const GLubyte *v )
{
	glColor4ubv( v );
}

void bglColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
	glColor4ui( red, green, blue, alpha );
}

void bglColor4uiv( const GLuint *v )
{
	glColor4uiv( v );
}

void bglColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
	glColor4us( red, green, blue, alpha );
}

void bglColor4usv( const GLushort *v )
{
	glColor4usv( v );
}

void bglColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
	glColorMask( red, green, blue, alpha );
}

void bglColorMaterial( GLenum face, GLenum mode )
{
	glColorMaterial( face, mode );
}

void bglColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	glColorPointer( size, type, stride, pointer );
}

void bglCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
	glCopyPixels( x, y, width, height, type );
}

void bglCopyTexImage1D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border )
{
	glCopyTexImage1D( target, level, internalFormat, x, y, width, border );
}

void bglCopyTexImage2D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
	glCopyTexImage2D( target, level, internalFormat, x, y, width, height, border );
}

void bglCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
	glCopyTexSubImage1D( target, level, xoffset, x, y, width );
}

void bglCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
	glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
}

void bglCullFace( GLenum mode )
{
	glCullFace( mode );
}

void bglDeleteLists( GLuint list, GLsizei range )
{
	glDeleteLists( list, range );
}

void bglDeleteTextures( GLsizei n, const GLuint *textures )
{
	glDeleteTextures( n, textures );
}

void bglDepthFunc( GLenum func )
{
	glDepthFunc( func );
}

void bglDepthMask( GLboolean flag )
{
	glDepthMask( flag );
}

void bglDepthRange( GLclampd zNear, GLclampd zFar )
{
	glDepthRange( zNear, zFar );
}

void bglDisable( GLenum cap )
{
	glDisable( cap );
}

void bglDisableClientState( GLenum array )
{
	glDisableClientState( array );
}

void bglDrawArrays( GLenum mode, GLint first, GLsizei count )
{
	INCR_STAT( _bglDrawCount );
	glDrawArrays( mode, first, count );
}

void bglDrawBuffer( GLenum mode )
{
	glDrawBuffer( mode );
}

void bglDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	glDrawElements( mode, count, type, indices );
}

void bglDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	glDrawPixels( width, height, format, type, pixels );
}

void bglEdgeFlag( GLboolean flag )
{
	glEdgeFlag( flag );
}

void bglEdgeFlagPointer( GLsizei stride, const GLvoid *pointer )
{
	glEdgeFlagPointer( stride, pointer );
}

void bglEdgeFlagv( const GLboolean *flag )
{
	glEdgeFlagv( flag );
}

void bglEnable( GLenum cap )
{
	glEnable( cap );
}

void bglEnableClientState( GLenum array )
{
	glEnableClientState( array );
}

void bglEnd()
{
	glEnd();
}

void bglEndList()
{
	glEndList();
}

void bglEvalCoord1d( GLdouble u )
{
	glEvalCoord1d( u );
}

void bglEvalCoord1dv( const GLdouble *u )
{
	glEvalCoord1dv( u );
}

void bglEvalCoord1f( GLfloat u )
{
	glEvalCoord1f( u );
}

void bglEvalCoord1fv( const GLfloat *u )
{
	glEvalCoord1fv( u );
}

void bglEvalCoord2d( GLdouble u, GLdouble v )
{
	glEvalCoord2d( u, v );
}

void bglEvalCoord2dv( const GLdouble *u )
{
	glEvalCoord2dv( u );
}

void bglEvalCoord2f( GLfloat u, GLfloat v )
{
	glEvalCoord2f( u, v );
}

void bglEvalCoord2fv( const GLfloat *u )
{
	glEvalCoord2fv( u );
}

void bglEvalMesh1( GLenum mode, GLint i1, GLint i2 )
{
	glEvalMesh1( mode, i1, i2 );
}

void bglEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
	glEvalMesh2( mode, i1, i2, j1, j2 );
}

void bglEvalPoint1( GLint i )
{
	glEvalPoint1( i );
}

void bglEvalPoint2( GLint i, GLint j )
{
	glEvalPoint2( i, j );
}

void bglFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer )
{
	glFeedbackBuffer( size, type, buffer );
}

void bglFinish()
{
	glFinish();
}

void bglFlush()
{
	glFlush();
}

void bglFogf( GLenum pname, GLfloat param )
{
	glFogf( pname, param );
}

void bglFogfv( GLenum pname, const GLfloat *params )
{
	glFogfv( pname, params );
}

void bglFogi( GLenum pname, GLint param )
{
	glFogi( pname, param );
}

void bglFogiv( GLenum pname, const GLint *params )
{
	glFogiv( pname, params );
}

void bglFrontFace( GLenum mode )
{
	glFrontFace( mode );
}

void bglFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	glFrustum( left, right, bottom, top, zNear, zFar );
}

GLuint bglGenLists( GLsizei range )
{
	return glGenLists( range );
}

void bglGenTextures( GLsizei n, GLuint *textures )
{
	glGenTextures( n, textures );
}

void bglGetBooleanv( GLenum pname, GLboolean *params )
{
	glGetBooleanv( pname, params );
}

void bglGetClipPlane( GLenum plane, GLdouble *equation )
{
	glGetClipPlane( plane, equation );
}

void bglGetDoublev( GLenum pname, GLdouble *params )
{
	glGetDoublev( pname, params );
}

GLenum bglGetError()
{
	return glGetError();
}

void bglGetFloatv( GLenum pname, GLfloat *params )
{
	glGetFloatv( pname, params );
}

void bglGetIntegerv( GLenum pname, GLint *params )
{
	glGetIntegerv( pname, params );
}

void bglGetLightfv( GLenum light, GLenum pname, GLfloat *params )
{
	glGetLightfv( light, pname, params );
}

void bglGetLightiv( GLenum light, GLenum pname, GLint *params )
{
	glGetLightiv( light, pname, params );
}

void bglGetMapdv( GLenum target, GLenum query, GLdouble *v )
{
	glGetMapdv( target, query, v );
}

void bglGetMapfv( GLenum target, GLenum query, GLfloat *v )
{
	glGetMapfv( target, query, v );
}

void bglGetMapiv( GLenum target, GLenum query, GLint *v )
{
	glGetMapiv( target, query, v );
}

void bglGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )
{
	glGetMaterialfv( face, pname, params );
}

void bglGetMaterialiv( GLenum face, GLenum pname, GLint *params )
{
	glGetMaterialiv( face, pname, params );
}

void bglGetPixelMapfv( GLenum map, GLfloat *values )
{
	glGetPixelMapfv( map, values );
}

void bglGetPixelMapuiv( GLenum map, GLuint *values )
{
	glGetPixelMapuiv( map, values );
}

void bglGetPixelMapusv( GLenum map, GLushort *values )
{
	glGetPixelMapusv( map, values );
}

void bglGetPointerv( GLenum pname, GLvoid* *params )
{
	glGetPointerv( pname, params );
}

void bglGetPolygonStipple( GLubyte *mask )
{
	glGetPolygonStipple( mask );
}

const char *bglGetString( GLenum name )
{
	return ( const char * )glGetString( name );
}

void bglGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )
{
	glGetTexEnvfv( target, pname, params );
}

void bglGetTexEnviv( GLenum target, GLenum pname, GLint *params )
{
	glGetTexEnviv( target, pname, params );
}

void bglGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )
{
	glGetTexGendv( coord, pname, params );
}

void bglGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )
{
	glGetTexGenfv( coord, pname, params );
}

void bglGetTexGeniv( GLenum coord, GLenum pname, GLint *params )
{
	glGetTexGeniv( coord, pname, params );
}

void bglGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
{
	glGetTexImage( target, level, format, type, pixels );
}

void bglGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params )
{
	glGetTexLevelParameterfv( target, level, pname, params );
}

void bglGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params )
{
	glGetTexLevelParameteriv( target, level, pname, params );
}

void bglGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
	glGetTexParameterfv( target, pname, params );
}

void bglGetTexParameteriv( GLenum target, GLenum pname, GLint *params )
{
	glGetTexParameteriv( target, pname, params );
}

void bglHint( GLenum target, GLenum mode )
{
	glHint( target, mode );
}

void bglIndexMask( GLuint mask )
{
	glIndexMask( mask );
}

void bglIndexPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
	glIndexPointer( type, stride, pointer );
}

void bglIndexd( GLdouble c )
{
	glIndexd( c );
}

void bglIndexdv( const GLdouble *c )
{
	glIndexdv( c );
}

void bglIndexf( GLfloat c )
{
	glIndexf( c );
}

void bglIndexfv( const GLfloat *c )
{
	glIndexfv( c );
}

void bglIndexi( GLint c )
{
	glIndexi( c );
}

void bglIndexiv( const GLint *c )
{
	glIndexiv( c );
}

void bglIndexs( GLshort c )
{
	glIndexs( c );
}

void bglIndexsv( const GLshort *c )
{
	glIndexsv( c );
}

void bglIndexub( GLubyte c )
{
	glIndexub( c );
}

void bglIndexubv( const GLubyte *c )
{
	glIndexubv( c );
}

void bglInitNames()
{
	glInitNames();
}

void bglInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer )
{
	glInterleavedArrays( format, stride, pointer );
}

GLboolean bglIsEnabled( GLenum cap )
{
	return glIsEnabled( cap );
}

GLboolean bglIsList( GLuint list )
{
	return glIsList( list );
}

GLboolean bglIsTexture( GLuint texture )
{
	return glIsTexture( texture );
}

void bglLightModelf( GLenum pname, GLfloat param )
{
	glLightModelf( pname, param );
}

void bglLightModelfv( GLenum pname, const GLfloat *params )
{
	glLightModelfv( pname, params );
}

void bglLightModeli( GLenum pname, GLint param )
{
	glLightModeli( pname, param );
}

void bglLightModeliv( GLenum pname, const GLint *params )
{
	glLightModeliv( pname, params );
}

void bglLightf( GLenum light, GLenum pname, GLfloat param )
{
	glLightf( light, pname, param );
}

void bglLightfv( GLenum light, GLenum pname, const GLfloat *params )
{
	glLightfv( light, pname, params );
}

void bglLighti( GLenum light, GLenum pname, GLint param )
{
	glLighti( light, pname, param );
}

void bglLightiv( GLenum light, GLenum pname, const GLint *params )
{
	glLightiv( light, pname, params );
}

void bglLineStipple( GLint factor, GLushort pattern )
{
	glLineStipple( factor, pattern );
}

void bglLineWidth( GLfloat width )
{
	glLineWidth( width );
}

void bglListBase( GLuint base )
{
	glListBase( base );
}

void bglLoadIdentity()
{
	glLoadIdentity();
}

void bglLoadMatrixd( const GLdouble *m )
{
	glLoadMatrixd( m );
}

void bglLoadMatrixf( const GLfloat *m )
{
	glLoadMatrixf( m );
}

void bglLoadName( GLuint name )
{
	glLoadName( name );
}

void bglLogicOp( GLenum opcode )
{
	glLogicOp( opcode );
}

void bglMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points )
{
	glMap1d( target, u1, u2, stride, order, points );
}

void bglMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points )
{
	glMap1f( target, u1, u2, stride, order, points );
}

void bglMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points )
{
	glMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}

void bglMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points )
{
	glMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
}

void bglMapGrid1d( GLint un, GLdouble u1, GLdouble u2 )
{
	glMapGrid1d( un, u1, u2 );
}

void bglMapGrid1f( GLint un, GLfloat u1, GLfloat u2 )
{
	glMapGrid1f( un, u1, u2 );
}

void bglMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 )
{
	glMapGrid2d( un, u1, u2, vn, v1, v2 );
}

void bglMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 )
{
	glMapGrid2f( un, u1, u2, vn, v1, v2 );
}

void bglMaterialf( GLenum face, GLenum pname, GLfloat param )
{
	glMaterialf( face, pname, param );
}

void bglMaterialfv( GLenum face, GLenum pname, const GLfloat *params )
{
	glMaterialfv( face, pname, params );
}

void bglMateriali( GLenum face, GLenum pname, GLint param )
{
	glMateriali( face, pname, param );
}

void bglMaterialiv( GLenum face, GLenum pname, const GLint *params )
{
	glMaterialiv( face, pname, params );
}

void bglMatrixMode( GLenum mode )
{
	glMatrixMode( mode );
}

void bglMultMatrixd( const GLdouble *m )
{
	glMultMatrixd( m );
}

void bglMultMatrixf( const GLfloat *m )
{
	glMultMatrixf( m );
}

void bglNewList( GLuint list, GLenum mode )
{
	glNewList( list, mode );
}

void bglNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )
{
	glNormal3b( nx, ny, nz );
}

void bglNormal3bv( const GLbyte *v )
{
	glNormal3bv( v );
}

void bglNormal3d( GLdouble nx, GLdouble ny, GLdouble nz )
{
	glNormal3d( nx, ny, nz );
}

void bglNormal3dv( const GLdouble *v )
{
	glNormal3dv( v );
}

void bglNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
	glNormal3f( nx, ny, nz );
}

void bglNormal3fv( const GLfloat *v )
{
	glNormal3fv( v );
}

void bglNormal3i( GLint nx, GLint ny, GLint nz )
{
	glNormal3i( nx, ny, nz );
}

void bglNormal3iv( const GLint *v )
{
	glNormal3iv( v );
}

void bglNormal3s( GLshort nx, GLshort ny, GLshort nz )
{
	glNormal3s( nx, ny, nz );
}

void bglNormal3sv( const GLshort *v )
{
	glNormal3sv( v );
}

void bglNormalPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
	glNormalPointer( type, stride, pointer );
}

void bglOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
	glOrtho( left, right, bottom, top, zNear, zFar );
}

void bglPassThrough( GLfloat token )
{
	glPassThrough( token );
}

void bglPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values )
{
	glPixelMapfv( map, mapsize, values );
}

void bglPixelMapuiv( GLenum map, GLsizei mapsize, const GLuint *values )
{
	glPixelMapuiv( map, mapsize, values );
}

void bglPixelMapusv( GLenum map, GLsizei mapsize, const GLushort *values )
{
	glPixelMapusv( map, mapsize, values );
}

void bglPixelStoref( GLenum pname, GLfloat param )
{
	glPixelStoref( pname, param );
}

void bglPixelStorei( GLenum pname, GLint param )
{
	glPixelStorei( pname, param );
}


void bglPixelTransferf( GLenum pname, GLfloat param )
{
	glPixelTransferf( pname, param );
}

void bglPixelTransferi( GLenum pname, GLint param )
{
	glPixelTransferi( pname, param );
}

void bglPixelZoom( GLfloat xfactor, GLfloat yfactor )
{
	glPixelZoom( xfactor, yfactor );
}

void bglPointSize( GLfloat size )
{
	glPointSize( size );
}

void bglPolygonMode( GLenum face, GLenum mode )
{
	glPolygonMode( face, mode );
}

void bglPolygonOffset( GLfloat factor, GLfloat units )
{
	glPolygonOffset( factor, units );
}

void bglPolygonStipple( const GLubyte *mask )
{
	glPolygonStipple( mask );
}

void bglPopAttrib()
{
	glPopAttrib();
}

void bglPopClientAttrib()
{
	glPopClientAttrib();
}

void bglPopMatrix()
{
	glPopMatrix();
}

void bglPopName()
{
	glPopName();
}

void bglPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities )
{
	glPrioritizeTextures( n, textures, priorities );
}

void bglPushAttrib( GLbitfield mask )
{
	// Pushing attributes causes cached state to be invalid.
	B_ASSERT( false );
	glPushAttrib( mask );
}

void bglPushClientAttrib( GLbitfield mask )
{
	// Pushing attributes causes cached state to be invalid.
	B_ASSERT( false );
	glPushClientAttrib( mask );
}

void bglPushMatrix()
{
	glPushMatrix();
}

void bglPushName( GLuint name )
{
	glPushName( name );
}

void bglRasterPos2d( GLdouble x, GLdouble y )
{
	glRasterPos2d( x, y );
}

void bglRasterPos2dv( const GLdouble *v )
{
	glRasterPos2dv( v );
}

void bglRasterPos2f( GLfloat x, GLfloat y )
{
	glRasterPos2f( x, y );
}

void bglRasterPos2fv( const GLfloat *v )
{
	glRasterPos2fv( v );
}

void bglRasterPos2i( GLint x, GLint y )
{
	glRasterPos2i( x, y );
}

void bglRasterPos2iv( const GLint *v )
{
	glRasterPos2iv( v );
}

void bglRasterPos2s( GLshort x, GLshort y )
{
	glRasterPos2s( x, y );
}

void bglRasterPos2sv( const GLshort *v )
{
	glRasterPos2sv( v );
}

void bglRasterPos3d( GLdouble x, GLdouble y, GLdouble z )
{
	glRasterPos3d( x, y, z );
}

void bglRasterPos3dv( const GLdouble *v )
{
	glRasterPos3dv( v );
}

void bglRasterPos3f( GLfloat x, GLfloat y, GLfloat z )
{
	glRasterPos3f( x, y, z );
}

void bglRasterPos3fv( const GLfloat *v )
{
	glRasterPos3fv( v );
}

void bglRasterPos3i( GLint x, GLint y, GLint z )
{
	glRasterPos3i( x, y, z );
}

void bglRasterPos3iv( const GLint *v )
{
	glRasterPos3iv( v );
}

void bglRasterPos3s( GLshort x, GLshort y, GLshort z )
{
	glRasterPos3s( x, y, z );
}

void bglRasterPos3sv( const GLshort *v )
{
	glRasterPos3sv( v );
}

void bglRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
	glRasterPos4d( x, y, z, w );
}

void bglRasterPos4dv( const GLdouble *v )
{
	glRasterPos4dv( v );
}

void bglRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	glRasterPos4f( x, y, z, w );
}

void bglRasterPos4fv( const GLfloat *v )
{
	glRasterPos4fv( v );
}

void bglRasterPos4i( GLint x, GLint y, GLint z, GLint w )
{
	glRasterPos4i( x, y, z, w );
}

void bglRasterPos4iv( const GLint *v )
{
	glRasterPos4iv( v );
}

void bglRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
	glRasterPos4s( x, y, z, w );
}

void bglRasterPos4sv( const GLshort *v )
{
	glRasterPos4sv( v );
}


void bglReadBuffer( GLenum mode )
{
	glReadBuffer( mode );
}

void bglReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
	glReadPixels( x, y, width, height, format, type, pixels );
}

void bglRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
void bglRectdv( const GLdouble *v1, const GLdouble *v2 );
void bglRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
void bglRectfv( const GLfloat *v1, const GLfloat *v2 );
void bglRecti( GLint x1, GLint y1, GLint x2, GLint y2 );
void bglRectiv( const GLint *v1, const GLint *v2 );
void bglRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
void bglRectsv( const GLshort *v1, const GLshort *v2 );
GLint bglRenderMode( GLenum mode );
void bglRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );

void bglRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
	glRotatef( angle, x, y, z );
}

void bglScaled( GLdouble x, GLdouble y, GLdouble z );

void bglScalef( GLfloat x, GLfloat y, GLfloat z )
{
	glScalef( x, y, z );
}

void bglScissor( GLint x, GLint y, GLsizei width, GLsizei height )
{
	glScissor( x, y, width, height );
}

void bglSelectBuffer( GLsizei size, GLuint *buffer );
void bglShadeModel( GLenum mode );

void bglStencilFunc( GLenum func, GLint ref, GLuint mask )
{
	glStencilFunc( func, ref, mask );
}

void bglStencilMask( GLuint mask )
{
	glStencilMask( mask );
}

void bglStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
	glStencilOp( fail, zfail, zpass );
}

void bglTexCoord1d( GLdouble s );
void bglTexCoord1dv( const GLdouble *v );
void bglTexCoord1f( GLfloat s );
void bglTexCoord1fv( const GLfloat *v );
void bglTexCoord1i( GLint s );
void bglTexCoord1iv( const GLint *v );
void bglTexCoord1s( GLshort s );
void bglTexCoord1sv( const GLshort *v );
void bglTexCoord2d( GLdouble s, GLdouble t );
void bglTexCoord2dv( const GLdouble *v );

void bglTexCoord2f( GLfloat s, GLfloat t )
{
	glTexCoord2f( s, t );
}

void bglTexCoord2fv( const GLfloat *v );
void bglTexCoord2i( GLint s, GLint t );
void bglTexCoord2iv( const GLint *v );
void bglTexCoord2s( GLshort s, GLshort t );
void bglTexCoord2sv( const GLshort *v );
void bglTexCoord3d( GLdouble s, GLdouble t, GLdouble r );
void bglTexCoord3dv( const GLdouble *v );
void bglTexCoord3f( GLfloat s, GLfloat t, GLfloat r );
void bglTexCoord3fv( const GLfloat *v );
void bglTexCoord3i( GLint s, GLint t, GLint r );
void bglTexCoord3iv( const GLint *v );
void bglTexCoord3s( GLshort s, GLshort t, GLshort r );
void bglTexCoord3sv( const GLshort *v );
void bglTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
void bglTexCoord4dv( const GLdouble *v );
void bglTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
void bglTexCoord4fv( const GLfloat *v );
void bglTexCoord4i( GLint s, GLint t, GLint r, GLint q );
void bglTexCoord4iv( const GLint *v );
void bglTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q );
void bglTexCoord4sv( const GLshort *v );

void bglTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	glTexCoordPointer( size, type, stride, pointer );
}

void bglTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
	glTexEnvf( target, pname, param );
}

void bglTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
void bglTexEnvi( GLenum target, GLenum pname, GLint param )
{
	glTexEnvi( target, pname, param );
}

void bglTexEnviv( GLenum target, GLenum pname, const GLint *params );
void bglTexGend( GLenum coord, GLenum pname, GLdouble param );
void bglTexGendv( GLenum coord, GLenum pname, const GLdouble *params );
void bglTexGenf( GLenum coord, GLenum pname, GLfloat param );
void bglTexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
void bglTexGeni( GLenum coord, GLenum pname, GLint param );
void bglTexGeniv( GLenum coord, GLenum pname, const GLint *params );
void bglTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );

void bglTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
	glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
}

void bglTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
	glTexParameterf( target, pname, param );
}

void bglTexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
	glTexParameterfv( target, pname, params );
}

void bglTexParameteri( GLenum target, GLenum pname, GLint param )
{
	glTexParameteri( target, pname, param );
}

void bglTexParameteriv( GLenum target, GLenum pname, const GLint *params );
void bglTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );

void bglTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
}

void bglTranslated( GLdouble x, GLdouble y, GLdouble z );

void bglTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
	glTranslatef( x, y, z );
}

void bglVertex2d( GLdouble x, GLdouble y );
void bglVertex2dv( const GLdouble *v );
void bglVertex2f( GLfloat x, GLfloat y );
void bglVertex2fv( const GLfloat *v );
void bglVertex2i( GLint x, GLint y );
void bglVertex2iv( const GLint *v );
void bglVertex2s( GLshort x, GLshort y );
void bglVertex2sv( const GLshort *v );
void bglVertex3d( GLdouble x, GLdouble y, GLdouble z );
void bglVertex3dv( const GLdouble *v );

void bglVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
	glVertex3f( x, y, z );
}

void bglVertex3fv( const GLfloat *v )
{
	glVertex3fv( v );
}

void bglVertex3i( GLint x, GLint y, GLint z );
void bglVertex3iv( const GLint *v );
void bglVertex3s( GLshort x, GLshort y, GLshort z );
void bglVertex3sv( const GLshort *v );
void bglVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
void bglVertex4dv( const GLdouble *v );
void bglVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
void bglVertex4fv( const GLfloat *v );
void bglVertex4i( GLint x, GLint y, GLint z, GLint w );
void bglVertex4iv( const GLint *v );
void bglVertex4s( GLshort x, GLshort y, GLshort z, GLshort w );
void bglVertex4sv( const GLshort *v );

void bglVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	glVertexPointer( size, type, stride, pointer );
}

void bglViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	glViewport( x, y, width, height );
}

// OpenGL 1.5
// 1.2
void bglBlendColor( GLclampf, GLclampf, GLclampf, GLclampf );

void bglBlendEquation( GLenum equation )
{
	glBlendEquation( equation );
}

void bglDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices )
{
	INCR_STAT( _bglDrawRangeCount );
	glDrawRangeElements( mode, start, end, count, type, indices );
}

void bglColorTable( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
void bglColorTableParameterfv( GLenum, GLenum, const GLfloat * );
void bglColorTableParameteriv( GLenum, GLenum, const GLint * );
void bglCopyColorTable( GLenum, GLenum, GLint, GLint, GLsizei );
void bglGetColorTable( GLenum, GLenum, GLenum, GLvoid * );
void bglGetColorTableParameterfv( GLenum, GLenum, GLfloat * );
void bglGetColorTableParameteriv( GLenum, GLenum, GLint * );
void bglColorSubTable( GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
void bglCopyColorSubTable( GLenum, GLsizei, GLint, GLint, GLsizei );
void bglConvolutionFilter1D( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
void bglConvolutionFilter2D( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
void bglConvolutionParameterf( GLenum, GLenum, GLfloat );
void bglConvolutionParameterfv( GLenum, GLenum, const GLfloat * );
void bglConvolutionParameteri( GLenum, GLenum, GLint );
void bglConvolutionParameteriv( GLenum, GLenum, const GLint * );
void bglCopyConvolutionFilter1D( GLenum, GLenum, GLint, GLint, GLsizei );
void bglCopyConvolutionFilter2D( GLenum, GLenum, GLint, GLint, GLsizei, GLsizei );
void bglGetConvolutionFilter( GLenum, GLenum, GLenum, GLvoid * );
void bglGetConvolutionParameterfv( GLenum, GLenum, GLfloat * );
void bglGetConvolutionParameteriv( GLenum, GLenum, GLint * );
void bglGetSeparableFilter( GLenum, GLenum, GLenum, GLvoid *, GLvoid *, GLvoid * );
void bglSeparableFilter2D( GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *, const GLvoid * );
void bglGetHistogram( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
void bglGetHistogramParameterfv( GLenum, GLenum, GLfloat * );
void bglGetHistogramParameteriv( GLenum, GLenum, GLint * );
void bglGetMinmax( GLenum, GLboolean, GLenum, GLenum, GLvoid * );
void bglGetMinmaxParameterfv( GLenum, GLenum, GLfloat * );
void bglGetMinmaxParameteriv( GLenum, GLenum, GLint * );
void bglHistogram( GLenum, GLsizei, GLenum, GLboolean );
void bglMinmax( GLenum, GLenum, GLboolean );
void bglResetHistogram( GLenum );
void bglResetMinmax( GLenum );

void bglTexImage3D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
	glTexImage3D( target, level, internalFormat, width, height, depth, border, format, type, pixels );
}

void bglTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid * );
void bglCopyTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei );

// 1.3
void bglActiveTexture( GLenum texUnit )
{
	glActiveTexture( texUnit );
}

void bglClientActiveTexture( GLenum texUnitStream )
{
	glClientActiveTexture( texUnitStream );
}

void bglMultiTexCoord1d( GLenum, GLdouble );
void bglMultiTexCoord1dv( GLenum, const GLdouble * );
void bglMultiTexCoord1f( GLenum, GLfloat );
void bglMultiTexCoord1fv( GLenum, const GLfloat * );
void bglMultiTexCoord1i( GLenum, GLint );
void bglMultiTexCoord1iv( GLenum, const GLint * );
void bglMultiTexCoord1s( GLenum, GLshort );
void bglMultiTexCoord1sv( GLenum, const GLshort * );
void bglMultiTexCoord2d( GLenum, GLdouble, GLdouble );
void bglMultiTexCoord2dv( GLenum, const GLdouble * );
void bglMultiTexCoord2f( GLenum, GLfloat, GLfloat );
void bglMultiTexCoord2fv( GLenum, const GLfloat * );
void bglMultiTexCoord2i( GLenum, GLint, GLint );
void bglMultiTexCoord2iv( GLenum, const GLint * );
void bglMultiTexCoord2s( GLenum, GLshort, GLshort );
void bglMultiTexCoord2sv( GLenum, const GLshort * );
void bglMultiTexCoord3d( GLenum, GLdouble, GLdouble, GLdouble );
void bglMultiTexCoord3dv( GLenum, const GLdouble * );
void bglMultiTexCoord3f( GLenum, GLfloat, GLfloat, GLfloat );
void bglMultiTexCoord3fv( GLenum, const GLfloat * );
void bglMultiTexCoord3i( GLenum, GLint, GLint, GLint );
void bglMultiTexCoord3iv( GLenum, const GLint * );
void bglMultiTexCoord3s( GLenum, GLshort, GLshort, GLshort );
void bglMultiTexCoord3sv( GLenum, const GLshort * );
void bglMultiTexCoord4d( GLenum, GLdouble, GLdouble, GLdouble, GLdouble );
void bglMultiTexCoord4dv( GLenum, const GLdouble * );
void bglMultiTexCoord4f( GLenum, GLfloat, GLfloat, GLfloat, GLfloat );
void bglMultiTexCoord4fv( GLenum, const GLfloat * );
void bglMultiTexCoord4i( GLenum, GLint, GLint, GLint, GLint );
void bglMultiTexCoord4iv( GLenum, const GLint * );
void bglMultiTexCoord4s( GLenum, GLshort, GLshort, GLshort, GLshort );
void bglMultiTexCoord4sv( GLenum, const GLshort * );

void bglLoadTransposeMatrixf( const GLfloat* matrix )
{
	glLoadTransposeMatrixf( matrix );
}

void bglLoadTransposeMatrixd( const GLdouble * );
void bglMultTransposeMatrixf( const GLfloat* matrix )
{
	glMultTransposeMatrixf( matrix );
}

void bglMultTransposeMatrixd( const GLdouble * );
void bglSampleCoverage( GLclampf, GLboolean );
void bglCompressedTexImage3D( GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid * );
void bglCompressedTexImage2D( GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data )
{
	glCompressedTexImage2D( target, level, internalFormat, width, height, border, imageSize, data );
}

void bglCompressedTexImage1D( GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid * );
void bglCompressedTexSubImage3D( GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid * );

void bglCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data )
{
	glCompressedTexSubImage2D( target, level, xoffset, yoffset, width, height, format, imageSize, data );
}

void bglCompressedTexSubImage1D( GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid * );
void bglGetCompressedTexImage( GLenum, GLint, GLvoid * );

// 1.4
void bglBlendFuncSeparate( GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha )
{
	glBlendFuncSeparate( sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha );
}
void bglFogCoordf( GLfloat );
void bglFogCoordfv( const GLfloat * );
void bglFogCoordd( GLdouble );
void bglFogCoorddv( const GLdouble * );
void bglFogCoordPointer( GLenum, GLsizei, const GLvoid * );
void bglMultiDrawArrays( GLenum, GLint *, GLsizei *, GLsizei );
void bglMultiDrawElements( GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei );
void bglPointParameterf( GLenum, GLfloat );
void bglPointParameterfv( GLenum, const GLfloat * );
void bglPointParameteri( GLenum, GLint );
void bglPointParameteriv( GLenum, const GLint * );
void bglSecondaryColor3b( GLbyte, GLbyte, GLbyte );
void bglSecondaryColor3bv( const GLbyte * );
void bglSecondaryColor3d( GLdouble, GLdouble, GLdouble );
void bglSecondaryColor3dv( const GLdouble * );
void bglSecondaryColor3f( GLfloat, GLfloat, GLfloat );
void bglSecondaryColor3fv( const GLfloat * );
void bglSecondaryColor3i( GLint, GLint, GLint );
void bglSecondaryColor3iv( const GLint * );
void bglSecondaryColor3s( GLshort, GLshort, GLshort );
void bglSecondaryColor3sv( const GLshort * );
void bglSecondaryColor3ub( GLubyte, GLubyte, GLubyte );
void bglSecondaryColor3ubv( const GLubyte * );
void bglSecondaryColor3ui( GLuint, GLuint, GLuint );
void bglSecondaryColor3uiv( const GLuint * );
void bglSecondaryColor3us( GLushort, GLushort, GLushort );
void bglSecondaryColor3usv( const GLushort * );
void bglSecondaryColorPointer( GLint, GLenum, GLsizei, const GLvoid * );
void bglWindowPos2d( GLdouble, GLdouble );
void bglWindowPos2dv( const GLdouble * );
void bglWindowPos2f( GLfloat, GLfloat );
void bglWindowPos2fv( const GLfloat * );
void bglWindowPos2i( GLint, GLint );
void bglWindowPos2iv( const GLint * );
void bglWindowPos2s( GLshort, GLshort );
void bglWindowPos2sv( const GLshort * );
void bglWindowPos3d( GLdouble, GLdouble, GLdouble );
void bglWindowPos3dv( const GLdouble * );
void bglWindowPos3f( GLfloat, GLfloat, GLfloat );
void bglWindowPos3fv( const GLfloat * );
void bglWindowPos3i( GLint, GLint, GLint );
void bglWindowPos3iv( const GLint * );
void bglWindowPos3s( GLshort, GLshort, GLshort );
void bglWindowPos3sv( const GLshort * );

// 1.5
void bglGenQueries( GLsizei n, GLuint *objects )
{
	glGenQueries( n, objects );
}

void bglDeleteQueries( GLsizei n, const GLuint *objects )
{
	glDeleteQueries( n, objects );
}

GLboolean bglIsQuery( GLuint object )
{
	return glIsQuery( object );
}

void bglBeginQuery( GLenum target, GLuint object )
{
	glBeginQuery( target, object );
}

void bglEndQuery( GLenum target )
{
	glEndQuery( target );
}

void bglGetQueryiv( GLenum object, GLenum pname, GLint *value )
{
	glGetQueryiv( object, pname, value );
}

void bglGetQueryObjectiv( GLuint object, GLenum pname, GLint *value )
{
	glGetQueryObjectiv( object, pname, value );
}

void bglGetQueryObjectuiv( GLuint object, GLenum pname, GLuint *value )
{
	glGetQueryObjectuiv( object, pname, value );
}

void bglBindBuffer( GLenum target, GLuint buffer )
{
	glBindBuffer( target, buffer );
}

void bglDeleteBuffers( GLsizei n, const GLuint *buffers )
{
	glDeleteBuffers( n, buffers );
}

void bglGenBuffers( GLsizei n, GLuint *buffers )
{
	glGenBuffers( n, buffers );
}

GLboolean bglIsBuffer( GLuint object )
{
	return glIsBuffer( object );
}

void bglBufferData( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage )
{
	glBufferData( target, size, data, usage );
}

void bglBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data )
{
	glBufferSubData( target, offset, size, data );
}

void bglGetBufferSubData( GLenum, GLintptr, GLsizeiptr, GLvoid * );

GLvoid *bglMapBuffer ( GLenum target, GLenum access )
{
	return glMapBuffer( target, access );
}

GLboolean bglUnmapBuffer( GLenum target )
{
	return glUnmapBuffer( target );
}

void bglGetBufferParameteriv( GLenum, GLenum, GLint * );
void bglGetBufferPointerv( GLenum, GLenum, GLvoid* * );

// 2.0
void bglBlendEquationSeparate( GLenum, GLenum );

void bglDrawBuffers( GLsizei n, const GLenum *bufs )
{
	glDrawBuffers( n, bufs );
}

void bglStencilOpSeparate( GLenum, GLenum, GLenum, GLenum );
void bglStencilFuncSeparate( GLenum, GLenum, GLint, GLuint );
void bglStencilMaskSeparate( GLenum, GLuint );

void bglAttachShader( GLuint program, GLuint shader )
{
	glAttachShader( program, shader );
}

void bglBindAttribLocation( GLuint program, GLuint index, const GLchar *name )
{
	glBindAttribLocation( program, index, name );
}

void bglCompileShader( GLuint shader )
{
	glCompileShader( shader );
}

GLuint bglCreateProgram( void )
{
	return glCreateProgram();
}

GLuint bglCreateShader( GLenum type )
{
	return glCreateShader( type );
}

void bglDeleteProgram( GLuint program )
{
	glDeleteProgram( program );
}

void bglDeleteShader( GLuint shader )
{
	glDeleteShader( shader );
}

void bglDetachShader( GLuint program, GLuint shader )
{
	glDetachShader( program, shader );
}

void bglDisableVertexAttribArray( GLuint array )
{
	glDisableVertexAttribArray( array );
}

void bglEnableVertexAttribArray( GLuint array )
{
	glEnableVertexAttribArray( array );
}

void bglGetActiveAttrib( GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar * );

void bglGetActiveUniform( GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name )
{
	glGetActiveUniform( program, index, bufSize, length, size, type, name );
}

void bglGetAttachedShaders( GLuint, GLsizei, GLsizei *, GLuint * );

GLint bglGetAttribLocation( GLuint program, const GLchar* name )
{
	return glGetAttribLocation( program, name );
}

void bglGetProgramiv( GLuint program, GLenum pname, GLint* params )
{
	glGetProgramiv( program, pname, params );
}

void bglGetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog )
{
	glGetProgramInfoLog( program, bufSize, length, infoLog );
}

void bglGetShaderiv( GLuint shader, GLenum pname, GLint* params )
{
	glGetShaderiv( shader, pname, params );
}

void bglGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog )
{
	glGetShaderInfoLog( shader, bufSize, length, infoLog );
}

void bglGetShaderSource( GLuint, GLsizei, GLsizei *, GLchar * );

GLint bglGetUniformLocation( GLuint program, const GLchar* name )
{
	return glGetUniformLocation( program, name );
}

void bglGetUniformfv( GLuint, GLint, GLfloat * );
void bglGetUniformiv( GLuint, GLint, GLint * );
void bglGetVertexAttribdv( GLuint, GLenum, GLdouble * );
void bglGetVertexAttribfv( GLuint, GLenum, GLfloat * );
void bglGetVertexAttribiv( GLuint, GLenum, GLint * );
void bglGetVertexAttribPointerv( GLuint, GLenum, GLvoid* * );
GLboolean bglIsProgram( GLuint );
GLboolean bglIsShader( GLuint );

void bglLinkProgram( GLuint program )
{
	glLinkProgram( program );
}

void bglShaderSource( GLuint shader, GLsizei count, const GLchar** strings, const GLint* length )
{
	glShaderSource( shader, count, strings, length );
}

void bglUseProgram( GLuint program )
{
	glUseProgram( program );
}

void bglUniform1f( GLint, GLfloat );
void bglUniform2f( GLint, GLfloat, GLfloat );
void bglUniform3f( GLint, GLfloat, GLfloat, GLfloat );
void bglUniform4f( GLint, GLfloat, GLfloat, GLfloat, GLfloat );

void bglUniform1i( GLint location, GLint v0 )
{
	glUniform1i( location, v0 );
}

void bglUniform2i( GLint, GLint, GLint );
void bglUniform3i( GLint, GLint, GLint, GLint );
void bglUniform4i( GLint, GLint, GLint, GLint, GLint );

void bglUniform1fv( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform1fv( location, count, value );
}

void bglUniform2fv( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform2fv( location, count, value );
}

void bglUniform3fv( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform3fv( location, count, value );
}

void bglUniform4fv( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform4fv( location, count, value );
}

void bglUniform1iv( GLint, GLsizei, const GLint * );
void bglUniform2iv( GLint, GLsizei, const GLint * );
void bglUniform3iv( GLint, GLsizei, const GLint * );
void bglUniform4iv( GLint, GLsizei, const GLint * );
void bglUniformMatrix2fv( GLint, GLsizei, GLboolean, const GLfloat * );

void bglUniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix3fv( location, count, transpose, value );
}

void bglUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix4fv( location, count, transpose, value );
}

void bglValidateProgram( GLuint program )
{
	glValidateProgram( program );
}

void bglVertexAttrib1d( GLuint, GLdouble );
void bglVertexAttrib1dv( GLuint, const GLdouble * );
void bglVertexAttrib1f( GLuint, GLfloat );
void bglVertexAttrib1fv( GLuint, const GLfloat * );
void bglVertexAttrib1s( GLuint, GLshort );
void bglVertexAttrib1sv( GLuint, const GLshort * );
void bglVertexAttrib2d( GLuint, GLdouble, GLdouble );
void bglVertexAttrib2dv( GLuint, const GLdouble * );

void bglVertexAttrib2f( GLuint index, GLfloat x, GLfloat y )
{
	glVertexAttrib2f( index, x, y );
}

void bglVertexAttrib2fv( GLuint, const GLfloat * );
void bglVertexAttrib2s( GLuint, GLshort, GLshort );
void bglVertexAttrib2sv( GLuint, const GLshort * );
void bglVertexAttrib3d( GLuint, GLdouble, GLdouble, GLdouble );
void bglVertexAttrib3dv( GLuint, const GLdouble * );

void bglVertexAttrib3f( GLuint index, GLfloat x, GLfloat y, GLfloat z )
{
	glVertexAttrib3f( index, x, y, z );
}

void bglVertexAttrib3fv( GLuint, const GLfloat * );
void bglVertexAttrib3s( GLuint, GLshort, GLshort, GLshort );
void bglVertexAttrib3sv( GLuint, const GLshort * );
void bglVertexAttrib4Nbv( GLuint, const GLbyte * );
void bglVertexAttrib4Niv( GLuint, const GLint * );
void bglVertexAttrib4Nsv( GLuint, const GLshort * );
void bglVertexAttrib4Nub( GLuint, GLubyte, GLubyte, GLubyte, GLubyte );
void bglVertexAttrib4Nubv( GLuint, const GLubyte * );
void bglVertexAttrib4Nuiv( GLuint, const GLuint * );
void bglVertexAttrib4Nusv( GLuint, const GLushort * );
void bglVertexAttrib4bv( GLuint, const GLbyte * );
void bglVertexAttrib4d( GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
void bglVertexAttrib4dv( GLuint, const GLdouble * );

void bglVertexAttrib4f( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	glVertexAttrib4f( index, x, y, z, w );
}

void bglVertexAttrib4fv( GLuint, const GLfloat * );
void bglVertexAttrib4iv( GLuint, const GLint * );
void bglVertexAttrib4s( GLuint, GLshort, GLshort, GLshort, GLshort );
void bglVertexAttrib4sv( GLuint, const GLshort * );
void bglVertexAttrib4ubv( GLuint, const GLubyte * );
void bglVertexAttrib4uiv( GLuint, const GLuint * );
void bglVertexAttrib4usv( GLuint, const GLushort * );

void bglVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
{
	glVertexAttribPointer( index, size, type, normalized, stride, pointer );
}

// 2.1
void bglUniformMatrix2x3fv( GLint, GLsizei, GLboolean, const GLfloat * );
void bglUniformMatrix3x2fv( GLint, GLsizei, GLboolean, const GLfloat * );
void bglUniformMatrix2x4fv( GLint, GLsizei, GLboolean, const GLfloat * );

void bglUniformMatrix4x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix4x2fv( location, count, transpose, value );
}

void bglUniformMatrix3x4fv( GLint, GLsizei, GLboolean, const GLfloat * );

void bglUniformMatrix4x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix4x3fv( location, count, transpose, value );
}

// Generic extensions req'd.
// GL_ARB_shader_objects
void bglDeleteObjectARB( GLhandleARB obj )
{
	glDeleteObjectARB( obj );
}

GLhandleARB bglGetHandleARB( GLenum pname )
{
	return glGetHandleARB( pname );
}

void bglDetachObjectARB( GLhandleARB containerObj, GLhandleARB attachedObj )
{
	glDetachObjectARB( containerObj, attachedObj );
}

GLhandleARB bglCreateShaderObjectARB( GLenum shaderType )
{
	return glCreateShaderObjectARB( shaderType );
}

void bglShaderSourceARB( GLhandleARB shaderObj, GLsizei count, const GLcharARB** string, const GLint* length )
{
	glShaderSourceARB( shaderObj, count, string, length );
}

void bglCompileShaderARB( GLhandleARB shaderObj )
{
	glCompileShaderARB( shaderObj );
}

GLhandleARB bglCreateProgramObjectARB()
{
	return glCreateProgramObjectARB();
}

void bglAttachObjectARB( GLhandleARB containerObj, GLhandleARB obj )
{
	glAttachObjectARB( containerObj, obj );
}

void bglLinkProgramARB( GLhandleARB programObj )
{
	glLinkProgramARB( programObj );
}

void bglUseProgramObjectARB( GLhandleARB programObj )
{
	glUseProgramObjectARB( programObj );
}

void bglValidateProgramARB( GLhandleARB programObj )
{
	glValidateProgramARB( programObj );
}

void bglUniform1fARB( GLint location, GLfloat v0 )
{
	glUniform1fARB( location, v0 );
}

void bglUniform2fARB( GLint location, GLfloat v0, GLfloat v1 )
{
	glUniform2fARB( location, v0, v1 );
}

void bglUniform3fARB( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
{
	glUniform3fARB( location, v0, v1, v2 );
}

void bglUniform4fARB( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
{
	glUniform4fARB( location, v0, v1, v2, v3 );
}

void bglUniform1iARB( GLint location, GLint v0 )
{
	glUniform1iARB( location, v0 );
}

void bglUniform2iARB( GLint location, GLint v0, GLint v1 )
{
	glUniform2iARB( location, v0, v1 );
}

void bglUniform3iARB( GLint location, GLint v0, GLint v1, GLint v2 )
{
	glUniform3iARB( location, v0, v1, v2 );
}

void bglUniform4iARB( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
{
	glUniform4iARB( location, v0, v1, v2, v3 );
}

void bglUniform1fvARB( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform1fvARB( location, count, value );
}

void bglUniform2fvARB( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform2fvARB( location, count, value );
}

void bglUniform3fvARB( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform3fvARB( location, count, value );
}

void bglUniform4fvARB( GLint location, GLsizei count, const GLfloat* value )
{
	glUniform4fvARB( location, count, value );
}

void bglUniform1ivARB( GLint location, GLsizei count, const GLint* value )
{
	glUniform1ivARB( location, count, value );
}

void bglUniform2ivARB( GLint location, GLsizei count, const GLint* value )
{
	glUniform2ivARB( location, count, value );
}

void bglUniform3ivARB( GLint location, GLsizei count, const GLint* value )
{
	glUniform3ivARB( location, count, value );
}

void bglUniform4ivARB( GLint location, GLsizei count, const GLint* value )
{
	glUniform4ivARB( location, count, value );
}

void bglUniformMatrix2fvARB( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix2fvARB( location, count, transpose, value );
}

void bglUniformMatrix3fvARB( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix3fvARB( location, count, transpose, value );
}

void bglUniformMatrix4fvARB( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value )
{
	glUniformMatrix4fvARB( location, count, transpose, value );
}

void bglGetObjectParameterfvARB( GLhandleARB obj, GLenum pname, GLfloat* params )
{
	glGetObjectParameterfvARB( obj, pname, params );
}

void bglGetObjectParameterivARB( GLhandleARB obj, GLenum pname, GLint* params )
{
	glGetObjectParameterivARB( obj, pname, params );
}

void bglGetInfoLogARB( GLhandleARB obj, GLsizei maxLength, GLsizei* length, GLcharARB* infoLog )
{
	glGetInfoLogARB( obj, maxLength, length, infoLog );
}

void bglGetAttachedObjectsARB( GLhandleARB containerObj, GLsizei maxCount, GLsizei* count, GLhandleARB* obj )
{
	glGetAttachedObjectsARB( containerObj, maxCount, count, obj );
}

GLint bglGetUniformLocationARB( GLhandleARB programObj, const GLcharARB* name )
{
	return glGetUniformLocationARB( programObj, name );
}

void bglGetActiveUniformARB( GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei* length, GLint* size, GLenum* type, GLcharARB* name )
{
	glGetActiveUniformARB( programObj, index, maxLength, length, size, type, name );
}

void bglGetUniformfvARB( GLhandleARB programObj, GLint location, GLfloat* params )
{
	glGetUniformfvARB( programObj, location, params );
}

void bglGetUniformivARB( GLhandleARB programObj, GLint location, GLint* params )
{
	glGetUniformivARB( programObj, location, params );
}

void bglGetShaderSourceARB( GLhandleARB obj, GLsizei maxLength, GLsizei* length, GLcharARB* source )
{
	glGetShaderSourceARB( obj, maxLength, length, source );
}

// ATI_draw_buffers
void bglDrawBuffersARB( GLsizei n, const GLenum* bufs )
{
	glDrawBuffersARB( n, bufs );
}

// GL_ARB_vertex_shader
// < inherited from GL_ARB_vertex_program >
void bglVertexAttrib1dARB( GLuint, GLdouble );
void bglVertexAttrib1dvARB( GLuint, const GLdouble * );
void bglVertexAttrib1fARB( GLuint, GLfloat );
void bglVertexAttrib1fvARB( GLuint, const GLfloat * );
void bglVertexAttrib1sARB( GLuint, GLshort );
void bglVertexAttrib1svARB( GLuint, const GLshort * );
void bglVertexAttrib2dARB( GLuint, GLdouble, GLdouble );
void bglVertexAttrib2dvARB( GLuint, const GLdouble * );
void bglVertexAttrib2fARB( GLuint, GLfloat, GLfloat );
void bglVertexAttrib2fvARB( GLuint, const GLfloat * );
void bglVertexAttrib2sARB( GLuint, GLshort, GLshort );
void bglVertexAttrib2svARB( GLuint, const GLshort * );
void bglVertexAttrib3dARB( GLuint, GLdouble, GLdouble, GLdouble );
void bglVertexAttrib3dvARB( GLuint, const GLdouble * );
void bglVertexAttrib3fARB( GLuint, GLfloat, GLfloat, GLfloat );
void bglVertexAttrib3fvARB( GLuint, const GLfloat * );
void bglVertexAttrib3sARB( GLuint, GLshort, GLshort, GLshort );
void bglVertexAttrib3svARB( GLuint, const GLshort * );
void bglVertexAttrib4NbvARB( GLuint, const GLbyte * );
void bglVertexAttrib4NivARB( GLuint, const GLint * );
void bglVertexAttrib4NsvARB( GLuint, const GLshort * );
void bglVertexAttrib4NubARB( GLuint, GLubyte, GLubyte, GLubyte, GLubyte );
void bglVertexAttrib4NubvARB( GLuint, const GLubyte * );
void bglVertexAttrib4NuivARB( GLuint, const GLuint * );
void bglVertexAttrib4NusvARB( GLuint, const GLushort * );
void bglVertexAttrib4bvARB( GLuint, const GLbyte * );
void bglVertexAttrib4dARB( GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
void bglVertexAttrib4dvARB( GLuint, const GLdouble * );
void bglVertexAttrib4fARB( GLuint, GLfloat, GLfloat, GLfloat, GLfloat );
void bglVertexAttrib4fvARB( GLuint, const GLfloat * );
void bglVertexAttrib4ivARB( GLuint, const GLint * );
void bglVertexAttrib4sARB( GLuint, GLshort, GLshort, GLshort, GLshort );
void bglVertexAttrib4svARB( GLuint, const GLshort * );
void bglVertexAttrib4ubvARB( GLuint, const GLubyte * );
void bglVertexAttrib4uivARB( GLuint, const GLuint * );
void bglVertexAttrib4usvARB( GLuint, const GLushort * );

void bglVertexAttribPointerARB( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr )
{
	glVertexAttribPointerARB( index, size, type, normalized, stride, ptr );
}

void bglEnableVertexAttribArrayARB( GLuint array )
{
	glEnableVertexAttribArrayARB( array );
}

void bglDisableVertexAttribArrayARB( GLuint array )
{
	glDisableVertexAttribArrayARB( array );
}

void bglProgramStringARB( GLenum target, GLenum format, GLsizei len, const GLvoid* string )
{
	glProgramStringARB( target, format, len, string );
}

void bglBindProgramARB( GLenum target, GLuint handle )
{
	glBindProgramARB( target, handle );
}

void bglDeleteProgramsARB( GLsizei n, const GLuint* programs )
{
	glDeleteProgramsARB( n, programs );
}

void bglGenProgramsARB( GLsizei n, GLuint* programs )
{
	glGenProgramsARB( n, programs );
}

void bglProgramEnvParameter4dARB( GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
void bglProgramEnvParameter4dvARB( GLenum, GLuint, const GLdouble * );
void bglProgramEnvParameter4fARB( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
	glProgramEnvParameter4fARB( target, index, x, y, z, w );
}

void bglProgramEnvParameter4fvARB( GLenum target, GLuint index, const GLfloat* params )
{
	glProgramEnvParameter4fvARB( target, index, params );
}

void bglProgramLocalParameter4dARB( GLenum, GLuint, GLdouble, GLdouble, GLdouble, GLdouble );
void bglProgramLocalParameter4dvARB( GLenum, GLuint, const GLdouble * );
void bglProgramLocalParameter4fARB( GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat );

void bglProgramLocalParameter4fvARB( GLenum target, GLuint index, const GLfloat* params )
{
	glProgramLocalParameter4fvARB( target, index, params );
}

void bglGetProgramEnvParameterdvARB( GLenum, GLuint, GLdouble * );
void bglGetProgramEnvParameterfvARB( GLenum, GLuint, GLfloat * );
void bglGetProgramLocalParameterdvARB( GLenum, GLuint, GLdouble * );
void bglGetProgramLocalParameterfvARB( GLenum, GLuint, GLfloat * );
void bglGetProgramivARB( GLenum, GLenum, GLint * );
void bglGetProgramStringARB( GLenum, GLenum, GLvoid * );
void bglGetVertexAttribdvARB( GLuint, GLenum, GLdouble * );
void bglGetVertexAttribfvARB( GLuint, GLenum, GLfloat * );
void bglGetVertexAttribivARB( GLuint, GLenum, GLint * );
void bglGetVertexAttribPointervARB( GLuint, GLenum, GLvoid* * );
// < /inherited from GL_ARB_vertex_program >

void bglBindAttribLocationARB( GLhandleARB program, GLuint index, const GLcharARB *name )
{
	glBindAttribLocationARB( program, index, name );
}

void bglGetActiveAttribARB( GLhandleARB, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLcharARB * );

GLint bglGetAttribLocationARB( GLhandleARB object, const GLcharARB *attrib )
{
	return glGetAttribLocationARB( object, attrib );
}

// GL_ARB_multisample
void bglSampleCoverageARB( GLclampf, GLboolean );

// GL_ARB_color_buffer_float
//void bglClampColorARB( GLenum target, GLenum clamp )
//{
//	glClampColorARB( target, clamp );
//}

// GL_EXT_framebuffer_object
GLboolean bglIsRenderbufferEXT( GLuint renderbuffer )
{
	return glIsRenderbufferEXT( renderbuffer );
}

void bglBindRenderbufferEXT( GLenum target, GLuint renderbuffer )
{
	glBindRenderbufferEXT( target, renderbuffer );
}

void bglDeleteRenderbuffersEXT( GLsizei n, const GLuint *renderbuffers )
{
	glDeleteRenderbuffersEXT( n, renderbuffers );
}

void bglGenRenderbuffersEXT( GLsizei n, GLuint *renderbuffers )
{
	glGenRenderbuffersEXT( n, renderbuffers );
}

void bglRenderbufferStorageEXT( GLenum target, GLenum internalformat, GLsizei width, GLsizei height )
{
	glRenderbufferStorageEXT( target, internalformat, width, height );
}

void bglGetRenderbufferParameterivEXT( GLenum target, GLenum pname, GLint* params )
{
	glGetRenderbufferParameterivEXT( target, pname, params );
}

GLboolean bglIsFramebufferEXT( GLuint framebuffer )
{
	return glIsFramebufferEXT( framebuffer );
}

void bglBindFramebufferEXT( GLenum target, GLuint framebuffer )
{
	glBindFramebufferEXT( target, framebuffer );
}

void bglDeleteFramebuffersEXT( GLsizei n, const GLuint *framebuffers )
{
	glDeleteFramebuffersEXT( n, framebuffers );
}

void bglGenFramebuffersEXT( GLsizei n, GLuint *framebuffers )
{
	glGenFramebuffersEXT( n, framebuffers );
}

GLenum bglCheckFramebufferStatusEXT( GLenum target )
{
	return glCheckFramebufferStatusEXT( target );
}

void bglFramebufferTexture1DEXT( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
	glFramebufferTexture1DEXT( target, attachment, textarget, texture, level );
}

void bglFramebufferTexture2DEXT( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
	glFramebufferTexture2DEXT( target, attachment, textarget, texture, level );
}

void bglFramebufferTexture3DEXT( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset )
{
	glFramebufferTexture3DEXT( target, attachment, textarget, texture, level, zoffset );
}

void bglFramebufferRenderbufferEXT( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )
{
	glFramebufferRenderbufferEXT( target, attachment, renderbuffertarget, renderbuffer );
}

void bglGetFramebufferAttachmentParameterivEXT( GLenum target, GLenum attachment, GLenum pname, GLint *params )
{
	glGetFramebufferAttachmentParameterivEXT( target, attachment, pname, params );
}

void bglGenerateMipmapEXT( GLenum target )
{
	glGenerateMipmapEXT( target );
}

// GL_EXT_depth_bounds_test
void bglDepthBoundsEXT( GLclampd zmin, GLclampd zmax )
{
	glDepthBoundsEXT( zmin, zmax );
}

// GL_NV_primitive_restart
void bglPrimitiveRestartNV( void );
void bglPrimitiveRestartIndexNV( GLuint );

// GL_NV_fence
void bglGenFencesNV( GLsizei n, GLuint* fences )
{
    B_ASSERT( glGenFencesNV );
    glGenFencesNV( n, fences );
}

void bglDeleteFencesNV( GLsizei n, const GLuint* fences )
{
    B_ASSERT( glDeleteFencesNV );
    glDeleteFencesNV( n, fences );
}

void bglSetFenceNV( GLuint fence, GLenum condition )
{
    B_ASSERT( glSetFenceNV );
    glSetFenceNV( fence, condition );
}

GLboolean bglTestFenceNV( GLuint fence )
{
    B_ASSERT( glTestFenceNV );
    return glTestFenceNV( fence );
}

void bglFinishFenceNV( GLuint fence )
{
    B_ASSERT( glFinishFenceNV );
    glFinishFenceNV( fence );
}

GLboolean bglIsFenceNV( GLuint fence )
{
    B_ASSERT( glIsFenceNV );
    return glIsFenceNV( fence );
}

void bglGetFenceivNV( GLuint fence, GLenum pname, GLint* params )
{
    B_ASSERT( glGetFenceivNV );
    glGetFenceivNV( fence, pname, params );
}

