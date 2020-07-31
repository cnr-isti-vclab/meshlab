/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef FILTERSCRIPT_H
#define FILTERSCRIPT_H

#include "filter_parameter/rich_parameter_list.h"

#include <QObject>
#include <QPair>


class QDomElement;

class FilterNameParameterValuesPair
{
public:
	virtual QString filterName() const { return pair.first; }
	virtual ~FilterNameParameterValuesPair() {}
	QPair< QString, RichParameterList > pair;
};

/*
The filterscipt class abstract the concept of history of processing.
It is simply a list of all the performed actions
Each action is a pair <filtername, parameters>
*/

class FilterScript : public QObject
{
	Q_OBJECT
public:
	FilterScript();
	~FilterScript();
	bool open(QString filename);
	bool save(QString filename);
	QDomDocument xmlDoc();

	QList< FilterNameParameterValuesPair* > filtparlist;
	typedef QList< FilterNameParameterValuesPair* >::iterator iterator;
};

#endif
