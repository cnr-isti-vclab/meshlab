/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2021                                           \/)\/    *
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

#include "python_utils.h"

#include <QRegularExpression>
#include "../parameters/rich_parameter.h"

QString pymeshlab::computePythonTypeString(const RichParameter& par)
{
	if (par.isOfType<RichEnum>())
		return PYTHON_TYPE_ENUM;
	if (par.isOfType<RichAbsPerc>())
		return PYTHON_TYPE_ABSPERC;
	if (par.isOfType<RichDynamicFloat>())
		return PYTHON_TYPE_DYNAMIC_FLOAT;
	if (par.isOfType<RichBool>())
		return PYTHON_TYPE_BOOL;
	if (par.isOfType<RichInt>())
		return PYTHON_TYPE_INT;
	if (par.isOfType<RichFloat>())
		return PYTHON_TYPE_FLOAT;
	if (par.isOfType<RichString>())
		return PYTHON_TYPE_STRING;
	if (par.isOfType<RichMatrix44f>())
		return PYTHON_TYPE_MATRIX44F;
	if (par.isOfType<RichPosition>())
		return PYTHON_TYPE_POINT3F;
	if (par.isOfType<RichDirection>())
		return PYTHON_TYPE_POINT3F;
	if (par.isOfType<RichShotf>())
		return PYTHON_TYPE_SHOTF;
	if (par.isOfType<RichColor>())
		return PYTHON_TYPE_COLOR;
	if (par.isOfType<RichMesh>())
		return PYTHON_TYPE_MESH;
	if (par.isOfType<RichSaveFile>() || par.isOfType<RichOpenFile>())
		return PYTHON_TYPE_FILENAME;
	return "still_unsupported";
}

QString pymeshlab::computePythonName(const QString& name)
{
	QString pythonName = name.toLower();
	pythonName.replace(' ', '_');
	pythonName.replace('/', '_');
	pythonName.replace('-', '_');
	pythonName.remove(QRegularExpression("[().,'\":+]+"));

	if (pythonKeywords.contains(pythonName))
		pythonName += "_";
	return pythonName;
}

void pymeshlab::printSaveMask(int mask)
{
	for (uint i = 0; i < 14; i++){
		if (mask & capabilitiesBits[i])
			std::cout << saveCapabilitiesStrings[i].toStdString() << "\n";
	}
}
