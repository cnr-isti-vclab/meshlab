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
#ifndef __RMPASS_H__
#define __RMPASS_H__

#include <QList>
#include <QString>
#include <QStringList>
#include <QDebug>
#include "GlState.h"
#include "UniformVar.h"

class RenderTarget
{
public:
	RenderTarget(QString _name = QString(), bool _renderToScreen = false,
	             bool _colorClear = false, bool _depthClear = false,
	             float _clearColorValue = 0, float _depthClearValue = 0)
	{
		name = _name;
		renderToScreen = _renderToScreen;
		colorClear = _colorClear;
		depthClear = _depthClear;
		clearColorValue = _clearColorValue;
		depthClearValue = _depthClearValue;
	}

	QString name;
	bool renderToScreen;
	bool colorClear;
	bool depthClear;
	float clearColorValue;
	float depthClearValue;
};


class RmPass
{
public:

	RmPass(QString _name = QString(), int _index = -1) :
		name(_name), index(_index)
	{}

	virtual ~RmPass( ) {}
	enum CodeType { FRAGMENT, VERTEX };

	UniformVar searchFragmentUniformVariable(QString &name);
	UniformVar searchVertexUniformVariable(QString &name);
	void addOpenGLState(GlState &state) { states.append(state); }
	bool hasIndex() { return index != -1; }
	int getIndex() { return index; }

	void setModelReference(QString _modelRef) { modelRef = _modelRef; }
	QString& getModelReference() { return modelRef; }
	void setModelReferenceFN(QString modRef) { modelRefFile = modRef; }
	QString& getModelReferenceFN() { return modelRefFile; }

	void setFragment(QString _fragment) { fragment = _fragment; }
	QString& getFragment() { return fragment; }

	void setVertex(QString _vertex) { vertex = _vertex; }
	QString& getVertex() { return vertex; }

	QString& getName() { return name; }
	void setRenderTarget(RenderTarget rt) { renderTarget = rt; }
	RenderTarget& getRenderTarget() { return renderTarget; }
	bool hasRenderTarget() { return !renderTarget.name.isNull(); }

	void addFragmentUniform(UniformVar &var) { fragUniform.append(var); }
	void addVertexUniform(UniformVar &var) { vertUniform.append(var); }

	int fragmentUniformVariableSize() { return fragUniform.size(); }
	int vertexUniformVariableSize() { return vertUniform.size(); }

	UniformVar& getFragmentUniform(int idx)
	{
		return getUniform(idx, FRAGMENT);
	}
	UniformVar& getVertexUniform(int idx)
	{
		return getUniform(idx, VERTEX);
	}
	UniformVar& getUniform(int idx, CodeType codetype)
	{
		return (codetype == FRAGMENT) ?
			fragUniform[idx] : vertUniform[idx];
	}

	int openGLStatesSize() { return states.size(); }
	GlState& getOpenGLState(int idx) { return states[idx]; }
	bool operator<(const RmPass &p) const { return index < p.index; }

private:
	QString name;
	int index;

	QString fragment;
	QString vertex;

	QList<UniformVar> fragUniform;
	QList<UniformVar> vertUniform;

	QString modelRef;
	QString modelRefFile;
	QList<GlState> states;

	RenderTarget renderTarget;

	// we look for a variable declared as uniform in a specific
	// source code (fragment or vertex's one) and check for its type
	UniformVar searchUniformVariable(QString &name, CodeType codetype);
};
#endif /* __RMPASS_H__ */
