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

#include "color_value.h"

#include <QDomElement>

ColorValue::ColorValue(QColor val) : pval(val)
{
}

QColor ColorValue::getColor() const
{
	return pval;
}

bool ColorValue::isColor() const
{
	return true;
}

QString ColorValue::typeName() const
{
	return QString("Color");
}

void ColorValue::set(const Value& p)
{
	pval = p.getColor();
}

ColorValue* ColorValue::clone() const
{
	return new ColorValue(*this);
}

bool ColorValue::operator==(const Value& p) const
{
	if (p.isColor())
		return pval == p.getColor();
	else
		return false;
}

void ColorValue::fillToXMLElement(QDomElement& element) const
{
	element.setAttribute("r", QString::number(pval.red()));
	element.setAttribute("g", QString::number(pval.green()));
	element.setAttribute("b", QString::number(pval.blue()));
	element.setAttribute("a", QString::number(pval.alpha()));
}
