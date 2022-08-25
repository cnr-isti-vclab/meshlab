#include "impostor_definition.h"

#include <GL/glew.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gl/splatting_apss/splatrenderer.h>
#include <wrap/gl/space.h>
#include <vcg/space/box3.h>
#include <vcg/complex/algorithms/clustering.h>

#include "stdmatrix3.h"
#include "plane_box_intersection.h"
 

	void Impostor::Render(bool renderMode){
		// renderMode: 0-points, 1-box

		vcg::Point3f p,n;
        vcg::Point3<unsigned char> c;

		glEnable(GL_COLOR_MATERIAL);
		glPointSize(1);
		glBegin(GL_POINTS);
		//for(	PointCellIterator pi   = this->proxies.begin(); pi != this->proxies .end(); ++pi  ){
		//		this->GetPointNormalColor(*pi,p,n,c);
		//		glNormal(n);
		//		glColor3ub(c[0],c[1],c[2]);
		//		glVertex(p);
		//}
	
		for(unsigned int i = 0; i< this->positionsV.size();++i){
			glNormal(normalsV[i]);
			glColor3ub(colorsV[i][0],colorsV[i][1],colorsV[i][2]);
			glVertex(positionsV[i]);
		}
		glEnd();
 
	}


 
