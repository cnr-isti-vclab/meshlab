#include "PhysicsEngineFacade.h"

#include <ode/ode.h>

#include <map>

class ODEFacade : public PhysicsEngineFacade{
public:
	ODEFacade();
	
        virtual void setGlobalForce(float force[3]);
        virtual void registerTriMesh(MeshModel& mesh);
        virtual void integrate(float step);
        virtual void clear();
	
protected:
	virtual void initialize();

    private:
        struct ODEMesh{
            ODEMesh(){
                vertices = 0;
                indices = 0;
            }

            ~ODEMesh(){
                delete[] vertices;
                delete[] indices;
            }

            dBodyID body;
            dGeomID geom;
            dTriMeshDataID data;
            dReal (*vertices)[3];
            dTriIndex (*indices)[3];
        };

        typedef std::map<MeshModel*, ODEMesh*> MeshMap;

        //This class is a monostate
        static bool m_initialized;
        static dWorldID m_world;
        static dSpaceID m_space;
        static MeshMap m_registeredMeshes;
};
