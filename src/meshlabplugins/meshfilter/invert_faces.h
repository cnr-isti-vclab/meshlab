#ifndef __VCGLIB_INVERT_FACES
#define __VCGLIB_INVERT_FACES

#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/update/normal.h>
#include <algorithm>

#include <iostream>
#include <QtGlobal>
namespace vcg{

	template<class MESH_TYPE>
		void InvertFaces(MESH_TYPE &m)
	{	
		typename MESH_TYPE::FaceIterator fi;
		typename MESH_TYPE::VertexType v1,v2;
		// Test per gli edge manifold
		
		for (fi = m.face.begin(); fi != m.face.end(); ++fi)
		{
				
				swap((*fi).V1(0), (*fi).V2(0));
		}
		vcg::tri::UpdateNormals<MESH_TYPE>::PerVertexNormalizedPerFace(m);
		vcg::tri::UpdateTopology<MESH_TYPE>::FaceFace(m);
		vcg::tri::UpdateTopology<MESH_TYPE>::VertexFace(m);

	}	

} // end of namespace

#endif
