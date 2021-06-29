/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#include <common/ml_document/mesh_model.h>

#include "io_gltf.h"

#include "gltf_loader.h"

QString IOglTFPlugin::pluginName() const
{
	return "IOglTF";
}

std::list<FileFormat> IOglTFPlugin::importFormats() const
{
	return { FileFormat("GLTF", tr("GLTF")) };
}

/*
	returns the list of the file's type which can be exported
*/
std::list<FileFormat> IOglTFPlugin::exportFormats() const
{
	return {};
}

/*
	returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void IOglTFPlugin::exportMaskCapability(
		const QString&,
		int &capability,
		int &defaultBits) const
{
	capability=defaultBits=0;
	return;
}


void IOglTFPlugin::open(
		const QString& fileFormat,
		const QString&fileName,
		MeshModel &m,
		int& mask,
		const RichParameterList & params,
		vcg::CallBackPos* cb)
{
	if (fileFormat.toUpper() == "GLTF"){
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		loader.LoadASCIIFromFile(&model, &err, &warn, fileName.toStdString().c_str());

		if (!err.empty())
			throw MLException("Failed opening gltf file");
		if (!warn.empty())
			reportWarning(QString::fromStdString(warn));

		for (const tinygltf::Mesh& tm : model.meshes)
			gltf::loadMesh(m, tm, model);
	}
	else {
		wrongOpenFormat(fileFormat);
	}
}

void IOglTFPlugin::save(
		const QString& fileFormat,
		const QString&,
		MeshModel&,
		const int ,
		const RichParameterList&,
		vcg::CallBackPos*)
{
	wrongSaveFormat(fileFormat);
}

MESHLAB_PLUGIN_NAME_EXPORTER(IOglTFPlugin)
