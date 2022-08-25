/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef PHYSICS_ENGINE_FACADE_H
#define PHYSICS_ENGINE_FACADE_H

#include <common/meshmodel.h>

#include <vcg/math/matrix44.h>

class PhysicsEngineFacade{
public:
        virtual void registerTriMesh(MeshModel& mesh, bool scenery = false) = 0;
        virtual void setAsRigidBody(MeshModel& mesh, bool isRigidBody = true) = 0;
        virtual void updateTransform() = 0;

        virtual vcg::Matrix44f getTransformationMatrix(MeshModel& mesh) = 0;

        virtual void setGlobalForce(float force[3]) = 0;
        virtual void setIterations(int iterations) = 0;
        virtual void setMaxContacts(int contacts) = 0;
        virtual void setBounciness(float bounciness) = 0;

        virtual void integrate(float step) = 0;
        virtual void clear(MeshDocument& md) = 0;
	
protected:
	virtual void initialize() = 0;
};

#endif
