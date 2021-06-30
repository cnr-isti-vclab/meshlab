#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <common/ml_document/mesh_model.h>

namespace gltf {

unsigned int getNumberMeshes(
		const tinygltf::Model& model);

void loadMeshes(
		const std::list<MeshModel*>& meshModelList,
		std::list<int>& maskList,
		const tinygltf::Model& model);

namespace internal {

enum GLTF_ATTR_TYPE {POSITION, NORMAL, COLOR_0, TEXCOORD_0, INDICES};
const std::array<std::string, 4> GLTF_ATTR_STR {"POSITION", "NORMAL", "COLOR_0", "TEXCOORD_0"};

unsigned int getNumberMeshes(
		const tinygltf::Model& model,
		unsigned int node);

void loadMeshesWhileTraversingNodes(
		const tinygltf::Model& model,
		std::list<MeshModel*>::const_iterator& currentMesh,
		std::list<int>::iterator& currentMask,
		Matrix44m currentMatrix,
		unsigned int currentNode);

Matrix44m getCurrentNodeTrMatrix(
		const tinygltf::Model& model,
		unsigned int currentNode);

void loadMesh(
		MeshModel& m,
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model);

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
}

}


#endif // GLTF_LOADER_H
