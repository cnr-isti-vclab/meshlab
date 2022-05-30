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

#include "string_value.h"

#include <QDomElement>

StringValue::StringValue(const QString& val) : pval(val)
{
}

QString StringValue::getString() const
{
	return pval;
}

bool StringValue::isString() const
{
	return true;
}

QString StringValue::typeName() const
{
	return QString("String");
}

void StringValue::set(const Value& p)
{
	pval = p.getString();
}

StringValue* StringValue::clone() const
{
	return new StringValue(*this);
}

bool StringValue::operator==(const Value& p) const
{
	if (p.isString())
		return pval == p.getString();
	else
		return false;
}

void StringValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("value", pval);
}
