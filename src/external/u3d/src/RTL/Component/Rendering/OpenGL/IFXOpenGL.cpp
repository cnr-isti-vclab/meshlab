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
// IFXOpenGL.cpp
#include "IFXRenderPCHOGL.h"

void IFXOpenGL::ClearOpenGLFuncs()
{
	glAccum =					0;
	glAlphaFunc =				0;
	glBegin =					0;
	glBindTexture =				0;
	glBlendFunc =				0;
	glCallList =				0;
	glClear =					0;
	glClearAccum =				0;
	glClearColor =				0;
	glClearDepth =				0;
	glClearStencil =			0;
	glColor3f =					0;
	glColor3fv =				0;
	glColor4f =					0;
	glColor4fv =				0;
	glColorMaterial =			0;
	glColorPointer =			0;
	glCullFace =				0;
	glDeleteLists =				0;
	glDeleteTextures =			0;
	glDepthFunc =				0;
	glDepthMask =				0;
	glDepthRange =				0;
	glDisable =					0;
	glDisableClientState =		0;
	glDrawElements =			0;
	glDrawPixels =				0;
	glEnable =					0;
	glEnableClientState =		0;
	glEnd =						0;
	glEndList =					0;
	glFinish =					0;
	glFlush =					0;
	glFogf =					0;
	glFogfv =					0;
	glFogi =					0;
	glFogiv =					0;
	glFrontFace =				0;
	glGenLists =				0;
	glGenTextures =				0;
	glGetBooleanv =				0;
	glGetClipPlane =			0;
	glGetError =				0;
	glGetFloatv =				0;
	glGetIntegerv =				0;
	glGetLightfv =				0;
	glGetLightiv =				0;
	glGetMaterialfv =			0;
	glGetMaterialiv =			0;
	glGetString =				0;
	glGetTexEnvfv =				0;
	glGetTexEnviv =				0;
	glGetTexImage =				0;
	glGetTexLevelParameterfv =	0;
	glGetTexLevelParameteriv =	0;
	glGetTexParameterfv =		0;
	glGetTexParameteriv =		0;
	glHint =					0;
	glIsEnabled =				0;
	glIsList =					0;
	glIsTexture =				0;
	glLightf =					0;
	glLightfv =					0;
	glLighti =					0;
	glLightiv =					0;
	glLightModelfv =			0;
	glLightModeli =				0;
	glLineWidth =				0;
	glLoadIdentity =			0;
	glLoadMatrixf =				0;
	glMaterialf =				0;
	glMaterialfv =				0;
	glMateriali =				0;
	glMaterialiv =				0;
	glMatrixMode =				0;
	glMultMatrixf =				0;
	glNewList =					0;
	glNormalPointer =			0;
	glOrtho =					0;
	glPixelStorei =				0;
	glPointSize =				0;
	glPolygonMode =				0;
	glPolygonOffset =			0;
	glPopMatrix =				0;
	glPushMatrix =				0;
	glRasterPos2i =				0;
	glReadBuffer =				0;
	glReadPixels =				0;
	glRenderMode =				0;
	glScissor =					0;
	glSelectBuffer =			0;
	glShadeModel =				0;
	glStencilFunc =				0;
	glStencilMask =				0;
	glStencilOp =				0;
	glTexCoordPointer =			0;
	glTexEnvf =					0;
	glTexEnvfv =				0;
	glTexEnvi =					0;
	glTexEnviv =				0;
	glTexGenf =					0;
	glTexGenfv =				0;
	glTexGeni =					0;
	glTexGeniv =				0;
	glTexImage2D =				0;
	glTexParameterf =			0;
	glTexParameterfv =			0;
	glTexParameteri =			0;
	glTexParameteriv =			0;
	glTexSubImage2D =			0;
	glVertex3f =				0;
	glVertexPointer =			0;
	glViewport =				0;

	glDrawRangeElements =		0;
	glActiveTextureARB =		0;
	glClientActiveTextureARB =	0;
	glCompressedTexImage3DARB =	0;
	glCompressedTexImage2DARB =	0;
	glCompressedTexImage1DARB =	0;
	glCompressedTexSubImage3DARB = 0;
	glCompressedTexSubImage2DARB = 0;
	glCompressedTexSubImage1DARB = 0;
	glGetCompressedTexImageARB = 0;
}

void IFXOpenGL::GetOpenGLFuncs()
{
	glAccum =					(GLAccum)					GetOpenGLFunc("glAccum");
	glAlphaFunc =				(GLAlphaFunc)				GetOpenGLFunc("glAlphaFunc");
	glBegin =					(GLBegin)					GetOpenGLFunc("glBegin");
	glBindTexture =				(GLBindTexture)				GetOpenGLFunc("glBindTexture");
	glBlendFunc =				(GLBlendFunc)				GetOpenGLFunc("glBlendFunc");
	glCallList =				(GLCallList)				GetOpenGLFunc("glCallList");
	glClear =					(GLClear)					GetOpenGLFunc("glClear");
	glClearAccum =				(GLClearAccum)				GetOpenGLFunc("glClearAccum");
	glClearColor =				(GLClearColor)				GetOpenGLFunc("glClearColor");
	glClearDepth =				(GLClearDepth)				GetOpenGLFunc("glClearDepth");
	glClearStencil =			(GLClearStencil)			GetOpenGLFunc("glClearStencil");
	glColor3f =					(GLColor3f)					GetOpenGLFunc("glColor3f");
	glColor3fv =				(GLColor3fv)				GetOpenGLFunc("glColor3fv");
	glColor4f =					(GLColor4f)					GetOpenGLFunc("glColor4f");
	glColor4fv =				(GLColor4fv)				GetOpenGLFunc("glColor4fv");
	glColorMaterial =			(GLColorMaterial)			GetOpenGLFunc("glColorMaterial");
	glColorPointer =			(GLColorPointer)			GetOpenGLFunc("glColorPointer");
	glCullFace =				(GLCullFace)				GetOpenGLFunc("glCullFace");
	glDeleteLists =				(GLDeleteLists)				GetOpenGLFunc("glDeleteLists");
	glDeleteTextures =			(GLDeleteTextures)			GetOpenGLFunc("glDeleteTextures");
	glDepthFunc =				(GLDepthFunc)				GetOpenGLFunc("glDepthFunc");
	glDepthMask =				(GLDepthMask)				GetOpenGLFunc("glDepthMask");
	glDepthRange =				(GLDepthRange)				GetOpenGLFunc("glDepthRange");
	glDisable =					(GLDisable)					GetOpenGLFunc("glDisable");
	glDisableClientState =		(GLDisableClientState)		GetOpenGLFunc("glDisableClientState");
	glDrawElements =			(GLDrawElements)			GetOpenGLFunc("glDrawElements");
	glDrawPixels =				(GLDrawPixels)				GetOpenGLFunc("glDrawPixels");
	glEnable =					(GLEnable)					GetOpenGLFunc("glEnable");
	glEnableClientState =		(GLEnableClientState)		GetOpenGLFunc("glEnableClientState");
	glEnd =						(GLEnd)						GetOpenGLFunc("glEnd");
	glEndList =					(GLEndList)					GetOpenGLFunc("glEndList");
	glFinish =					(GLFinish)					GetOpenGLFunc("glFinish");
	glFlush =					(GLFlush)					GetOpenGLFunc("glFlush");
	glFogf =					(GLFogf)					GetOpenGLFunc("glFogf");
	glFogfv =					(GLFogfv)					GetOpenGLFunc("glFogfv");
	glFogi =					(GLFogi)					GetOpenGLFunc("glFogi");
	glFogiv =					(GLFogiv)					GetOpenGLFunc("glFogiv");
	glFrontFace =				(GLFrontFace)				GetOpenGLFunc("glFrontFace");
	glGenLists =				(GLGenLists)				GetOpenGLFunc("glGenLists");
	glGenTextures =				(GLGenTextures)				GetOpenGLFunc("glGenTextures");
	glGetBooleanv =				(GLGetBooleanv)				GetOpenGLFunc("glGetBooleanv");
	glGetClipPlane =			(GLGetClipPlane)			GetOpenGLFunc("glGetClipPlane");
	glGetError =				(GLGetError)				GetOpenGLFunc("glGetError");
	glGetFloatv =				(GLGetFloatv)				GetOpenGLFunc("glGetFloatv");
	glGetIntegerv =				(GLGetIntegerv)				GetOpenGLFunc("glGetIntegerv");
	glGetLightfv =				(GLGetLightfv)				GetOpenGLFunc("glGetLightfv");
	glGetLightiv =				(GLGetLightiv)				GetOpenGLFunc("glGetLightiv");
	glGetMaterialfv =			(GLGetMaterialfv)			GetOpenGLFunc("glGetMaterialfv");
	glGetMaterialiv =			(GLGetMaterialiv)			GetOpenGLFunc("glGetMaterialiv");
	glGetString =				(GLGetString)				GetOpenGLFunc("glGetString");
	glGetTexEnvfv =				(GLGetTexEnvfv)				GetOpenGLFunc("glGetTexEnvfv");
	glGetTexEnviv =				(GLGetTexEnviv)				GetOpenGLFunc("glGetTexEnviv");
	glGetTexImage =				(GLGetTexImage)				GetOpenGLFunc("glGetTexImage");
	glGetTexLevelParameterfv =	(GLGetTexLevelParameterfv)	GetOpenGLFunc("glGetTexLevelParameterfv");
	glGetTexLevelParameteriv =	(GLGetTexLevelParameteriv)	GetOpenGLFunc("glGetTexLevelParameteriv");
	glGetTexParameterfv =		(GLGetTexParameterfv)		GetOpenGLFunc("glGetTexParameterfv");
	glGetTexParameteriv =		(GLGetTexParameteriv)		GetOpenGLFunc("glGetTexParameteriv");
	glHint =					(GLHint)					GetOpenGLFunc("glHint");
	glIsEnabled =				(GLIsEnabled)				GetOpenGLFunc("glIsEnabled");
	glIsList =					(GLIsList)					GetOpenGLFunc("glIsList");
	glIsTexture =				(GLIsTexture)				GetOpenGLFunc("glIsTexture");
	glLightf =					(GLLightf)					GetOpenGLFunc("glLightf");
	glLightfv =					(GLLightfv)					GetOpenGLFunc("glLightfv");
	glLighti =					(GLLighti)					GetOpenGLFunc("glLighti");
	glLightiv =					(GLLightiv)					GetOpenGLFunc("glLightiv");
	glLightModelfv =			(GLLightModelfv)			GetOpenGLFunc("glLightModelfv");
	glLightModeli =				(GLLightModeli)				GetOpenGLFunc("glLightModeli");
	glLineWidth =				(GLLineWidth)				GetOpenGLFunc("glLineWidth");
	glLoadIdentity =			(GLLoadIdentity)			GetOpenGLFunc("glLoadIdentity");
	glLoadMatrixf =				(GLLoadMatrixf)				GetOpenGLFunc("glLoadMatrixf");
	glMaterialf =				(GLMaterialf)				GetOpenGLFunc("glMaterialf");
	glMaterialfv =				(GLMaterialfv)				GetOpenGLFunc("glMaterialfv");
	glMateriali =				(GLMateriali)				GetOpenGLFunc("glMateriali");
	glMaterialiv =				(GLMaterialiv)				GetOpenGLFunc("glMaterialiv");
	glMatrixMode =				(GLMatrixMode)				GetOpenGLFunc("glMatrixMode");
	glMultMatrixf =				(GLMultMatrixf)				GetOpenGLFunc("glMultMatrixf");
	glNewList =					(GLNewList)					GetOpenGLFunc("glNewList");
	glNormalPointer =			(GLNormalPointer)			GetOpenGLFunc("glNormalPointer");
	glOrtho =					(GLOrtho)					GetOpenGLFunc("glOrtho");
	glPixelStorei =				(GLPixelStorei)				GetOpenGLFunc("glPixelStorei");
	glPointSize =				(GLPointSize)				GetOpenGLFunc("glPointSize");
	glPolygonMode =				(GLPolygonMode)				GetOpenGLFunc("glPolygonMode");
	glPolygonOffset =			(GLPolygonOffset)			GetOpenGLFunc("glPolygonOffset");
	glPopMatrix =				(GLPopMatrix)				GetOpenGLFunc("glPopMatrix");
	glPushMatrix =				(GLPushMatrix)				GetOpenGLFunc("glPushMatrix");
	glRasterPos2i =				(GLRasterPos2i)				GetOpenGLFunc("glRasterPos2i");
	glReadBuffer =				(GLReadBuffer)				GetOpenGLFunc("glReadBuffer");
	glReadPixels =				(GLReadPixels)				GetOpenGLFunc("glReadPixels");
	glRenderMode =				(GLRenderMode)				GetOpenGLFunc("glRenderMode");
	glScissor =					(GLScissor)					GetOpenGLFunc("glScissor");
	glSelectBuffer =			(GLSelectBuffer)			GetOpenGLFunc("glSelectBuffer");
	glShadeModel =				(GLShadeModel)				GetOpenGLFunc("glShadeModel");
	glStencilFunc =				(GLStencilFunc)				GetOpenGLFunc("glStencilFunc");
	glStencilMask =				(GLStencilMask)				GetOpenGLFunc("glStencilMask");
	glStencilOp =				(GLStencilOp)				GetOpenGLFunc("glStencilOp");
	glTexCoordPointer =			(GLTexCoordPointer)			GetOpenGLFunc("glTexCoordPointer");
	glTexEnvf =					(GLTexEnvf)					GetOpenGLFunc("glTexEnvf");
	glTexEnvfv =				(GLTexEnvfv)				GetOpenGLFunc("glTexEnvfv");
	glTexEnvi =					(GLTexEnvi)					GetOpenGLFunc("glTexEnvi");
	glTexEnviv =				(GLTexEnviv)				GetOpenGLFunc("glTexEnviv");
	glTexGenf =					(GLTexGenf)					GetOpenGLFunc("glTexGenf");
	glTexGenfv =				(GLTexGenfv)				GetOpenGLFunc("glTexGenfv");
	glTexGeni =					(GLTexGeni)					GetOpenGLFunc("glTexGeni");
	glTexGeniv =				(GLTexGeniv)				GetOpenGLFunc("glTexGeniv");
	glTexImage2D =				(GLTexImage2D)				GetOpenGLFunc("glTexImage2D");
	glTexParameterf =			(GLTexParameterf)			GetOpenGLFunc("glTexParameterf");
	glTexParameterfv =			(GLTexParameterfv)			GetOpenGLFunc("glTexParameterfv");
	glTexParameteri =			(GLTexParameteri)			GetOpenGLFunc("glTexParameteri");
	glTexParameteriv =			(GLTexParameteriv)			GetOpenGLFunc("glTexParameteriv");
	glTexSubImage2D =			(GLTexSubImage2D)			GetOpenGLFunc("glTexSubImage2D");
	glVertex3f =				(GLVertex3f)				GetOpenGLFunc("glVertex3f");
	glVertexPointer =			(GLVertexPointer)			GetOpenGLFunc("glVertexPointer");
	glViewport =				(GLViewport)				GetOpenGLFunc("glViewport");
}

