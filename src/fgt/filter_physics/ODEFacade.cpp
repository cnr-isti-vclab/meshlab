#include "ODEFacade.h"

#include <vcg/complex/trimesh/inertia.h>

#include <algorithm>
#include <cmath>

using namespace std;
using namespace vcg;

bool ODEFacade::m_initialized;
float ODEFacade::m_bounciness = 0.1f;
float ODEFacade::m_friction = 10.f;

dSpaceID ODEFacade::m_space;
std::vector<dContact> ODEFacade::m_contacts(20);
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
        m_contactGroup = dJointGroupCreate(0);
}

void ODEFacade::clear(){
    for(MeshIterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        dGeomDestroy(i->second->geom);
        if(i->second->body) dBodyDestroy(i->second->body);
        delete i->second;
    }
    m_registeredMeshes.clear();
}

void ODEFacade::setGlobalForce(float force[3]){
	dWorldSetGravity(m_world, force[0], force[1], force[2]);
}

void ODEFacade::registerTriMesh(MeshModel& mesh, bool scenery){
    MeshIterator entry = m_registeredMeshes.find(&mesh);
    if(entry != m_registeredMeshes.end())
        return;

    if(!scenery && hasBorders(mesh))
        throw ODEInvalidMeshException();

    if(mesh.cm.Tr != vcg::Matrix44f::Identity()){
        vcg::tri::UpdatePosition<CMeshO>::Matrix(mesh.cm, mesh.cm.Tr);
        mesh.cm.Tr.SetIdentity();
    }

    tri::Inertia<CMeshO> inertia;
    inertia.Compute(mesh.cm);
    Point3f center = mesh.cm.bbox.Center();

    ODEMesh* odeMesh = new ODEMesh();
    odeMesh->vertices = new dReal[mesh.cm.vert.size()][3];
    odeMesh->normals = new dReal[mesh.cm.face.size()][3];
    odeMesh->indices = new dTriIndex[mesh.cm.face.size()][3];
    odeMesh->centerOfMass[0] = scenery ? 0.f : inertia.CenterOfMass()[0];
    odeMesh->centerOfMass[1] = scenery ? 0.f : inertia.CenterOfMass()[1];
    odeMesh->centerOfMass[2] = scenery ? 0.f : inertia.CenterOfMass()[2];

    int i = 0;
    for(CMeshO::ConstVertexIterator vi = mesh.cm.vert.begin(); vi != mesh.cm.vert.end(); vi++, i++){
        for(int j = 0; j < 3; j++){
            odeMesh->vertices[i][j] = vi->P()[j] - odeMesh->centerOfMass[j] ;
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
    m_registeredMeshes.insert(make_pair(&mesh, odeMesh));
    setAsRigidBody(mesh, !scenery);
}

bool ODEFacade::hasBorders(MeshModel& mesh){
    for(CMeshO::FaceIterator i = mesh.cm.face.begin(); i != mesh.cm.face.end(); i++)
        for(int j = 0; j < 3; j++)
            if(vcg::face::IsBorder(*i, j)) return true;

    return false;
}


void ODEFacade::setAsRigidBody(MeshModel& mesh, bool isRigidBody) throw(ODEInvalidMeshException) {
    MeshIterator entry = m_registeredMeshes.find(&mesh);
    if(entry == m_registeredMeshes.end())
        return;

    if(!isRigidBody && entry->second->body != 0){ // This mesh was registered as a rigid body
        dBodyDestroy(entry->second->body);
        entry->second->body = 0;
    }else if(isRigidBody && entry->second->body == 0){ // This mesh was registered as part of the scenery
        entry->second->body = dBodyCreate(m_world);
        dGeomSetBody(entry->second->geom, entry->second->body);

        dMass* mass = &entry->second->mass;
        dMassSetTrimesh(mass, 1.0f, entry->second->geom);
        if(mass->mass < 0) throw ODEInvalidMeshException();
        mass->mass = 5;

        dReal centerOfMass[3] = { mass->c[0], mass->c[1], mass->c[2] };
        dMassTranslate(&entry->second->mass, -centerOfMass[0], -centerOfMass[1], -centerOfMass[2]);
        dBodySetMass(entry->second->body, &entry->second->mass);
        dBodySetPosition(entry->second->body, entry->second->centerOfMass[0], entry->second->centerOfMass[1], entry->second->centerOfMass[2]);
    }
}

void ODEFacade::updateTransform(){
    for(MeshContainer::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        if(i->second->body == 0)
            return;

        const dReal* position = dBodyGetPosition(i->second->body);
        const dReal* rotation = dBodyGetRotation(i->second->body);

        i->first->cm.Tr[0][0] = rotation[0];
        i->first->cm.Tr[0][1] = rotation[1];
        i->first->cm.Tr[0][2] = rotation[2];
        i->first->cm.Tr[0][3] = position[0];
        i->first->cm.Tr[1][0] = rotation[4];
        i->first->cm.Tr[1][1] = rotation[5];
        i->first->cm.Tr[1][2] = rotation[6];
        i->first->cm.Tr[1][3] = position[1];
        i->first->cm.Tr[2][0] = rotation[8];
        i->first->cm.Tr[2][1] = rotation[9];
        i->first->cm.Tr[2][2] = rotation[10];
        i->first->cm.Tr[2][3] = position[2];
        i->first->cm.Tr[3][0] = 0.f;
        i->first->cm.Tr[3][1] = 0.f;
        i->first->cm.Tr[3][2] = 0.f;
        i->first->cm.Tr[3][3] = 1.f;

        vcg::Matrix44f m;
        m.SetTranslate(-i->second->centerOfMass[0], -i->second->centerOfMass[1], -i->second->centerOfMass[2]);
        i->first->cm.Tr *= m;
    }
}

void ODEFacade::integrate(float step){
    dSpaceCollide(m_space, this, collisionCallback);
    dWorldQuickStep(m_world, step);
    dJointGroupEmpty(m_contactGroup);
}

void ODEFacade::collisionCallback(void* data, dGeomID o1, dGeomID o2){
    ODEFacade* engine = static_cast<ODEFacade*>(data);
    engine->collisionCallback(o1, o2);
}

void ODEFacade::collisionCallback(dGeomID o1, dGeomID o2){
    dBodyID body1 = dGeomGetBody(o1);
    dBodyID body2 = dGeomGetBody(o2);

    for(unsigned i = 0; i < m_contacts.size(); i++){
        m_contacts[i].surface.mode = dContactBounce;
        m_contacts[i].surface.mu = m_friction;
        m_contacts[i].surface.bounce = m_bounciness;
        m_contacts[i].surface.bounce_vel = 0.1;
    }

    int collisions = dCollide(o1, o2, m_contacts.size(), &m_contacts[0].geom, sizeof(dContact));
    for(int i = 0; i < collisions; i++){
        dJointID joint = dJointCreateContact(m_world, m_contactGroup, &m_contacts[i]);
        dJointAttach(joint, body1, body2);
    }
}

vcg::Matrix44f ODEFacade::getTransformationMatrix(MeshModel& mesh){
    MeshIterator entry = m_registeredMeshes.find(&mesh);
    if(entry == m_registeredMeshes.end() || entry->second->body == 0)
        return mesh.cm.Tr;

    vcg::Matrix44f matrix;
    const dReal* position = dBodyGetPosition(entry->second->body);
    const dReal* rotation = dBodyGetRotation(entry->second->body);

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

    vcg::Matrix44f m;
    m.SetTranslate(-entry->second->centerOfMass[0], -entry->second->centerOfMass[1], -entry->second->centerOfMass[2]);

    return matrix * m;
}

void ODEFacade::setIterations(int iterations){
    dWorldSetQuickStepNumIterations(m_world, iterations);
}

void ODEFacade::setMaxContacts(int contacts){
    m_contacts.resize(contacts);
}

void ODEFacade::setBounciness(float bounciness){
    m_bounciness = bounciness;
}

void ODEFacade::setFriction(float friction){
    m_friction = friction;
}
