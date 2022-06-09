/*****************************************************************************
 * MeshLab                                                           o o     *
 * An extendible mesh processor                                    o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005-2022                                           \/)\/    *
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

#include "filter.h"

Filter::Filter()
{
}

Filter::Filter(
	const FilterPlugin* plugin,
	const QAction* filter,
	const RichParameterList& paramList) :
		plugin(plugin),
		filter(filter),
		paramList(paramList)
{
}

void Filter::setParameterValue(const std::string &parameter, const Value &value)
{
	RichParameter& rp = paramList.getParameterByName(QString::fromStdString(parameter));
	rp.setValue(value);
}

std::string Filter::pythonCall(std::string meshSetName) const
{
	bool first = true;
	std::string call = meshSetName + "." + plugin->pythonFilterName(filter).toStdString() + "(";
	for (const RichParameter& p : paramList) {
		// parameter must be added only if its value is not default
		if (!p.isValueDefault()) {
			// if it is not the first parameter we are adding, we put the comma after the last
			// parameter, because we are going to add another one
			if (first) {
				first = false;
			}
			else {
				call += ", ";
			}
		}
	}
	call += ")";
	return call;
}
