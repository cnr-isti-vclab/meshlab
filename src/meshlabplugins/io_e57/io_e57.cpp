/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
#include <Qt>

#include "io_e57.h"

//#include <wrap/io_trimesh/export.h>

using namespace vcg;

void E57IOPlugin::initPreOpenParameter(const QString &format, RichParameterList & parlst)
{
	return;
}

void E57IOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &parlst, CallBackPos * /*cb*/)
{
	if (formatName.toUpper() == tr("E57")) {
	    throw MLException("Error while opening E57 file.");
	}
	else {
		wrongOpenFormat(formatName);
	}
}

void E57IOPlugin::save(const QString & formatName, const QString & /*fileName*/, MeshModel & /*m*/, const int /*mask*/, const RichParameterList &, vcg::CallBackPos * /*cb*/)
{
	wrongSaveFormat(formatName);
}

/*
	returns the list of the file's type which can be imported
*/
QString E57IOPlugin::pluginName() const
{
	return QString{"IOE57"};
}

std::list<FileFormat> E57IOPlugin::importFormats() const
{
	return {
	    FileFormat("E57 (E57 point cloud)", tr("E57"))
	};
}

/*
	Returns the list of the file's type which can be exported
*/
std::list<FileFormat> E57IOPlugin::exportFormats() const
{
	return {};
}

/*
	Returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void E57IOPlugin::exportMaskCapability(const QString & /*format*/, int &capability, int &defaultBits) const
{
	capability = defaultBits=0;
	return;
}

MESHLAB_PLUGIN_NAME_EXPORTER(E57IOPlugin)
