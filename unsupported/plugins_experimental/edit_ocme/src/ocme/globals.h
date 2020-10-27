#ifndef GLOBALS_H
#define GLOBALS_H

#include "../utils/name_access_function_bounds.h"
#include "vcg_mesh.h"
#include <string>
/* Everything in this file prevents multiple ocme files to be safely used concurrently
 */


struct OcmeGlobals{

	/*
	  This is global table that connects the name of the components to the access function.
	  It should be in the OCME class but then we should move the functions for transferring
	  the attributes from the mesh to ocm (and return) (those in cell_attribues.h) in the OCME
	  class as well and it would be pretty ugly.
	  */
	static 	nafb::NameAccessFunctionBounds & NAFB(){static 	nafb::NameAccessFunctionBounds   bounds; return bounds; }

	template <class MeshType>
	static void  FillNAFB(){
		/* vertex components */
		nafb::AddNameAccessFunctionBound<vcg::Color4b,typename MeshType::VertexType>(std::string("vertex::Color4b"),& MeshType::VertexType::C,NAFB());
		nafb::AddNameAccessFunctionBound<vcg::Point3f,typename MeshType::VertexType>(std::string("vertex::Normal3f"),& MeshType::VertexType::N,NAFB());
		nafb::AddNameAccessFunctionBound<float,typename MeshType::VertexType>(std::string("vertex::Qualityf"),& MeshType::VertexType::Q,NAFB());
		nafb::AddNameAccessFunctionBound<vcg::TexCoord2f,typename MeshType::VertexType>(std::string("vertex::TexCoord2f"),& MeshType::VertexType::T,NAFB());
		/* face components */
		nafb::AddNameAccessFunctionBound<vcg::Color4b,typename MeshType::FaceType>(std::string("face::Color4b"),& MeshType::FaceType::C,NAFB());
		nafb::AddNameAccessFunctionBound<vcg::Point3f,typename MeshType::FaceType>(std::string("face::Normal3f"),& MeshType::FaceType::N,NAFB());
	}
};

#endif // GLOBALS_H
