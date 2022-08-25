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
#include <QMessageBox>
#include "rfx_uniform.h"

#include "rfx_specialattribute.h"

#include "rfx_state.h"

class RfxGLPass
{
public:
	RfxGLPass() : passIndex(-1) { useRenderTarget = false; shaderLinked = false; }
	RfxGLPass(int passidx) : passIndex(passidx) { useRenderTarget = false; shaderLinked = false; }
	virtual ~RfxGLPass();
	bool checkSpecialAttributeDataMask(MeshDocument*);
	void SetShaderSource(const QString &source, bool isFragment);
	const QString& GetVertexSource() { return vert; }
	const QString& GetFragmentSource() { return frag; }
	int GetPassIndex() { return passIndex; }
	void SetPassIndex(int passidx) { passIndex = passidx; }
	const QString& GetPassName() { return passName; }
	void SetPassName(const QString &n) { passName = n; }
	void AddGLState(RfxState *s) { rfxStates.append(s); }
	void AddUniform(RfxUniform *u) { shaderUniforms.append(u); }

	/*
		Appends a new Special Attribute to the pass.
		@param s the attribute to be appended.
	*/
	void AddSpecialAttribute(RfxSpecialAttribute *s){ shaderSpecialAttributes.append(s); }

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
	
	/*
		Returns the list of special attribute contained in the pass.
		@return the list of special attribute
	*/
	QList<RfxSpecialAttribute*>* AttributesList()
	{
		return &shaderSpecialAttributes;
	}

	/*
		Returns true if the pass has at least a specialAttribute, false otherwise.
		@return true if the pass contains at least a special attribute, false otherwise.
	*/
	bool hasSpecialAttribute(){
		return !this->shaderSpecialAttributes.isEmpty();
	}

	RfxUniform* getUniform(int uniIdx) { return shaderUniforms.at(uniIdx); }
	RfxUniform* getUniform(const QString& uniIdx);
	
	/*
		Returns the shader program.
		@return the shader program.
	*/
	GLuint* getProgram() { 
		return &shaderProgram; 
	}

	void CompileAndLink();
	void Start();
	const QString GetCompilationLog() { return compileLog; }

private:
	QString vert;
	QString frag;
	QString compileLog;

	QString passName;
	int passIndex;
	bool useRenderTarget;
	RfxRenderTarget *rt;

	GLuint shaderProgram;
	bool shaderLinked;

	QList<RfxState*> rfxStates;
	QList<RfxUniform*> shaderUniforms;

	/* The list of special attribute the pass contains */
	QList<RfxSpecialAttribute*> shaderSpecialAttributes;

	void FillInfoLog(GLhandleARB);
};

#endif /* RFX_GLPASS_H_ */
