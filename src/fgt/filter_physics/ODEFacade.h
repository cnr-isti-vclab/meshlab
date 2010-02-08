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

/**
  * This class provides a facade (a simplified abstraction) for the use of the Open Dynamics Engine within Meshlab.
  * To use it one has to:
  * 1) register one or more meshes with registerTriMesh method
  * 2) change the default parameters of the simulation via the set* methods, if needed
  * 3) perform one ore more simulation steps via the integrate method
  * 4) synchronize the transformation matrices of meshlab via the updateTransform method
  * Please keep in mind that by no circumstances the transformation matrices of the registered meshes will be automatically
  * updated by this class.
  */
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
          * Sets a global force that will act on all registered meshes. The force is expressed in Newton and you can use it
          * to set the gravity of your simulation, for example. If not set there will be no force acting in the simulation.
          */
        virtual void setGlobalForce(float force[3]);
        /**
          * Sets the number of iterations of the iterative stepping function. The default value is set to 10. The higher
          * the value the more accurate will be the simulation.
          */
        virtual void setIterations(int iterations);
        /**
          * Sets the maximum number of contacts to be generated in a collision of a pair of objects. The standard value is set to
          * 20. The higher the value the more contacts will be generated and the less interpenetration will occur.
          */
        virtual void setMaxContacts(int contacts);
        /**
          * Sets the bounciness of a contact between (0..1). 0 means the surfaces are not bouncy at all, 1 is the maximum bouncyness.
          * The default bounciness is set to 0.1.
          */
        virtual void setBounciness(float bounciness);
        /**
          * Sets the coloumb friction coefficient of contact between 0 and infinity. The default value is set to 10.
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
