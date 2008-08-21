/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "rfx_rendertarget.h"

RfxRenderTarget::RfxRenderTarget(const QString& rtName)
{
	name = rtName;
	width = 0;
	height = 0;
	texture = 0;
	fbo = 0;
}

RfxRenderTarget::~RfxRenderTarget()
{
	glDeleteRenderbuffersEXT(1, &depthbuffer);
	glDeleteTextures(1, &texture);
	glDeleteFramebuffersEXT(1, &fbo);
}

void RfxRenderTarget::SetClear(int pass, float depthClear, float *colorClear)
{
	passOptions[pass].clearMask = 0;

	// valid depth clear values are [0, 1]
	// -1.0 disable depth clear
	if (depthClear == -1.0) {
		passOptions[pass].depthClear = false;
	} else {
		passOptions[pass].depthClearVal = depthClear;
		passOptions[pass].clearMask |= GL_DEPTH_BUFFER_BIT;
	}

	// color should be a 4-elements array.
	// NULL disable color clear
	if (colorClear == NULL) {
		passOptions[pass].colorClear = false;
	} else {
		passOptions[pass].colorClearVal = colorClear;
		passOptions[pass].clearMask |= GL_COLOR_BUFFER_BIT;
	}
}

bool RfxRenderTarget::Setup(int pass)
{
	if (!GLEW_EXT_framebuffer_object) {
		qWarning("FBO not supported!");
		return false;
	}

	// create depth renderbuffer
	glGenRenderbuffersEXT(1, &depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
	                         width, height);

	// create empty texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  width, height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, NULL);

	// set texture state based on the first uniform that will use RT
	QList<int> k = passStates.keys();
	for (int i = 0; i < k.size(); ++i) {
		if (k.at(i) > pass) {
			foreach (RfxState *s, passStates.value(i))
				s->SetEnvironment(GL_TEXTURE_2D);
		}
	}

	// create FBO and attach depth and color buffer
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
	                             GL_RENDERBUFFER_EXT, depthbuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
	                          GL_TEXTURE_2D, texture, 0);

	// check if everything went ok
	GLint status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	return (status == GL_FRAMEBUFFER_COMPLETE_EXT);
}

void RfxRenderTarget::Bind(int pass)
{
	bool colClear = passOptions.value(pass).colorClear;
	bool depClear = passOptions.value(pass).depthClear;

	if (colClear) {
		GLfloat *cols = passOptions.value(pass).colorClearVal;
		glClearColor(cols[0], cols[1], cols[2], cols[3]);
	}

	if (depClear)
		glClearDepth(passOptions.value(pass).depthClearVal);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	if (colClear || depClear)
		glClear(passOptions.value(pass).clearMask);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);
}

void RfxRenderTarget::Unbind()
{
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
