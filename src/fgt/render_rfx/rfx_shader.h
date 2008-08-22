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

#ifndef RFX_SHADER_H_
#define RFX_SHADER_H_

#include <QList>
#include "rfx_glpass.h"
#include "rfx_state.h"
#include "rfx_uniform.h"
#include "rfx_rendertarget.h"

class RfxShader
{
public:
	RfxShader();
	virtual ~RfxShader();

	void AddRT(RfxRenderTarget *rt) { renderTargets.append(rt); }
	RfxRenderTarget* GetRT(int idx) { return renderTargets.at(idx); }
	int FindRT(const QString&);
	void AddGLPass(RfxGLPass *pass) { shaderPasses.append(pass); }
	RfxGLPass* GetPass(int idx) { return shaderPasses.at(idx); }
	void SortPasses();
	QListIterator<RfxGLPass*> PassesIterator()
	{
		return QListIterator<RfxGLPass*>(shaderPasses);
	}
	void CompileAndLink(QGLContext *);
	void Start();
	void Stop() { glUseProgram(0); }

private:
	QList<RfxGLPass*> shaderPasses;
	QList<RfxRenderTarget*> renderTargets;
};

#endif /* RFX_SHADER_H_ */
