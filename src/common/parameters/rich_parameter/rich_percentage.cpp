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

#include "rich_percentage.h"

RichPercentage::RichPercentage(
	const QString& nm,
	const Scalarm defval,
	const Scalarm minval,
	const Scalarm maxval,
	const QString& desc,
	const QString& tltip,
	bool hidden,
	const QString& category) :
		RichParameter(nm, FloatValue(defval), desc, tltip, hidden, category), min(minval), max(maxval)
{
}

RichPercentage::~RichPercentage()
{
}

QString RichPercentage::stringType() const
{
	return "RichAbsPerc";
}

QDomElement RichPercentage::fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip) const
{
	QDomElement parElem = RichParameter::fillToXMLDocument(doc, saveDescriptionAndTooltip);
	parElem.setAttribute("min",QString::number(min));
	parElem.setAttribute("max",QString::number(max));
	return parElem;
}

RichPercentage* RichPercentage::clone() const
{
	return new RichPercentage(*this);
}

bool RichPercentage::operator==( const RichParameter& rb )
{
	return (rb.isOfType<RichPercentage>() &&(pName == rb.name()) && (value().getFloat() == rb.value().getFloat()));
}
