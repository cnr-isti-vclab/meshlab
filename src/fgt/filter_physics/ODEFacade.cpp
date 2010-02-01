#include "ODEFacade.h"

#include <vcg/complex/trimesh/inertia.h>

#include <algorithm>
#include <cmath>

using namespace std;
using namespace vcg;

bool ODEFacade::m_initialized;
int ODEFacade::m_maxIterations = 10;
float ODEFacade::m_bounciness = 0.1f;

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

void ODEFacade::clear(MeshDocument& md){
    for(MeshContainer::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        if(md.meshList.contains(i->first))
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

        dMass* mass = &m_registeredMeshes[index()].second->mass;
        dMassSetTrimesh(mass, 1.0f, m_registeredMeshes[index()].second->geom);
        mass->mass = 10;

        dReal centerOfMass[3] = { mass->c[0], mass->c[1], mass->c[2] };
        dMassTranslate(&m_registeredMeshes[index()].second->mass, -centerOfMass[0], -centerOfMass[1], -centerOfMass[2]);
        dBodySetMass(m_registeredMeshes[index()].second->body, &m_registeredMeshes[index()].second->mass);
        dBodySetPosition(m_registeredMeshes[index()].second->body, inertia.CenterOfMass()[0], inertia.CenterOfMass()[1], inertia.CenterOfMass()[2]);
    }
}

void ODEFacade::updateTransform(){
    for(MeshContainer::iterator i = m_registeredMeshes.begin(); i != m_registeredMeshes.end(); i++){
        const dReal* position = 0;
        const dReal* rotation = 0;

        if(i->second->body != 0){
            position = dBodyGetPosition(i->second->body);
            rotation = dBodyGetRotation(i->second->body);
        }else{
            position = dGeomGetPosition(i->second->geom);
            rotation = dGeomGetRotation(i->second->geom);
        }

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
    dWorldStepFast1(m_world, step, m_maxIterations);
    dJointGroupEmpty(m_contactGroup);
}

void ODEFacade::collisionCallback(void* data, dGeomID o1, dGeomID o2){
    ODEFacade* engine = static_cast<ODEFacade*>(data);
    engine->collisionCallback(o1, o2);
}

void ODEFacade::collisionCallback(dGeomID o1, dGeomID o2){
    dBodyID body1 = dGeomGetBody(o1);
    dBodyID body2 = dGeomGetBody(o2);

    for(int i = 0; i < m_contacts.size(); i++){
        m_contacts[i].surface.mode = dContactBounce | dContactSoftCFM;
        m_contacts[i].surface.mu = 0.5;
        m_contacts[i].surface.bounce = m_bounciness;
        m_contacts[i].surface.bounce_vel = 0.1;
        m_contacts[i].surface.soft_cfm = 0.01;
    }

    int collisions = dCollide(o1, o2, m_contacts.size(), &m_contacts[0].geom, sizeof(dContact));
    for(int i = 0; i < collisions; i++){
        dJointID joint = dJointCreateContact(m_world, m_contactGroup, &m_contacts[i]);
        dJointAttach(joint, body1, body2);
    }
}

vcg::Matrix44f ODEFacade::getTransformationMatrix(MeshModel& mesh){
    if(!tri::HasPerMeshAttribute(mesh.cm, "physicsID"))
        return mesh.cm.Tr;

    MeshIndex index = tri::Allocator<CMeshO>::GetPerMeshAttribute<unsigned int>(mesh.cm, "physicsID");

    vcg::Matrix44f matrix;
    const dReal* position = 0;
    const dReal* rotation = 0;

    if(m_registeredMeshes[index()].second->body != 0){
        position = dBodyGetPosition(m_registeredMeshes[index()].second->body);
        rotation = dBodyGetRotation(m_registeredMeshes[index()].second->body);
    }else{
        position = dGeomGetPosition(m_registeredMeshes[index()].second->geom);
        rotation = dGeomGetRotation(m_registeredMeshes[index()].second->geom);
    }

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
    m.SetTranslate(-m_registeredMeshes[index()].second->centerOfMass[0], -m_registeredMeshes[index()].second->centerOfMass[1], -m_registeredMeshes[index()].second->centerOfMass[2]);

    return matrix * m;
}

void ODEFacade::setIterations(int iterations){
    m_maxIterations = iterations;
}

void ODEFacade::setMaxContacts(int contacts){
    m_contacts.resize(contacts);
}

void ODEFacade::setBounciness(float bounciness){
    m_bounciness = bounciness;
}
