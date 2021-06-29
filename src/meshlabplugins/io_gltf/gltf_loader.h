#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

class MeshModel;

namespace gltf {

void loadMesh(
		MeshModel& m,
		const tinygltf::Mesh& tm,
		const tinygltf::Model& model);

}


#endif // GLTF_LOADER_H
