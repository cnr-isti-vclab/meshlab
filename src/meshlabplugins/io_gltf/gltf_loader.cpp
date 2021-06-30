#include "gltf_loader.h"

#include <regex>
#include <common/mlexception.h>

namespace gltf {

/**
 * @brief Loads the list of rotation matrices for each mesh contained in the
 * gltf file.
 * @param model
 * @return a vector containing N 4x4 matrices
 */
std::vector<Matrix44m> loadTrMatrices(
		const tinygltf::Model& model)
{
	std::vector<Matrix44m> trm(model.meshes.size());
	std::vector<bool> visited(model.nodes.size(), false);

	//this for will visit all the root nodes of the file
	for (unsigned int i = 0; i < model.nodes.size(); ++i){
		if (!visited[i]){ //if not visited, it is a root node
			Matrix44m startM = Matrix44m::Identity();

			//recursive call: it will visit all the children of ith node
			internal::visitNodeAndGetTrMatrix(model, i, startM, visited, trm);
		}
	}
	return trm;
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
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model)
{
	if (!tm.name.empty())
		m.setLabel(QString::fromStdString(tm.name));

	//for each primitive, load it into the mesh
	for (const tinygltf::Primitive& p : tm.primitives){
		internal::loadMeshPrimitive(m, model, p);
	}
}

namespace internal {

/**
 * @brief Recursive function that visits a node and calls the visit on all its
 * children nodes.
 * Then, if a node is associated to a mesh, the combination of the matrices
 * of the current node and all its parents is set to trm vector
 *
 * @param model
 * @param i: id of the node that is currently visited
 * @param m: the combination of the matrices of all the parents of i
 * @param visited: vector of visited flags
 * @param trm: vector of matrices: it will be updated when a mesh node is found
 */
void visitNodeAndGetTrMatrix(
		const tinygltf::Model& model,
		unsigned int i,
		Matrix44m m,
		std::vector<bool>& visited,
		std::vector<Matrix44m>& trm)
{
	visited[i] = true; //current node is visited
	//if the current node contains a 4x4 matrix
	if (model.nodes[i].matrix.size() == 16) {
		vcg::Matrix44d curr(model.nodes[i].matrix.data());
		curr.transposeInPlace();
		m = m * Matrix44m::Construct(curr);
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
		if (model.nodes[i].rotation.size() == 4) {
			vcg::Quaterniond qr(
					model.nodes[i].rotation[3],
					model.nodes[i].rotation[0],
					model.nodes[i].rotation[1],
					model.nodes[i].rotation[2]);
			qr.ToMatrix(rot); //set 4x4 matrix quaternion
		}
		//if node contains scale
		if (model.nodes[i].scale.size() == 3) {
			//set 4x4 matrix scale
			scale.ElementAt(0,0) = model.nodes[i].scale[0];
			scale.ElementAt(1,1) = model.nodes[i].scale[1];
			scale.ElementAt(2,2) = model.nodes[i].scale[2];
		}
		//if node contains translation
		if (model.nodes[i].translation.size() == 3) {
			//set 4x4 matrix translation
			trans.ElementAt(0,3) = model.nodes[i].translation[0];
			trans.ElementAt(1,3) = model.nodes[i].translation[1];
			trans.ElementAt(2,3) = model.nodes[i].translation[2];
		}

		//M = T * R * S
		vcg::Matrix44d curr = trans * rot * scale;

		//combine current matrix with parents
		m = m * Matrix44m::Construct(curr);
	}

	//if this node represents a mesh
	if (model.nodes[i].mesh >= 0){
		//set the m matrix to trm vector
		trm[model.nodes[i].mesh] = m;
	}

	//for each child
	for (int c : model.nodes[i].children){
		if (c>=0){ //if it is valid
			//visit child, passing the current matrix
			visitNodeAndGetTrMatrix(model, c, m, visited, trm);
		}
	}
}

/**
 * @brief loads the given primitive into the mesh
 * @param m
 * @param model
 * @param p
 */
void loadMeshPrimitive(
		MeshModel& m,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p)
{
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
		//add the path of the texture to the mesh
		std::string uri = model.images[model.textures[textureImg].source].uri;
		uri = std::regex_replace(uri, std::regex("\\%20"), " ");
		m.cm.textures.push_back(uri);
		//set the id of the texture: we need it when set uv coords
		textureImg = m.cm.textures.size()-1;
	}
	//vector of vertex pointers added to the mesh
	//this vector is modified only when adding vertex position attributes
	std::vector<CMeshO::VertexPointer> ivp;

	//load vertex position attribute, sets also the ivp vector
	loadAttribute(m, ivp, model, p, POSITION);

	//if the mesh has a base color, set it to vertex colors
	if (vCol) {
		m.enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
		for (auto v : ivp)
			v->C() = col;
	}

	//load all the other vertex attributes (ivp is not modified by these calls)
	loadAttribute(m, ivp, model, p, NORMAL);
	loadAttribute(m, ivp, model, p, COLOR_0);
	loadAttribute(m, ivp, model, p, TEXCOORD_0, textureImg);

	//load triangles
	loadAttribute(m, ivp, model, p, INDICES);

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
 */
void loadAttribute(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		GLTF_ATTR_TYPE attr,
		int textID)
{
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
		}
		//if data is double
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
			//get the starting point of the data as double pointer
			const double* posArray = (const double*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count, textID);
		}
		//if data is ubyte
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			//get the starting point of the data as uchar pointer
			const unsigned char* triArray = (const unsigned char*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
		}
		//if data is ushort
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			//get the starting point of the data as ushort pointer
			const unsigned short* triArray = (const unsigned short*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
		}
		//if data is uint
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
			//get the starting point of the data as uint pointer
			const unsigned int* triArray = (const unsigned int*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
		}
	}
	//if accessor not found and attribute is indices, it means that
	//the mesh is not indexed, and triplets of contiguous vertices
	//generate triangles
	else if (attr == INDICES) {
		//this case is managed when passing nullptr as data
		populateAttr<unsigned char>(attr, m, ivp, nullptr, 0);
	}

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
