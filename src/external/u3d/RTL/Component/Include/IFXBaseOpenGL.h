//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
// IFXBaseOpenGL.h
#ifndef IFX_BASE_OPENGL_H
#define IFX_BASE_OPENGL_H

#include "IFXUnknown.h"
#include "IFXMesh.h"

typedef int (APIENTRY *IFXOGLPROC)();
// OpenGL 1.1 base
typedef void (APIENTRY * GLAccum)       (GLenum, GLfloat);
typedef void (APIENTRY * GLAlphaFunc)     (GLenum, GLclampf);
typedef void (APIENTRY * GLBegin)       (GLenum);
typedef void (APIENTRY * GLBindTexture)   (GLenum, GLuint );
typedef void (APIENTRY * GLBlendFunc )    (GLenum, GLenum );
typedef void (APIENTRY * GLCallList)      (GLuint);
typedef void (APIENTRY * GLClear)       (GLbitfield);
typedef void (APIENTRY * GLClearAccum )   (GLfloat, GLfloat , GLfloat , GLfloat );
typedef void (APIENTRY * GLClearColor )   (GLclampf, GLclampf , GLclampf , GLclampf );
typedef void (APIENTRY * GLClearDepth )   (GLclampd);
typedef void (APIENTRY * GLClearStencil)    (GLint);
typedef void (APIENTRY * GLClipPlane)     (GLenum , const GLdouble *);
typedef void (APIENTRY * GLColor3f )      (GLfloat, GLfloat , GLfloat );
typedef void (APIENTRY * GLColor3fv )     (const GLfloat *);
typedef void (APIENTRY * GLColor4f )      (GLfloat, GLfloat , GLfloat, GLfloat );
typedef void (APIENTRY * GLColor4fv )     (const GLfloat *);
typedef void (APIENTRY * GLColorMaterial )  (GLenum, GLenum);
typedef void (APIENTRY * GLColorPointer )   (GLint , GLenum , GLsizei , const GLvoid *);
typedef void (APIENTRY * GLCullFace )     (GLenum);
typedef void (APIENTRY * GLDeleteLists )    (GLuint , GLsizei );
typedef void (APIENTRY * GLDeleteTextures ) (GLsizei , const GLuint *);
typedef void (APIENTRY * GLDepthFunc )    (GLenum );
typedef void (APIENTRY * GLDepthMask)     (GLboolean);
typedef void (APIENTRY * GLDepthRange )   (GLclampd , GLclampd );
typedef void (APIENTRY * GLDisable )      (GLenum );
typedef void (APIENTRY * GLDisableClientState)(GLenum );
typedef void (APIENTRY * GLDrawElements )   (GLenum , GLsizei , GLenum , const GLvoid *);
typedef void (APIENTRY * GLDrawPixels )   (GLsizei , GLsizei , GLenum , GLenum , const GLvoid *);
typedef void (APIENTRY * GLEnable )     (GLenum );
typedef void (APIENTRY * GLEnableClientState )(GLenum );
typedef void (APIENTRY * GLEnd )        (void);
typedef void (APIENTRY * GLEndList )      (void);
typedef GLenum (APIENTRY * GLGetError )   (void);
typedef void (APIENTRY * GLFinish )     (void);
typedef void (APIENTRY * GLFlush )      (void);
typedef void (APIENTRY * GLFogf )       (GLenum , GLfloat );
typedef void (APIENTRY * GLFogfv )      (GLenum , const GLfloat *);
typedef void (APIENTRY * GLFogi )       (GLenum , GLint );
typedef void (APIENTRY * GLFogiv )      (GLenum , const GLint *);
typedef void (APIENTRY * GLFrontFace )    (GLenum );
typedef GLuint (APIENTRY * GLGenLists)    (GLsizei);
typedef void (APIENTRY * GLGenTextures )    (GLsizei, GLuint * );
typedef void (APIENTRY * GLGetBooleanv )    (GLenum , GLboolean *);
typedef void (APIENTRY * GLGetClipPlane )   (GLenum , GLdouble *);
typedef void (APIENTRY * GLGetFloatv )    (GLenum , GLfloat *);
typedef void (APIENTRY * GLGetIntegerv )    (GLenum , GLint *);
typedef void (APIENTRY * GLGetLightfv )   (GLenum , GLenum , GLfloat *);
typedef void (APIENTRY * GLGetLightiv )   (GLenum , GLenum , GLint *);
typedef void (APIENTRY * GLGetMaterialfv )  (GLenum , GLenum , GLfloat *);
typedef void (APIENTRY * GLGetMaterialiv )  (GLenum , GLenum , GLint *);
typedef const GLubyte * (APIENTRY * GLGetString )(GLenum );
typedef void (APIENTRY * GLGetTexEnvfv )    (GLenum , GLenum , GLfloat *);
typedef void (APIENTRY * GLGetTexEnviv )    (GLenum , GLenum , GLint *);
typedef void (APIENTRY * GLGetTexImage )    (GLenum , GLint, GLenum, GLenum , GLvoid *);
typedef void (APIENTRY * GLGetTexLevelParameterfv )(GLenum , GLint, GLenum , GLfloat *);
typedef void (APIENTRY * GLGetTexLevelParameteriv )(GLenum , GLint, GLenum , GLint *);
typedef void (APIENTRY * GLGetTexParameterfv )(GLenum , GLenum , GLfloat *);
typedef void (APIENTRY * GLGetTexParameteriv )(GLenum , GLenum , GLint *);
typedef void (APIENTRY * GLHint )       (GLenum , GLenum );
typedef GLboolean (APIENTRY * GLIsEnabled ) (GLenum );
typedef GLboolean (APIENTRY * GLIsList )    (GLuint );
typedef GLboolean (APIENTRY * GLIsTexture ) (GLuint );
typedef void (APIENTRY * GLLightf )     (GLenum , GLenum , GLfloat );
typedef void (APIENTRY * GLLightfv )      (GLenum , GLenum , const GLfloat *);
typedef void (APIENTRY * GLLighti )     (GLenum , GLenum , GLint );
typedef void (APIENTRY * GLLightiv )      (GLenum , GLenum , const GLint *);
typedef void (APIENTRY * GLLightModelfv)    (GLenum, const GLfloat *);
typedef void (APIENTRY * GLLightModeli)   (GLenum, GLint);
typedef void (APIENTRY * GLLineWidth )    (GLfloat );
typedef void (APIENTRY * GLLoadIdentity )   (void);
typedef void (APIENTRY * GLLoadMatrixf )    (const GLfloat *);
typedef void (APIENTRY * GLMaterialf )    (GLenum , GLenum , GLfloat );
typedef void (APIENTRY * GLMaterialfv )   (GLenum , GLenum , const GLfloat *);
typedef void (APIENTRY * GLMateriali )    (GLenum , GLenum , GLint );
typedef void (APIENTRY * GLMaterialiv )   (GLenum , GLenum , const GLint *);
typedef void (APIENTRY * GLMatrixMode )   (GLenum );
typedef void (APIENTRY * GLMultMatrixf )    (const GLfloat *);
typedef void (APIENTRY * GLNewList )      (GLuint , GLenum );
typedef void (APIENTRY * GLNormalPointer )  (GLenum , GLsizei , const GLvoid *);
typedef void (APIENTRY * GLOrtho)       (GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
typedef void (APIENTRY * GLPixelStorei )    (GLenum, GLint);
typedef void (APIENTRY * GLPointSize )    (GLfloat );
typedef void (APIENTRY * GLPolygonMode )    (GLenum , GLenum );
typedef void (APIENTRY * GLPolygonOffset )  (GLfloat , GLfloat );
typedef void (APIENTRY * GLPopMatrix )    (void);
typedef void (APIENTRY * GLPushMatrix )   (void);
typedef void (APIENTRY * GLRasterPos2i)   (GLint, GLint);
typedef void (APIENTRY * GLReadBuffer )   (GLenum );
typedef void (APIENTRY * GLReadPixels )   (GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid *);
typedef GLint (APIENTRY * GLRenderMode )    (GLenum );
typedef void (APIENTRY * GLScissor )      (GLint , GLint , GLsizei , GLsizei );
typedef void (APIENTRY * GLSelectBuffer )   (GLsizei , GLuint *buffer);
typedef void (APIENTRY * GLShadeModel )   (GLenum );
typedef void (APIENTRY * GLStencilFunc )    (GLenum , GLint ref, GLuint );
typedef void (APIENTRY * GLStencilMask )    (GLuint );
typedef void (APIENTRY * GLStencilOp )    (GLenum , GLenum , GLenum );
typedef void (APIENTRY * GLTexCoordPointer )  (GLint , GLenum , GLsizei , const GLvoid *);
typedef void (APIENTRY * GLTexEnvf )      (GLenum , GLenum , GLfloat );
typedef void (APIENTRY * GLTexEnvfv )     (GLenum , GLenum , const GLfloat *);
typedef void (APIENTRY * GLTexEnvi )      (GLenum , GLenum , GLint );
typedef void (APIENTRY * GLTexEnviv )     (GLenum , GLenum , const GLint *);
typedef void (APIENTRY * GLTexGend )      (GLenum , GLenum , GLdouble );
typedef void (APIENTRY * GLTexGendv )     (GLenum , GLenum , const GLdouble *);
typedef void (APIENTRY * GLTexGenf )      (GLenum , GLenum , GLfloat );
typedef void (APIENTRY * GLTexGenfv )     (GLenum , GLenum , const GLfloat *);
typedef void (APIENTRY * GLTexGeni )      (GLenum , GLenum , GLint );
typedef void (APIENTRY * GLTexGeniv )     (GLenum , GLenum , const GLint *);
typedef void (APIENTRY * GLTexImage2D )   (GLenum , GLint , GLint , GLsizei , GLsizei , GLint , GLenum , GLenum , const GLvoid *);
typedef void (APIENTRY * GLTexParameterf )  (GLenum , GLenum , GLfloat );
typedef void (APIENTRY * GLTexParameterfv ) (GLenum , GLenum , const GLfloat *);
typedef void (APIENTRY * GLTexParameteri )  (GLenum , GLenum , GLint );
typedef void (APIENTRY * GLTexParameteriv ) (GLenum , GLenum , const GLint *);
typedef void (APIENTRY * GLTexSubImage2D)   (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
typedef void (APIENTRY * GLVertex3f)      (GLfloat , GLfloat , GLfloat );
typedef void (APIENTRY * GLVertexPointer )  (GLint , GLenum , GLsizei , const GLvoid *);
typedef void (APIENTRY * GLViewport )     (GLint , GLint , GLsizei , GLsizei );

// OpenGL 1.2 and common extensions
typedef void (APIENTRY * GLDrawRangeElements) (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
typedef void (APIENTRY * GLActiveTextureARB) (GLenum texture);
typedef void (APIENTRY * GLClientActiveTextureARB) (GLenum texture);
typedef void (APIENTRY * GLCompressedTexImage3DARB) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * GLCompressedTexImage2DARB) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * GLCompressedTexImage1DARB) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * GLCompressedTexSubImage3DARB) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * GLCompressedTexSubImage2DARB) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * GLCompressedTexSubImage1DARB) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (APIENTRY * GLGetCompressedTexImageARB) (GLenum target, GLint level, void *img);

// {C10CB345-1BCF-43f7-AC8E-ED81F1998436}
IFXDEFINE_GUID(IID_IFXOpenGL,
         0xc10cb345, 0x1bcf, 0x43f7, 0xac, 0x8e, 0xed, 0x81, 0xf1, 0x99, 0x84, 0x36);

class IFXOpenGL : virtual public IFXUnknown//IFXObject
{
  //  IFX_DECLARE_IDENTITY(IFXOpenGL);
public:
  virtual IFXRESULT IFXAPI LoadOpenGL()=0;
  virtual IFXRESULT IFXAPI UnloadOpenGL()=0;

  /**
  *  Loads OS dependent OpenGL function pointers.  Most of these are in the
  *  agl, glX, or wgl family of functions.
  */
  virtual IFXRESULT IFXAPI LoadOSGL()=0;

  /**
  *  Loads function pointers for needed GL extensions.  This must be
  *  called @b after a valid OpenGL context is made current.
  */
  virtual IFXRESULT IFXAPI LoadExtensions()=0;

  BOOL IFXAPI TexEnvCombineSupport() const { return m_bTexCombine; }
  BOOL IFXAPI MultiTexSupport() const { return m_bMultiTexture; }
  BOOL IFXAPI CubeTextureSupport() const { return m_bCubeTexture; }
  BOOL IFXAPI BGRASupport() const { return m_bBGRA; }
  BOOL IFXAPI PackedPixelSupport() const { return m_bPackedPixels; }
  BOOL IFXAPI AutoMipMapSupport() const { return m_bAutoMipMap; }
  BOOL IFXAPI SeparateSpecularSupport() const { return m_bSeparateSpecular; }

  //static void IFXAPI Shutdown();

  BOOL m_bValidOpenGL;

  // Function pointer prototypes
  GLAccum           glAccum;
  GLAlphaFunc         glAlphaFunc;
  GLBegin           glBegin;
  GLBindTexture       glBindTexture;
  GLBlendFunc         glBlendFunc;
  GLCallList          glCallList;
  GLClear           glClear;
  GLClearAccum        glClearAccum;
  GLClearColor        glClearColor;
  GLClearDepth        glClearDepth;
  GLClearStencil        glClearStencil;
  GLColor3f         glColor3f;
  GLColor3fv          glColor3fv;
  GLColor4f         glColor4f;
  GLColor4fv          glColor4fv;
  GLColorMaterial       glColorMaterial;
  GLColorPointer        glColorPointer;
  GLCullFace          glCullFace;
  GLDeleteLists       glDeleteLists;
  GLDeleteTextures      glDeleteTextures;
  GLDepthFunc         glDepthFunc;
  GLDepthMask         glDepthMask;
  GLDepthRange        glDepthRange;
  GLDisable         glDisable;
  GLDisableClientState    glDisableClientState;
  GLDrawElements        glDrawElements;
  GLDrawPixels        glDrawPixels;
  GLEnable          glEnable;
  GLEnableClientState     glEnableClientState;
  GLEnd           glEnd;
  GLEndList         glEndList;
  GLFinish          glFinish;
  GLFlush           glFlush;
  GLFogf            glFogf;
  GLFogfv           glFogfv;
  GLFogi            glFogi;
  GLFogiv           glFogiv;
  GLFrontFace         glFrontFace;
  GLGenLists          glGenLists;
  GLGenTextures       glGenTextures;
  GLGetBooleanv       glGetBooleanv;
  GLGetClipPlane        glGetClipPlane;
  GLGetError          glGetError;
  GLGetFloatv         glGetFloatv;
  GLGetIntegerv       glGetIntegerv;
  GLGetLightfv        glGetLightfv;
  GLGetLightiv        glGetLightiv;
  GLGetMaterialfv       glGetMaterialfv;
  GLGetMaterialiv       glGetMaterialiv;
  GLGetString         glGetString;
  GLGetTexEnvfv       glGetTexEnvfv;
  GLGetTexEnviv       glGetTexEnviv;
  GLGetTexImage       glGetTexImage;
  GLGetTexLevelParameterfv  glGetTexLevelParameterfv;
  GLGetTexLevelParameteriv  glGetTexLevelParameteriv;
  GLGetTexParameterfv     glGetTexParameterfv;
  GLGetTexParameteriv     glGetTexParameteriv;
  GLHint            glHint;
  GLIsEnabled         glIsEnabled;
  GLIsList          glIsList;
  GLIsTexture         glIsTexture;
  GLLightf          glLightf;
  GLLightfv         glLightfv;
  GLLighti          glLighti;
  GLLightiv         glLightiv;
  GLLightModelfv        glLightModelfv;
  GLLightModeli       glLightModeli;
  GLLineWidth         glLineWidth;
  GLLoadIdentity        glLoadIdentity;
  GLLoadMatrixf       glLoadMatrixf;
  GLMaterialf         glMaterialf;
  GLMaterialfv        glMaterialfv;
  GLMateriali         glMateriali;
  GLMaterialiv        glMaterialiv;
  GLMatrixMode        glMatrixMode;
  GLMultMatrixf       glMultMatrixf;
  GLNewList         glNewList;
  GLNormalPointer       glNormalPointer;
  GLOrtho           glOrtho;
  GLPixelStorei       glPixelStorei;
  GLPointSize         glPointSize;
  GLPolygonMode       glPolygonMode;
  GLPolygonOffset       glPolygonOffset;
  GLPopMatrix         glPopMatrix;
  GLPushMatrix        glPushMatrix;
  GLRasterPos2i       glRasterPos2i;
  GLReadBuffer        glReadBuffer;
  GLReadPixels        glReadPixels;
  GLRenderMode        glRenderMode;
  GLScissor         glScissor;
  GLSelectBuffer        glSelectBuffer;
  GLShadeModel        glShadeModel;
  GLStencilFunc       glStencilFunc;
  GLStencilMask       glStencilMask;
  GLStencilOp         glStencilOp;
  GLTexCoordPointer     glTexCoordPointer;
  GLTexEnvf         glTexEnvf;
  GLTexEnvfv          glTexEnvfv;
  GLTexEnvi         glTexEnvi;
  GLTexEnviv          glTexEnviv;
  GLTexGenf         glTexGenf;
  GLTexGenfv          glTexGenfv;
  GLTexGeni         glTexGeni;
  GLTexGeniv          glTexGeniv;
  GLTexImage2D        glTexImage2D;
  GLTexParameterf       glTexParameterf;
  GLTexParameterfv      glTexParameterfv;
  GLTexParameteri       glTexParameteri;
  GLTexParameteriv      glTexParameteriv;
  GLTexSubImage2D       glTexSubImage2D;
  GLVertex3f          glVertex3f;
  GLVertexPointer       glVertexPointer;
  GLViewport          glViewport;

  // OGL 1.2 funcs and GL_EXT funcs that we may use
  GLDrawRangeElements         glDrawRangeElements;
  GLActiveTextureARB          glActiveTextureARB;
  GLClientActiveTextureARB      glClientActiveTextureARB;
  GLCompressedTexImage3DARB     glCompressedTexImage3DARB;
  GLCompressedTexImage2DARB     glCompressedTexImage2DARB;
  GLCompressedTexImage1DARB     glCompressedTexImage1DARB;
  GLCompressedTexSubImage3DARB    glCompressedTexSubImage3DARB;
  GLCompressedTexSubImage2DARB    glCompressedTexSubImage2DARB;
  GLCompressedTexSubImage1DARB    glCompressedTexSubImage1DARB;
  GLGetCompressedTexImageARB      glGetCompressedTexImageARB;

protected:
  virtual IFXOGLPROC IFXAPI GetOpenGLFunc(const char* szFuncName) = 0;
  void IFXAPI ClearOpenGLFuncs();
  void IFXAPI GetOpenGLFuncs();
  IFXOpenGL()
  {
    m_bMultiTexture = 0;
    m_bTexCompress = 0;
    m_bPackedPixels = 0;
    m_bBGRA = 0;
    m_bSeparateSpecular = 0;
    m_bTexCombine = 0;
    m_bTexAdd = 0;
    m_bTexDot3 = 0;
    m_bCubeTexture = 0;
    m_bOpenGL_1_2 = 0;
    m_bAutoMipMap = 0;

    m_bOpenGLLoaded = FALSE;
    m_bValidOpenGL = FALSE;
  }

  virtual ~IFXOpenGL() {}

  BOOL m_bOpenGLLoaded;

  // A Bunch of BOOLS for determining whether or not extensions are supported
  U32 m_bMultiTexture : 1;
  U32 m_bTexCompress : 1;
  U32 m_bPackedPixels : 1;
  U32 m_bBGRA : 1;
  U32 m_bSeparateSpecular : 1;
  U32 m_bTexCombine : 1;
  U32 m_bTexAdd : 1;
  U32 m_bTexDot3 : 1;
  U32 m_bCubeTexture : 1;
  U32 m_bOpenGL_1_2 : 1;
  U32 m_bAutoMipMap : 1;

  /// @todo: fill in more extension flags as needed.
};

//#define IFX_LOG_OGL

#ifdef IFX_LOG_OGL
#define IFXLOGOGL IFXTRACE_GENERIC
#else
#define IFXLOGOGL
#endif // IFX_LOG_OGL


#endif // IFX_BASE_OPENGL_H

// END OF FILE
