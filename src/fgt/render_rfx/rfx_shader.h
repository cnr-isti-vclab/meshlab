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
	enum SemanticValue {
		VIEWPORTWIDTH, VIEWPORTHEIGHT, VIEWPORTDIMENSIONS, VIEWPORTWIDTHINVERSE,
		VIEWPORTHEIGHTINVERSE, INVERSEVIEWPORTDIMENSIONS, PASSINDEX,
		TOT_SEMANTICS
	};


	RfxShader();
	virtual ~RfxShader();

	void AddRT(RfxRenderTarget *rt) { renderTargets.append(rt); }
	RfxRenderTarget* GetRT(int idx) { return renderTargets.at(idx); }
	int FindRT(const QString&);
	void AddGLPass(RfxGLPass *pass) { shaderPasses.append(pass); }
	bool AddSemanticUniform(RfxUniform*, const QString&);
	RfxGLPass* GetPass(int idx) { return shaderPasses.at(idx); }
	QString GetSemantic(RfxUniform*);
	void SortPasses();
	QListIterator<RfxGLPass*> PassesIterator()
	{
		return QListIterator<RfxGLPass*>(shaderPasses);
	}
	void CompileAndLink();
	void Start();
	void Stop() { glUseProgram(0); }

private:
	void UpdateSemanticUniforms(int);

	QList<RfxGLPass*> shaderPasses;
	QList<RfxRenderTarget*> renderTargets;
	QMultiMap<SemanticValue, RfxUniform*> semUniforms;
	static const char *semantic[];
};

#endif /* RFX_SHADER_H_ */
