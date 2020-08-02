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
#include <QDomElement>

class MeshDocument;

class RichParameter
{
public:
	RichParameter(const RichParameter& rp);
	RichParameter(RichParameter&& rp);
	RichParameter(const QString& nm, const Value& v, const QString& desc = QString(), const QString& tltip = QString());
	virtual ~RichParameter();

	const QString& name() const;
	const Value& value() const;
	const QString& fieldDescription() const;
	const QString& toolTip() const;
	virtual QString stringType() const = 0;

	void setValue(const Value& ov);

	virtual QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	virtual RichParameter* clone() const = 0;
	RichParameter& operator=(const RichParameter& rp);
	RichParameter& operator=(RichParameter&& rp);
	virtual bool operator==(const RichParameter& rp) = 0;

protected:
	QString pName;
	Value* val;
	QString fieldDesc;
	QString tooltip;
};


class RichBool : public RichParameter
{
public:
	RichBool(const QString& nm, const bool defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichBool();

	QString stringType() const;

	RichBool* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichInt : public RichParameter
{
public:
	RichInt(const QString& nm, const int defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichInt();

	QString stringType() const;

	RichInt* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichFloat : public RichParameter
{
public:
	RichFloat(const QString& nm, const float defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichFloat();

	QString stringType() const;

	RichFloat* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichString : public RichParameter
{
public:
	RichString(const QString& nm, const QString& defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichString();

	QString stringType() const;

	RichString* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichMatrix44f : public RichParameter
{
public:
	RichMatrix44f(const QString& nm, const vcg::Matrix44f& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichMatrix44f(const QString& nm, const vcg::Matrix44d& defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichMatrix44f();

	QString stringType() const;

	RichMatrix44f* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichPoint3f : public RichParameter
{
public:
	RichPoint3f(const QString& nm, const vcg::Point3f& defval, const QString& desc = QString(), const QString& tltip = QString());
	RichPoint3f(const QString& nm, const vcg::Point3d& defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichPoint3f();

	QString stringType() const;

	RichPoint3f* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichShotf : public RichParameter
{
public:
	RichShotf(const QString& nm, const vcg::Shotf& defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichShotf();

	QString stringType() const;

	RichShotf* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichColor : public RichParameter
{
public:
	RichColor(const QString& nm, const QColor& defval, const QString& desc = QString(), const QString& tltip = QString());
	~RichColor();

	QString stringType() const;

	RichColor* clone() const;
	bool operator==(const RichParameter& rb);
};

class RichAbsPerc : public RichParameter
{
public:
	RichAbsPerc(const QString& nm, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	~RichAbsPerc();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichAbsPerc* clone() const;
	bool operator==(const RichParameter& rb);
	float min;
	float max;
};

class RichEnum : public RichParameter
{
public:
	RichEnum(const QString& nm, const int defval, const QStringList& values, const QString& desc = QString(), const QString& tltip = QString());
	~RichEnum();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichEnum* clone() const;
	bool operator==(const RichParameter& rb);
	QStringList enumvalues;
};

class RichDynamicFloat : public RichParameter
{
public:
	RichDynamicFloat(const QString& nm, const float defval, const float minval, const float maxval, const QString& desc = QString(), const QString& tltip = QString());
	~RichDynamicFloat();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichDynamicFloat* clone() const;
	bool operator==(const RichParameter& rb);
	float min;
	float max;
};

class RichOpenFile : public RichParameter
{
public:
	RichOpenFile(const QString& nm, const QString& directorydefval, const QStringList& exts, const QString& desc = QString(), const QString& tltip = QString());
	~RichOpenFile();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichOpenFile* clone() const;
	bool operator==(const RichParameter& rb);
	QStringList exts;
};

class RichSaveFile : public RichParameter
{
public:
	RichSaveFile(const QString& nm, const QString& filedefval, const QString& ext, const QString& desc = QString(), const QString& tltip = QString());
	~RichSaveFile();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichSaveFile* clone() const;
	bool operator==(const RichParameter& rb);
	QString ext;
};

class RichMesh : public RichParameter
{
public:
	RichMesh(const QString& nm, MeshModel* defval, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());
	RichMesh(const QString& nm, int meshindex, MeshDocument* doc, const QString& desc = QString(), const QString& tltip = QString());
	//WARNING: IT SHOULD BE USED ONLY BY MESHLABSERVER!!!!!!!
	RichMesh(const QString& nm, int meshind, const QString& desc = QString(), const QString& tltip = QString());
	~RichMesh();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichMesh* clone() const;
	bool operator==(const RichParameter& rb);
	MeshDocument* meshdoc;
	int meshindex;
};

class RichParameterAdapter
{
public:
	static bool create(const QDomElement& np, RichParameter** par);
};

#endif // MESHLAB_RICH_PARAMETER_H
