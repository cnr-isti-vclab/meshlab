#pragma once

#include "SyntopiaCore/Math/Vector3.h"
#include "Object3D.h"

namespace SyntopiaCore {
	namespace GLEngine {	

		using namespace SyntopiaCore::Math;

		// Helper class for tesselated objects
		class RaytraceTriangle {
		public:
			RaytraceTriangle(Vector3f p1, Vector3f p2, Vector3f p3, Vector3f n1, Vector3f n2, Vector3f n3);
			RaytraceTriangle() : cullBackFaces(false) {};
			~RaytraceTriangle(void);
			void expandBoundingBox(Vector3f& from,Vector3f& to);
			void setColor(float r, float g, float b, float a) { 
				color1[0] = r; color1[1] = g; color1[2] = b; color1[3] = a;
				color2[0] = r; color2[1] = g; color2[2] = b; color2[3] = a;
				color3[0] = r; color3[1] = g; color3[2] = b; color3[3] = a;
			}
			virtual bool intersectsRay(RayInfo* /*rayInfo*/);
				
			static void Vertex4(Vector3f p1,Vector3f p2,Vector3f p3,Vector3f p4,bool reverse, QVector<RaytraceTriangle>& list, float r, float b, float g, float a);
		
           	Vector3f p1;
			Vector3f p2;
			Vector3f p3;
			Vector3f n1;
			Vector3f n2;
			Vector3f n3;
			GLfloat color1[4];
			GLfloat color2[4];
			GLfloat color3[4];
			Vector3f npn1;
			Vector3f npn2;
			Vector3f npn3;
			
			Vector3f to;
			Vector3f from;
			Vector3f n;
			bool bad;
			bool cullBackFaces;
		
		};

	}
}

