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

/**
 * @brief returns a string containing a pymeshlab python call of the filter with the current
 * parameters set.
 * @param meshSetName: the name of the MeshSet object.
 * @return
 */
std::string Filter::pyMeshLabCall(std::string meshSetName) const
{
	bool first = true;
	std::string call = meshSetName + "." + plugin->pythonFilterName(filter).toStdString() + "(";
	for (const RichParameter& p : paramList) {
		// parameter must be added only if its value is not default
		if (!p.isValueDefault()) {
			if (!p.isOfType<RichShot>()) { // RichShot not yet supported by pymeshlab
				// if it is not the first parameter we are adding, we put the comma after the last
				// parameter, because we are going to add another one
				if (first) {
					first = false;
				}
				else {
					call += ", ";
				}
				call += p.pythonName().toStdString() + " = ";
				if (p.isOfType<RichBool>()) {
					if (p.value().getBool()) {
						call += "True";
					}
					else {
						call += "False";
					}
				}
				else if (p.isOfType<RichColor>()) {
					QColor c = p.value().getColor();
					call += "pymeshlab.Color(";
					call += std::to_string(c.red()) + ", ";
					call += std::to_string(c.green()) + ", ";
					call += std::to_string(c.blue()) + ", ";
					call += std::to_string(c.alpha()) + ")";
				}
				else if (p.isOfType<RichDirection>() || p.isOfType<RichPosition>()) {
					Point3m pp = p.value().getPoint3();
					call += "numpy.array([";
					call += std::to_string(pp[0]) + ", ";
					call += std::to_string(pp[1]) + ", ";
					call += std::to_string(pp[2]) + "])";
				}
				else if (p.isOfType<RichDynamicFloat>() || p.isOfType<RichFloat>()) {
					call += std::to_string(p.value().getFloat());
				}
				else if (
					p.isOfType<RichEnum>() || p.isOfType<RichInt>() || p.isOfType<RichMesh>()) {
					call += std::to_string(p.value().getInt());
				}
				else if (
					p.isOfType<RichString>() || p.isOfType<RichFileOpen>() ||
					p.isOfType<RichFileSave>()) {
					call += "'" + p.value().getString().toStdString() + "'";
				}
				else if (p.isOfType<RichMatrix44>()) {
					Matrix44m m = p.value().getMatrix44();
					call += "numpy.array([";
					bool firstRow = true;
					for (unsigned int i = 0; i < 4 ; i++) {
						if (firstRow)
							firstRow = false;
						else
							call += ", ";
						call += "[";
						bool firstCol = true;
						for (unsigned int j = 0; j < 4 ; j++) {
							if (firstCol)
								firstCol = false;
							else
								call += ", ";
							call += std::to_string(m.ElementAt(i,j));
						}
						call += "]";
					}
					call += "])";
				}
				else if (p.isOfType<RichPercentage>()) {
					const RichPercentage& rp = dynamic_cast<const RichPercentage&>(p);
					float per = (p.value().getFloat() / (rp.max - rp.min) ) * 100;
					call += "pymeshlab.PercentageValue(" + std::to_string(per) + ")";
				}
			}
		}
	}
	call += ")";
	return call;
}
