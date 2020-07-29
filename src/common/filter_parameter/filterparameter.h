/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2008                                           \/)\/    *
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

#ifndef MESHLAB_FILTERPARAMETER_H
#define MESHLAB_FILTERPARAMETER_H
#include <QtCore>
#include <QtXml>

#include <QMap>
#include <QPair>
#include <QAction>
#include <wrap/callback.h>

#include "value.h"

class MeshModel;
class MeshDocument;



/*************************/

class ParameterDecoration
{
public:
	QString fieldDesc;
	QString tooltip;
	Value* defVal;

	ParameterDecoration(Value* defvalue, const QString& desc = QString(), const QString& tltip = QString());

	virtual ~ParameterDecoration();

};

class BoolDecoration : public ParameterDecoration
{
public:
	BoolDecoration(BoolValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~BoolDecoration() {}
};

class IntDecoration : public ParameterDecoration
{
public:
	IntDecoration(IntValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~IntDecoration() {}
};

class FloatDecoration : public ParameterDecoration
{
public:
	FloatDecoration(FloatValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~FloatDecoration() {}
};

class StringDecoration : public ParameterDecoration
{
public:
	StringDecoration(StringValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~StringDecoration() {}
};

class Matrix44fDecoration : public ParameterDecoration
{
public:
	Matrix44fDecoration(Matrix44fValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~Matrix44fDecoration() {}
};

class Point3fDecoration : public ParameterDecoration
{
public:
	Point3fDecoration(Point3fValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~Point3fDecoration() {}
};

class ShotfDecoration : public ParameterDecoration
{
public:
	ShotfDecoration(ShotfValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~ShotfDecoration() {}
};

class ColorDecoration : public ParameterDecoration
{
public:
	ColorDecoration(ColorValue* defvalue, const QString& desc = QString(), const QString& tltip = QString());
	~ColorDecoration() {}
};

class AbsPercDecoration : public ParameterDecoration
{
public:
	AbsPercDecoration(AbsPercValue* defvalue, const float minVal, const float maxVal, const QString& desc = QString(), const QString& tltip = QString());
	float min;
	float max;
	~AbsPercDecoration() {}
};

class EnumDecoration : public ParameterDecoration
{
public:
	EnumDecoration(EnumValue* defvalue, QStringList values, const QString& desc = QString(), const QString& tltip = QString());
	QStringList enumvalues;
	~EnumDecoration() {}
};

class FloatListDecoration : public ParameterDecoration
{
public:
	FloatListDecoration(FloatListValue* defvalue, const QString& desc = QString(), const QString& tltip = QString())
		:ParameterDecoration(defvalue, desc, tltip) {}
	~FloatListDecoration() {}
};

class DynamicFloatDecoration : public ParameterDecoration
{
public:
	DynamicFloatDecoration(DynamicFloatValue* defvalue, const float minVal, const float maxVal, const QString& desc = QString(), const QString& tltip = QString());
	~DynamicFloatDecoration() {};
	float min;
	float max;
};

class SaveFileDecoration : public ParameterDecoration
{
public:
	SaveFileDecoration(FileValue* defvalue, const QString& extension, const QString& desc = QString(), const QString& tltip = QString());
	~SaveFileDecoration() {}

	QString ext;
};

class OpenFileDecoration : public ParameterDecoration
{
public:
	OpenFileDecoration(FileValue* directorydefvalue, const QStringList& extensions, const QString& desc = QString(), const QString& tltip = QString());
	~OpenFileDecoration() {}

	QStringList exts;
};


class MeshDecoration : public ParameterDecoration
{
public:
	MeshDecoration(MeshValue* defvalue, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());

	MeshDecoration(int meshind, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());

	//WARNING: IT SHOULD BE USED ONLY BY MESHLABSERVER!!!!!!!
	MeshDecoration(int meshind, const QString& desc = QString(), const QString& tooltip = QString());

	~MeshDecoration() {}

	MeshDocument* meshdoc;
	int meshindex;
};

/******************************/
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
class RichFloatList;
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
	virtual void visit(RichFloatList& pd) = 0;
	virtual void visit(RichDynamicFloat& pd) = 0;
	virtual void visit(RichOpenFile& pd) = 0;
	virtual void visit(RichSaveFile& pd) = 0;
	virtual void visit(RichMesh& pd) = 0;

	virtual ~Visitor() {}
};

class RichParameter
{
public:
	const QString name;

	Value* val;

	ParameterDecoration* pd;

	RichParameter(const QString& nm, Value* v, ParameterDecoration* prdec);
	virtual void accept(Visitor& v) = 0;
	virtual bool operator==(const RichParameter& rp) = 0;
	virtual ~RichParameter();
};


class RichBool : public RichParameter
{
public:
	RichBool(const QString& nm, const bool defval, const QString& desc = QString(), const QString& tltip = QString());
	RichBool(const QString& nm, const bool val, const bool defval, const QString& desc, const QString& tltip);
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);

	~RichBool();
};

class RichInt : public RichParameter
{
public:
	RichInt(const QString& nm, const int defval, const QString& desc = QString(), const QString& tltip = QString());
	RichInt(const QString& nm, const int val, const int defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichInt();
};

class RichFloat : public RichParameter
{
public:
	RichFloat(const QString& nm, const float defval, const QString& desc = QString(), const QString& tltip = QString());
	RichFloat(const QString& nm, const float val, const float defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichFloat();
};

class RichString : public RichParameter
{
public:
	RichString(const QString& nm, const QString& defval, const QString& desc, const QString& tltip);
	RichString(const QString& nm, const QString& defval);
	RichString(const QString& nm, const QString& defval, const QString& desc);
	RichString(const QString& nm, const QString& val, const QString& defval, const QString& desc, const QString& tltip);
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichString();
};

class RichMatrix44f : public RichParameter
{
public:
	RichMatrix44f(const QString& nm, const vcg::Matrix44f& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichMatrix44f(const QString& nm, const vcg::Matrix44d& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichMatrix44f(const QString& nm, const vcg::Matrix44f& val, const vcg::Matrix44f& defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichMatrix44f();
};

class RichPoint3f : public RichParameter
{
public:
	RichPoint3f(const QString& nm, const vcg::Point3f& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichPoint3f(const QString& nm, const vcg::Point3d& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichPoint3f(const QString& nm, const vcg::Point3f& val, const vcg::Point3f& defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichPoint3f();
};
class RichShotf : public RichParameter
{
public:
	RichShotf(const QString& nm, const vcg::Shotf& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichShotf(const QString& nm, const vcg::Shotf& val, const vcg::Shotf& defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichShotf();
};

class RichColor : public RichParameter
{
public:
	RichColor(const QString& nm, const QColor& defval);
	RichColor(const QString& nm, const QColor& defval, const QString& desc);
	RichColor(const QString& nm, const QColor& defval, const QString& desc, const QString& tltip);
	RichColor(const QString& nm, const QColor& val, const QColor& defval, const QString& desc, const QString& tltip);
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichColor();

};

class RichAbsPerc : public RichParameter
{
public:
	RichAbsPerc(const QString& nm, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	RichAbsPerc(const QString& nm, const float val, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichAbsPerc();
};

class RichEnum : public RichParameter
{
public:
	RichEnum(const QString& nm, const int defval, const QStringList& values, const QString& desc = QString(), const QString& tltip = QString());
	RichEnum(const QString& nm, const int val, const int defval, const QStringList& values, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichEnum();
};

class RichMesh : public RichParameter
{
public:
	RichMesh(const QString& nm, MeshModel* defval, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());
	RichMesh(const QString& nm, MeshModel* val, MeshModel* defval, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());

	RichMesh(const QString& nm, int meshindex, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());

	//WARNING: IT SHOULD BE USED ONLY BY MESHLABSERVER!!!!!!!
	RichMesh(const QString& nm, int meshind, const QString& desc = QString(), const QString& tltip = QString());

	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichMesh();

	int meshindex;
};

class RichFloatList : public RichParameter
{
public:
	RichFloatList(const QString& nm, FloatListValue* v, FloatListDecoration* prdec);
	RichFloatList(const QString& nm, FloatListValue* val, FloatListValue* v, FloatListDecoration* prdec);
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichFloatList();
};

class RichDynamicFloat : public RichParameter
{
public:
	RichDynamicFloat(const QString& nm, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	RichDynamicFloat(const QString& nm, const float val, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichDynamicFloat();

};

class RichOpenFile : public RichParameter
{
public:
	RichOpenFile(const QString& nm, const QString& directorydefval, const QStringList& exts, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichOpenFile();
};

class RichSaveFile : public RichParameter
{
public:
	RichSaveFile(const QString& nm, const QString& filedefval, const QString& ext, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	bool operator==(const RichParameter& rb);
	~RichSaveFile();
};



/******************************/

class RichParameterCopyConstructor : public Visitor
{
public:
	RichParameterCopyConstructor() : lastCreated(nullptr){}

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
	void visit(RichFloatList& pd);

	void visit(RichDynamicFloat& pd);

	void visit(RichOpenFile& pd);
	void visit(RichSaveFile& pd);
	void visit(RichMesh& pd);

	~RichParameterCopyConstructor() {}

	RichParameter* lastCreated;
};

class RichParameterAdapter
{
public:
	static bool create(const QDomElement& np, RichParameter** par);

	static QString convertToStringValue(RichParameter& rp);
};

class RichParameterValueToStringVisitor : public Visitor
{
public:
	RichParameterValueToStringVisitor() : stringvalue() {}

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
	void visit(RichFloatList& pd);

	void visit(RichDynamicFloat& pd);

	void visit(RichOpenFile& pd);
	void visit(RichSaveFile& pd);
	void visit(RichMesh& pd);

	~RichParameterValueToStringVisitor() {}

	QString stringvalue;
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
	void visit(RichFloatList& pd);

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

class RichParameterSet
{

public:
	RichParameterSet();
	RichParameterSet(const RichParameterSet& rps);
	// The data is just a list of Parameters
	//QMap<QString, FilterParameter *> paramMap;
	QList<RichParameter*> paramList;
	bool isEmpty() const;
	RichParameter* findParameter(const QString& name) const;
	bool hasParameter(const QString& name) const;


	RichParameterSet& operator=(const RichParameterSet& rps);
	RichParameterSet& copy(const RichParameterSet& rps);
	RichParameterSet& join(const RichParameterSet& rps);
	bool operator==(const RichParameterSet& rps);

	RichParameterSet& addParam(RichParameter* pd);

	//remove a parameter from the set by name
	RichParameterSet& removeParameter(const QString& name);

	void clear();

	void setValue(const QString& name, const Value& val);

	bool				getBool(const QString& name) const;
	int					getInt(const QString& name) const;
	float				getFloat(const QString& name) const;
	QString			getString(const QString& name) const;
	vcg::Matrix44f		getMatrix44(const QString& name) const;
	vcg::Matrix44<MESHLAB_SCALAR>		getMatrix44m(const QString& name) const;
	vcg::Point3f getPoint3f(const QString& name) const;
	vcg::Point3<MESHLAB_SCALAR> getPoint3m(const QString& name) const;
	vcg::Shotf getShotf(const QString& name) const;
	vcg::Shot<MESHLAB_SCALAR> getShotm(const QString& name) const;
	QColor		   getColor(const QString& name) const;
	vcg::Color4b getColor4b(const QString& name) const;
	float		     getAbsPerc(const QString& name) const;
	int					 getEnum(const QString& name) const;
	MeshModel*   getMesh(const QString& name) const;
	QList<float> getFloatList(const QString& name) const;
	float        getDynamicFloat(const QString& name) const;
	QString getOpenFileName(const QString& name) const;
	QString getSaveFileName(const QString& name) const;


	~RichParameterSet();
};

/****************************/



#endif
