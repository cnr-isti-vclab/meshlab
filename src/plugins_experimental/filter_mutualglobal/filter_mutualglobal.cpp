/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "filter_mutualglobal.h"
#include <QtScript>

#include "alignset.h"


#include "solver.h"
#include "mutual.h"


//#include "shutils.h"

#include <wrap/gl/shot.h>
#include <wrap/gl/camera.h>

#include <vcg/complex/algorithms/point_sampling.h>

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

AlignSet alignset;



FilterMutualInfoPlugin::FilterMutualInfoPlugin() 
{ 
	typeList << FP_IMAGE_GLOBALIGN;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString FilterMutualInfoPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_IMAGE_GLOBALIGN :  return QString("Image Registration: Global refinement using Mutual Information"); 
		default : assert(0); 
	}
  return QString();
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterMutualInfoPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_IMAGE_GLOBALIGN :  return QString("Calculate a global refinement of image registration, in order to obtain a better alignment of fine detail. It will refine only the shots associated to the active rasters, the non-active ones will be used but not refined. This filter is an implementation of Dellepiane et al. 'Global refinement of image-to-geometry registration for color projection', 2013, and it was used in Corsini et al 'Fully Automatic Registration of Image Sets on Approximate Geometry', 2013. Please cite!"); 
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
FilterMutualInfoPlugin::FilterClass FilterMutualInfoPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_IMAGE_GLOBALIGN :  return MeshFilterInterface::Camera; 
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterMutualInfoPlugin::initParameterSet(QAction *action,MeshDocument & md, RichParameterSet & parlst) 
{
	QStringList rendList; 
	switch(ID(action))	 {
		case FP_IMAGE_GLOBALIGN :  
			//parlst.addParam(new RichMesh ("SourceMesh", md.mm(),&md, "Source Mesh",
				//								"The mesh on which the image must be aligned"));
			/*parlst.addParam(new RichRaster ("SourceRaster", md.rm(),&md, "Source Raster",
												"The mesh on which the image must be aligned"));*/
			
			
			rendList.push_back("Combined");
			rendList.push_back("Normal map");
			rendList.push_back("Color per vertex");
			rendList.push_back("Specular");
			rendList.push_back("Silhouette");
			rendList.push_back("Specular combined");

			//rendList.push_back("ABS Curvature");
			parlst.addParam(new RichEnum("RenderingMode", 0, rendList, tr("Rendering mode:"),
                                QString("Rendering modes")));
			
			//parlst.addParam(new RichShotf  ("Shot", vcg::Shotf(),"Smoothing steps", "The number of times that the whole algorithm (normal smoothing + vertex fitting) is iterated."));
			
			parlst.addParam(new RichInt ("Max number of refinement steps",
				5,
				"Maximum number of minimizations step",
				"Maximum number of minimizations step on the global graph"));
			parlst.addParam(new RichFloat ("Threshold for refinement convergence",
				1.2,
				"Threshold for refinement convergence (in pixels)",
				"The threshold (average quadratic variation in the projection on image plane of some samples of the mesh before and after each step of refinement) that stops the refinement"));

			parlst.addParam(new RichBool("Pre-alignment",false,"Pre-alignment step","Pre-alignment step"));
			parlst.addParam(new RichBool("Estimate Focal",true,"Estimate focal length","Estimate focal length"));
			parlst.addParam(new RichBool("Fine",true,"Fine Alignment","Fine alignment"));

 		  /*parlst.addParam(new RichBool ("UpdateNormals",
											true,
											"Recompute normals",
											"Toggle the recomputation of the normals after the random displacement.\n\n"
											"If disabled the face normals will remains unchanged resulting in a visually pleasant effect."));
			parlst.addParam(new RichAbsPerc("Displacement",
												m.cm.bbox.Diag()/100.0,0,m.cm.bbox.Diag(),
												"Max displacement",
												"The vertex are displaced of a vector whose norm is bounded by this value"));*/
											break;


		
											
		default : assert(0); 
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool FilterMutualInfoPlugin::applyFilter(QAction *action, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	QTime filterTime;
	filterTime.start();

	float thresDiff=par.getFloat("Threshold for refinement convergence");

	std::vector<vcg::Point3f> myVec;
	int leap=(int)((float)md.mm()->cm.vn/1000.0f);
	CMeshO::VertexIterator vi;
	for(int i=0;i<=md.mm()->cm.vn;i=i+leap) 
	{
		myVec.push_back(md.mm()->cm.vert[i].P());
	}
	std::vector<vcg::Shotf> oldShots;
	for (int r=0; r<md.rasterList.size();r++)
	{
		oldShots.push_back(md.rasterList[r]->shot);
	}

	Log(0,"Sampled has %i vertices",myVec.size());
	
	std::vector<SubGraph> Graphs;
	/// Preliminary singular alignment using classic MI
	switch(ID(action))	 {
		case FP_IMAGE_GLOBALIGN :  
			/// Building of the graph of images
			if (md.rasterList.size()==0)
			{
				 Log(0, "You need a Raster Model to apply this filter!");
				 return false;
				 
			}

			this->glContext->makeCurrent();

			this->initGL();

			if (par.getBool("Pre-alignment"))
			{
				preAlignment(md, par, cb);
			}

			if (par.getInt("Max number of refinement steps")!=0)
			{
				
				Graphs=buildGraph(md);
				Log(0, "BuildGraph completed");
				for (int i=0; i<par.getInt("Max number of refinement steps"); i++)
				{
					AlignGlobal(md, Graphs);
					float diff=calcShotsDifference(md,oldShots,myVec);
					Log(0, "AlignGlobal %d of %d completed, average improvement %f pixels",i+1,par.getInt("Max number of refinement steps"),diff);
					if (diff<thresDiff)
						break;
					oldShots.clear();
					for (int r=0; r<md.rasterList.size();r++)
					{
						oldShots.push_back(md.rasterList[r]->shot);
					}
				}
			}
			
			this->glContext->doneCurrent();
			Log(0, "Done!");
			break;
		
		default : assert(0); 
	}

	Log(0,"Filter completed in %i sec",(int)((float)filterTime.elapsed()/1000.0f));

		return true;



}

float FilterMutualInfoPlugin::calcShotsDifference(MeshDocument &md, std::vector<vcg::Shotf> oldShots, std::vector<vcg::Point3f> points)
{
	std::vector<float> distances;
	for (int i=0; i<points.size(); i++)
	{
		for(int j=0; j<md.rasterList.size(); j++)
		{
			vcg::Point2f pp=md.rasterList[j]->shot.Project(points[i]);
			if(pp[0]>0 && pp[1]>0 && pp[0]<md.rasterList[j]->shot.Intrinsics.ViewportPx[0] && pp[1]<md.rasterList[j]->shot.Intrinsics.ViewportPx[1])
			{
				vcg::Point2f ppOld=oldShots[j].Project(points[i]);
				vcg::Point2f d=pp-ppOld;
				distances.push_back(vcg::math::Sqrt( d[0]*d[0] + d[1]*d[1] ));
			}
		}
	}
	float totErr=0.0;
	for (int i=0; i<distances.size(); i++)
	{
		totErr+=(distances[i]*distances[i]);
	}

	return totErr/(float)distances.size();

}

void FilterMutualInfoPlugin::initGL()
{
  GLenum err = glewInit();
  Log(0, "GL Initialization");
  if (GLEW_OK != err) {
    Log(0, "GLEW initialization error!");
    exit(-1);
  }

  if (!glewIsSupported("GL_EXT_framebuffer_object")) {
    Log(0, "Graphics hardware does not support FBOs");
    exit(0);
  }
  if (!glewIsSupported("GL_ARB_vertex_shader") || !glewIsSupported("GL_ARB_fragment_shader") ||
      !glewIsSupported("GL_ARB_shader_objects") || !glewIsSupported("GL_ARB_shading_language")) {
    //QMessageBox::warning(this, "Danger, Will Robinson!",
//                         "Graphics hardware does not fully support Shaders");
  }

  if (!glewIsSupported("GL_ARB_texture_non_power_of_two")) {
    Log(0,"Graphics hardware does not support non-power-of-two textures");
    exit(0);
  }
  if (!glewIsSupported("GL_ARB_vertex_buffer_object")) {
    Log(0, "Graphics hardware does not support vertex buffer objects");
    exit(0);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GLfloat light_position[] = {-600.0f, 500.0f, 700.0f, 0.0f};
  GLfloat light_ambient[]  = {0.1f,  0.1f, 0.1f, 1.0f};
  GLfloat light_diffuse[]  = {0.8f,  0.8f, 0.8f, 1.0f};
  GLfloat light_specular[] = {0.9f,  0.9f, 0.9f, 1.0f};

  glEnable(GL_LIGHTING);
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);
  glLightfv (GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  glEnable  (GL_LIGHT0);
  glDisable(GL_LIGHTING);

  glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

  glEnable(GL_NORMALIZE);
  glDepthRange (0.0, 1.0);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POLYGON_SMOOTH);
  glShadeModel(GL_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);

  //AlignSet &alignset = Autoreg::instance().alignset;
  alignset.initializeGL();
  //alignset.resize(800);
  assert(glGetError() == 0);

  Log(0, "GL Initialization done");

}

QString FilterMutualInfoPlugin::filterScriptFunctionName( FilterIDType filterID )
{
	switch(filterID) {
		case FP_IMAGE_GLOBALIGN :  return QString("imagealignment"); 
		default : assert(0); 
	}
	return QString();
}

bool FilterMutualInfoPlugin::preAlignment(MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	Solver solver;
	MutualInfo mutual;
	if (md.rasterList.size()==0)
	{
		 Log(0, "You need a Raster Model to apply this filter!");
		 return false;
	}
	else {

	alignset.mesh=&md.mm()->cm;
	
	solver.optimize_focal=par.getBool("Estimate Focal");
	solver.fine_alignment=par.getBool("Fine");

	int rendmode= par.getEnum("RenderingMode");
			
			switch(rendmode){ 
				case 0: 
					alignset.mode=AlignSet::COMBINE;
					break;
				case 1: 
					alignset.mode=AlignSet::NORMALMAP;
					break;
				case 2: 
					alignset.mode=AlignSet::COLOR;
					break;
				case 3: 
					alignset.mode=AlignSet::SPECULAR;
					break;
				case 4: 
					alignset.mode=AlignSet::SILHOUETTE;
					break;
				case 5: 
					alignset.mode=AlignSet::SPECAMB;
					break;
				default:
					alignset.mode=AlignSet::COMBINE;
					break;
			}

	vcg::Point3f *vertices = new vcg::Point3f[alignset.mesh->vn];
  vcg::Point3f *normals = new vcg::Point3f[alignset.mesh->vn];
  vcg::Color4b *colors = new vcg::Color4b[alignset.mesh->vn];
  unsigned int *indices = new unsigned int[alignset.mesh->fn*3];

  for(int i = 0; i < alignset.mesh->vn; i++) {
    vertices[i] = alignset.mesh->vert[i].P();
    normals[i] = alignset.mesh->vert[i].N();
    colors[i] = alignset.mesh->vert[i].C();
  }

  for(int i = 0; i < alignset.mesh->fn; i++) 
    for(int k = 0; k < 3; k++) 
      indices[k+i*3] = alignset.mesh->face[i].V(k) - &*alignset.mesh->vert.begin();

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.vbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
                  vertices, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.nbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
                  normals, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.cbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Color4b), 
                  colors, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.ibo);
  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.mesh->fn*3*sizeof(unsigned int), 
                  indices, GL_STATIC_DRAW_ARB);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);


  // it is safe to delete after copying data to VBO
  delete []vertices;
  delete []normals;
  delete []colors;
  delete []indices;

	for (int r=0; r<md.rasterList.size();r++)
	{
		if(md.rasterList[r]->visible)
		{
				alignset.image=&md.rasterList[r]->currentPlane->image;
				alignset.shot=md.rasterList[r]->shot;
				
				alignset.resize(800);

				alignset.shot.Intrinsics.ViewportPx[0]=int((double)alignset.shot.Intrinsics.ViewportPx[1]*alignset.image->width()/alignset.image->height());
				alignset.shot.Intrinsics.CenterPx[0]=(int)(alignset.shot.Intrinsics.ViewportPx[0]/2);

				if (solver.fine_alignment)
				solver.optimize(&alignset, &mutual, alignset.shot);
				else
				{
				solver.iterative(&alignset, &mutual, alignset.shot);
				Log(0, "Vado di rough",r);
				}
				
				md.rasterList[r]->shot=alignset.shot;
				float ratio=(float)md.rasterList[r]->currentPlane->image.height()/(float)alignset.shot.Intrinsics.ViewportPx[1];
				md.rasterList[r]->shot.Intrinsics.ViewportPx[0]=md.rasterList[r]->currentPlane->image.width();
				md.rasterList[r]->shot.Intrinsics.ViewportPx[1]=md.rasterList[r]->currentPlane->image.height();
				md.rasterList[r]->shot.Intrinsics.PixelSizeMm[1]/=ratio;
				md.rasterList[r]->shot.Intrinsics.PixelSizeMm[0]/=ratio;
				md.rasterList[r]->shot.Intrinsics.CenterPx[0]=(int)((float)md.rasterList[r]->shot.Intrinsics.ViewportPx[0]/2.0);
				md.rasterList[r]->shot.Intrinsics.CenterPx[1]=(int)((float)md.rasterList[r]->shot.Intrinsics.ViewportPx[1]/2.0);

				Log(0, "Image %d completed",r);
				
		}
		else
			Log(0, "Image %d skipped",r);
		}
	}
	
	return true;
}

std::vector<SubGraph> FilterMutualInfoPlugin::buildGraph(MeshDocument &md, bool globalign)
{
		
	std::vector<AlignPair> allArcs;
		
	allArcs=CalcPairs(md, globalign);
	Log(0, "Calcpairs completed");
	return CreateGraphs(md, allArcs);
		 
}

std::vector<AlignPair> FilterMutualInfoPlugin::CalcPairs(MeshDocument &md, bool globalign)
{
	Solver solver;
	MutualInfo mutual;
	
	std::vector<AlignPair> list;

	alignset.mesh=&md.mm()->cm;
	
	/*solver.optimize_focal=true;
	solver.fine_alignment=true;*/

	{
		vcg::Point3f *vertices = new vcg::Point3f[alignset.mesh->vn];
		vcg::Point3f *normals = new vcg::Point3f[alignset.mesh->vn];
		vcg::Color4b *colors = new vcg::Color4b[alignset.mesh->vn];
		unsigned int *indices = new unsigned int[alignset.mesh->fn*3];

		for(int i = 0; i < alignset.mesh->vn; i++) {
			vertices[i] = alignset.mesh->vert[i].P();
			normals[i] = alignset.mesh->vert[i].N();
			colors[i] = alignset.mesh->vert[i].C();
		}

		for(int i = 0; i < alignset.mesh->fn; i++) 
		for(int k = 0; k < 3; k++) 
		indices[k+i*3] = alignset.mesh->face[i].V(k) - &*alignset.mesh->vert.begin();

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.vbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
				  vertices, GL_STATIC_DRAW_ARB);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.nbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
				  normals, GL_STATIC_DRAW_ARB);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.cbo);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Color4b), 
				  colors, GL_STATIC_DRAW_ARB);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.ibo);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.mesh->fn*3*sizeof(unsigned int), 
				  indices, GL_STATIC_DRAW_ARB);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

		// it is safe to delete after copying data to VBO
		delete []vertices;
		delete []normals;
		delete []colors;
		delete []indices;
	}

	//alignset.mode=AlignSet::PROJIMG;
		
	//this->glContext->makeCurrent();
	for (int r=0; r<md.rasterList.size(); r++)
	{
		if(md.rasterList[r]->visible)
		{
			AlignPair pair;
			alignset.image=&md.rasterList[r]->currentPlane->image;
			alignset.shot=md.rasterList[r]->shot;

			//this->initGL();
			alignset.resize(800);

			//alignset.shot=par.getShotf("Shot");

			alignset.shot.Intrinsics.ViewportPx[0]=int((double)alignset.shot.Intrinsics.ViewportPx[1]*alignset.image->width()/alignset.image->height());
			alignset.shot.Intrinsics.CenterPx[0]=(int)(alignset.shot.Intrinsics.ViewportPx[0]/2);

			alignset.mode=AlignSet::COMBINE;
			alignset.renderScene(alignset.shot, 3, true);
			alignset.comb=alignset.rend;
			QImage covered=alignset.comb;
			std::vector<AlignPair> weightList;

			for (int p=0; p<md.rasterList.size(); p++)
			{
				if (p!=r)
				{
					alignset.mode=AlignSet::PROJIMG;
					alignset.shotPro=md.rasterList[p]->shot;
					alignset.imagePro=&md.rasterList[p]->currentPlane->image;
					alignset.ProjectedImageChanged(*alignset.imagePro);
					float countTot=0.0;
					float countCol=0.0;
					float countCov=0.0;
					alignset.RenderShadowMap();
					alignset.renderScene(alignset.shot, 2, true);
					//alignset.readRender(1);
					for (int x=0; x<alignset.wt; x++)
						for (int y=0; y<alignset.ht; y++)
						{
							QColor color;
							color.setRgb(alignset.comb.pixel(x,y));
							if (color!=qRgb(0,0,0))
							{
								countTot++;
								if (alignset.comb.pixel(x,y)!=alignset.rend.pixel(x,y))
								{
									countCol++;
								}
							}
						}
						pair.area=countCol/countTot;
											
						if (pair.area>0.2)
						{
							pair.mutual=mutual.info(alignset.wt,alignset.ht,alignset.target,alignset.render);
							pair.imageId=r;
							pair.projId=p;
							pair.weight=pair.area*pair.mutual;
							weightList.push_back(pair);

						}
				}
			}

			Log(0, "Image %d completed",r);
			if (!globalign)
			{
				for (int i=0; i<weightList.size(); i++)
				{
					Log(0, "Area %3.2f, Mutual %3.2f",weightList[i].area,weightList[i].mutual);
					list.push_back(weightList[i]);

				}

				//Log(0, "Tot arcs %d, Valid arcs %d",(md.rasterList.size())*(md.rasterList.size()-1),list.size());
				//return list;

			}
			else
			{


				std::sort(weightList.begin(), weightList.end(), orderingW());

				///////////////////////////////////////7
				for (int i=0; i<weightList.size(); i++)
				{
					int p=weightList[i].projId;
					alignset.mode=AlignSet::PROJIMG;
					alignset.shotPro=md.rasterList[p]->shot;
					alignset.imagePro=&md.rasterList[p]->currentPlane->image;
					alignset.ProjectedImageChanged(*alignset.imagePro);
					float countTot=0.0;
					float countCol=0.0;
					float countCov=0.0;
					alignset.RenderShadowMap();
					alignset.renderScene(alignset.shot, 2, true);
					//alignset.readRender(1);
					for (int x=0; x<alignset.wt; x++)
						for (int y=0; y<alignset.ht; y++)
						{
							QColor color;
							color.setRgb(alignset.comb.pixel(x,y));
							if (color!=qRgb(0,0,0))
							{
								countTot++;
								if (alignset.comb.pixel(x,y)!=alignset.rend.pixel(x,y))
								{
									if (covered.pixel(x,y)!=qRgb(255,0,0))
									{
										countCov++;
										covered.setPixel(x,y,qRgb(255,0,0));
									}
									countCol++;
								}
							}
						}
						pair.area=countCol/countTot;
						/*covered.save("covered.jpg");
						alignset.rend.save("rend.jpg");
						alignset.comb.save("comb.jpg");*/
						
						pair.area*=countCov/countTot;
						pair.mutual=mutual.info(alignset.wt,alignset.ht,alignset.target,alignset.render);
						pair.imageId=r;
						pair.projId=p;
						pair.weight=weightList[i].weight;
						list.push_back(pair);
						Log(0, "Area %3.2f, Mutual %3.2f",pair.area,pair.mutual);
					}
			}
			
		}
		}
//////////////////////////////////////////////////////

	

	Log(0, "Tot arcs %d, Valid arcs %d",(md.rasterList.size())*(md.rasterList.size()-1),list.size());
		
	
		//emit md.rasterSetChanged();
	//this->glContext->doneCurrent();
	return list;

}

std::vector<SubGraph> FilterMutualInfoPlugin::CreateGraphs(MeshDocument &md, std::vector<AlignPair> arcs)
{
	std::vector<SubGraph> Gr;
	SubGraph allNodes;
	int numNodes=md.rasterList.size();
	Log(0, "Tuttok1");
	for (int s=0; s<numNodes; s++)
	{
		Node n;
		n.active=false;
		n.assigned=false;
		n.grNum=0;
		n.avMut=0.0;
		allNodes.nodes.push_back(n);
	}
	int totGr=1;
	bool done=false;
	std::vector<int> nod;
	for (int i=0; i<arcs.size(); i++)
		{
			int cand=arcs[i].imageId;
			bool insert=true;
			for (int j=0; j<nod.size(); j++)
			{
				if(nod[j]==cand)
				{
					insert=false;
					break;
				}
			}
			if(insert)
				nod.push_back(cand);
			int cand2=arcs[i].projId;
			insert=true;
			for (int j=0; j<nod.size(); j++)
			{
				if(nod[j]==cand2)
				{
					insert=false;
					break;
				}
			}
			if(insert)
				nod.push_back(cand2);

		}
	int involvedNodes=nod.size();
	int totNodes=0;
	Log(0, "Tuttok2");
	while (!done)
	{
		for (int i=0; i<arcs.size(); i++)
		{
			int candidate=arcs[i].imageId;
			if(allNodes.nodes[candidate].assigned==false)
			{
				allNodes.nodes[candidate].assigned=true;
				allNodes.nodes[candidate].grNum=totGr;
				totNodes++;
				for (int j=0; j<arcs.size(); j++)
				{
					if (arcs[j].imageId==candidate)
					{
						if (allNodes.nodes[arcs[j].projId].assigned==false)
						{
							allNodes.nodes[arcs[j].projId].assigned=true;
							allNodes.nodes[arcs[j].projId].grNum=totGr;
							totNodes++;
						}
					}
				}
			}
		}
		if (totNodes==involvedNodes)
			done=true;
		else totGr++;
	}
	Log(0, "Tuttok3");
	
	for (int i=1; i<totGr+1; i++)
	{
		SubGraph graph;
		graph.id=i;
		for (int j=0; j<numNodes; j++)
		{
			Log(0, "Node %d of %d",j,numNodes);
			if (allNodes.nodes[j].grNum==i)
			{
				Node n;
				double mut=0.0; double are=0.00001;
				if(md.rasterList[j]->visible)
					n.active=false;
				else 
					n.active=true;
				n.id=j;
				n.avMut=0.0;
				for (int k=0; k<arcs.size(); k++)
				{
					if(arcs[k].imageId==j)
					{
						n.avMut+=arcs[k].weight;
						n.arcs.push_back(arcs[k]);

					}
				}
				std::sort(n.arcs.begin(), n.arcs.end(), ordering());
				graph.nodes.push_back(n);
				Log(0, "Node %d of %d: avMut %3.2f, arch %d",j,numNodes, n.avMut, n.arcs.size());
			}
			else
				{
				Node n;
				n.active=true;
				n.id=j;
				n.avMut=0.0;
				graph.nodes.push_back(n);
				Log(0, "Node %d of %d: not used",j,numNodes);
			}
		}
		Gr.push_back(graph);
	}
	Log(0, "Tuttok5");
	Log(0, "Tot nodes %d, SubGraphs %d",numNodes,totGr);
	
	return Gr;
}

bool FilterMutualInfoPlugin::AlignGlobal(MeshDocument &md, std::vector<SubGraph> graphs)
{
	for (int j=0; j<1; j++)
	{
	for (int i=0; i<graphs.size(); i++)
	{
		int n=0;
		while (!allActive(graphs[i]))
		{
			//Log(0, "Round %d of %d: get the right node",n+1,graphs[i].nodes.size());
			int curr= getTheRightNode(graphs[i]);
			graphs[i].nodes[curr].active=true;
			//Log(0, "Round %d of %d: alignset the node",n+1,graphs[i].nodes.size());
			AlignNode(md, graphs[i].nodes[curr]);
			//Log(0, "Round %d of %d: update the graph",n+1,graphs[i].nodes.size());
			UpdateGraph(md, graphs[i], curr);
			//Log(0, "Image %d completed",curr);
			n++;

		}
		for (int l=0; l<graphs[i].nodes.size(); l++)
		{
			graphs[i].nodes[l].active=false;
		}

	}
	}

	return true;
}

int FilterMutualInfoPlugin::getTheRightNode(SubGraph graph)
{
	int bestLinks=0; int bestActive=-1;
	int cand;
	for (int k=0; k<graph.nodes.size(); k++)
	{
		int act=0;
		if (graph.nodes[k].arcs.size()>=bestLinks && !graph.nodes[k].active)
		{
			for (int l=0; l<graph.nodes[k].arcs.size(); l++)
			{
				if (graph.nodes[graph.nodes[k].arcs[l].projId].active)
					act++;
			}
			if (act>bestActive)
			{
				bestActive=act;			
				bestLinks=graph.nodes[k].arcs.size();
				cand=k;
			}
			else if (act==bestActive && graph.nodes[k].avMut>graph.nodes[cand].avMut)
			{
				bestActive=act;			
				bestLinks=graph.nodes[k].arcs.size();
				cand=k;
			}
		}
	}
	return cand;

}

bool FilterMutualInfoPlugin::allActive(SubGraph graph)
{
	for (int k=0; k<graph.nodes.size(); k++)
	{
		if (!graph.nodes[k].active)
			return false;

	}
	return true;

}

bool FilterMutualInfoPlugin::AlignNode(MeshDocument &md, Node node)
{
	Solver solver;
	MutualInfo mutual;

	alignset.mode=AlignSet::NODE;
	alignset.node=&node;

	alignset.image=&md.rasterList[node.id]->currentPlane->image;
	alignset.shot=md.rasterList[node.id]->shot;

	alignset.mesh=&md.mm()->cm;

	for (int l=0; l<node.arcs.size(); l++)
	{
		alignset.arcImages.push_back(&md.rasterList[node.arcs[l].projId]->currentPlane->image);
		alignset.arcShots.push_back(&md.rasterList[node.arcs[l].projId]->shot);
		alignset.arcMI.push_back(node.arcs[l].mutual);

	}

	if(alignset.arcImages.size()==0)
		return true;
	else if(alignset.arcImages.size()==1)
	{
		alignset.arcImages.push_back(&md.rasterList[node.arcs[0].projId]->currentPlane->image);
		alignset.arcShots.push_back(&md.rasterList[node.arcs[0].projId]->shot);
		alignset.arcMI.push_back(node.arcs[0].mutual);
		alignset.arcImages.push_back(&md.rasterList[node.arcs[0].projId]->currentPlane->image);
		alignset.arcShots.push_back(&md.rasterList[node.arcs[0].projId]->shot);
		alignset.arcMI.push_back(node.arcs[0].mutual);
	}
	else if(alignset.arcImages.size()==2)
	{
		alignset.arcImages.push_back(&md.rasterList[node.arcs[0].projId]->currentPlane->image);
		alignset.arcShots.push_back(&md.rasterList[node.arcs[0].projId]->shot);
		alignset.arcMI.push_back(node.arcs[0].mutual);
	}

	alignset.ProjectedMultiImageChanged();
	
	/*solver.optimize_focal=true;
	solver.fine_alignment=true;*/

	//this->glContext->makeCurrent();
	/*this->initGL();*/
	alignset.resize(800);

	vcg::Point3f *vertices = new vcg::Point3f[alignset.mesh->vn];
	vcg::Point3f *normals = new vcg::Point3f[alignset.mesh->vn];
	vcg::Color4b *colors = new vcg::Color4b[alignset.mesh->vn];
	unsigned int *indices = new unsigned int[alignset.mesh->fn*3];

	for(int i = 0; i < alignset.mesh->vn; i++) {
	vertices[i] = alignset.mesh->vert[i].P();
	normals[i] = alignset.mesh->vert[i].N();
	colors[i] = alignset.mesh->vert[i].C();
	}

	for(int i = 0; i < alignset.mesh->fn; i++) 
	for(int k = 0; k < 3; k++) 
	indices[k+i*3] = alignset.mesh->face[i].V(k) - &*alignset.mesh->vert.begin();

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
			  vertices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.nbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
			  normals, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.cbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Color4b), 
			  colors, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.ibo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.mesh->fn*3*sizeof(unsigned int), 
			  indices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);


	// it is safe to delete after copying data to VBO
	delete []vertices;
	delete []normals;
	delete []colors;
	delete []indices;
	
    //alignset.shot=par.getShotf("Shot");
		
	alignset.shot.Intrinsics.ViewportPx[0]=int((double)alignset.shot.Intrinsics.ViewportPx[1]*alignset.image->width()/alignset.image->height());
	alignset.shot.Intrinsics.CenterPx[0]=(int)(alignset.shot.Intrinsics.ViewportPx[0]/2);
	
	int iter;
	if (solver.fine_alignment)
		iter=solver.optimize(&alignset, &mutual, alignset.shot);
	else
		solver.iterative(&alignset, &mutual, alignset.shot);
	//alignset.renderScene(alignset.shot, 3);
	//alignset.readRender(0);
		
	
	//md.rasterList[node.id]->shot=alignset.shot;
	md.rasterList[node.id]->shot=alignset.shot;
	float ratio=(float)md.rasterList[node.id]->currentPlane->image.height()/(float)alignset.shot.Intrinsics.ViewportPx[1];
	md.rasterList[node.id]->shot.Intrinsics.ViewportPx[0]=md.rasterList[node.id]->currentPlane->image.width();
	md.rasterList[node.id]->shot.Intrinsics.ViewportPx[1]=md.rasterList[node.id]->currentPlane->image.height();
	md.rasterList[node.id]->shot.Intrinsics.PixelSizeMm[1]/=ratio;
	md.rasterList[node.id]->shot.Intrinsics.PixelSizeMm[0]/=ratio;
	md.rasterList[node.id]->shot.Intrinsics.CenterPx[0]=(int)((float)md.rasterList[node.id]->shot.Intrinsics.ViewportPx[0]/2.0);
	md.rasterList[node.id]->shot.Intrinsics.CenterPx[1]=(int)((float)md.rasterList[node.id]->shot.Intrinsics.ViewportPx[1]/2.0);
	//this->glContext->doneCurrent();
	//emit md.rasterSetChanged();
	for (int l=0; l<alignset.arcImages.size(); l++)
	{
		alignset.arcImages.pop_back();
		alignset.arcMI.pop_back();
		alignset.arcShots.pop_back();
		alignset.arcImages.clear();
		alignset.arcMI.clear();
		alignset.arcShots.clear();
		alignset.prjMats.clear();
	}

	return true;
}

bool FilterMutualInfoPlugin::UpdateGraph(MeshDocument &md, SubGraph graph, int n)
{
	Solver solver;
	MutualInfo mutual;
	
	alignset.mesh=&md.mm()->cm;

	vcg::Point3f *vertices = new vcg::Point3f[alignset.mesh->vn];
	vcg::Point3f *normals = new vcg::Point3f[alignset.mesh->vn];
	vcg::Color4b *colors = new vcg::Color4b[alignset.mesh->vn];
	unsigned int *indices = new unsigned int[alignset.mesh->fn*3];

	for(int i = 0; i < alignset.mesh->vn; i++) {
	vertices[i] = alignset.mesh->vert[i].P();
	normals[i] = alignset.mesh->vert[i].N();
	colors[i] = alignset.mesh->vert[i].C();
	}

	for(int i = 0; i < alignset.mesh->fn; i++) 
	for(int k = 0; k < 3; k++) 
	indices[k+i*3] = alignset.mesh->face[i].V(k) - &*alignset.mesh->vert.begin();

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
	  vertices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.nbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Point3f), 
	  normals, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, alignset.cbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, alignset.mesh->vn*sizeof(vcg::Color4b), 
	  colors, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.ibo);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, alignset.mesh->fn*3*sizeof(unsigned int), 
	  indices, GL_STATIC_DRAW_ARB);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);


	// it is safe to delete after copying data to VBO
	delete []vertices;
	delete []normals;
	delete []colors;
	delete []indices;
	
	for (int h=0; h<graph.nodes.size(); h++)
		for (int l=0; l<graph.nodes[h].arcs.size(); l++)
		{
			if(graph.nodes[h].arcs[l].imageId==n || graph.nodes[h].arcs[l].projId==n)
			{
//////////////////
				int imageId=graph.nodes[h].arcs[l].imageId;
				int imageProj=graph.nodes[h].arcs[l].projId;

				//this->glContext->makeCurrent();
				
				alignset.image=&md.rasterList[imageId]->currentPlane->image;
				alignset.shot=md.rasterList[imageId]->shot;

				//this->initGL();
				alignset.resize(800);

				

				//alignset.shot=par.getShotf("Shot");

				alignset.shot.Intrinsics.ViewportPx[0]=int((double)alignset.shot.Intrinsics.ViewportPx[1]*alignset.image->width()/alignset.image->height());
				alignset.shot.Intrinsics.CenterPx[0]=(int)(alignset.shot.Intrinsics.ViewportPx[0]/2);

				/*alignset.mode=AlignSet::COMBINE;
				alignset.renderScene(alignset.shot, 3, true);
				alignset.comb=alignset.rend;*/

				alignset.mode=AlignSet::PROJIMG;
				alignset.shotPro=md.rasterList[imageProj]->shot;
				alignset.imagePro=&md.rasterList[imageProj]->currentPlane->image;
				alignset.ProjectedImageChanged(*alignset.imagePro);
				float countTot=0.0;
				float countCol=0.0;
				alignset.RenderShadowMap();
				alignset.renderScene(alignset.shot, 1, true);
				//alignset.readRender(1);
				/*for (int x=0; x<alignset.wt; x++)
					for (int y=0; y<alignset.ht; y++)
					{
						QColor color;
						color.setRgb(alignset.comb.pixel(x,y));
						if (color!=qRgb(0,0,0))
						{
							countTot++;
							if (alignset.comb.pixel(x,y)!=alignset.rend.pixel(x,y))
								countCol++;
						}
					}
				graph.nodes[h].arcs[l].area=countCol/countTot;*/
				graph.nodes[h].arcs[l].mutual=mutual.info(alignset.wt,alignset.ht,alignset.target,alignset.render);
					
					
				//this->glContext->doneCurrent();

//////////////////////////77

			}
		}


	return true;

}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterMutualInfoPlugin)
