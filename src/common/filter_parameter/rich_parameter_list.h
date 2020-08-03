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

#ifndef MESHLAB_RICH_PARAMETER_LIST_H
#define MESHLAB_RICH_PARAMETER_LIST_H

#include "rich_parameter.h"

class RichParameterList
{

public:
//	class iterator : public std::list<RichParameter*>::iterator {

//	};
//	class const_iterator : std::list<RichParameter*>::const_iterator {

//	};

	typedef std::list<RichParameter*>::iterator iterator;
	typedef std::list<RichParameter*>::const_iterator const_iterator;

	RichParameterList();
	RichParameterList(const RichParameterList& rps);
	RichParameterList(RichParameterList&& rps);
	~RichParameterList();

	bool isEmpty() const;
	unsigned int size() const;
	void clear();

	bool getBool(const QString& name) const;
	int getInt(const QString& name) const;
	float getFloat(const QString& name) const;
	QString getString(const QString& name) const;
	vcg::Matrix44f getMatrix44(const QString& name) const;
	vcg::Matrix44<MESHLAB_SCALAR> getMatrix44m(const QString& name) const;
	vcg::Point3f getPoint3f(const QString& name) const;
	vcg::Point3<MESHLAB_SCALAR> getPoint3m(const QString& name) const;
	vcg::Shotf getShotf(const QString& name) const;
	vcg::Shot<MESHLAB_SCALAR> getShotm(const QString& name) const;
	QColor getColor(const QString& name) const;
	vcg::Color4b getColor4b(const QString& name) const;
	float getAbsPerc(const QString& name) const;
	int getEnum(const QString& name) const;
	MeshModel* getMesh(const QString& name) const;
	QList<float> getFloatList(const QString& name) const;
	float getDynamicFloat(const QString& name) const;
	QString getOpenFileName(const QString& name) const;
	QString getSaveFileName(const QString& name) const;

	bool hasParameter(const QString& name) const;
	RichParameter& getParameterByName(const QString& name);
	const RichParameter& getParameterByName(const QString& name) const;
	iterator findParameter(const QString& name);
	const_iterator findParameter(const QString& name) const;
	RichParameter& at(unsigned int i);
	const RichParameter& at(unsigned int i) const;

	void setValue(const QString& name, const Value& val);
	RichParameter& addParam(const RichParameter& pd);
	void join(const RichParameterList& rps);


	void pushFromQDomElement(QDomElement np);

	void swap(RichParameterList& oth);
	bool operator==(const RichParameterList& rps);
	RichParameterList& operator=(RichParameterList rps);



	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

private:
	std::list<RichParameter*> paramList;
};

#endif // MESHLAB_RICH_PARAMETER_LIST_H
