
// new_refine just includes refine.h
#include "new_refine.h"

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

// struct MyFace;
// struct MyEdge;
// struct MyVertex: public VertexAFVN<float,MyEdge,MyFace>{};
// struct MyFace: public FaceAFAV<MyVertex,MyEdge,MyFace>{};
// struct MyMesh: public tri::TriMesh< vector<MyVertex>, vector<MyFace> >{};
class MyEdge;    // dummy prototype never used
class MyFace;
class MyVertex;

// Opt stuff

class MyVertex  : public VertexSimp2< MyVertex, MyEdge, MyFace, vert::Coord3f, vert::Normal3f, vert::BitFlags, vert::VFAdj >{};
class MyFace    : public FaceSimp2< MyVertex, MyEdge, MyFace, face::InfoOcf, face::FFAdj, face::WedgeTexture2f, face::VertexRef, face::BitFlags, face::Normal3f > {};
class MyMesh    : public vcg::tri::TriMesh< vector<MyVertex>, face::vector_ocf<MyFace> > {};

#define FLAT	0
#define BUTTERFLY 2
#define LOOP 3
	
int  main(int argc, char **argv)
{
 if(argc<4)
	{
		printf(
		"\n                  PlyRefine ("__DATE__")\n"
			"						Visual Computing Group I.S.T.I. C.N.R.\n"
      "Usage: PlyRefine filein.ply fileout.ply ref_step [opt] \n"
			"Commands: \n"
			" Refinement rules:\n"
      "     -m  use simple midpoint subdivision (default) \n"
  		"     -b  use butterfly subdivision scheme \n"
	  	"     -l# refine only if the the edge is longer than #(default 0.0)\n"
	 	  "     -L  use Loop subdivision scheme \n"
			);
		exit(0);
	}

	int RefMode = FLAT	;
  int i=4; int n_steps; float length=0;
	while(i<argc)
		{
			if(argv[i][0]!='-')
				{printf("Error unable to parse option '%s'\n",argv[i]); exit(0);}
			switch(argv[i][1])
			{				
			case 'm' :	RefMode=FLAT; break;
			case 'L' :  RefMode=LOOP; break;
			case 'b' :	RefMode=BUTTERFLY; break;
			case 'l' :	length=(float)atof(argv[i]+2); break;
			default : {printf("Error unable to parse option '%s'\n",argv[i]); exit(0);}
			}
			++i;
		}

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
	
  n_steps=atoi(argv[3]);
	
  for(i=0;i < n_steps;++i)			
  {
    switch(RefMode){
			case FLAT:      Refine<MyMesh, MidPoint<MyMesh> >(m,MidPoint<MyMesh>(),length); 					break;
			case BUTTERFLY: Refine<MyMesh, MidPointButterfly<MyMesh> >(m,MidPointButterfly<MyMesh>(),length); break;
 			case LOOP: RefineOddEvenE<MyMesh, OddPointLoop<MyMesh>, EvenPointLoop<MyMesh> >(m,OddPointLoop<MyMesh>(), EvenPointLoop<MyMesh>(),length); break;
				

//			case LOOP: Refine<MyMesh, EvenPointLoop<MyMesh> >(m, EvenPointLoop<MyMesh>(),length); break;
    }					
  }
  
  printf("Output mesh vn:%i fn:%i\n",m.vn,m.fn);
	
  vcg::tri::io::PlyInfo pi;
	vcg::tri::io::ExporterPLY<MyMesh>::Save(m,argv[2],pi.mask);
	return 0;
	}
