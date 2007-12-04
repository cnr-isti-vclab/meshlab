/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#ifndef AMBIENT_OCCLUSION_H
#define AMBIENT_OCCLUSION_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/space/box3.h>
#include <wrap/gl/shot.h>
#include <wrap/gl/gl_surface.h>

class AmbientOcclusionPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

// Attributes
private:
	Point3f cameraDir;
	GLuint  fboDepth,
	        fboResult,
	        depthBufferTex,
	        vertexCoordTex,
	        vertexNormalsTex,
	        resultBufferTex;
	
	unsigned int numViews,
	             texSize,
	             texArea;

	bool useGPU;

// Methods
public:
	enum { FP_AMBIENT_OCCLUSION  } ;

	AmbientOcclusionPlugin();
	~AmbientOcclusionPlugin();
	
	virtual const QString     filterName      (FilterIDType filter);
	virtual const QString     filterInfo      (FilterIDType filter);
	virtual const PluginInfo &pluginInfo();
	virtual const int         getRequirements (QAction *action);
	virtual       bool        autoDialog      (QAction *) {return true;}
	virtual       void        initParameterSet(QAction *,
	                                           MeshModel &/*m*/,
	                                           FilterParameterSet & /*parent*/);
	virtual       bool        applyFilter     (QAction *filter,
	                                           MeshModel &m,
	                                           FilterParameterSet & /*parent*/,
	                                           vcg::CallBackPos * cb) ;
private:
	void  initTextures          (GLenum colorFormat,
	                             GLenum depthFormat);
	bool  initContext           (QGLWidget &qWidget,
	                             vcg::CallBackPos *cb);

	bool  checkFramebuffer();

	void  vertexCoordsToTexture (MeshModel &m);

	void  renderMesh            (MeshModel &m);
	void  setCamera             (Point3f camDir,
	                             Box3f &meshBBox);

	void  generateOcclusionHW   ();
	void  generateOcclusionSW   (MeshModel &m,
	                             GLfloat *occlusion);

	void  applyOcclusionHW      (MeshModel &m);
	void  applyOcclusionSW      (MeshModel &m,
	                             GLfloat *aoValues);

	void  set_shaders           (char *shaderName,
	                             GLuint &vs,
	                             GLuint &fs,
	                             GLuint &pr);

	void dumpFloatTexture(QString filename, float *texdata, int elems);

};

#endif