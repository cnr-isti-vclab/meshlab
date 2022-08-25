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
Revision 1.4  2007/12/11 16:19:37  corsini
add mapping between rm code and opengl code

Revision 1.3  2007/12/03 10:29:37  corsini
code restyling


****************************************************************************/

#ifndef __RMXMLPARSER_H__
#define __RMXMLPARSER_H__

#include <sstream>
#include <QString>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QList>
#include <GL/glew.h>
#include "RmEffect.h"
#include "RmPass.h"
#include "UniformVar.h"
#include "GlState.h"

/**
 * This is the main class for RenderMonkey file (.rfx) Xml Parser.
 * A RenderMonkey file can contain any number of RmOpenGLEffect, each of which can
 * contain any number of RmGLPass. Not all the pass have a fragment or vertex
 * program, so we ignore the one that do not have them
 * It works on a file corresponding to the given file name and the parsing
 * is invoked by calling parse(). On success, this is the tree of class
 * collected by the parser:
 *
 *		RmXmlParser:
 *			- List< RmEffect >, each RmEffect has:
 *				- List< RmPass >, each RmPass has:
 *					- the fragment program source code
 *					- the vertex program source code
 *					- the uniform variable used in the fragment program
 *					- the uniform variable used in the vertex program
 *					- other infos such as openGL state and model reference
 *
 * Any class that is a container of List< other class > has these three methods:
 *		- int size()                  to know the list size
 *		- T & at(int idx)             to get the idx-th element of the list
 *		- T & operator[] (int idx)    as above
 *
 *
 */

class RmXmlParser
{
public:
	typedef std::pair<std::string, int> GlParamType;
	typedef std::map<std::string, GlParamType> MapCodeType;

	RmXmlParser();
	RmXmlParser(QString filename);
	virtual ~RmXmlParser() {}

	/**
	 * start the parsing
	 *
	 * return true on success
	 * return false on failing, then use getError()
	 */
	bool parse(QString filename = QString());

	QString& getFileName() { return filename; }
	QString& errorString() { return error; }
	void setFileName(QString _filename) { filename = _filename; }

	/// These are to manipulate the list of effects
	int size() { return effects.size(); }
	RmEffect& at(int idx) { return effects[idx]; }
	RmEffect& operator[](int idx) { return effects[idx]; }

	QString convertGlStateToString(GlState &glstate);
	int convertGlStateToInt(GlState &glstate);

	/// debug purpose
	void VarDump(bool extendedDump = false);

	static QDomElement getDomElement(QDomElement &root, QString tagname,
	                                 QString name);

// private members
private:
	void initializeCodeMapping();
	QString error;
	QString filename;
	QDomDocument doc;
	QList<RmEffect> effects;
	MapCodeType mapcode;
};
#endif /* __RMXMLPARSER_H__ */
