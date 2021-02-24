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

#ifndef PYMESHLAB_PYTHON_UTILS_H
#define PYMESHLAB_PYTHON_UTILS_H

#include <QStringList>
#include <array>
#include <vcg/../wrap/io_trimesh/io_mask.h>

class RichParameter;

namespace pymeshlab {

const static std::array<int, 14> capabilitiesBits= {
	vcg::tri::io::Mask::IOM_VERTQUALITY,
	vcg::tri::io::Mask::IOM_VERTFLAGS,
	vcg::tri::io::Mask::IOM_VERTCOLOR,
	vcg::tri::io::Mask::IOM_VERTTEXCOORD,
	vcg::tri::io::Mask::IOM_VERTNORMAL,
	vcg::tri::io::Mask::IOM_VERTRADIUS,
	vcg::tri::io::Mask::IOM_FACEQUALITY,
	vcg::tri::io::Mask::IOM_FACEFLAGS,
	vcg::tri::io::Mask::IOM_FACECOLOR,
	vcg::tri::io::Mask::IOM_FACENORMAL,
	vcg::tri::io::Mask::IOM_WEDGCOLOR,
	vcg::tri::io::Mask::IOM_WEDGTEXCOORD,
	vcg::tri::io::Mask::IOM_WEDGNORMAL,
	vcg::tri::io::Mask::IOM_BITPOLYGONAL
};

const static std::array<QString, 14> saveCapabilitiesStrings = {
	"Save Vertex Quality",
	"Save Vertex Flag",
	"Save Vertex Color",
	"Save Vertex Coord",
	"Save Vertex Normal",
	"Save Vertex Radius",
	"Save Face Quality",
	"Save Face Flag",
	"Save Face Color",
	"Save Face Normal",
	"Save Wedge Color",
	"Save Wedge TexCoord",
	"Save Wedge Normal",
	"Save Polygonal"
};

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

/** Utilities **/
QString computePythonTypeString(const RichParameter& par);
QString computePythonName(const QString& name);
void printSaveMask(int mask);

}

#endif // PYMESHLAB_PYTHON_UTILS_H
