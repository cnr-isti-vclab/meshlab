#include "ODEFacade.h"

using namespace std;

bool ODEFacade::m_initialized;
dSpaceID ODEFacade::m_space;
dWorldID ODEFacade::m_world;
ODEFacade::MeshMap ODEFacade::m_registeredMeshes;

ODEFacade::ODEFacade(){
    if(!m_initialized)
	initialize();
}

void ODEFacade::initialize(){ 
        m_initialized = true;
	dInitODE();
	m_world = dWorldCreate();
	m_space = dSimpleSpaceCreate(0);
}

void ODEFacade::clear(){
    for(MeshMap::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        dBodyDestroy(i->second->body);
        delete i->second;
    }
    m_registeredMeshes.clear();
}

void ODEFacade::setGlobalForce(float force[3]){
	dWorldSetGravity(m_world, force[0], force[1], force[2]);
}

void ODEFacade::registerTriMesh(MeshModel& mesh){
        if(m_registeredMeshes.find(&mesh) != m_registeredMeshes.end())
		return;

        ODEMesh* odeMesh = new ODEMesh();
        odeMesh->body = dBodyCreate(m_world);
        odeMesh->vertices = new dReal[mesh.cm.vert.size()][3];
        odeMesh->indices = new dTriIndex[mesh.cm.face.size()][3];
  	
        int i = 0;
        for(CMeshO::ConstVertexIterator vi = mesh.cm.vert.begin(); vi != mesh.cm.vert.end(); vi++, i++){
		for(int j = 0; j < 3; j++){
                        odeMesh->vertices[i][j] = vi->P()[j];
		}
  	}

        i = 0;
        for(CMeshO::ConstFaceIterator fi = mesh.cm.face.begin(); fi != mesh.cm.face.end(); fi++, i++){
    	for(int j = 0; j < 3; j++){
                odeMesh->indices[i][j] = fi->V(j) - &mesh.cm.vert[0];
      	}
  	}

        odeMesh->data = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSingle(odeMesh->data, odeMesh->vertices, 3*sizeof(dReal), mesh.cm.vert.size(), odeMesh->indices, 3*mesh.cm.face.size(), 3*sizeof(dTriIndex));
	
        odeMesh->geom = dCreateTriMesh(m_space, odeMesh->data, 0, 0, 0);
        dGeomSetBody(odeMesh->geom, odeMesh->body);
	
        m_registeredMeshes.insert(pair<MeshModel*, ODEMesh*>(&mesh, odeMesh));
}

void ODEFacade::integrate(float step){
    dWorldStep(m_world, step);

    for(MeshMap::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        MeshModel& mesh = *i->first;

        const dReal* position = dBodyGetPosition(i->second->body);
        mesh.cm.Tr.SetTranslate(position[0], position[1], position[2]);
    }
}
