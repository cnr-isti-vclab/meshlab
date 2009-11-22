#include "PhysicsEngineFacade.h"

#include <ode/ode.h>

#include <map>

struct ODETriMesh{
	ODETriMesh(){
		vertices = 0;
		indices = 0;
	}
	
	~ODETriMesh(){
		delete[] vertices;
		delete[] indices;
	}
	
	dReal (*vertices)[3];
	dTriIndex (*indices)[3];
	dTriMeshDataID data;
	dGeomID geom;
};

class ODEFacade : public PhysicsEngineFacade{
public:
	ODEFacade();
	~ODEFacade();
	
	virtual void setGlobalForce(int force[3]);
        virtual void registerTriMesh(const MeshModel& mesh);
	
protected:
	virtual void initialize();
	virtual void finalize();
	
private:
	dWorldID m_world;
	dSpaceID m_space;
	
        std::map<const MeshModel*, ODETriMesh> m_triMeshMap;
};
