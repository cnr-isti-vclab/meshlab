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

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

//function declaration
void loadVAttributes(
		MeshModel& m,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p);

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

		const tinygltf::Mesh& tm = model.meshes[0];
		for (const tinygltf::Primitive& p : tm.primitives){
			loadVAttributes(m, model, p);
		}

		//todo remove this
		//throw MLException("gltf still not supported");
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


template <typename Scalar>
void populateVertices(MeshModel&m, const Scalar* posArray, unsigned int vertNumber)
{
	CMeshO::VertexIterator vi =
			vcg::tri::Allocator<CMeshO>::AddVertices(m.cm, vertNumber);
	for (unsigned int i = 0; i < vertNumber*3; i+= 3, ++vi){
		vi->P() = CMeshO::CoordType(posArray[i], posArray[i+1], posArray[i+2]);
	}
}


//import
void loadVAttributes(
		MeshModel& m,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p)
{
	const auto it = p.attributes.find("POSITION");

	if (it == p.attributes.end())
		throw MLException("File has not 'Position' attribute");
	int positionAccessorID = it->second;

	const tinygltf::Accessor& posAccessor = model.accessors[positionAccessorID];

	if (posAccessor.type != TINYGLTF_TYPE_VEC3)
		throw MLException("File positions are not 3D coordinates!");


	const tinygltf::BufferView bw = model.bufferViews[posAccessor.bufferView];
	const std::vector<unsigned char>& data = model.buffers[bw.buffer].data;
	unsigned int offset = bw.byteOffset;

	if (posAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
		const float * posArray = (const float*) (data.data() + offset);
		populateVertices(m, posArray, posAccessor.count);

	}
	else if (posAccessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
		const double * posArray = (const double*) (data.data() + offset);
		populateVertices(m, posArray, posAccessor.count);
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(IOglTFPlugin)
