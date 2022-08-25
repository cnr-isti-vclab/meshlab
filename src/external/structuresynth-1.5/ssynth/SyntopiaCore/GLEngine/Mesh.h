#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "Object3D.h"
#include "RaytraceTriangle.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		class Mesh : public Object3D {
		public:
			Mesh(SyntopiaCore::Math::Vector3f startBase, 
				SyntopiaCore::Math::Vector3f startDir1 , 
				 SyntopiaCore::Math::Vector3f startDir2, 
				 SyntopiaCore::Math::Vector3f endBase, 
				SyntopiaCore::Math::Vector3f endDir1 , 
				 SyntopiaCore::Math::Vector3f endDir2 
				);

			virtual ~Mesh();

			virtual QString name() { return "Mesh"; }
			
			virtual void draw() const;
			void initTriangles();

			virtual bool intersectsRay(RayInfo* /*rayInfo*/);
			virtual bool intersectsAABB(SyntopiaCore::Math::Vector3f /*from*/, SyntopiaCore::Math::Vector3f /*to*/);
			virtual void prepareForRaytracing() {initTriangles(); };
			void setPreviousColor(SyntopiaCore::Math::Vector3f oldRgb, float oldAlpha) { this->oldRgb = oldRgb; this->oldAlpha = oldAlpha; }
			
			

		private:
			SyntopiaCore::Math::Vector3f startBase;
			SyntopiaCore::Math::Vector3f startDir1;
			SyntopiaCore::Math::Vector3f startDir2;
			SyntopiaCore::Math::Vector3f endBase;
			SyntopiaCore::Math::Vector3f endDir1;
			SyntopiaCore::Math::Vector3f endDir2;  
			QVector<RaytraceTriangle> triangles;
			SyntopiaCore::Math::Vector3f oldRgb;
			float oldAlpha;
		};

	}
}

