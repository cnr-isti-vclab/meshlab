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

#ifndef MESHLAB_RICH_ENUM_H
#define MESHLAB_RICH_ENUM_H

#include "rich_parameter.h"

class RichEnum : public RichParameter
{
public:
	RichEnum(
		const QString& nm,
		const int defval,
		const QStringList& values,
		const QString& desc = QString(),
		const QString& tltip = QString(),
		bool hidden = false,
		const QString& category = QString());
	~RichEnum();

	QString stringType() const;
	QDomElement fillToXMLDocument(QDomDocument& doc, bool saveDescriptionAndTooltip = true) const;

	RichEnum* clone() const;
	bool operator==(const RichParameter& rb);
	QStringList enumvalues;
};

#endif // MESHLAB_RICH_ENUM_H
