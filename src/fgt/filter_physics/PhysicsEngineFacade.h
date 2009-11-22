#ifndef PHYSICS_ENGINE_FACADE_H
#define PHYSICS_ENGINE_FACADE_H

#include <meshlab/meshmodel.h>

class PhysicsEngineFacade{
public:
	virtual void setGlobalForce(int force[3]) = 0;
        virtual void registerTriMesh(const MeshModel& mesh) = 0;
	
protected:
	virtual void initialize() = 0;
	virtual void finalize() = 0;
};

#endif
