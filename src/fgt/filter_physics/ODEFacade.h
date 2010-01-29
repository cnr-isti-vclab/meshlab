/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

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
        virtual void updateTransform();

        virtual vcg::Matrix44f getTransformationMatrix(MeshModel& mesh);

        virtual void setGlobalForce(float force[3]);
        virtual void setIterations(int iterations);
        virtual void setMaxContacts(int contacts);
        virtual void setBounciness(float bounciness);

        virtual void integrate(float step);
        virtual void clear(MeshDocument& md);
	
protected:
	virtual void initialize();

    private:
        struct ODEMesh{
            ODEMesh() : body(0), vertices(0), normals(0), indices(0){
            }

            ~ODEMesh(){
                delete[] vertices;
                delete[] normals;
                delete[] indices;
            }

            dBodyID body;
            dGeomID geom;
            dMass mass;

            dTriMeshDataID data;
            dReal (*vertices)[3];
            dReal (*normals)[3];
            dTriIndex (*indices)[3];
        };

        typedef std::vector<std::pair<MeshModel*, ODEMesh*> > MeshContainer;
        typedef CMeshO::PerMeshAttributeHandle<unsigned int> MeshIndex;

        static void collisionCallback(void* data, dGeomID o1, dGeomID o2);
        void collisionCallback(dGeomID o1, dGeomID o2);
        void updateEngineTransform(MeshModel& mesh);

        //This class is a monostate
        static bool m_initialized;
        static int m_maxIterations;
        static float m_bounciness;

        static dWorldID m_world;
        static dSpaceID m_space;
        static std::vector<dContact> m_contacts;
        static dJointGroupID m_contactGroup;

        static MeshContainer m_registeredMeshes;
};

#endif
