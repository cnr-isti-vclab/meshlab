#include "ODEFacade.h"

using namespace std;

ODEFacade::ODEFacade(){
	initialize();
}

ODEFacade::~ODEFacade(){
	finalize();
}

void ODEFacade::initialize(){ 
	dInitODE();
	m_world = dWorldCreate();
	m_space = dSimpleSpaceCreate(0);
}

void ODEFacade::finalize(){
	dWorldDestroy(m_world);
	dCloseODE();
}

void ODEFacade::setGlobalForce(int force[3]){
	dWorldSetGravity(m_world, force[0], force[1], force[2]);
}

void ODEFacade::registerTriMesh(const MeshModel& mesh){
	if(m_triMeshMap.find(&mesh) != m_triMeshMap.end())
		return;
	
	ODETriMesh odeTriMesh;
        odeTriMesh.vertices = new dReal[mesh.cm.vert.size()][3];
        odeTriMesh.indices = new dTriIndex[mesh.cm.face.size()][3];
  	
        int i = 0;
        for(CMeshO::ConstVertexIterator vi = mesh.cm.vert.begin(); vi != mesh.cm.vert.end(); vi++, i++){
		for(int j = 0; j < 3; j++){
			odeTriMesh.vertices[i][j] = vi->P()[j];
		}
  	}

        i = 0;
        for(CMeshO::ConstFaceIterator fi = mesh.cm.face.begin(); fi != mesh.cm.face.end(); fi++, i++){
    	for(int j = 0; j < 3; j++){
                odeTriMesh.indices[i][j] = fi->V(j) - &mesh.cm.vert[0];
      	}
  	}

	odeTriMesh.data = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSingle(odeTriMesh.data, odeTriMesh.vertices, 3*sizeof(dReal), mesh.cm.vert.size(), odeTriMesh.indices, 3*mesh.cm.face.size(), 3*sizeof(dTriIndex));
	
        odeTriMesh.geom = dCreateTriMesh(m_space, odeTriMesh.data, 0, 0, 0);
	
        m_triMeshMap.insert(pair<const MeshModel*, ODETriMesh>(&mesh, odeTriMesh));
}
