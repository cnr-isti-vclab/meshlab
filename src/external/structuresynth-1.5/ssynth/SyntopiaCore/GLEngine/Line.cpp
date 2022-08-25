#include "Line.h"

#include "SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {


		//GLUquadric* Sphere::myQuad = 0;    

		Line::Line(SyntopiaCore::Math::Vector3f from, SyntopiaCore::Math::Vector3f to) : from(from), to(to)
		{
			/// Bounding box
			from = from;
			to = to;
		};

		Line::~Line() {
		};

		void vertex(Vector3f v) {
			glVertex3f(v.x(), v.y(), v.z());
		}

		void Line::draw() const {
			
			glLineWidth( 1.0 );
			glDisable (GL_LIGHTING);
			glColor4fv(primaryColor);
			
			glBegin(GL_LINES);
			vertex(from);
			vertex(to);
			glEnd();

			glEnable (GL_LIGHTING);
			
		};

	}
}

