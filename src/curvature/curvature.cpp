#include <iostream>
#include<vcg/simplex/vertex/vertex.h>
#include<vcg/simplex/vertex/with/vn.h>
#include<vcg/simplex/face/with/fn.h>
#include<vcg/simplex/face/with/fcfn.h>
#include<vcg/complex/trimesh/base.h>

using namespace vcg;
using namespace std;

class AEdge;    // dummy prototype never used
class AFace;


class AVertex   : public vcg::VertexVN< double,AEdge,AFace > {
protected:
	float _c;
public:
	float & C() {
		return _c;
	}
	void setC(float c) {
		_c = c;
	}
};


class AFace     : public vcg::FaceFN< AVertex,AEdge,AFace > {};
class AMesh     : public vcg::tri::TriMesh< std::vector<AVertex>, std::vector<AFace> > {};


int main(int argc, char**argv) {
	
	AVertex v;
	v.setC(2.0f);

	std::cout << "la curvatura e' : " << v.C() << endl;
	

	return 1;
}