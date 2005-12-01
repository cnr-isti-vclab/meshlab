
// new_refine just includes refine.h
#include "decimator.h"

// std
#include <vector>

#include<vcg/simplex/vertexplus/base.h>
#include<vcg/simplex/faceplus/base.h>
#include<vcg/simplex/face/topology.h>

#include<vcg/simplex/vertexplus/component_ocf.h>
#include<vcg/simplex/faceplus/component_ocf.h>
#include<vcg/complex/trimesh/base.h>
#include<vcg/complex/trimesh/create/platonic.h>
#include<vcg/complex/trimesh/update/topology.h>
#include<vcg/complex/trimesh/update/flag.h>
#include<vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/refine.h>

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/gl/trimesh.h>
using namespace vcg;
using namespace std;

class MyEdge;    // dummy prototype never used
class MyFace;
class MyVertex;

// Opt stuff

class MyVertex  : public VertexSimp2< MyVertex, MyEdge, MyFace, vert::Coord3f, vert::Normal3f, vert::BitFlags, vert::VFAdj >{};
class MyFace    : public FaceSimp2< MyVertex, MyEdge, MyFace, face::InfoOcf, face::FFAdj, face::WedgeTexture2f, face::VertexRef, face::BitFlags, face::Normal3f > {};
class MyMesh    : public vcg::tri::TriMesh< vector<MyVertex>, face::vector_ocf<MyFace> > {};

int  main(int argc, char **argv)
{
 if(argc<2)
	{
		printf(
		"\n                  Decimator ("__DATE__")\n"
			"						Visual Computing Group I.S.T.I. C.N.R.\n"
      "Usage: decimator filein.ply fileout.ply \n"
			);
		exit(0);
	}

  int i=4; int n_steps; float length=0;

	MyMesh m;
	if(vcg::tri::io::ImporterPLY<MyMesh>::Open(m,argv[1])!=0)
		{
      printf("Error reading file  %s\n",argv[1]);
			exit(0);
		}
	vcg::tri::UpdateTopology<MyMesh>::VertexFace(m);
	vcg::tri::UpdateTopology<MyMesh>::FaceFace(m);
  vcg::tri::UpdateFlags<MyMesh>::FaceBorderFromFF(m);
 	vcg::tri::UpdateNormals<MyMesh>::PerVertexNormalized(m);

  printf("Input mesh  vn:%i fn:%i\n",m.vn,m.fn);
	
	Decimator<class MyMesh>(m,10);

	printf("Output mesh vn:%i fn:%i\n",m.vn,m.fn);
	
  vcg::tri::io::PlyInfo pi;
	vcg::tri::io::ExporterPLY<MyMesh>::Save(m,argv[2],pi.mask);
	return 0;
	}
