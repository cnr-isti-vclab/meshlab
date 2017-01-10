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
#ifndef RADIANCESCALINGRENDERER_H
#define RADIANCESCALINGRENDERER_H

#include <GL/glew.h>
#include <common/interfaces.h>

#include <QAction>
#include <QString>
#include <map>

#include "shaderDialog.h"
#include "gpuProgram.h"
#include "framebufferObject.h"
#include "texture2D.h"

class RadianceScalingRendererPlugin : public QObject, public MeshRenderInterface {
    Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_RENDER_INTERFACE_IID)
    Q_INTERFACES(MeshRenderInterface)

    bool             _supported;
    QList<QAction *> _actionList;
    ShaderDialog    *_sDialog;

 public:
    RadianceScalingRendererPlugin();
    
    QList<QAction *> actions() {
      if(_actionList.isEmpty()) initActionList();
      return _actionList;
    }
    
    void initActionList();
    
    virtual bool isSupported() {return _supported;}
    virtual void Init(QAction *a, MeshDocument &m, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *gla);
    virtual void Finalize(QAction *a, MeshDocument *m, GLArea * gla);
    virtual void Render(QAction *a, MeshDocument &m, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *gla);

    inline void setEnable(bool enabled);
    inline void setLit(bool lit);
    inline void setInvert(int invert);
	inline void setDoubleSide(int doubleSide);
    inline void setDisplay(int index);
    inline void setEnhancement(float enhancement);
    inline void setTransition(float transition);
    inline void setConvexLit(const QString &filename);
    inline void setConcavLit(const QString &filename);

 private:
    FramebufferObject *_fbo;
    GPUProgram        *_buffPass;
    GPUProgram        *_rsPass;
    FloatTexture2D    *_depthTex;
    FloatTexture2D    *_gradTex;
    FloatTexture2D    *_normTex;
    UbyteTexture2D    *_colorTex;
    UbyteTexture2D    *_convexLS;
    UbyteTexture2D    *_concavLS;
    int                _w,_h;
    
    void initShaders(bool reload=false);
    void initFBOs();
    void cleanShaders();
    void cleanFBOs();

    void createLit(const QString &filename,int type);

    inline void drawQuad();
    inline void swapToScreenMode();
    inline void swapToWorldMode();
    inline void checkViewport();
};

inline void RadianceScalingRendererPlugin::swapToScreenMode() {
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  //glDepthMask(GL_FALSE);
}

inline void RadianceScalingRendererPlugin::swapToWorldMode() {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
}

inline void RadianceScalingRendererPlugin::drawQuad() {
  glBegin(GL_QUADS);

  glTexCoord2f(0.0,0.0); glVertex2f(-1.0,-1.0);
  glTexCoord2f(1.0,0.0); glVertex2f( 1.0,-1.0);
  glTexCoord2f(1.0,1.0); glVertex2f( 1.0, 1.0);
  glTexCoord2f(0.0,1.0); glVertex2f(-1.0, 1.0);

  glEnd();
}

inline void RadianceScalingRendererPlugin::checkViewport() {
  int v[4];
  glGetIntegerv(GL_VIEWPORT,v);
  
  if(v[2]!=_w || v[3]!=_h) {
    _w = v[2];
    _h = v[3];
    
    cleanFBOs();
    initFBOs();
    initShaders(true);
  }
}

inline void RadianceScalingRendererPlugin::setEnable(bool enabled) {
  _rsPass->enable();
  _rsPass->setUniform1i("enabled",enabled);
  _rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setLit(bool lit) 
{
	  initShaders(false);
		_rsPass->enable();
	  _rsPass->setUniform1i("lit",lit);
	  _rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setInvert(int invert) {
  _rsPass->enable();
  _rsPass->setUniform1i("invert",invert);
  _rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setDoubleSide(int doubleSide) {
	_rsPass->enable();
	_rsPass->setUniform1i("doubleSide", doubleSide);
	_rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setDisplay(int index) {
	if(index==1) {
		initShaders(false);
	}
  _rsPass->enable();
  _rsPass->setUniform1i("display",index);
  _rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setEnhancement(float enhancement) {
  _rsPass->enable();
  _rsPass->setUniform1f("enhancement",enhancement);
  _rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setTransition(float transition) {
  _rsPass->enable();
  _rsPass->setUniform1f("transition",transition);
  _rsPass->disable();
}

inline void RadianceScalingRendererPlugin::setConvexLit(const QString &filename) {
  initShaders(false);
	createLit(filename,0);
  
}

inline void RadianceScalingRendererPlugin::setConcavLit(const QString &filename) {
 initShaders(false);
  createLit(filename,1);
}

#endif

