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

#include "shot_value.h"

#include <QDomElement>

ShotValue::ShotValue(const Shotm& val) : pval(val)
{
}

Shotm ShotValue::getShot() const
{
	return pval;
}

bool ShotValue::isShot() const
{
	return true;
}

QString ShotValue::typeName() const
{
	return QString("Shot");
}

void ShotValue::set(const Value& p)
{
	pval = p.getShot();
}

ShotValue* ShotValue::clone() const
{
	return new ShotValue(*this);
}

bool ShotValue::operator==(const Value& p) const
{
	if (p.isShot())
		return pval == p.getShot();
	else
		return false;
}

void ShotValue::fillToXMLElement(QDomElement&) const
{
	assert(0);
	// TODO!!!
}
