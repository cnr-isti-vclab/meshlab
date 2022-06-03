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

#include "rich_file_save.h"

RichFileSave::RichFileSave(
	const QString& nm,
	const QString& filedefval,
	const QString& ext,
	const QString& desc,
	const QString& tltip,
	bool hidden,
	const QString& category) :
		RichParameter(nm, StringValue(filedefval), desc, tltip, hidden, category), ext(ext)
{
}

RichFileSave::~RichFileSave()
{
}

QString RichFileSave::stringType() const
{
	return "RichSaveFile";
}

QDomElement RichFileSave::fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip) const
{
	QDomElement parElem = RichParameter::fillToXMLDocument(doc, saveDescriptionAndTooltip);
	parElem.setAttribute("ext", ext);
	return parElem;
}

RichFileSave* RichFileSave::clone() const
{
	return new RichFileSave(*this);
}

bool RichFileSave::operator==( const RichParameter& rb )
{
	return (rb.isOfType<RichFileSave>() &&(pName == rb.name()) && (value().getString() == rb.value().getString()));
}
