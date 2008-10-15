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

#ifndef SPLATRENDERER_H
#define SPLATRENDERER_H

#include <QObject>
#include <QAction>
#include <QString>
#include <map>

#include <GL/glew.h>
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <wrap/gl/shaders.h>
class QGLFramebufferObject;

class SplatRendererPlugin : public QObject, public MeshRenderInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshRenderInterface)

	bool mIsSupported;
	QList <QAction *> actionList;

	enum {
		DEFERRED_SHADING_BIT	= 0x000001,
		DEPTH_CORRECTION_BIT	= 0x000002,
		OUTPUT_DEPTH_BIT			= 0x000004,
		BACKFACE_SHADING_BIT	= 0x000008,
		FLOAT_BUFFER_BIT			= 0x000010
	};
	int mFlags;
	int mCachedFlags;
	int mRenderBufferMask;
	int mSupportedMask;

	int mCurrentPass;
	int mBindedPass;
	GLuint mNormalTextureID;
	GLuint mDepthTextureID;
	ProgramVF mShaders[3];
	QString mShaderSrcs[6];
	QGLFramebufferObject* mRenderBuffer;
	float mCachedMV[16];
	float mCachedProj[16];
	GLint mCachedVP[4];

	struct UniformParameters
	{
		float radiusScale;
		float preComputeRadius;
		float depthOffset;
		float oneOverEwaRadius;
		vcg::Point2f halfVp;
		vcg::Point3f rayCastParameter1;
		vcg::Point3f rayCastParameter2;
		vcg::Point2f depthParameterCast;

		void loadTo(Program& prg);
		void update(float* mv, float* proj, GLint* vp);
	};

	UniformParameters mParams;

	QString loadSource(const QString& func,const QString& file);
	void configureShaders();
	void updateRenderBuffer();
	void enablePass(int n);
	void drawSplats(MeshModel &m, RenderMode &rm);

public:

	SplatRendererPlugin();

	QList<QAction *> actions ()
	{
		if(actionList.isEmpty()) initActionList();
		return actionList;
	}

	void initActionList();

	virtual bool isSupported() {return mIsSupported;}
	virtual void Init(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
	virtual void Render(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
	virtual void Draw(QAction *a, MeshModel &m, RenderMode &rm, QGLWidget *gla);
	virtual int passNum() { return 3; }

};

#endif

