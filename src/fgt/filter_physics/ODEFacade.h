/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef ODE_FACADE_H
#define ODE_FACADE_H

#include <vcg/math/matrix44.h>
#include <common/meshmodel.h>

#include <ode/ode.h>

#include <map>
#include <exception>

class ODEInvalidMeshException : public std::exception{
public:
    const char* what() const throw(){
        return "Incorrect mesh: dynamic meshes can't have borders or a negative mass (check normals)!";
    }
};

class ODEFacade{
public:
	ODEFacade();
	
        /**
         * Registers a mesh to the ODE physics engine. Once registered the mesh will be part of the physics
         * simulation and will interact dynamically with other registered meshes.
         * \param mesh The mesh to register
         * \param scenery If true the mesh is part of the scenery and will act as static environment otherwise it will be set as a rigid body.
         */
        virtual void registerTriMesh(MeshModel& mesh, bool scenery = false);
        /**
          * Sets an already registered mesh as a rigid body or as part of the static environment
          * \param mesh A registered mesh
          * \param isRigidBody If true the mesh will be set as rigid body otherwise it will be set as part of the static environment
          * \throws ODEInvalidMeshException if a rigid body mesh has borders or has a negative mass (ODE can't handle it)
          */
        virtual void setAsRigidBody(MeshModel& mesh, bool isRigidBody = true) throw(ODEInvalidMeshException);
        /**
          * This method updates meshlab's transformation matrices of all registered meshes.
          */
        virtual void updateTransform();

        /**
          * Returns the transformation matrix of a registered mesh.
          */
        virtual vcg::Matrix44f getTransformationMatrix(MeshModel& mesh);

        /**
          * Sets a global force that will act on all registered meshes.
          */
        virtual void setGlobalForce(float force[3]);
        /**
          * Sets the number of iterations of the iterative stepping function.
          */
        virtual void setIterations(int iterations);
        /**
          * Sets the maximum number of contacts to be generated in a collision of a pair of objects
          */
        virtual void setMaxContacts(int contacts);
        /**
          * Sets the bounciness of a contact
          */
        virtual void setBounciness(float bounciness);
        /**
          * Sets the coloumb friction coefficient of contact
          */
        virtual void setFriction(float friction);

        /**
          * Performs an integration step of simulation
          */
        virtual void integrate(float step);
        /**
          * Unregisters all meshes
          */
        virtual void clear();
	
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
            dReal centerOfMass[3];
            dTriIndex (*indices)[3];
        };

        typedef std::map<MeshModel*, ODEMesh*> MeshContainer;
        typedef MeshContainer::iterator MeshIterator;

        static void collisionCallback(void* data, dGeomID o1, dGeomID o2);
        void collisionCallback(dGeomID o1, dGeomID o2);
        bool hasBorders(MeshModel& mesh);

        //This class is a monostate
        static bool m_initialized;
        static float m_bounciness;
        static float m_friction;

        static dWorldID m_world;
        static dSpaceID m_space;
        static std::vector<dContact> m_contacts;
        static dJointGroupID m_contactGroup;

        static MeshContainer m_registeredMeshes;
    };

#endif
