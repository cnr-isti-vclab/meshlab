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

#include "rich_float.h"

RichFloat::RichFloat(
	const QString& nm,
	const Scalarm defval,
	const QString& desc,
	const QString& tltip,
	bool hidden,
	const QString& category) :
		RichParameter(nm, FloatValue(defval),desc, tltip, hidden, category)
{
}

RichFloat::~RichFloat()
{
}

QString RichFloat::stringType() const
{
	return "RichFloat";
}

RichFloat* RichFloat::clone() const
{
	return new RichFloat(*this);
}

bool RichFloat::operator==( const RichParameter& rb )
{
	return (rb.value().isFloat() &&(pName == rb.name()) && (value().getFloat() == rb.value().getFloat()));
}

