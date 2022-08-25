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
#include <common//interfaces.h>
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

	//int mCurrentPass;
	int mBindedPass;
	GLuint mDummyTexId; // on ATI graphics card we need to bind a texture to get point sprite working !
	bool mWorkaroundATI;
	bool mBuggedAtiBlending;
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
	//QString filterName(MeshLabInterface::FilterIDType) const;

public:

	SplatRendererPlugin();

	QList<QAction *> actions ()
	{
		if(actionList.isEmpty()) initActionList();
		return actionList;
	}

	void initActionList();

	bool isSupported() {return mIsSupported;}
	void Init(QAction *a, MeshDocument &m, RenderMode &rm, QGLWidget *gla);
	void Render(QAction *a, MeshDocument &m, RenderMode &rm, QGLWidget *gla);

};

#endif

