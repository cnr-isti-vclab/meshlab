/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef PHYSICS_ENGINE_FACADE_H
#define PHYSICS_ENGINE_FACADE_H

#include <meshlab/meshmodel.h>

class PhysicsEngineFacade{
public:
        virtual void registerTriMesh(MeshModel& mesh, bool scenery = false) = 0;
        virtual void setAsRigidBody(MeshModel& mesh, bool isRigidBody = true) = 0;
        virtual void updateTransform() = 0;

        virtual void setGlobalForce(float force[3]) = 0;
        virtual void integrate(float step) = 0;
        virtual void clear() = 0;
	
protected:
	virtual void initialize() = 0;
};

#endif
