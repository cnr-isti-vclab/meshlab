/****************************************************************************
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

#include "rich_bool.h"

RichBool::RichBool(
		const QString& nm,
		const bool defval,
		const QString& desc,
		const QString& tltip,
		bool hidden,
		const QString& category) :
	RichParameter(nm, BoolValue(defval), desc, tltip, hidden, category)
{
}

RichBool::~RichBool()
{
}

QString RichBool::stringType() const
{
	return "RichBool";
}

RichBool* RichBool::clone() const
{
	return new RichBool(*this);
}

bool RichBool::operator==( const RichParameter& rb )
{
	return (rb.value().isBool() && (pName == rb.name()) && (value().getBool() == rb.value().getBool()));
}
