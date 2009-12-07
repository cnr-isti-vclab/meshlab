#include "ODEFacade.h"

#include <vcg/complex/trimesh/inertia.h>

using namespace std;
using namespace vcg;

bool ODEFacade::m_initialized;
const int ODEFacade::m_maxContacts = 40;
dSpaceID ODEFacade::m_space;
dWorldID ODEFacade::m_world;
dJointGroupID ODEFacade::m_contactGroup;
ODEFacade::MeshContainer ODEFacade::m_registeredMeshes;

ODEFacade::ODEFacade(){
    if(!m_initialized)
	initialize();
}

void ODEFacade::initialize(){ 
        m_initialized = true;
	dInitODE();
	m_world = dWorldCreate();
	m_space = dSimpleSpaceCreate(0);
        m_contactGroup = dJointGroupCreate(m_maxContacts);
}

void ODEFacade::clear(){
    for(MeshContainer::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        tri::Allocator<CMeshO>::DeletePerMeshAttribute(i->first->cm, "physicsID");

        dGeomDestroy(i->second->geom);
        if(i->second->body)
            dBodyDestroy(i->second->body);

        delete i->second;
    }
    m_registeredMeshes.clear();
}

void ODEFacade::setGlobalForce(float force[3]){
	dWorldSetGravity(m_world, force[0], force[1], force[2]);
}

void ODEFacade::registerTriMesh(MeshModel& mesh, bool scenery){
    if(tri::HasPerMeshAttribute(mesh.cm, "physicsID"))
        return;

    ODEMesh* odeMesh = new ODEMesh();
    odeMesh->vertices = new dReal[mesh.cm.vert.size()][3];
    odeMesh->normals = new dReal[mesh.cm.face.size()][3];
    odeMesh->indices = new dTriIndex[mesh.cm.face.size()][3];

    int i = 0;
    for(CMeshO::ConstVertexIterator vi = mesh.cm.vert.begin(); vi != mesh.cm.vert.end(); vi++, i++){
        for(int j = 0; j < 3; j++){
            odeMesh->vertices[i][j] = vi->P()[j];
        }
    }

    i = 0;
    for(CMeshO::FaceIterator fi = mesh.cm.face.begin(); fi != mesh.cm.face.end(); fi++, i++){
        for(int j = 0; j < 3; j++){
            odeMesh->indices[i][j] = fi->V(j) - &mesh.cm.vert[0];
            odeMesh->normals[i][j] = fi->N()[j];
        }
    }

    odeMesh->data = dGeomTriMeshDataCreate();
    dGeomTriMeshDataBuildSingle1(odeMesh->data, odeMesh->vertices, 3*sizeof(dReal), mesh.cm.vert.size(), odeMesh->indices, 3*mesh.cm.face.size(), 3*sizeof(dTriIndex), odeMesh->normals);

    odeMesh->geom = dCreateTriMesh(m_space, odeMesh->data, 0, 0, 0);

    MeshIndex index = tri::Allocator<CMeshO>::AddPerMeshAttribute<unsigned int>(mesh.cm, "physicsID");
    index() = m_registeredMeshes.size();
    m_registeredMeshes.push_back(make_pair(&mesh, odeMesh));

    setAsRigidBody(mesh, !scenery);
}

void ODEFacade::setAsRigidBody(MeshModel& mesh, bool isRigidBody){
    if(!tri::HasPerMeshAttribute(mesh.cm, "physicsID"))
        return;

    MeshIndex index = tri::Allocator<CMeshO>::GetPerMeshAttribute<unsigned int>(mesh.cm, "physicsID");

    if(!isRigidBody && m_registeredMeshes[index()].second->body != 0){ // This mesh was registered as a rigid body
        dBodyDestroy(m_registeredMeshes[index()].second->body);
        m_registeredMeshes[index()].second->body = 0;
    }else if(isRigidBody && m_registeredMeshes[index()].second->body == 0){ // This mesh was registered as part of the scenery
        m_registeredMeshes[index()].second->body = dBodyCreate(m_world);
        dGeomSetBody(m_registeredMeshes[index()].second->geom, m_registeredMeshes[index()].second->body);

        tri::Inertia<CMeshO> inertia;
        inertia.Compute(m_registeredMeshes[index()].first->cm);

        Matrix33f IT;
        inertia.InertiaTensor(IT);
        dMassSetParameters(&m_registeredMeshes[index()].second->mass, inertia.Mass(),
                           inertia.CenterOfMass()[0], inertia.CenterOfMass()[1], inertia.CenterOfMass()[2],
                           IT[0][0], IT[1][1], IT[2][2], IT[0][1], IT[0][2], IT[1][2]);

    }

    updateEngineTransform(mesh);
}

void ODEFacade::updateEngineTransform(MeshModel& mesh){
    if(!tri::HasPerMeshAttribute(mesh.cm, "physicsID"))
        return;

    MeshIndex index = tri::Allocator<CMeshO>::GetPerMeshAttribute<unsigned int>(mesh.cm, "physicsID");

    Point3<float> trans = mesh.cm.Tr.GetColumn3(3);
    dGeomSetPosition(m_registeredMeshes[index()].second->geom, trans.X(), trans.Y(), trans.Z());

    dMatrix3 rotationMatrix;

    rotationMatrix[0] = mesh.cm.Tr[0][0];
    rotationMatrix[1] = mesh.cm.Tr[0][1];
    rotationMatrix[2] = mesh.cm.Tr[0][2];
    rotationMatrix[3] = 0.f;
    rotationMatrix[4] = mesh.cm.Tr[1][0];
    rotationMatrix[5] = mesh.cm.Tr[1][1];
    rotationMatrix[6] = mesh.cm.Tr[1][2];
    rotationMatrix[7] = 0.f;
    rotationMatrix[8] = mesh.cm.Tr[2][0];
    rotationMatrix[9] = mesh.cm.Tr[2][1];
    rotationMatrix[10] = mesh.cm.Tr[2][2];
    rotationMatrix[11] = 0.f;

    dGeomSetRotation(m_registeredMeshes[index()].second->geom, rotationMatrix);
}

void ODEFacade::updateTransform(){
    for(MeshContainer::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        const dReal* position = dGeomGetPosition(i->second->geom);
        i->first->cm.Tr.SetTranslate(position[0], position[1], position[2]);

        const dReal* rotation = dGeomGetRotation(i->second->geom);
        i->first->cm.Tr[0][0] = rotation[0];
        i->first->cm.Tr[0][1] = rotation[1];
        i->first->cm.Tr[0][2] = rotation[2];
        i->first->cm.Tr[1][0] = rotation[4];
        i->first->cm.Tr[1][1] = rotation[5];
        i->first->cm.Tr[1][2] = rotation[6];
        i->first->cm.Tr[2][0] = rotation[8];
        i->first->cm.Tr[2][1] = rotation[9];
        i->first->cm.Tr[2][2] = rotation[10];
    }
}

void ODEFacade::integrate(float step){
    dSpaceCollide(m_space, this, collisionCallback);
    dWorldStep(m_world, step);
    dJointGroupEmpty(m_contactGroup);
}

void ODEFacade::collisionCallback(void* data, dGeomID o1, dGeomID o2){
    ODEFacade* engine = static_cast<ODEFacade*>(data);
    engine->collisionCallback(o1, o2);
}

void ODEFacade::collisionCallback(dGeomID o1, dGeomID o2){
    dBodyID body1 = dGeomGetBody(o1);
    dBodyID body2 = dGeomGetBody(o2);

    dContact contacts[m_maxContacts];

    for(int i = 0; i < m_maxContacts; i++){
        contacts[i].surface.mode = dContactBounce | dContactSoftCFM;
        contacts[i].surface.mu = 0.5;
        contacts[i].surface.bounce = 0.1;
        contacts[i].surface.bounce_vel = 0.1;
        contacts[i].surface.soft_cfm = 0.01;
    }

    int collisions = dCollide(o1, o2, m_maxContacts, &contacts[0].geom, sizeof(dContact));
    for(int i = 0; i < collisions; i++){
        dJointID joint = dJointCreateContact(m_world, m_contactGroup, &contacts[i]);
        dJointAttach(joint, body1, body2);
    }
}

vcg::Matrix44f ODEFacade::getTransformationMatrix(MeshModel& mesh){
    if(!tri::HasPerMeshAttribute(mesh.cm, "physicsID"))
        return vcg::Matrix44f();

    MeshIndex index = tri::Allocator<CMeshO>::GetPerMeshAttribute<unsigned int>(mesh.cm, "physicsID");

    vcg::Matrix44f matrix;
    const dReal* position = dGeomGetPosition(m_registeredMeshes[index()].second->geom);
    const dReal* rotation = dGeomGetRotation(m_registeredMeshes[index()].second->geom);

    matrix[0][0] = rotation[0];
    matrix[0][1] = rotation[1];
    matrix[0][2] = rotation[2];
    matrix[0][3] = position[0];
    matrix[1][0] = rotation[4];
    matrix[1][1] = rotation[5];
    matrix[1][2] = rotation[6];
    matrix[1][3] = position[1];
    matrix[2][0] = rotation[8];
    matrix[2][1] = rotation[9];
    matrix[2][2] = rotation[10];
    matrix[2][3] = position[2];
    matrix[3][0] = 0.f;
    matrix[3][1] = 0.f;
    matrix[3][2] = 0.f;
    matrix[3][3] = 1.f;

    return matrix;
}
