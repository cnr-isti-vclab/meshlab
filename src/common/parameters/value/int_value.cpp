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

#include "int_value.h"

#include <QDomElement>

IntValue::IntValue(const int val) : pval(val)
{
}

int IntValue::getInt() const
{
	return pval;
}

QString IntValue::typeName() const
{
	return QString("Int");
}

void IntValue::set(const Value& p)
{
	pval = p.getInt();
}

IntValue* IntValue::clone() const
{
	return new IntValue(*this);
}

bool IntValue::operator==(const Value& p) const
{
	if (p.isInt())
		return pval == p.getInt();
	else
		return false;
}

void IntValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", QString::number(pval));
}
