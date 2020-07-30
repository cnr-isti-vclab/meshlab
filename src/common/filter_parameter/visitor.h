/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#ifndef MESHLAB_VISITOR_H
#define MESHLAB_VISITOR_H

#include <QString>
#include <QDomDocument>
#include <QDomElement>

class RichParameter;
class RichBool;
class RichInt;
class RichFloat;
class RichString;
class RichMatrix44f;
class RichPoint3f;
class RichShotf;
class RichColor;
class RichAbsPerc;
class RichEnum;
//class RichFloatList;
class RichDynamicFloat;
class RichOpenFile;
class RichSaveFile;
class RichMesh;

class Visitor
{
public:

	virtual void visit(RichBool& pd) = 0;
	virtual void visit(RichInt& pd) = 0;
	virtual void visit(RichFloat& pd) = 0;
	virtual void visit(RichString& pd) = 0;
	virtual void visit(RichMatrix44f& pd) = 0;
	virtual void visit(RichPoint3f& pd) = 0;
	virtual void visit(RichShotf& pd) = 0;
	virtual void visit(RichColor& pd) = 0;
	virtual void visit(RichAbsPerc& pd) = 0;
	virtual void visit(RichEnum& pd) = 0;
	//virtual void visit(RichFloatList& pd) = 0;
	virtual void visit(RichDynamicFloat& pd) = 0;
	virtual void visit(RichOpenFile& pd) = 0;
	virtual void visit(RichSaveFile& pd) = 0;
	virtual void visit(RichMesh& pd) = 0;

	virtual ~Visitor() {}
};

class RichParameterXMLVisitor : public Visitor
{
public:
	RichParameterXMLVisitor(const QDomDocument& doc) : docdom(doc) {}

	void visit(RichBool& pd);
	void visit(RichInt& pd);
	void visit(RichFloat& pd);
	void visit(RichString& pd);
	void visit(RichMatrix44f& pd);
	void visit(RichPoint3f& pd);
	void visit(RichShotf& pd);
	void visit(RichColor& pd);
	void visit(RichAbsPerc& pd);

	void visit(RichEnum& pd);
	//void visit(RichFloatList& pd);

	void visit(RichDynamicFloat& pd);

	void visit(RichOpenFile& pd);
	void visit(RichSaveFile& pd);
	void visit(RichMesh& pd);

	~RichParameterXMLVisitor() {}

	QDomDocument docdom;
	QDomElement parElem;
private:
	void fillRichParameterAttribute(const QString& type, const QString& name, const QString& desc, const QString& tooltip);
	void fillRichParameterAttribute(const QString& type, const QString& name, const QString& val, const QString& desc, const QString& tooltip);
};

#endif // MESHLAB_VISITOR_H
