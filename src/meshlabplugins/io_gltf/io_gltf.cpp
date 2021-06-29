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
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model);

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
			loadMesh(m, tm, model);

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
void populateVNormals(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* normArray,
		unsigned int vertNumber)
{
	for (unsigned int i = 0; i < vertNumber*3; i+= 3){
		ivp[i/3]->N() = CMeshO::CoordType(normArray[i], normArray[i+1], normArray[i+2]);
	}
}

template <typename Scalar>
void populateTriangles(
		MeshModel&m,
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* triArray,
		unsigned int triNumber)
{
	if (triArray != nullptr) {
		CMeshO::FaceIterator fi =
				vcg::tri::Allocator<CMeshO>::AddFaces(m.cm, triNumber);
		for (unsigned int i = 0; i < triNumber*3; i+=3, ++fi) {
			fi->V(0) = ivp[triArray[i]];
			fi->V(1) = ivp[triArray[i+1]];
			fi->V(2) = ivp[triArray[i+2]];
		}
	}
	else {
		CMeshO::FaceIterator fi =
				vcg::tri::Allocator<CMeshO>::AddFaces(m.cm, ivp.size()/3);
		for (unsigned int i = 0; i < ivp.size(); i+=3, ++fi) {
			fi->V(0) = ivp[i];
			fi->V(1) = ivp[i+1];
			fi->V(2) = ivp[i+2];
		}
	}
}

enum GLTF_ATTR_TYPE {POSITION, NORMAL, TEXCOORD_0, INDICES};
const std::array<std::string, 4> GLTF_ATTR_STR {"POSITION", "NORMAL", "TEXCOORD_0"};

template <typename Scalar>
void populateAttr(
		GLTF_ATTR_TYPE attr,
		MeshModel&m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* array,
		unsigned int number)
{
	switch (attr) {
	case POSITION:
		populateVertices(m, ivp, array, number); break;
	case NORMAL:
		populateVNormals(ivp, array, number); break;
	case TEXCOORD_0:
		break;
	case INDICES:
		populateTriangles(m, ivp, array, number/3); break;
	}
}

void loadAttribute(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		GLTF_ATTR_TYPE attr)
{
	const tinygltf::Accessor* accessor = nullptr;

	if (attr != INDICES) {
		auto it = p.attributes.find(GLTF_ATTR_STR[attr]);

		if (it != p.attributes.end()) {
			accessor = &model.accessors[it->second];
		}
		else if (attr == POSITION) {
			throw MLException("File has not 'Position' attribute");
		}
	}
	else {
		if (p.mode == 4 && p.indices >= 0 &&
				(unsigned int)p.indices < model.accessors.size()) {
			accessor = &model.accessors[p.indices];
		}
	}

	if (accessor) {

		const tinygltf::BufferView& posbw = model.bufferViews[accessor->bufferView];
		const std::vector<unsigned char>& posdata = model.buffers[posbw.buffer].data;
		unsigned int posOffset = posbw.byteOffset + accessor->byteOffset;


		if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
			const float* posArray = (const float*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
			const double* posArray = (const double*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			const unsigned char* triArray = (const unsigned char*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			const unsigned short* triArray = (const unsigned short*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
			const unsigned int* triArray = (const unsigned int*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count);
		}
	}
	else if (attr == INDICES) {
		populateAttr<unsigned char>(attr, m, ivp, nullptr, 0);
	}

}

//import
void loadMeshPrimitive(
		MeshModel& m,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p)
{
	std::vector<CMeshO::VertexPointer> ivp;
	loadAttribute(m, ivp, model, p, POSITION);
	loadAttribute(m, ivp, model, p, NORMAL);
	loadAttribute(m, ivp, model, p, INDICES);
}

void loadMesh(
		MeshModel& m,
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model)
{
	if (!tm.name.empty())
		m.setLabel(QString::fromStdString(tm.name));
	for (const tinygltf::Primitive& p : tm.primitives){
		loadMeshPrimitive(m, model, p);
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(IOglTFPlugin)
