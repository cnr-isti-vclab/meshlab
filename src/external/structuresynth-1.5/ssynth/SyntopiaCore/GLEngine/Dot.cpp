#include "Dot.h"

#include "SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {

		Dot::Dot(SyntopiaCore::Math::Vector3f pos) : pos(pos)
		{
			/// Bounding box
			from = pos;
			to = pos;
		};

		Dot::~Dot() { };

		void Dot::draw() const {
			glDisable (GL_LIGHTING);
			glColor4fv(primaryColor);
			glBegin(GL_POINTS);
			glVertex3f(pos.x(), pos.y(), pos.z());
			glEnd();			
			
			glEnable (GL_LIGHTING);

		};

	}
}

