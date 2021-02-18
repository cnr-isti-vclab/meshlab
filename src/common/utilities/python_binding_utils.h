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

#ifndef MESHLAB_PYTHON_BINDING_UTILS_H
#define MESHLAB_PYTHON_BINDING_UTILS_H

#include <QStringList>

class RichParameter;

namespace ml_python {

const char PYTHON_TYPE_ENUM[] = "str";
const char PYTHON_TYPE_ABSPERC[] = "Percentage";
const char PYTHON_TYPE_DYNAMIC_FLOAT[] = "float (bounded)";
const char PYTHON_TYPE_BOOL[] = "bool";
const char PYTHON_TYPE_INT[] = "int";
const char PYTHON_TYPE_FLOAT[] = "float";
const char PYTHON_TYPE_STRING[] = "str";
const char PYTHON_TYPE_MATRIX44F[] = "numpy.ndarray[numpy.float64[4, 4]]";
const char PYTHON_TYPE_POINT3F[] = "numpy.ndarray[numpy.float64[3]]";
const char PYTHON_TYPE_SHOTF[] = "Shotf [still unsupported]";
const char PYTHON_TYPE_COLOR[] = "Color";
const char PYTHON_TYPE_MESH[] = "int";
const char PYTHON_TYPE_FILENAME[] = "str";

const static QStringList pythonKeywords = {
	"False", "await", "else", "import", "pass",
	"None", "break", "except", "in", "raise",
	"True", "class", "finally", "is", "return",
	"and", "continue", "for", "lambda", "try",
	"as", "def	from", "nonlocal", "while",
	"assert", "del", "global", "not", "with",
	"async", "elif", "if", "or", "yield"
};

QString computePythonTypeString(const RichParameter& par);
QString computePythonName(const QString& name);

}

#endif // MESHLAB_PYTHON_BINDING_UTILS_H
