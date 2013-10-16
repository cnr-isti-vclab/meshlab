#include "Triangle.h"

#include "SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {


		Triangle::Triangle(SyntopiaCore::Math::Vector3f p1 , 
				 SyntopiaCore::Math::Vector3f p2, 
				 SyntopiaCore::Math::Vector3f p3) : p1(p1), p2(p2), p3(p3) 
		{
			from = p1;
			to = p3;

			Vector3f n1 = Vector3f::cross(p3-p1, p2-p1);
			n1.normalize();
			rt = RaytraceTriangle(p1,p2,p3,n1,n1,n1);
			rt.expandBoundingBox(from,to);
			rt.cullBackFaces = false;
		};

		Triangle::~Triangle() { };

		void Triangle::prepareForRaytracing() {
			rt.setColor(primaryColor[0],primaryColor[1],primaryColor[2], primaryColor[3]);
		}

		void Triangle::draw() const {
			glPushMatrix();
			
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, primaryColor );
			glPolygonMode(GL_FRONT, GL_FILL);
				
			glEnable(GL_CULL_FACE);
			glEnable (GL_LIGHTING);

			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glMateriali(GL_FRONT, GL_SPECULAR, 30);
			glMateriali(GL_FRONT, GL_SHININESS, 127);
			
			glBegin(GL_TRIANGLES);
			vertex3n(p1,p2,p3);
			glEnd();	
			
			glPopMatrix();			
		};


		bool Triangle::intersectsRay(RayInfo* ri) {
			return rt.intersectsRay(ri);
		};


		bool Triangle::intersectsAABB(Vector3f from2, Vector3f to2) {
				return   (from.x() < to2.x()) && (to.x() > from2.x()) &&
						(from.y() < to2.y()) && (to.y() > from2.y()) &&
						(from.z() < to2.z()) && (to.z() > from2.z());
		};



	}
}

