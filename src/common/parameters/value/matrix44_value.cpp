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

#include "matrix44_value.h"

#include <QDomElement>

Matrix44Value::Matrix44Value(const Matrix44m& val) : pval(val)
{
}

Matrix44m Matrix44Value::getMatrix44() const
{
	return pval;
}

bool Matrix44Value::isMatrix44() const
{
	return true;
}

QString Matrix44Value::typeName() const
{
	return QString("Matrix44");
}

void Matrix44Value::set(const Value& p)
{
	pval = p.getMatrix44();
}

Matrix44Value* Matrix44Value::clone() const
{
	return new Matrix44Value(*this);
}

bool Matrix44Value::operator==(const Value& p) const
{
	if (p.isMatrix44())
		return pval == p.getMatrix44();
	else
		return false;
}

void Matrix44Value::fillToXMLElement(QDomElement& element) const
{
	for (unsigned int ii = 0; ii < 16; ++ii)
		element.setAttribute(QString("val") + QString::number(ii), QString::number(pval.V()[ii]));
}
