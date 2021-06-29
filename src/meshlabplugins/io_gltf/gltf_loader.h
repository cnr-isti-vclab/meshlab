#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <common/ml_document/mesh_model.h>

namespace gltf {

std::vector<Matrix44m> loadTrMatrices(
		const tinygltf::Model& model);

void loadMesh(
		MeshModel& m,
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model);

}


#endif // GLTF_LOADER_H
