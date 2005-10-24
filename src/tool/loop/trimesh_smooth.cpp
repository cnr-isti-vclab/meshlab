#include <vector>

#include<vcg/simplex/vertex/vertex.h>
#include<vcg/simplex/face/with/fn.h>
#include<vcg/complex/trimesh/base.h>
// to clean up a mesh
#include<vcg/complex/trimesh/clean.h>
#include<vcg/complex/trimesh/smooth.h>

// input output
#include <wrap/io_trimesh/import.h>
#include <wrap/io_trimesh/export_ply.h>


using namespace vcg;

class MyFace;
class MyEdge;
class MyVertex:public Vertex<float,MyEdge,MyFace>{};
class MyFace :public FaceFN<MyVertex,MyEdge,MyFace>{};
class MyMesh: public tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace > >{};


int main(int argc,char ** argv)
{
if(argc<3) 
{
  printf("Usage: trimesh_smooth <filename> <steps>\n");
  return 0;
}

	MyMesh m;

	//open a mesh
	int err = tri::io::Importer<MyMesh>::Open(m,argv[1]);
  if(err) {
      printf("Error in reading %s: '%s'\n",argv[1],tri::io::Importer<MyMesh>::ErrorMsg(err));
      exit(-1);
    }
  // some cleaning to get rid of bad file formats like stl that duplicate vertexes..
  int dup = tri::Clean<MyMesh>::RemoveDuplicateVertex(m);
  int unref =  tri::Clean<MyMesh>::RemoveUnreferencedVertex(m);

  printf("Removed %i duplicate and %i unreferenced vertices from mesh %s\n",dup,unref,argv[1]);

  LaplacianSmooth(m,atoi(argv[2]));
  tri::io::ExporterPLY<MyMesh>::Save(m,"out.ply");

  return 0;
}

