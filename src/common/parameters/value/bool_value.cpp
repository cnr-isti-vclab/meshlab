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

#include "bool_value.h"

#include <QDomElement>

BoolValue::BoolValue(const bool val) : pval(val)
{
}

bool BoolValue::getBool() const
{
	return pval;
}

bool BoolValue::isBool() const
{
	return true;
}

QString BoolValue::typeName() const
{
	return QString("Bool");
}

void BoolValue::set(const Value &p)
{
	pval = p.getBool();
}

BoolValue *BoolValue::clone() const
{
	return new BoolValue(*this);
}

bool BoolValue::operator==(const Value &p) const
{
	if (p.isBool())
		return pval == p.getBool();
	else
		return false;
}

void BoolValue::fillToXMLElement(QDomElement& element) const
{
	QString v =  pval ? "true" : "false";
	element.setAttribute("value", v);
}
