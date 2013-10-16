#include "Mesh.h"

#include "SyntopiaCore/Math/Vector3.h"

#include "../Logging/Logging.h"

using namespace SyntopiaCore::Logging;


using namespace SyntopiaCore::Math;

namespace SyntopiaCore {
	namespace GLEngine {


		Mesh::Mesh(SyntopiaCore::Math::Vector3f startBase, 
				SyntopiaCore::Math::Vector3f startDir1 , 
				 SyntopiaCore::Math::Vector3f startDir2, 
				 SyntopiaCore::Math::Vector3f endBase, 
				SyntopiaCore::Math::Vector3f endDir1 , 
				 SyntopiaCore::Math::Vector3f endDir2) : 
				startBase(startBase), startDir1(startDir1), startDir2(startDir2),
				endBase(endBase), endDir1(endDir1), endDir2(endDir2)
		{
			/// Bounding Mesh (not really accurate)
			from = startBase;
			to = startBase;

			Expand(from,to, startBase+startDir1);
			Expand(from,to, startBase+startDir2);
			Expand(from,to, startBase+startDir2+startDir1);
			Expand(from,to, endBase);
			Expand(from,to, endBase+endDir1);
			Expand(from,to, endBase+endDir2);
			Expand(from,to, endBase+endDir1+endDir2);
	
		};

		Mesh::~Mesh() { };

		void Mesh::draw() const {

			// --- TODO: Rewrite - way to many state changes...

			glPushMatrix();
			glTranslatef( startBase.x(), startBase.y(), startBase.z() );
			

			GLfloat col[4] = {0.0f, 1.0f, 1.0f, 0.1f} ;
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, col );
			

			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
				
			Vector3f O(0,0,0);
			Vector3f end = endBase - startBase;
			
			glEnable (GL_LIGHTING);

			glDisable(GL_CULL_FACE); // TODO: do we need this?
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			GLfloat c3[4] = {0.3f, 0.3f, 0.3f, 0.5f};
			glMaterialfv( GL_FRONT, GL_SPECULAR, c3 );
			glMateriali(GL_FRONT, GL_SHININESS, 127);
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);
			
					
			glBegin( GL_QUADS );
			glColor4fv(primaryColor);
			GLfloat secondaryColor[4] = {oldRgb[0], oldRgb[1], oldRgb[2], oldAlpha};
			
			//vertex4n(O, startDir1,startDir2+startDir1,startDir2);
			Vector3f c1(startDir1*0.5f+startDir2*0.5f);
			Vector3f c2(end+endDir1*0.5f+endDir2*0.5f);
			vertex4(primaryColor, c1, O, startDir1, secondaryColor,c2,  end+endDir1,end,false);
			vertex4(primaryColor,c1,  O, startDir2, secondaryColor,c2,  end+endDir2,end,false);
			vertex4(primaryColor,c1,  startDir1, startDir1+startDir2,secondaryColor,c2,   end+endDir1+endDir2, end+endDir1,false);
			vertex4(primaryColor,c1,  startDir2, startDir1+startDir2,secondaryColor,c2,  end+endDir1+endDir2, end+endDir2,false);
			//vertex4n(O+end, endDir1+end,endDir2+endDir1+end,endDir2+end);
			glEnd();
			
			glDisable(GL_COLOR_MATERIAL);
			

			glPopMatrix();			
		};

	

		bool Mesh::intersectsRay(RayInfo* ri) {
			if (triangles.count()==0) initTriangles();	

			for (int i = 0; i < triangles.count(); i++) {
				if (triangles[i].intersectsRay(ri)) return true;
			}
			return false;
		};

		void Mesh::initTriangles() {
			triangles.clear();
			
			
			RaytraceTriangle::Vertex4(startBase, startBase+startDir1,endBase+endDir1,endBase, true,triangles,primaryColor[0],primaryColor[1],primaryColor[2],primaryColor[3]);
			RaytraceTriangle::Vertex4(startBase, endBase,endBase+endDir2,startBase+startDir2, true,triangles,primaryColor[0],primaryColor[1],primaryColor[2],primaryColor[3]);
			RaytraceTriangle::Vertex4(startBase+startDir1, startBase+startDir1+startDir2, endBase+endDir1+endDir2, endBase+endDir1, true,triangles,primaryColor[0],primaryColor[1],primaryColor[2],primaryColor[3]);
			RaytraceTriangle::Vertex4(startBase+startDir2, endBase+endDir2, endBase+endDir1+endDir2, startBase+startDir1+startDir2, true,triangles,primaryColor[0],primaryColor[1],primaryColor[2],primaryColor[3]);			
			from = startBase;
			to = startBase;
			for (int i = 0; i < triangles.count(); i++) {
				triangles[i].expandBoundingBox(from,to);
			}
		}

		

		bool Mesh::intersectsAABB(Vector3f from2, Vector3f to2) {
			if (triangles.count()==0) initTriangles();
			return
					    (from.x() < to2.x()) && (to.x() > from2.x()) &&
						(from.y() < to2.y()) && (to.y() > from2.y()) &&
						(from.z() < to2.z()) && (to.z() > from2.z());
		};



	}
}

