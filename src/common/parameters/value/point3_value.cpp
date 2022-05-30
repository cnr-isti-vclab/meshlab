/*****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2022                                           \/)\/    *
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

#include "point3_value.h"

#include <QDomElement>

Point3Value::Point3Value(const Point3m& val) : pval(val)
{
}

Point3m Point3Value::getPoint3() const
{
	return pval;
}

bool Point3Value::isPoint3() const
{
	return true;
}

QString Point3Value::typeName() const
{
	return QString("Point3");
}

void Point3Value::set(const Value& p)
{
	pval = p.getPoint3();
}

Point3Value* Point3Value::clone() const
{
	return new Point3Value(*this);
}

bool Point3Value::operator==(const Value& p) const
{
	if (p.isPoint3())
		return pval == p.getPoint3();
	else
		return false;
}

void Point3Value::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("x", QString::number(pval.X()));
	element.setAttribute("y", QString::number(pval.Y()));
	element.setAttribute("z", QString::number(pval.Z()));
}
