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

#ifndef RFX_UNIFORM_H_
#define RFX_UNIFORM_H_

#include <cassert>
#include <QString>
#include <QList>
#include <QFileInfo>
#include <QImage>
#include <GL/glew.h>
#include <QGLWidget>
#include "rfx_state.h"
#include "rfx_rendertarget.h"
#include "rfx_textureloader.h"

class RfxUniform
{
public:
	enum UniformType {
		INT, FLOAT, BOOL,
		VEC2, VEC3, VEC4,
		IVEC2, IVEC3, IVEC4,
		BVEC2, BVEC3, BVEC4,
		MAT2, MAT3, MAT4,
		SAMPLER1D, SAMPLER2D, SAMPLER3D, SAMPLERCUBE,
		SAMPLER1DSHADOW, SAMPLER2DSHADOW,
		TOTAL_TYPES
	};

	RfxUniform(const QString&, const QString&);
	virtual ~RfxUniform();

	void AddGLState(RfxState *s) { textureStates.append(s); }
	void UpdateUniformLocation(GLuint programId);
	void LoadTexture();
	void PassToShader();

	void SetValue(float[16]);
	void SetValue(const QString&);
	void SetValue(RfxRenderTarget *_rt) { rTarget = _rt; textureRendered = true; }
	void SetTU(GLint tu) { texUnit = tu; }
	float* GetValue() { return value; }
	GLint GetTU() { return texUnit; }

	QString GetName() { return identifier; }
	UniformType GetType() { return type; }
	bool isTexture() { return (type > MAT4); }
	bool isTextureLoaded() { return textureLoaded; }
	bool isTextureFound() { return !textureNotFound; }
	bool isRenderable() { return textureRendered; }
	QString& GetTextureFName() { return textureFile; }
	QImage GetRTTexture() { assert(rTarget); return rTarget->GetQImage(); }

	static UniformType GetUniformType(const QString&);
	static QString GetTypeString(UniformType u) { return UniformTypeString[u]; }
	QListIterator<RfxState*> StatesIterator()
	{
		return QListIterator<RfxState*>(textureStates);
	}
	void SetSemantic(const QString& sem) { semantic = sem; }
	QString GetSemantic() { return semantic; }
	bool HasMinMax() { return (minVal != 0.0 || maxVal != 0.0); }
	float GetMinRange() { return minVal; }
	float GetMaxRange() { return maxVal; }
	void SetValRange(float _min, float _max) { minVal = _min; maxVal = _max; }

	/*
	Sets the value of _isRmColorVariable variable.
	true if the uniform is a "RmColorVariable", false otherwise
	*/
	void setIsRmColorVariable(bool val){ _isRmColorVariable = val; }

	/*
	Returns the value of _isRmColorVariable. True if the uniform is a RmColorVariable, false otherwise.
	*/
	bool isRmColorVariable(){ return _isRmColorVariable; }

private:
	QString identifier;
	UniformType type;
	float *value;

	bool textureLoaded;
	bool textureNotFound;
	bool textureRendered;

	/*
	True if the uniform is a RenderMonkey Color Variable. False otherwise.
	*/
	bool _isRmColorVariable;

	RfxRenderTarget *rTarget;
	QList<RfxState*> textureStates;
	QString textureFile;
	GLuint textureId;
	GLint textureTarget;
	GLint texUnit;
	GLint maxTexUnits;
	GLint location;
	QString semantic;

	float minVal;
	float maxVal;

	static const char *UniformTypeString[];
};

#endif /* RFX_UNIFORM_H_ */
