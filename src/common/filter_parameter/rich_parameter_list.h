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
	RichParameterList();
	RichParameterList(const RichParameterList& rps);
	~RichParameterList();

	bool isEmpty() const;
	RichParameter* findParameter(const QString& name) const;
	bool hasParameter(const QString& name) const;


	RichParameterList& operator=(const RichParameterList& rps);
	RichParameterList& copy(const RichParameterList& rps);
	RichParameterList& join(const RichParameterList& rps);
	bool operator==(const RichParameterList& rps);

	RichParameterList& addParam(RichParameter* pd);

	//remove a parameter from the set by name
	RichParameterList& removeParameter(const QString& name);

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

	std::list<RichParameter*> paramList;
};

#endif // MESHLAB_RICH_PARAMETER_LIST_H
