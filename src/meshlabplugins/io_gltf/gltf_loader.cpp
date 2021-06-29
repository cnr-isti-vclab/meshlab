#include "gltf_loader.h"

#include <common/mlexception.h>

void visitNode(
		const tinygltf::Model& model,
		unsigned int i,
		Matrix44m m,
		std::vector<bool>& visited,
		std::vector<Matrix44m>& trm);

//declarations
enum GLTF_ATTR_TYPE {POSITION, NORMAL, COLOR_0, TEXCOORD_0, INDICES};
const std::array<std::string, 4> GLTF_ATTR_STR {"POSITION", "NORMAL", "COLOR_0", "TEXCOORD_0"};

void loadMeshPrimitive(
		MeshModel& m,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p);

void loadAttribute(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		GLTF_ATTR_TYPE attr,
		int textID = -1);

template <typename Scalar>
void populateAttr(
		GLTF_ATTR_TYPE attr,
		MeshModel&m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* array,
		unsigned int number,
		int textID = -1);

template <typename Scalar>
void populateVertices(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* posArray,
		unsigned int vertNumber);

template <typename Scalar>
void populateVNormals(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* normArray,
		unsigned int vertNumber);

template <typename Scalar>
void populateVColors(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* colorArray,
		unsigned int vertNumber,
		int nElemns);

template <typename Scalar>
void populateVTextCoords(
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* textCoordArray,
		unsigned int vertNumber,
		int textID);

template <typename Scalar>
void populateTriangles(
		MeshModel&m,
		const std::vector<CMeshO::VertexPointer>& ivp,
		const Scalar* triArray,
		unsigned int triNumber);

 /**************
 * Definitions *
 **************/

std::vector<Matrix44m> gltf::loadTrMatrices(
		const tinygltf::Model& model)
{
	std::vector<Matrix44m> trm(model.meshes.size());
	std::vector<bool> visited(model.nodes.size(), false);
	for (unsigned int i = 0; i < model.nodes.size(); ++i){
		if (!visited[i]){
			Matrix44m startM = Matrix44m::Identity();
			visitNode(model, i, startM, visited, trm);
			visited[i] = true;
		}
	}
	return trm;
}

void visitNode(
		const tinygltf::Model& model,
		unsigned int i,
		Matrix44m m,
		std::vector<bool>& visited,
		std::vector<Matrix44m>& trm)
{
	if (model.nodes[i].matrix.size() == 16) {
		vcg::Matrix44d curr(model.nodes[i].matrix.data());
		curr.transposeInPlace();
		m = m * Matrix44m::Construct(curr);
	}
	else {
		vcg::Matrix44d rot;   rot.SetIdentity();
		vcg::Matrix44d scale; scale.SetIdentity();
		vcg::Matrix44d trans; trans.SetIdentity();
		if (model.nodes[i].rotation.size() == 4) {
			vcg::Quaterniond qr(
					model.nodes[i].rotation[3],
					model.nodes[i].rotation[0],
					model.nodes[i].rotation[1],
					model.nodes[i].rotation[2]);
			qr.ToMatrix(rot);
		}
		if (model.nodes[i].scale.size() == 3) {
			scale.ElementAt(0,0) = model.nodes[i].scale[0];
			scale.ElementAt(1,1) = model.nodes[i].scale[1];
			scale.ElementAt(2,2) = model.nodes[i].scale[2];
		}
		if (model.nodes[i].translation.size() == 3) {
			trans.ElementAt(0,3) = model.nodes[i].translation[0];
			trans.ElementAt(1,3) = model.nodes[i].translation[1];
			trans.ElementAt(2,3) = model.nodes[i].translation[2];
		}
		vcg::Matrix44d curr = trans * rot * scale;
		m = m * Matrix44m::Construct(curr);
	}

	if (model.nodes[i].mesh >= 0){
		trm[model.nodes[i].mesh] = m;
	}
	for (int c : model.nodes[i].children){
		if (c>=0){
			visitNode(model, c, m, visited, trm);
			visited[c] = true;
		}
	}
}

void gltf::loadMesh(
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

void loadMeshPrimitive(
		MeshModel& m,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p)
{
	int textureImg = -1;
	bool vCol = false;
	vcg::Color4b col;
	if (p.material >= 0) {
		const tinygltf::Material& mat = model.materials[p.material];
		auto it = mat.values.find("baseColorTexture");
		if (it != mat.values.end()){ //the material is a texture
			auto it2 = it->second.json_double_value.find("index");
			if (it2 != it->second.json_double_value.end()){
				textureImg = it2->second;
			}
		}
		it = mat.values.find("baseColorFactor");
		if (it != mat.values.end()) { //vertex color, the same for a primitive
			vCol = true;
			const std::vector<double>& vc = it->second.number_array;
			for (unsigned int i = 0; i < 4; i++)
				col[i] = vc[i] * 255.0;
		}
	}
	if (textureImg != -1) {
		m.cm.textures.push_back(model.images[textureImg].uri);
		textureImg = m.cm.textures.size()-1;
	}
	std::vector<CMeshO::VertexPointer> ivp;
	loadAttribute(m, ivp, model, p, POSITION);

	if (vCol) {
		m.enable(vcg::tri::io::Mask::IOM_VERTCOLOR);
		for (auto v : ivp)
			v->C() = col;
	}

	loadAttribute(m, ivp, model, p, NORMAL);
	loadAttribute(m, ivp, model, p, COLOR_0);
	loadAttribute(m, ivp, model, p, TEXCOORD_0, textureImg);
	loadAttribute(m, ivp, model, p, INDICES);

}

void loadAttribute(
		MeshModel& m,
		std::vector<CMeshO::VertexPointer>& ivp,
		const tinygltf::Model& model,
		const tinygltf::Primitive& p,
		GLTF_ATTR_TYPE attr,
		int textID)
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
		//if the attribute is a color, textid is used to tell the size of the
		//color (3 or 4 components)
		if (attr == COLOR_0){
			if (accessor->type == TINYGLTF_TYPE_VEC3)
				textID = 3;
			else if (accessor->type == TINYGLTF_TYPE_VEC4)
				textID = 4;
		}

		if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
			const float* posArray = (const float*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count, textID);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
			const double* posArray = (const double*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, posArray, accessor->count, textID);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
			const unsigned char* triArray = (const unsigned char*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
			const unsigned short* triArray = (const unsigned short*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
		}
		else if (accessor->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
			const unsigned int* triArray = (const unsigned int*) (posdata.data() + posOffset);
			populateAttr(attr, m, ivp, triArray, accessor->count, textID);
		}
	}
	else if (attr == INDICES) {
		populateAttr<unsigned char>(attr, m, ivp, nullptr, 0);
	}

}

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
