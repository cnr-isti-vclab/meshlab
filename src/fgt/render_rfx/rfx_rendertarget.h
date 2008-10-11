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

#ifndef RFX_RENDERTARGET_H_
#define RFX_RENDERTARGET_H_

#include <cassert>
#include <QString>
#include <QMap>
#include <QImage>
#include <GL/glew.h>
#include "rfx_state.h"

class RfxRenderTarget
{
public:
	RfxRenderTarget(const QString&);
	virtual ~RfxRenderTarget();

	void SetSize(int w, int h) { width = w; height = h; }
	void SetClear(int pass, float depthClear, float *colorClear);
	void AddGLState(int p, RfxState *s) { passStates[p].append(s); }
	const QString& GetName() { return name; }
	GLuint GetTexture() { return colTex; }
	QImage GetQImage();
	bool Setup(int pass);
	void Bind(int pass);
	void Unbind();
	void UseViewPortDim(bool useVD) { vportdim = useVD; }
	void GenMipmaps(bool genMip) { mipmaps = genMip; }

private:
	GLuint fbo;
	GLuint colTex;
	GLuint depTex;

	QString name;
	int width;
	int height;
	bool mipmaps;
	bool vportdim;
	bool initOk;

	struct RTOptions {
		GLint clearMask;
		bool depthClear;
		float depthClearVal;
		bool colorClear;
		float colorClearVal[4];
	};
	QMap<int, RTOptions> passOptions;
	QMap<int, QList<RfxState*> > passStates;
};

#endif /* RFX_RENDERTARGET_H_ */
