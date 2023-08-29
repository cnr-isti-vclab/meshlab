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

#include "rich_enum.h"

RichEnum::RichEnum(
	const QString& nm,
	const int defval,
	const QStringList& values,
	const QString& desc,
	const QString& tltip,
	bool hidden,
	const QString& category) :
		RichParameter(nm, IntValue(defval),desc, tltip, hidden, category), enumvalues(values)
{
}

RichEnum::~RichEnum()
{
}

QString RichEnum::stringType() const
{
	return "RichEnum";
}

QDomElement RichEnum::fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip) const
{
	QDomElement parElem = RichParameter::fillToXMLDocument(doc, saveDescriptionAndTooltip);
	parElem.setAttribute("enum_cardinality", enumvalues.size());
	for(int ii = 0; ii < enumvalues.size(); ++ii)
		parElem.setAttribute(QString("enum_val")+QString::number(ii), enumvalues.at(ii));
	return parElem;
}

RichEnum* RichEnum::clone() const
{
	return new RichEnum(*this);
}

bool RichEnum::operator==( const RichParameter& rb )
{
	return (rb.isOfType<RichEnum>() &&(pName == rb.name()) && (value().getInt() == rb.value().getInt()));
}

