
// mesh definition 
#include <vcg/simplex/vertex/with/vn.h>
#include <vcg/simplex/face/with/af.h>
#include <vcg/complex/trimesh/base.h>

#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/flag.h>

#include "import_obj.h"
#include "export_obj.h"

#include <iostream>

using namespace vcg;
using namespace std;

struct MyFace;
struct MyEdge;
struct MyVertex: public VertexVN<float,MyEdge,MyFace>{};
struct MyFace: public FaceAF<MyVertex,MyEdge,MyFace>{};
struct MyMesh: public vcg::tri::TriMesh< vector<MyVertex>, vector<MyFace> >{};

int main(int argc, char **argv)
{
	if(argc < 3){return 0;}
	MyMesh m;


	vcg::tri::io::ImporterOBJ<MyMesh>::OpenAscii(m,argv[1],0); 
	bool result = vcg::tri::io::ExporterOBJ<MyMesh>::Save(m,argv[2],false,0);
	if(result){std::cout << "file is copied!" << std::endl;}else{std::cout << " file is not copied!" << std::endl;}

    return 0;
}
