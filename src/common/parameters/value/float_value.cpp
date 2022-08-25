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

#include "float_value.h"

#include <QDomElement>

FloatValue::FloatValue(const float val) : pval(val)
{
}

Scalarm FloatValue::getFloat() const
{
	return pval;
}

bool FloatValue::isFloat() const
{
	return true;
}

QString FloatValue::typeName() const
{
	return QString("Float");
}

void FloatValue::set(const Value& p)
{
	pval = p.getFloat();
}

FloatValue* FloatValue::clone() const
{
	return new FloatValue(*this);
}

bool FloatValue::operator==(const Value& p) const
{
	if (p.isFloat())
		return pval == p.getFloat();
	else
		return false;
}

void FloatValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", QString::number(pval));
}
