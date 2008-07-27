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
/****************************************************************************
History
$Log$
Revision 1.8  2007/12/10 15:15:46  corsini
code re-styling

Revision 1.7  2007/12/10 14:22:35  corsini
add documentation

Revision 1.6  2007/12/03 11:08:48  corsini
code restyling

Revision 1.5  2007/12/03 10:26:02  corsini
code restyling


****************************************************************************/

#ifndef __GLSTATEHOLDER_H__
#define __GLSTATEHOLDER_H__

// Local headers
#include "parser/RmPass.h"
#include "parser/UniformVar.h"
#include <meshlab/meshmodel.h>

// QT heades
#include <QString>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QVariant>
#include <QFileDialog>
#include <QApplication>
#include <QImage>
#include <QMessageBox>
#include <GL/glew.h>
#include <QGLWidget>
#include <QGLFramebufferObject>

// Standard headers
#include <cassert>

/* FIXME: remove this define */
#define FBO_SIZE 512

/**
 * Extends the uniform variable class of the RM Parser
 * to add information such as memory location and
 * actual value.
 */
class UniformValue : public UniformVar
{
public:
	UniformValue(UniformVar&);
	virtual ~UniformValue();

	void updateUniformVariableValuesFromDialog(int, int, QVariant);
	bool updateValueInGLMemory();
	void VarDump();

	static int textureUnit;

	/// ARB memory location
	int location;

	/// load texture flag
	bool textureLoaded;

	/// texture id
	GLuint textureId;
};

/**
 * It is the descriptor of a pass: it has the list
 * of uniform variables with their value and memory
 * location, and the ARB program handler.
 */
class GLStatePassHolder : public QObject
{
	Q_OBJECT

public:
	GLStatePassHolder(RmPass&);
	~GLStatePassHolder();

	bool compile();
	bool link();

	bool hasVertexProgram()   { return setVertexProgram;   }
	bool hasFragmentProgram() { return setFragmentProgram; }

	QString & getLastError()  { return lastError; }

	void updateUniformVariableValuesFromDialog(QString, int, int, QVariant);
	bool updateUniformVariableValuesInGLMemory();
	bool adjustSampler2DUniformVar(QString, GLuint);

	void VarDump();
	void useProgram();
	void execute();

	QString passName;
	QString modelName;

private:
	GLhandleARB vhandler;
	GLhandleARB fhandler;

	bool setVertexProgram;
	bool setFragmentProgram;
	QString lastError;

	GLhandleARB program;

	QMap<QString, UniformValue*> uniformValues;
};

/**
 * This class hold the state of the rmshader render:
 * it has e list of passes and for each pass it keeps the
 * list of uniform variable with their memory location and
 * actual value, and the ARB program handler.
 */
class GLStateHolder : public QObject
{
	Q_OBJECT
	friend class RmMeshShaderRenderPlugin;

public:
	GLStateHolder();
	GLStateHolder(QList<RmPass>&);
	~GLStateHolder();

	/// Compile all shaders
	bool compile();

	/// Link all shaders
	bool link();

	void updateUniformVariableValuesFromDialog(QString passname, QString varname,
	                                           int rowIdx, int colIdx, QVariant newValue);
	bool updateUniformVariableValuesInGLMemory();
	bool updateUniformVariableValuesInGLMemory(int pass_idx);
	void VarDump();
	void setPasses(QList<RmPass> &passes);

	/// Reset OpenGL memory
	inline void reset() { needUpdateInGLMemory = true; }

	/// Return true if the current shader is supported
	bool isSupported()  { return supported; }

	/// Set the i-th rendering pass.
	void usePassProgram(int i);

	/// Execute the i-th rendering pass.
	bool executePass(int i);

	int passNumber()         { return passes.size(); }
	QString& getLastError()  { return lastError; }

private:
	void genPassTextures();

	QList<GLStatePassHolder*> passes;
	GLuint* passTextures;
	QGLFramebufferObject* fbo;
	QString lastError;
	GLint currentDrawbuf;
	bool supported;
	bool needUpdateInGLMemory;
	int currentPass;
};
#endif /* __GLSTATEHOLDER_H__ */
