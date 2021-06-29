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
void loadMesh(
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
			loadMesh(m, model, p);
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
void populateVertices(
		MeshModel&m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* posArray,
		unsigned int vertNumber)
{
	ivp.clear();
	ivp.resize(vertNumber);
	CMeshO::VertexIterator vi =
			vcg::tri::Allocator<CMeshO>::AddVertices(m.cm, vertNumber);
	for (unsigned int i = 0; i < vertNumber*3; i+= 3, ++vi){
		ivp[i/3] = &*vi;
		vi->P() = CMeshO::CoordType(posArray[i], posArray[i+1], posArray[i+2]);
	}
}

template <typename Scalar>
void populateTriangles(
		MeshModel&m,
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* triArray,
		unsigned int triNumber)
{
	CMeshO::FaceIterator fi =
			vcg::tri::Allocator<CMeshO>::AddFaces(m.cm, triNumber);
	for (unsigned int i = 0; i < triNumber*3; i+=3, ++fi) {
		fi->V(0) = ivp[triArray[i]];
		fi->V(1) = ivp[triArray[i+1]];
		fi->V(2) = ivp[triArray[i+2]];
	}
}


//import
void loadMesh(
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


	const tinygltf::BufferView& posbw = model.bufferViews[posAccessor.bufferView];
	const std::vector<unsigned char>& posdata = model.buffers[posbw.buffer].data;
	unsigned int posOffset = posbw.byteOffset + posAccessor.byteOffset;
	std::vector<CMeshO::VertexPointer> ivp;

	if (posAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
		const float * posArray = (const float*) (posdata.data() + posOffset);
		populateVertices(m, ivp, posArray, posAccessor.count);

	}
	else if (posAccessor.componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
		const double * posArray = (const double*) (posdata.data() + posOffset);
		populateVertices(m, ivp, posArray, posAccessor.count);
	}
	else {
		throw MLException("Coord type not supported");
	}

	//triangles
	if (p.mode == 4 && p.indices >= 0 &&
			(unsigned int)p.indices < model.accessors.size()) {

		const tinygltf::Accessor& triAccessor = model.accessors[p.indices];

		if (triAccessor.type != TINYGLTF_TYPE_SCALAR)
			throw MLException("Primitive indices must be scalars!");

		const tinygltf::BufferView tribw = model.bufferViews[triAccessor.bufferView];
		const std::vector<unsigned char>& tridata = model.buffers[tribw.buffer].data;
		unsigned int triOffset = tribw.byteOffset;

		if (triAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			const unsigned char* triArray = (const unsigned char*) (tridata.data() + triOffset);
			populateTriangles(m, ivp, triArray, triAccessor.count/3);
		}
		else if (triAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			const unsigned short* triArray = (const unsigned short*) (tridata.data() + triOffset);
			populateTriangles(m, ivp, triArray, triAccessor.count/3);
		}
		else if (triAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
			const unsigned int* triArray = (const unsigned int*) (tridata.data() + triOffset);
			populateTriangles(m, ivp, triArray, triAccessor.count/3);
		}
		else {
			throw MLException("Malformed GLTF: wrong primitive indices component type.");
		}
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(IOglTFPlugin)
