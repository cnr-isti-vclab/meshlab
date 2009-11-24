#ifndef PHYSICS_ENGINE_FACADE_H
#define PHYSICS_ENGINE_FACADE_H

#include <meshlab/meshmodel.h>

class PhysicsEngineFacade{
public:
        virtual void setGlobalForce(float force[3]) = 0;
        virtual void registerTriMesh(MeshModel& mesh) = 0;
        virtual void integrate(float step) = 0;
        virtual void clear() = 0;
	
protected:
	virtual void initialize() = 0;
};

#endif
