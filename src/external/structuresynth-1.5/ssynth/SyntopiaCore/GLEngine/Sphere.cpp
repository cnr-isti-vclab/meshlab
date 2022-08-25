#include "Sphere.h"

using namespace SyntopiaCore::Math;


namespace SyntopiaCore {
	namespace GLEngine {


		//GLUquadric* Sphere::myQuad = 0; 
		int Sphere::displayListIndex = 0;

		Sphere::Sphere(SyntopiaCore::Math::Vector3f center, float radius) : center(center), radius(radius) {
			myQuad = gluNewQuadric();    
			gluQuadricDrawStyle(myQuad, GLU_FILL);

			/// Bounding box
			Vector3f v = Vector3f(radius,radius,radius);
			from = center-v;
			to = center+v;

			if (displayListIndex == 0) {
				displayListIndex = glGenLists(1);
				glNewList(displayListIndex, GL_COMPILE);
				gluSphere(myQuad, 1, 7,7);	
				glEndList();
			}
			

		};

		Sphere::~Sphere() {
			gluDeleteQuadric(myQuad);
		};

		void Sphere::draw() const {
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, primaryColor );
			if (primaryColor[3] < 1) {
				glEnable( GL_BLEND );
			}

			glPushMatrix();
			glTranslatef( center.x(), center.y(), center.z() );
			if (displayListIndex!=0) {
				glScalef(radius,radius,radius);
				glCallList(displayListIndex);
			} else {
				gluSphere(myQuad, radius, 7, 7);	
			}
			glPopMatrix();			
		};


			

		bool Sphere::intersectsRay(RayInfo* ri) {
			// Following: http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/

			double a = ri->lineDirection.sqrLength();
			double b = 2*(Vector3f::dot(ri->lineDirection, (ri->startPoint - center)));
			double c = (center-ri->startPoint).sqrLength() - radius*radius;

			double d = b*b-4*a*c;

			if (d<0) {
				ri->intersection = -1;
				return false;
			} else {
				// We always choose the negative solution, since it will be closest to the startPoint. (If intersection is positive)
				ri->intersection = (b - sqrt(d))/(-2*a);
				double intersection2 = (b + sqrt(d))/(-2*a);
				if (intersection2<ri->intersection) ri->intersection = intersection2;

				ri->normal = (ri->startPoint + ri->lineDirection*ri->intersection)-center;
				ri->normal.normalize();
				for (int i = 0; i < 4; i++) ri->color[i] = primaryColor[i];
				return true;
			}
		};

		bool Sphere::intersectsAABB(Vector3f from, Vector3f to) {
			// Based on http://www.gamasutra.com/features/19991018/Gomez_4.htm
			float s, d = 0; 

			//find the square of the distance
			//from the sphere to the box
			for( long i=0 ; i<3 ; i++ )
			{
				if( center[i] < from[i] )
				{
					s = center[i] - from[i];
					d += s*s; 
				} else if( center[i] > to[i] ) {
					s = center[i] - to[i];
					d += s*s; 
				}
			}
			return d <= radius*radius;
		};



	}
}

