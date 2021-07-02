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

#include "gltf_loader.h"

#include <regex>
#include <common/mlexception.h>

namespace gltf {

/**
 * @brief returns the number of meshes referred by the nodes contained in the
 * gltf file.
 *
 * Note: this number may differ from model.meshes.size().
 * This is because some gltf file may duplicate a mesh in the scene, and place
 * it in different positions using the node hierarchy.
 *
 * This function actually returns how many (referenced) nodes contain a mesh.
 *
 * @param model: the tinygltf content of the file
 * @return
 */
unsigned int getNumberMeshes(
		const tinygltf::Model& model)
{
	unsigned int nMeshes = 0;
	for (unsigned int s = 0; s < model.scenes.size(); ++s){
		const tinygltf::Scene& scene = model.scenes[s];
		for (unsigned int n = 0; n < scene.nodes.size(); ++n){
			nMeshes += internal::getNumberMeshes(model, scene.nodes[n]);
		}
	}
	return nMeshes;
}

/**
 * @brief Loads all the meshes referred in the scene of the gltf file into
 * the list of meshes.
 *
 * @param meshModelList
 * @param maskList
 * @param model
 */
void loadMeshes(
		const std::list<MeshModel*>& meshModelList,
		std::list<int>& maskList,
		const tinygltf::Model& model,
		bool loadInSingleLayer,
		vcg::CallBackPos* cb)
{
	CallBackProgress progress(100.0/meshModelList.size());
	maskList.resize(meshModelList.size(), 0);
	std::list<MeshModel*>::const_iterator meshit = meshModelList.begin();
	std::list<int>::iterator maskit = maskList.begin();
	for (unsigned int s = 0; s < model.scenes.size(); ++s){
		const tinygltf::Scene& scene = model.scenes[s];
		for (unsigned int n = 0; n < scene.nodes.size(); ++n){
			internal::loadMeshesWhileTraversingNodes(
						model,
						meshit,
						maskit,
						Matrix44m::Identity(),
						scene.nodes[n],
						loadInSingleLayer,
						cb,
						progress);
		}
	}
	if (cb)
		cb(100, "GLTF File loaded");
}

namespace internal {

/**
 * @brief Recursive function that returns the number of meshes contained
 * in the current node (0 or 1) plus the number of meshes contained in the
 * children of the node.
 *
 * Call this function from a root node to know how many meshes are referred
 * in the scene.
 *
 * @param model
 * @param node
 * @return
 */
unsigned int getNumberMeshes(
		const tinygltf::Model& model,
		unsigned int node)
{
	unsigned int nMeshes = 0;
	if (model.nodes[node].mesh >= 0){
		nMeshes = 1;
	}
	for (int c : model.nodes[node].children){
		if (c>=0){
			nMeshes += getNumberMeshes(model, c);
		}
	}
	return nMeshes;
}

/**
 * @brief Recursive function that loads a mesh if the current node contains one,
 * and then calls itself on the children of the node.
 *
 * @param model
 * @param currentMesh
 * @param currentMask
 * @param currentMatrix
 * @param currentNode
 */
void loadMeshesWhileTraversingNodes(
		const tinygltf::Model& model,
		std::list<MeshModel*>::const_iterator& currentMesh,
		std::list<int>::iterator& currentMask,
		Matrix44m currentMatrix,
		unsigned int currentNode,
		bool loadInSingleLayer,
		vcg::CallBackPos* cb,
		CallBackProgress& progress)
{
	currentMatrix = currentMatrix * getCurrentNodeTrMatrix(model, currentNode);
	if (model.nodes[currentNode].mesh >= 0) {

		int meshid = model.nodes[currentNode].mesh;
		loadMesh(
				**currentMesh,
				*currentMask,
				model.meshes[meshid],
				model,
				loadInSingleLayer,
				currentMatrix,
				cb,
				progress);
		if (!loadInSingleLayer) {
			(*currentMesh)->cm.Tr = currentMatrix;
			++currentMesh;
			++currentMask;
		}
	}

	//for each child
	for (int c : model.nodes[currentNode].children){
		if (c>=0){ //if it is valid
			//visit child
			loadMeshesWhileTraversingNodes(
						model,
						currentMesh,
						currentMask,
						currentMatrix,
						c,
						loadInSingleLayer,
						cb,
						progress);
		}
	}
}

/**
 * @brief Gets the 4x4 transformation matrix contained in the node itself,
 * without taking into account parent transformations of the node.
 *
 * @param model
 * @param currentNode
 * @return
 */
Matrix44m getCurrentNodeTrMatrix(
		const tinygltf::Model& model,
		unsigned int currentNode)
{
	Matrix44m currentMatrix = Matrix44m::Identity();
	//if the current node contains a 4x4 matrix
	if (model.nodes[currentNode].matrix.size() == 16) {
		vcg::Matrix44d curr(model.nodes[currentNode].matrix.data());
		curr.transposeInPlace();
		currentMatrix = Matrix44m::Construct(curr);
	}
	//if the current node contains rotation quaternion, scale vector or
	// translation vector
	else {
		//note: if one or more of these are missing, identity is used.
		//note: final matrix is computed as M = T * R * S, as specified by
		//gltf docs: https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_004_ScenesNodes.md
		//4x4 matrices associated to rotation, translation and scale
		vcg::Matrix44d rot;   rot.SetIdentity();
		vcg::Matrix44d scale; scale.SetIdentity();
		vcg::Matrix44d trans; trans.SetIdentity();

		//if node contains rotation quaternion
		if (model.nodes[currentNode].rotation.size() == 4) {
			vcg::Quaterniond qr(
					model.nodes[currentNode].rotation[3],
					model.nodes[currentNode].rotation[0],
					model.nodes[currentNode].rotation[1],
					model.nodes[currentNode].rotation[2]);
			qr.ToMatrix(rot); //set 4x4 matrix quaternion
		}
		//if node contains scale
		if (model.nodes[currentNode].scale.size() == 3) {
			//set 4x4 matrix scale
			scale.ElementAt(0,0) = model.nodes[currentNode].scale[0];
			scale.ElementAt(1,1) = model.nodes[currentNode].scale[1];
			scale.ElementAt(2,2) = model.nodes[currentNode].scale[2];
		}
		//if node contains translation
		if (model.nodes[currentNode].translation.size() == 3) {
			//set 4x4 matrix translation
			trans.ElementAt(0,3) = model.nodes[currentNode].translation[0];
			trans.ElementAt(1,3) = model.nodes[currentNode].translation[1];
			trans.ElementAt(2,3) = model.nodes[currentNode].translation[2];
		}

		//M = T * R * S
		vcg::Matrix44d curr = trans * rot * scale;
		currentMatrix = Matrix44m::Construct(curr);
	}
	return currentMatrix;
}

/**
 * @brief loads a mesh from gltf file.
 * It merges all the primitives in the loaded mesh.
 *
 * @param m: the mesh that will contain the loaded mesh
 * @param tm: tinygltf structure of the mesh to load
 * @param model: tinygltf file
 */
void loadMesh(
		MeshModel& m,
		int& mask,
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model,
		bool loadInSingleLayer,
		const Matrix44m& transf,
		vcg::CallBackPos* cb,
		CallBackProgress& progress)
{
	if (!tm.name.empty())
		m.setLabel(QString::fromStdString(tm.name));

	double oldStep = progress.step();
	progress.setStep(oldStep / tm.primitives.size());

	//for each primitive, load it into the mesh
	for (const tinygltf::Primitive& p : tm.primitives){
		internal::loadMeshPrimitive(
					m,
					mask,
					model,
					p,
					loadInSingleLayer,
					transf,
					cb,
					progress);
	}
	if (cb)
		cb(progress.progress(), "Loaded all primitives for current mesh.");
	progress.setStep(oldStep);
}

/**
 * @brief loads the given primitive into the mesh
 * @param m
 * @param model
 * @param p
 */
void loadMeshPrimitive(
		MeshModel& m,
		int& mask,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		bool loadInSingleLayer,
		const Matrix44m& transf,
		vcg::CallBackPos* cb,
		CallBackProgress& progress)
{
	double oldStep = progress.step();
	progress.setStep(oldStep/GLTF_ATTR_STR.size()+1);

	int textureImg = -1; //id of the texture associated to the material
	bool vCol = false; //used if a material has a base color for all the primitive
	vcg::Color4b col; //the base color, to be set to all the vertices

	if (p.material >= 0) { //if the primitive has a material
		const tinygltf::Material& mat = model.materials[p.material];
		auto it = mat.values.find("baseColorTexture");
		if (it != mat.values.end()){ //the material is a texture
			auto it2 = it->second.json_double_value.find("index");
			if (it2 != it->second.json_double_value.end()){
				textureImg = it2->second; //get the id of the texture
			}
		}
		it = mat.values.find("baseColorFactor");
		if (it != mat.values.end()) { //vertex base color, the same for a primitive
			vCol = true;
			const std::vector<double>& vc = it->second.number_array;
			for (unsigned int i = 0; i < 4; i++)
				col[i] = vc[i] * 255.0;
		}
	}
	if (textureImg != -1) { //if we found a texture
		const tinygltf::Image& img = model.images[model.textures[textureImg].source];
		//add the path of the texture to the mesh
		std::string uri = img.uri;
		uri = std::regex_replace(uri, std::regex("\\%20"), " ");

		if (img.image.size() > 0) {
			if (img.bits == 8 || img.component == 4) {
				QImage qimg(img.image.data(), img.width, img.height, QImage::Format_RGBA8888);
				if (!qimg.isNull()){
					QImage copy = qimg.copy();
					m.addTexture(uri, copy);
				}
				else {
					m.cm.textures.push_back(uri);
				}
			}
			else {
				m.cm.textures.push_back(uri);
			}
		}
		else {
			//set to load later (could be format non-supported by tinygltf)
			m.cm.textures.push_back(uri);
		}
		//set the id of the texture: we need it when set uv coords
		textureImg = m.cm.textures.size()-1;
	}
	//vector of vertex pointers added to the mesh
	//this vector is modified only when adding vertex position attributes
	std::vector<CMeshO::VertexPointer> ivp;

	//load vertex position attribute, sets also the ivp vector
	if (cb)
		cb(progress.progress(), "Loading vertex coordinates");
	loadAttribute(m, ivp, model, p, POSITION);
	progress.increment();

	//if all the meshes are loaded in a single layer, I need to apply
	//the transformation matrix to the loaded coordinates
	if (loadInSingleLayer){
		for (CMeshO::VertexPointer p : ivp){
			p->P() = transf * p->P();
		}
	}

	//if the mesh has a base color, set it to vertex colors
	if (vCol) {
		mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
		m.enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
		for (auto v : ivp)
			v->C() = col;
	}

	if (cb)
		cb(progress.progress(), "Loading vertex normals");
	//load all the other vertex attributes (ivp is not modified by these calls)
	bool res = loadAttribute(m, ivp, model, p, NORMAL);
	if (res) {
		mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;
		//if all the meshes are loaded in a single layer, I need to apply
		//the transformation matrix to the loaded coordinates
		if (loadInSingleLayer){
			Matrix33m mat33(transf,3);
			for (CMeshO::VertexPointer p : ivp){
				p->N() = mat33 * p->N();
			}
		}
	}
	progress.increment();

	if (cb)
		cb(progress.progress(), "Loading vertex colors");
	res = loadAttribute(m, ivp, model, p, COLOR_0);
	if (res)
		mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
	progress.increment();
	if (cb)
		cb(progress.progress(), "Loading vertex texcoords");
	res = loadAttribute(m, ivp, model, p, TEXCOORD_0, textureImg);
	if (res)
		mask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;
	progress.increment();


	//load triangles
	if (cb)
		cb(progress.progress(), "Loading triangle indices");
	loadAttribute(m, ivp, model, p, INDICES);
	progress.increment();

	if (cb)
		cb(progress.progress(), "Loaded all attributes of current mesh");

	progress.setStep(oldStep);
}

/**
 * @brief loads the attribute attr from the primitive p contained in the
 * gltf model. If the attribute is vertex position, sets also vertex pointers
 * vector ivp. For all the other parameters, ivp is a const input.
 *
 * If the primitive does not contain the primitive p, nothing is done.
 * Howerver, id the attribute is POSITION, then a MLException will be thrown.
 *
 *
 * @param m
 * @param ivp
 * @param model
 * @param p
 * @param attr
 * @param textID: id of the texture in case of the attr is TEXCOORD_0
 * @return true if the attribute has been loaded
 */
bool loadAttribute(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		GLTF_ATTR_TYPE attr,
		int textID)
{
	bool attrLoaded = false;
	const tinygltf::Accessor* accessor = nullptr;

	//get the accessor associated to the attribute
	if (attr != INDICES) {
		auto it = p.attributes.find(GLTF_ATTR_STR[attr]);

		if (it != p.attributes.end()) { //accessor found
			accessor = &model.accessors[it->second];
		}
		else if (attr == POSITION) { //if we were looking for POSITION and didn't find any
			throw MLException("File has not 'Position' attribute");
		}
	}
	else { //if the attribute is triangle indices

		//if the mode is GL_TRIANGLES and we have triangle indices
		if (p.mode == 4 && p.indices >= 0 &&
				(unsigned int)p.indices < model.accessors.size()) {
			accessor = &model.accessors[p.indices];
		}
	}

	//if we found an accessor of the attribute
	if (accessor) {
		//bufferview: contains infos on how to access buffer with the accessor
		const tinygltf::BufferView& posbw = model.bufferViews[accessor->bufferView];

		//data of the whole buffer (vector of bytes);
		//may contain also other data not associated to our attribute
		const std::vector<unsigned char>& posdata = model.buffers[posbw.buffer].data;

		//offset where the data of the attribute starts
		unsigned int posOffset = posbw.byteOffset + accessor->byteOffset;
		//hack:
		//if the attribute is a color, textid is used to tell the size of the
		//color (3 or 4 components)
		if (attr == COLOR_0){
			if (accessor->type == TINYGLTF_TYPE_VEC3)
				textID = 3;
			else if (accessor->type == TINYGLTF_TYPE_VEC4)
				textID = 4;
		}

		//if data is float
		if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
			//get the starting point of the data as float pointer
			const float* posArray = (const float*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count, textID);
			attrLoaded = true;
		}
		//if data is double
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
			//get the starting point of the data as double pointer
			const double* posArray = (const double*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count, textID);
			attrLoaded = true;
		}
		//if data is ubyte
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			//get the starting point of the data as uchar pointer
			const unsigned char* triArray = (const unsigned char*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
			attrLoaded = true;
		}
		//if data is ushort
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			//get the starting point of the data as ushort pointer
			const unsigned short* triArray = (const unsigned short*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
			attrLoaded = true;
		}
		//if data is uint
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
			//get the starting point of the data as uint pointer
			const unsigned int* triArray = (const unsigned int*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
			attrLoaded = true;
		}
	}
	//if accessor not found and attribute is indices, it means that
	//the mesh is not indexed, and triplets of contiguous vertices
	//generate triangles
	else if (attr == INDICES) {
		//this case is managed when passing nullptr as data
		populateAttr<unsigned char>(attr, m, ivp, nullptr, 0);
		attrLoaded = true;
	}
	return attrLoaded;
}

/**
 * @brief given the attribute and the pointer to the data,
 * it calls the appropriate functions that put the data into the mesh
 * appropriately
 * @param attr
 * @param m
 * @param ivp: modified only if attr
 * @param array: plain vector containing the data
 * @param number: number of elements contained in the data
 * @param textID:
 *     if attr is texcoord, it is the texture id
 *     if attr is color, tells if color has 3 or 4 components
 */
template <typename Scalar>
void populateAttr(
		GLTF_ATTR_TYPE attr,
		MeshModel&m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* array,
		unsigned int number,
		int textID)
{
	switch (attr) {
	case POSITION:
		populateVertices(m, ivp, array, number); break;
	case NORMAL:
		populateVNormals(ivp, array, number); break;
	case COLOR_0:
		populateVColors(ivp, array, number, textID); break;
		break;
	case TEXCOORD_0:
		m.enable(vcg::tri::io::Mask::IOM_VERTTEXCOORD);
		populateVTextCoords(ivp, array, number, textID); break;
		break;
	case INDICES:
		populateTriangles(m, ivp, array, number/3); break;
	}
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
void populateVColors(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* colorArray,
		unsigned int vertNumber,
		int nElemns)
{
	for (unsigned int i = 0; i < vertNumber*nElemns; i+= nElemns){
		if (!std::is_floating_point<Scalar>::value) {
			int alpha = nElemns == 4 ? colorArray[i+3] : 255;
			ivp[i/nElemns]->C() = vcg::Color4b(colorArray[i], colorArray[i+1], colorArray[i+2], alpha);
		}
		else {
			int alpha = nElemns == 4 ? colorArray[i+3] * 255 : 255;
			ivp[i/nElemns]->C() = vcg::Color4b(colorArray[i] * 255, colorArray[i+1]*255, colorArray[i+2]*255, alpha);
		}
	}
}

template <typename Scalar>
void populateVTextCoords(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* textCoordArray,
		unsigned int vertNumber,
		int textID)
{
	for (unsigned int i = 0; i < vertNumber*2; i+= 2) {
		ivp[i/2]->T() = CMeshO::VertexType::TexCoordType(textCoordArray[i], 1-textCoordArray[i+1]);
		ivp[i/2]->T().N() = textID;
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

} //namespace gltf::internal
} //namespace gltf
