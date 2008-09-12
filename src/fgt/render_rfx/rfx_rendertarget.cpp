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
}

RfxRenderTarget::~RfxRenderTarget()
{
	if (qfbo)
		delete qfbo;
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

	qfbo = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth);

	glBindTexture(GL_TEXTURE_2D, qfbo->texture());
	// set texture state based on the first uniform that will use RT
	QList<int> k = passStates.keys();
	for (int i = 0; i < k.size(); ++i) {
		if (k.at(i) > pass) {
			foreach (RfxState *s, passStates.value(i))
				s->SetEnvironment(GL_TEXTURE_2D);
		}
	}

	return qfbo->isValid();
}

void RfxRenderTarget::Bind(int pass)
{
	assert(qfbo->isValid());

	bool colClear = passOptions.value(pass).colorClear;
	bool depClear = passOptions.value(pass).depthClear;

	if (colClear) {
		GLfloat *cols = passOptions.value(pass).colorClearVal;
		glClearColor(cols[0], cols[1], cols[2], cols[3]);
	}

	if (depClear)
		glClearDepth(passOptions.value(pass).depthClearVal);

	qfbo->bind();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);

	if (colClear || depClear)
		glClear(passOptions.value(pass).clearMask);
}

void RfxRenderTarget::Unbind()
{
	if (!qfbo)
		return;

	qfbo->release();
	glPopAttrib();
}
