#ifndef ODE_FACADE_H
#define ODE_FACADE_H

#include "PhysicsEngineFacade.h"

#include <vcg/complex/trimesh/allocate.h>

#include <ode/ode.h>

#include <vector>

class ODEFacade : public PhysicsEngineFacade{
public:
	ODEFacade();
	
        virtual void registerTriMesh(MeshModel& mesh, bool scenery = false);
        virtual void setAsRigidBody(MeshModel& mesh, bool isRigidBody = true);
        virtual void updateTransform(MeshModel& mesh);

        virtual void setGlobalForce(float force[3]);
        virtual void integrate(float step);
        virtual void clear();
	
protected:
	virtual void initialize();

    private:
        struct ODEMesh{
            ODEMesh() : body(0), vertices(0), indices(0){
            }

            ~ODEMesh(){
                delete[] vertices;
                delete[] indices;
            }

            dBodyID body;
            dGeomID geom;
            dMass mass;

            dTriMeshDataID data;
            dReal (*vertices)[3];
            dTriIndex (*indices)[3];
        };

        typedef std::vector<std::pair<CMeshO*, ODEMesh*> > MeshContainer;
        typedef CMeshO::PerMeshAttributeHandle<unsigned int> MeshIndex;

        static void collisionCallback(void* data, dGeomID o1, dGeomID o2);
        void collisionCallback(dGeomID o1, dGeomID o2);

        //This class is a monostate
        static bool m_initialized;
        static const int m_maxContacts;

        static dWorldID m_world;
        static dSpaceID m_space;
        static dJointGroupID m_contactGroup;

        static MeshContainer m_registeredMeshes;
};

#endif
