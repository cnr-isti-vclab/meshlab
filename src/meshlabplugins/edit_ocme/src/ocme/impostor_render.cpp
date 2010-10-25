#include "impostor_definition.h"

#include <GL/glew.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gl/space.h>
#include <vcg/space/box3.h>

#include <vcg/complex/trimesh/clustering.h>

#include "stdmatrix3.h"
#include "plane_box_intersection.h"
 

	void Impostor::Render(bool drawsubcells){
		glPushAttrib(GL_CURRENT_BIT);
		if(drawsubcells){
			glColor3f(1.0,1.0,1.0);
			for(unsigned int i = 0; i < Gridsize(); ++i)
				for(unsigned int j = 0; j < Gridsize(); ++j)
					for(unsigned int k = 0; k < Gridsize(); ++k)
					if(n_samples.At(i,j,k) )

					{
						vcg::Box3f subcell;subcell.SetNull();
						subcell.Add(box.min+vcg::Point3f(i*cellsize,j*cellsize,k*cellsize));
						subcell.Add(box.min+vcg::Point3f((i+1)*cellsize,(j+1)*cellsize,(k+1)*cellsize));
						vcg::glBoxWire ( subcell );
					}
			vcg::Color4b c;
			c.Scatter(1<<31,static_cast<int>(box.Diag()));
			glColor(c);
			vcg::glBoxWire ( box );
		}
		glPopAttrib();

		vcg::Point3f p,n;
        vcg::Point3<unsigned char> c;

		glPointSize(8);
		glBegin(GL_POINTS);
		
		for(	PointCellIterator pi   = this->proxies.begin(); pi != this->proxies .end(); ++pi  ){
                this->GetPointNormalColor(*pi,p,n,c);
				glNormal(n);
                glColor3ub(c[0],c[1],c[2]);
				//glColor3f(.3f,.3f,.3f);
				glVertex(p);
		}
		glEnd();

	}


 
