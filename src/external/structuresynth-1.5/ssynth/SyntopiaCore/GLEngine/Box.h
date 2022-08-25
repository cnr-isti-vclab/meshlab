#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "Object3D.h"
#include "RaytraceTriangle.h"


namespace SyntopiaCore {
	namespace GLEngine {	

		class Box : public Object3D {
		public:
			Box(SyntopiaCore::Math::Vector3f base, 
				SyntopiaCore::Math::Vector3f dir1 , 
				 SyntopiaCore::Math::Vector3f dir2, 
				 SyntopiaCore::Math::Vector3f dir3);

			virtual ~Box();

			virtual QString name() { return "Box"; }
		
			virtual void draw() const;

			virtual bool intersectsRay(RayInfo* /*rayInfo*/);
			virtual bool intersectsAABB(SyntopiaCore::Math::Vector3f /*from*/, SyntopiaCore::Math::Vector3f /*to*/);
			virtual void prepareForRaytracing();
			
		private:
			bool useTriangles;
			QVector<RaytraceTriangle> triangles;
			SyntopiaCore::Math::Vector3f base;
			
			SyntopiaCore::Math::Vector3f v1;
			SyntopiaCore::Math::Vector3f v2;
			SyntopiaCore::Math::Vector3f v3;

			SyntopiaCore::Math::Vector3f n21 ;
			SyntopiaCore::Math::Vector3f n32;
			SyntopiaCore::Math::Vector3f n13;
			SyntopiaCore::Math::Vector3f ac; 
			SyntopiaCore::Math::Vector3f a[3];
			float h[3];
  
		};

	}
}

