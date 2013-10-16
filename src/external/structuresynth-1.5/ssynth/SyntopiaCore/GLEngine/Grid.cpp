#include "Grid.h"

#include "SyntopiaCore/Math/Vector3.h"

using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {

		Grid::Grid(SyntopiaCore::Math::Vector3f base, 
				SyntopiaCore::Math::Vector3f dir1 , 
				 SyntopiaCore::Math::Vector3f dir2, 
				 SyntopiaCore::Math::Vector3f dir3) : base(base), v1(dir1), v2(dir2), v3(dir3) 
		{
			/// Bounding box
			from = base;
			to = base + dir1 + dir2 + dir3;
		};


		Grid::~Grid() { };

		void Grid::draw() const {
			glPushMatrix();
			glTranslatef( base.x(), base.y(), base.z() );
			glLineWidth( 1.0 );

			glDisable (GL_LIGHTING);
			glColor4fv( primaryColor );

			glBegin( GL_LINE_LOOP  );
			Vector3f O(0,0,0);
			vertex(O);
			vertex(v2);
			vertex(v2+v1);
			vertex(v1);
			glEnd();

			glBegin( GL_LINE_LOOP  );
			vertex(v3);
			vertex(v2+v3);
			vertex(v2+v1+v3);
			vertex(v1+v3);
			glEnd();
			
			glBegin( GL_LINES  );
			vertex( v3 );   vertex( O );
			vertex( v2 );   vertex( v2+v3 );
			vertex( v1+v2 );   vertex( v1+v2+v3 );
			vertex( v1 );   vertex( v1+v3 );
			glEnd();

			glEnable (GL_LIGHTING);			

			glPopMatrix();			
		};

	}
}

