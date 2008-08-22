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

#ifndef RFX_GLPASS_H_
#define RFX_GLPASS_H_

#include <cassert>
#include <QString>
#include <QList>
#include <GL/glew.h>
#include <QGLWidget>
#include "rfx_uniform.h"
#include "rfx_state.h"

class RfxGLPass
{
public:
	RfxGLPass() : passIndex(-1) { useRenderTarget = false; }
	RfxGLPass(int passidx) : passIndex(passidx) { useRenderTarget = false; }
	virtual ~RfxGLPass();

	void SetShaderSource(const QString &source, bool isFragment);
	const QString& GetVertexSource() { return vert; }
	const QString& GetFragmentSource() { return frag; }
	int GetPassIndex() { return passIndex; }
	void SetPassIndex(int passidx) { passIndex = passidx; }
	const QString& GetPassName() { return passName; }
	void SetPassName(const QString &n) { passName = n; }
	void AddGLState(RfxState *s) { rfxStates.append(s); }
	void AddUniform(RfxUniform *u) { shaderUniforms.append(u); }
	void SetRenderToTexture(bool t) { useRenderTarget = t; }
	bool wantsRenderTarget() { return useRenderTarget; }
	void LinkRenderTarget(RfxRenderTarget *_rt) { rt = _rt; }
	RfxRenderTarget* GetRenderTarget() { assert(useRenderTarget); return rt; }
	QListIterator<RfxUniform*> UniformsIterator()
	{
		return QListIterator<RfxUniform*>(shaderUniforms);
	}
	QListIterator<RfxState*> StatesIterator()
	{
		return QListIterator<RfxState*>(rfxStates);
	}
	RfxUniform* getUniform(int uniIdx) { return shaderUniforms.at(uniIdx); }
	RfxUniform* getUniform(const QString& uniIdx);
	void CompileAndLink();
	void Start();

private:
	QString vert;
	QString frag;

	QString passName;
	int passIndex;
	bool useRenderTarget;
	RfxRenderTarget *rt;

	GLuint shaderProgram;
	bool shaderLinked;

	QList<RfxState*> rfxStates;
	QList<RfxUniform*> shaderUniforms;

	void printInfoLog(GLhandleARB);
};

#endif /* RFX_GLPASS_H_ */
