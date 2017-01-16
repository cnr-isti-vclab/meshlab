/****************************************************************************
* Render Radiance Scaling                                                   *
* Meshlab's plugin                                                          *
*                                                                           *
* Copyright(C) 2010                                                         *
* Vergne Romain, Dumas Olivier                                              *
* INRIA - Institut Nationnal de Recherche en Informatique et Automatique    *
*                                                                           *
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
#include "radianceScalingRenderer.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <QGLWidget>
#include <QTextStream>
#include <QResource>
#include "meshlab/glarea.h"

using namespace std;

#define GL_TEST_ERR							\
  {									\
   GLenum eCode;							\
   if((eCode=glGetError())!=GL_NO_ERROR)				\
     std::cerr << "OpenGL error : " <<  gluErrorString(eCode) << " in " <<  __FILE__ << " : " << __LINE__ << std::endl;	\
 }

RadianceScalingRendererPlugin::RadianceScalingRendererPlugin()
	: _supported(false),
	_sDialog(0),
	_fbo(NULL),
	_buffPass(NULL),
	_rsPass(NULL),
	_depthTex(NULL),
	_gradTex(NULL),
	_normTex(NULL),
	_colorTex(NULL),
	_convexLS(NULL),
	_concavLS(NULL) {

}

void RadianceScalingRendererPlugin::Init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *gla) {
	if (_sDialog) {
		_sDialog->close();
		delete _sDialog;
		_sDialog = 0;
	}

	gla->makeCurrent();
	glewInit();

	GL_TEST_ERR

		if (!GLEW_ARB_vertex_program ||
			!GLEW_ARB_fragment_program ||
			!GLEW_ARB_texture_float ||
			!GLEW_ARB_draw_buffers ||
			!GLEW_EXT_framebuffer_object) {

			_supported = false;
			return;
		}

	_supported = true;
	_sDialog = new ShaderDialog(this, gla, gla);
	_sDialog->move(10, 100);
	_sDialog->show();

	_sDialog->changeIcon(":/RadianceScalingRenderer/litSpheres/ls02.png", ShaderDialog::convex_icon);
	_sDialog->changeIcon(":/RadianceScalingRenderer/litSpheres/ls01.png", ShaderDialog::concav_icon);

	createLit(":/RadianceScalingRenderer/litSpheres/ls02.png", 0);
	createLit(":/RadianceScalingRenderer/litSpheres/ls01.png", 1);

	initFBOs();
	GL_TEST_ERR

		initShaders();
	GL_TEST_ERR
}

void RadianceScalingRendererPlugin::Render(QAction *, MeshDocument &md, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *gla)
{
	if (gla == NULL)
		return;

	MultiViewer_Container* mvcont = gla->mvc();
	if (mvcont == NULL)
		return;

	MLSceneGLSharedDataContext* shared = mvcont->sharedDataContext();
	if (shared == NULL)
		return;

	checkViewport();

	// first pass: buffers
	_fbo->bind();
	glDrawBuffers(3, FramebufferObject::buffers(0));
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vcg::glColor(vcg::Color4b(vcg::Color4b::LightGray));
	_buffPass->enable();

	foreach(MeshModel *mp, md.meshList)
	{
		if ((mp != NULL) && (gla->meshVisibilityMap[mp->id()]))
			shared->draw(mp->id(), gla->context());
	}
	_buffPass->disable();

	// second pass: descriptor, radiance scaling, lighting
	FramebufferObject::unbind();
	swapToScreenMode();
	_rsPass->enable();
	drawQuad();
	_rsPass->disable();
	swapToWorldMode();
}

void RadianceScalingRendererPlugin::Finalize(QAction *, MeshDocument *, GLArea *) {
	cleanShaders();
	cleanFBOs();

	if (_sDialog) {
		_sDialog->close();
		delete _sDialog;
		_sDialog = 0;
	}

	if (_convexLS != NULL) {
		delete _convexLS;
		_convexLS = NULL;
	}

	if (_concavLS != NULL) {
		delete _concavLS;
		_concavLS = NULL;
	}
}

void RadianceScalingRendererPlugin::initActionList() {
	_actionList << new QAction("Radiance Scaling", this);
}

void RadianceScalingRendererPlugin::createLit(const QString &filename, int type) {
	QImage t;
	QImage b;

	if (!b.load(filename)) {
		return;
	}

	t = QGLWidget::convertToGLFormat(b);

	if (type == 0) {
		if (_convexLS != NULL) {
			delete _convexLS;
			_convexLS = NULL;
		}

		_convexLS = new UbyteTexture2D(TextureFormat(GL_TEXTURE_2D, t.width(), t.height(), 3, GL_RGBA, GL_UNSIGNED_BYTE), TextureParams(GL_LINEAR, GL_LINEAR), t.bits());
	}
	else {

		if (_concavLS != NULL) {
			delete _concavLS;
			_concavLS = NULL;
		}

		_concavLS = new UbyteTexture2D(TextureFormat(GL_TEXTURE_2D, t.width(), t.height(), 3, GL_RGBA, GL_UNSIGNED_BYTE), TextureParams(GL_LINEAR, GL_LINEAR), t.bits());
	}
}

void RadianceScalingRendererPlugin::initShaders(bool reload)
{
	if (!reload) {
		string path = ":/RadianceScalingRenderer/shaders/";
		delete _buffPass;
		delete _rsPass;
		_buffPass = new GPUProgram(path + "01_buffer.vs", path + "01_buffer.fs");
		_rsPass = new GPUProgram(path + "02_rs.vs", path + "02_rs.fs");
		GL_TEST_ERR

			_rsPass->enable();
		_rsPass->addUniform("sw");
		_rsPass->addUniform("sh");
		_rsPass->addUniform("enhancement");
		_rsPass->addUniform("transition");
		_rsPass->addUniform("enabled");
		_rsPass->addUniform("invert");
		_rsPass->addUniform("doubleSide");
		_rsPass->addUniform("twoLS");
		_rsPass->addUniform("display");
		_rsPass->addUniform("grad");
		_rsPass->addUniform("norm");
		_rsPass->addUniform("convexLS");
		_rsPass->addUniform("concavLS");
		_rsPass->addUniform("colormap");
		_rsPass->disable();

		GL_TEST_ERR
	}
	else {
		_buffPass->reload();
		_rsPass->reload();
		GL_TEST_ERR
	}

	float sw = 1.0f / (float)_w;
	float sh = 1.0f / (float)_h;

	_rsPass->enable();
	_rsPass->setUniform1f("sw", sw);
	_rsPass->setUniform1f("sh", sh);
	_rsPass->setUniform1f("enhancement", _sDialog->getEnhancement());
	_rsPass->setUniform1f("transition", _sDialog->getTransition());
	_rsPass->setUniform1i("enabled", _sDialog->getEnable());
	_rsPass->setUniform1i("display", _sDialog->getDisplay());
	_rsPass->setUniform1i("invert", _sDialog->getInvert());
	_rsPass->setUniform1i("doubleSide", _sDialog->getDoubleSide());
	_rsPass->setUniform1i("twoLS", _sDialog->getTwoLS());
	_rsPass->setUniformTexture("grad", 0, _gradTex->format().target(), _gradTex->id());
	_rsPass->setUniformTexture("norm", 1, _normTex->format().target(), _normTex->id());
	_rsPass->setUniformTexture("colormap", 2, _colorTex->format().target(), _colorTex->id());
	_rsPass->setUniformTexture("convexLS", 3, _convexLS->format().target(), _convexLS->id());
	_rsPass->setUniformTexture("concavLS", 4, _concavLS->format().target(), _concavLS->id());
	_rsPass->disable();
	GL_TEST_ERR
}

void RadianceScalingRendererPlugin::initFBOs() {
	int v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	_w = v[2];
	_h = v[3];

	if (_fbo == NULL) {
		GLenum filter = GL_LINEAR;
		GLenum format = GL_RGBA16F_ARB;
		_fbo = new FramebufferObject();
		_depthTex = new FloatTexture2D(TextureFormat(GL_TEXTURE_2D, _w, _h, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT), TextureParams(filter, filter));
		_gradTex = new FloatTexture2D(TextureFormat(GL_TEXTURE_2D, _w, _h, format, GL_RGBA, GL_FLOAT), TextureParams(filter, filter));
		_normTex = new FloatTexture2D(_gradTex->format(), _gradTex->params());
		_colorTex = new UbyteTexture2D(TextureFormat(GL_TEXTURE_2D, _w, _h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE), TextureParams(GL_LINEAR, GL_LINEAR));//new FloatTexture2D(_gradTex->format(),_gradTex->params());
	}

	_fbo->bind();
	_fbo->unattachAll();
	_depthTex->bind();
	_fbo->attachTexture(_depthTex->format().target(), _depthTex->id(), GL_DEPTH_ATTACHMENT_EXT);
	_gradTex->bind();
	_fbo->attachTexture(_gradTex->format().target(), _gradTex->id(), GL_COLOR_ATTACHMENT0_EXT);
	_normTex->bind();
	_fbo->attachTexture(_normTex->format().target(), _normTex->id(), GL_COLOR_ATTACHMENT1_EXT);
	_colorTex->bind();
	_fbo->attachTexture(_colorTex->format().target(), _colorTex->id(), GL_COLOR_ATTACHMENT2_EXT);
	_fbo->isValid();

	FramebufferObject::unbind();
}

void RadianceScalingRendererPlugin::cleanShaders() {
	if (_buffPass != NULL) {
		delete _buffPass;
		delete _rsPass;

		_buffPass = NULL;
		_rsPass = NULL;
	}
}

void RadianceScalingRendererPlugin::cleanFBOs() 
{
	if (_fbo != NULL) 
	{
		delete _fbo;
		delete _depthTex;
		delete _gradTex;
		delete _normTex;
		delete _colorTex;

		_fbo = NULL;
		_depthTex = NULL;
		_gradTex = NULL;
		_normTex = NULL;
		_colorTex = NULL;
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(RadianceScalingRendererPlugin)
