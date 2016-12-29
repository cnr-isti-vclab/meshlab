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
	fbo = 0;
	depTex = 0;
	colTex = 0;
	mipmaps = false;
	vportdim = false;
	initOk = false;
}

RfxRenderTarget::~RfxRenderTarget()
{
	if (initOk) {
		glDeleteFramebuffersEXT(1, &fbo);
		glDeleteRenderbuffersEXT(1, &depTex);
		glDeleteTextures(1, &colTex);
	}
}

void RfxRenderTarget::SetClear(int pass, float depthClear, float *colorClear)
{
	passOptions[pass].clearMask = 0;
	passOptions[pass].colorClear = false;
	passOptions[pass].depthClear = false;

	// valid depth clear values are [0, 1]
	// -1.0 disable depth clear
	if (depthClear != -1.0) {
		passOptions[pass].depthClear = true;

		passOptions[pass].depthClearVal = depthClear;
		passOptions[pass].clearMask |= GL_DEPTH_BUFFER_BIT;
	}

	// color should be a 4-elements array.
	// NULL disable color clear
	if (colorClear != NULL) {
		passOptions[pass].colorClear = true;

		passOptions[pass].colorClearVal[0] = colorClear[0];
		passOptions[pass].colorClearVal[1] = colorClear[1];
		passOptions[pass].colorClearVal[2] = colorClear[2];
		passOptions[pass].colorClearVal[3] = colorClear[3];
		passOptions[pass].clearMask |= GL_COLOR_BUFFER_BIT;
	}
}

bool RfxRenderTarget::Setup(int pass)
{
	if (!GLEW_EXT_framebuffer_object) {
		qWarning("FBO not supported!");
		return false;
	}

	if (initOk)
		return true;

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	// if "use viewport dimensions" at this point we have a gl context
	if (vportdim) {
		GLfloat dims[4];
		glGetFloatv(GL_VIEWPORT, dims);
		width = (int)dims[2];
		height = (int)dims[3];
	}

	// depth buffer
	glGenRenderbuffersEXT(1, &depTex);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depTex);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depTex);

	// color buffer
	glGenTextures(1, &colTex);
	glBindTexture(GL_TEXTURE_2D, colTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, colTex, 0);

	// set texture state based on the first uniform that will use RT
	QList<int> k = passStates.keys();
	for (int i = 0; i < k.size(); ++i) {
		if (k.at(i) > pass) {
			foreach (RfxState *s, passStates.value(i))
				s->SetEnvironment(GL_TEXTURE_2D);
		}
	}

	initOk = (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return initOk;
}

void RfxRenderTarget::Bind(int pass)
{
	assert(initOk);

	bool colClear = passOptions.value(pass).colorClear;
	bool depClear = passOptions.value(pass).depthClear;

	if (colClear) {
		const float *cols = passOptions.value(pass).colorClearVal;
		glClearColor(cols[0], cols[1], cols[2], cols[3]);
	}

	if (depClear)
		glClearDepth(passOptions.value(pass).depthClearVal);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);

	if (colClear || depClear)
		glClear(passOptions.value(pass).clearMask);
}

void RfxRenderTarget::Unbind()
{
	if (!initOk)
		return;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
}

QImage RfxRenderTarget::GetQImage()
{
	if (!initOk)
		return QImage();

	QImage img(width, height, QImage::Format_RGB32);

	unsigned char *tempBuf = new unsigned char[width * height * 3];
	unsigned char *tempBufPtr = tempBuf;
	glBindTexture(GL_TEXTURE_2D, colTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
	for (int i = 0; i < height; ++i) {
		QRgb *scanLine = (QRgb*)img.scanLine(i);
		for (int j = 0; j < width; ++j) {
			scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
			tempBufPtr += 3;
		}
	}
	delete[] tempBuf;

	return img.mirrored();
}
