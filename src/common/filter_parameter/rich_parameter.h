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

#ifndef MESHLAB_RICH_PARAMETER_H
#define MESHLAB_RICH_PARAMETER_H

#include "value.h"
#include "visitor.h"
#include <QDomElement>

class MeshDocument;

class RichParameter
{
public:
	RichParameter(const RichParameter& rp);
	RichParameter(RichParameter&& rp);
	RichParameter(const QString& nm, const Value& v, const QString& desc = QString(), const QString& tltip = QString());

	const QString& name() const;

	virtual void accept(Visitor& v) = 0;
	virtual RichParameter* clone() const = 0;
	RichParameter& operator=(const RichParameter& rp);
	RichParameter& operator=(RichParameter&& rp);
	virtual bool operator==(const RichParameter& rp) = 0;
	virtual ~RichParameter();
protected:
	QString pName;
public:
	Value* val;

	QString fieldDesc;
	QString tooltip;
};


class RichBool : public RichParameter
{
public:
	RichBool(const QString& nm, const bool defval, const QString& desc = QString(), const QString& tltip = QString());
	RichBool(const QString& nm, const bool val, const bool defval, const QString& desc, const QString& tltip);
	void accept(Visitor& v);
	RichBool* clone() const;
	bool operator==(const RichParameter& rb);

	~RichBool();
};

class RichInt : public RichParameter
{
public:
	RichInt(const QString& nm, const int defval, const QString& desc = QString(), const QString& tltip = QString());
	RichInt(const QString& nm, const int val, const int defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichInt* clone() const;
	bool operator==(const RichParameter& rb);
	~RichInt();
};

class RichFloat : public RichParameter
{
public:
	RichFloat(const QString& nm, const float defval, const QString& desc = QString(), const QString& tltip = QString());
	RichFloat(const QString& nm, const float val, const float defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichFloat* clone() const;
	bool operator==(const RichParameter& rb);
	~RichFloat();
};

class RichString : public RichParameter
{
public:
	RichString(const QString& nm, const QString& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichString(const QString& nm, const QString& val, const QString& defval, const QString& desc, const QString& tltip);
	void accept(Visitor& v);
	RichString* clone() const;
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
	RichMatrix44f* clone() const;
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
	RichPoint3f* clone() const;
	bool operator==(const RichParameter& rb);
	~RichPoint3f();
};
class RichShotf : public RichParameter
{
public:
	RichShotf(const QString& nm, const vcg::Shotf& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichShotf(const QString& nm, const vcg::Shotf& val, const vcg::Shotf& defval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichShotf* clone() const;
	bool operator==(const RichParameter& rb);
	~RichShotf();
};

class RichColor : public RichParameter
{
public:
	RichColor(const QString& nm, const QColor& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichColor(const QString& nm, const QColor& val, const QColor& defval, const QString& desc, const QString& tltip);
	void accept(Visitor& v);
	RichColor* clone() const;
	bool operator==(const RichParameter& rb);
	~RichColor();

};

class RichAbsPerc : public RichParameter
{
public:
	RichAbsPerc(const QString& nm, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	RichAbsPerc(const QString& nm, const float val, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichAbsPerc* clone() const;
	bool operator==(const RichParameter& rb);
	float min;
	float max;
	~RichAbsPerc();
};

class RichEnum : public RichParameter
{
public:
	RichEnum(const QString& nm, const int defval, const QStringList& values, const QString& desc = QString(), const QString& tltip = QString());
	RichEnum(const QString& nm, const int val, const int defval, const QStringList& values, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichEnum* clone() const;
	bool operator==(const RichParameter& rb);
	QStringList enumvalues;
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
	RichMesh* clone() const;
	bool operator==(const RichParameter& rb);
	~RichMesh();

	MeshDocument* meshdoc;
	int meshindex;


};

//class RichFloatList : public RichParameter
//{
//public:
//	RichFloatList(const QString& nm, FloatListValue* v, FloatListDecoration* prdec);
//	RichFloatList(const QString& nm, FloatListValue* val, FloatListValue* v, FloatListDecoration* prdec);
//	void accept(Visitor& v);
//	bool operator==(const RichParameter& rb);
//	~RichFloatList();
//};

class RichDynamicFloat : public RichParameter
{
public:
	RichDynamicFloat(const QString& nm, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	RichDynamicFloat(const QString& nm, const float val, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichDynamicFloat* clone() const;
	bool operator==(const RichParameter& rb);
	float min;
	float max;
	~RichDynamicFloat();

};

class RichOpenFile : public RichParameter
{
public:
	RichOpenFile(const QString& nm, const QString& directorydefval, const QStringList& exts, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichOpenFile* clone() const;
	bool operator==(const RichParameter& rb);
	QStringList exts;
	~RichOpenFile();
};

class RichSaveFile : public RichParameter
{
public:
	RichSaveFile(const QString& nm, const QString& filedefval, const QString& ext, const QString& desc = QString(), const QString& tltip = QString());
	void accept(Visitor& v);
	RichSaveFile* clone() const;
	bool operator==(const RichParameter& rb);
	QString ext;
	~RichSaveFile();
};

class RichParameterAdapter
{
public:
	static bool create(const QDomElement& np, RichParameter** par);

	static QString convertToStringValue(RichParameter& rp);
};

#endif // MESHLAB_RICH_PARAMETER_H
