#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "Object3D.h"
#include "RaytraceTriangle.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		class Triangle : public Object3D {
		public:
			Triangle(SyntopiaCore::Math::Vector3f p1 , 
				 SyntopiaCore::Math::Vector3f p2, 
				 SyntopiaCore::Math::Vector3f p3);

			virtual ~Triangle();

			virtual QString name() { return "Triangle"; }
			virtual void draw() const;
			virtual void prepareForRaytracing();
			virtual bool intersectsRay(RayInfo* /*rayInfo*/);
			virtual bool intersectsAABB(SyntopiaCore::Math::Vector3f /*from*/, SyntopiaCore::Math::Vector3f /*to*/);
		
		private:
			RaytraceTriangle rt;
			SyntopiaCore::Math::Vector3f p1;
			SyntopiaCore::Math::Vector3f p2;
			SyntopiaCore::Math::Vector3f p3;
		};

	}
}

