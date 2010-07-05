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
		glBegin(GL_POINTS);
		for(	PointCellIterator pi   = this->proxies.begin(); pi != this->proxies .end(); ++pi  ){
				this->GetPointNormal(*pi,p,n);
				glNormal(n);
				glVertex(p);
		}
		glEnd();

//		if(!polygons.empty())
//			for(unsigned int pi = 0; pi < polygons.size(); ++pi){
//
//				glBegin(GL_POLYGON);
//					glNormal(polygons[pi][0]);
//						for(int  i = 1;  i < polygons[pi].size(); ++i) {
//							glVertex(polygons[pi][i]);
//						}
//				glEnd();
//
//			}
//		else
//		{
//			glBegin(GL_POINTS);
//			glVertex(box.Center());
//			glEnd();
//		}
	}
/*
template <class ImpostorMeshType, int GRIDSIZE>

	void ImpostorClustering<ImpostorMeshType,   GRIDSIZE>::Render(){
		if(!uptodate)
		{vcg::tri::UpdateNormals<ImpostorMeshType>::PerVertexPerFace (mesh );uptodate=true;}
		glw.template Draw<vcg::GLW::DMFlat,     vcg::GLW::CMNone,vcg::GLW::TMNone> ();
	}*/

 
