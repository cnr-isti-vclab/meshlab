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
/****************************************************************************
  History


****************************************************************************/

#include <Qt>
#include <QtGui>
#include "filter_qhull.h"


#include <math.h>
#include <stdlib.h>
#include <vcg/complex/trimesh/clean.h>

#include "qhull_tools.h"

using namespace std;
using namespace vcg;


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

QhullPlugin::QhullPlugin() 
{ 
	typeList << FP_QHULL_CONVEX_HULL  
			 <<	FP_QHULL_DELAUNAY_TRIANGULATION
			 <<	FP_QHULL_VORONOI_FILTERING
			 << FP_QHULL_ALPHA_COMPLEX_AND_SHAPE
			 << FP_QHULL_VISIBLE_POINTS;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

QhullPlugin::~QhullPlugin()
{
	for (int i = 0; i < actionList.count() ; i++ )
		delete actionList.at(i);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString QhullPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_QHULL_CONVEX_HULL :  return QString("Convex Hull"); 
		case FP_QHULL_DELAUNAY_TRIANGULATION :  return QString("Delaunay Triangulation"); 
		case FP_QHULL_VORONOI_FILTERING :  return QString("Voronoi Filtering"); 
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE : return QString("Alpha Complex/Shape");
		case FP_QHULL_VISIBLE_POINTS: return QString("Select Visible Points");
		default : assert(0); 
	}
  return QString("Error: Unknown Filter"); 
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString QhullPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_QHULL_CONVEX_HULL :  return QString("Calculate the <b>convex hull</b> with Qhull library (http://www.qhull.org/html/qconvex.htm).<br><br> "
										 "The convex hull of a set of points is the boundary of the minimal convex set containing the given non-empty finite set of points."); 
		case FP_QHULL_DELAUNAY_TRIANGULATION :  return QString("Calculate the <b>Delaunay triangulation</b> with Qhull library (http://www.qhull.org/html/qdelaun.htm).<br><br>"
													"The Delaunay triangulation DT(P) of a set of points P in d-dimensional spaces is a triangulation of the convex hull "
													"such that no point in P is inside the circum-sphere of any simplex in DT(P).<br> "); 
		case FP_QHULL_VORONOI_FILTERING :  return QString("Compute a <b>Voronoi filtering</b> (Amenta and Bern 1998) with Qhull library (http://www.qhull.org/). <br><br>" 
											   "The algorithm calculates a triangulation of the input point cloud without requiring vertex normals."
											   "It uses a subset of the Voronoi vertices to remove triangles from the Delaunay triangulation. <br>"
											   "After computing the Voronoi diagram, foreach sample point it chooses the two farthest opposite Voronoi vertices."
											   "Then computes a Delaunay triangulation of the sample points and the selected Voronoi vertices, and keep "
											   "only those triangles in witch all three vertices are sample points."); 
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE: return QString("Calculate the <b>Alpha Shape</b> of the mesh(Edelsbrunner and P.Mucke 1994) with Qhull library (http://www.qhull.org/). <br><br>"
										"From a given finite point set in the space it computes 'the shape' of the set."
										"The Alpha Shape is the boundary of the alpha complex, that is a subcomplex of the Delaunay triangulation of the given point set.<br>" 
										"For a given value of 'alpha', the alpha complex includes all the simplices in the Delaunay "
										"triangulation which have an empty circumsphere with radius equal or smaller than 'alpha'.<br>"
										"The filter inserts the minimum value of alpha (the circumradius of the triangle) in attribute Quality foreach face.");
		case FP_QHULL_VISIBLE_POINTS: return QString("Select the <b>visible points</b> in a point cloud, as viewed from a given viewpoint.<br>"
										  "It uses the Qhull library (http://www.qhull.org/ <br><br>"
										  "The algorithm used (Katz, Tal and Basri 2007) determines visibility without reconstructing a surface or estimating normals."
										  "A point is considered visible if its transformed point lies on the convex hull of a trasformed points cloud from the original mesh points.");
		default : assert(0); 
	}
	return QString("Error: Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
 QhullPlugin::FilterClass QhullPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_QHULL_CONVEX_HULL :
		case FP_QHULL_DELAUNAY_TRIANGULATION :
		case FP_QHULL_VORONOI_FILTERING :
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
			return FilterClass (MeshFilterInterface::Remeshing) ; 
		case FP_QHULL_VISIBLE_POINTS:
			return FilterClass (MeshFilterInterface::Selection + MeshFilterInterface::PointSet);
		default : assert(0); 
	}
  return FilterClass(0);
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void QhullPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_QHULL_CONVEX_HULL :
			{
				parlst.addParam(new RichBool("reorient", false,"Re-orient all faces coherentely","Re-orient all faces coherentely"));
				break;
			}
		case FP_QHULL_DELAUNAY_TRIANGULATION :
			{
				break;
			}
		case FP_QHULL_VORONOI_FILTERING :
			{
				parlst.addParam(new RichDynamicFloat("threshold",10.0f, 0.0f, 2000.0f,"Pole Discard Thr",
				"Threshold used to discard the Voronoi vertices too far from the origin."
				"We discard vertices are further than this factor times the bbox diagonal <br>"
				"Growing values of this value will add more Voronoi vertices for a better tightier surface reconstruction."
				"On the other hand they will increase processing time and could cause numerical problems to the qhull library.<br>"
																						 ));
				break;
			}	
		case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
			{
			    parlst.addParam(new RichAbsPerc("alpha",m.cm.bbox.Diag()/100.0,0,m.cm.bbox.Diag(),tr("Alpha value"),tr("Compute the alpha value as percentage of the diagonal of the bbox")));
				parlst.addParam(new RichEnum("Filtering", 0, 
									QStringList() << "Alpha Complex" << "Alpha Shape" , 
									tr("Get:"), 
									tr("Select the output. The Alpha Shape is the boundary of the Alpha Complex")));
				break;
			}
		case FP_QHULL_VISIBLE_POINTS:
			{
				parlst.addParam(new RichDynamicFloat("radiusThreshold",
												 0.0f, 0.0f, 7.0f,
												 "radius threshold ","Bounds the radius of the sphere used to select visible points."
												 "It is used to adjust the radius of the sphere (calculated as distance between the center and the farthest point from it) "
												 "according to the following equation: <br>"
												 "radius = radius * pow(10,threshold); <br>"
												 "As the radius increases more points are marked as visible."
												 "Use a big threshold for dense point clouds, a small one for sparse clouds."));
				
				parlst.addParam(new RichBool ("usecamera",
												false,
												"Use ViewPoint from Mesh Camera",
												"Uses the ViewPoint from the camera associated to the current mesh\n if there is no camera, an error occurs"));
				parlst.addParam(new RichPoint3f("viewpoint",
												Point3f(0.0f, 0.0f, 0.0f),
												"ViewPoint",
												"if UseCamera is true, this value is ignored"));
				
				parlst.addParam(new RichBool("convex_hullFP",false,"Show Partial Convex Hull of flipped points", "Show Partial Convex Hull of the transformed point cloud"));
				parlst.addParam(new RichBool("triangVP",false,"Show a triangulation of the visible points", "Show a triangulation of the visible points"));
				parlst.addParam(new RichBool("reorient", false,"Re-orient all faces of the CH coherentely","Re-orient all faces of the CH coherentely."
								"If no Convex Hulls are selected , this value is ignored"));
				break;
				break;
			}
   default: break; // do not add any parameter for the other filters
	}
}

// The Real Core Function doing the actual mesh processing.
bool QhullPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos */* cb*/)
{
	switch(ID(filter))
	{
		case FP_QHULL_CONVEX_HULL :
			{
				MeshModel &m=*md.mm();
				MeshModel &pm =*md.addNewMesh("Convex Hull");
	
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
			    
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */
				
				//facet_list contains the convex hull  
				//facet_list contains simplicial (triangulated) facets.
				//The convex hull of a set of points is the smallest convex set containing the points.

				facetT *facet_list = compute_convex_hull(dim,numpoints,m);

				if(facet_list!=NULL){

					int convexNumFaces = qh num_facets;
					int convexNumVert = qh_setsize(qh_facetvertices (facet_list, NULL, false));
					assert( qh num_vertices == convexNumVert);

					tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);
					tri::Allocator<CMeshO>::AddFaces(pm.cm,convexNumFaces);


					/*ivp length is 'numpoints' because each vertex is accessed through its ID whose range is 
					  0<=qh_pointid(vertex->point)<numpoints. qh num_vertices is < numpoints*/
					vector<tri::Allocator<CMeshO>::VertexPointer> ivp(numpoints);
					vertexT *vertex;
					int i=0;
					FORALLvertices{	
						if ((*vertex).point){
							pm.cm.vert[i].P()[0] = (*vertex).point[0];
							pm.cm.vert[i].P()[1] = (*vertex).point[1];
							pm.cm.vert[i].P()[2] = (*vertex).point[2];
							ivp[qh_pointid(vertex->point)] = &pm.cm.vert[i];
							i++;
						}
					}

					facetT *facet;
					i=0;
					FORALLfacet_(facet_list){		
						vertexT *vertex;
						int vertex_n, vertex_i;
						FOREACHvertex_i_((*facet).vertices){
							pm.cm.face[i].V(vertex_i)= ivp[qh_pointid(vertex->point)];	
						}
						i++;
					}

					assert( pm.cm.fn == convexNumFaces);
					//m.cm.Clear();

					//Re-orient normals
					bool reorient= par.getBool("reorient");
					if (reorient){
						pm.updateDataMask(MeshModel::MM_FACEFACETOPO);
						bool oriented, orientable;

						tri::Clean<CMeshO>::IsOrientedMesh(pm.cm, oriented,orientable);
						vcg::tri::UpdateTopology<CMeshO>::FaceFace(pm.cm);
						vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(pm.cm);
						pm.clearDataMask(MeshModel::MM_FACEFACETOPO);
						tri::UpdateSelection<CMeshO>::ClearFace(pm.cm);
					}

					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",m.cm.vn,m.cm.fn);
					
					int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
					qh_freeqhull(!qh_ALL);  
					qh_memfreeshort (&curlong, &totlong);
					if (curlong || totlong)
						fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
									 totlong, curlong);

					return true;

				}
				else
					return false;
							
			}
		case FP_QHULL_DELAUNAY_TRIANGULATION:
			{
				MeshModel &m=*md.mm();
			    MeshModel &pm =*md.addNewMesh("Delaunay Triangulation");
				
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
			    
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */
				
				//facet_list contains the Delaunauy triangulation as a list of tetrahedral facets */ 	
				facetT *facet_list = compute_delaunay(dim,numpoints,m);

				if(facet_list!=NULL){

					int convexNumVert = qh_setsize(qh_facetvertices (facet_list, NULL, false));
					assert( qh num_vertices == convexNumVert);

					tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);

					vector<tri::Allocator<CMeshO>::VertexPointer> ivp(qh num_vertices);
					vertexT *vertex;
					int i=0;
					FORALLvertices{		
						if ((*vertex).point){
							pm.cm.vert[i].P()[0] = (*vertex).point[0];
							pm.cm.vert[i].P()[1] = (*vertex).point[1];
							pm.cm.vert[i].P()[2] = (*vertex).point[2];
							ivp[qh_pointid(vertex->point)] = &pm.cm.vert[i];
							i++;
						}
					}
					
					// In 3-d Delaunay triangulation each facet is a tetrahedron. If triangulated,
					//each ridge (d-1 vertices between two neighboring facets) is a triangle.

					facetT *facet, *neighbor;
					qh visit_id++;
					int ridgeCount=0;

					//Compute each ridge (triangle) once
					FORALLfacet_(facet_list)
					if (!facet->upperdelaunay) {
						facet->visitid= qh visit_id;
						qh_makeridges(facet);
						ridgeT *ridge, **ridgep;
						FOREACHridge_(facet->ridges) {
							neighbor= otherfacet_(ridge, facet);
							if (neighbor->visitid != qh visit_id) {
								tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
								ridgeCount++;
								int vertex_n, vertex_i;
								FOREACHvertex_i_(ridge->vertices)
									(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
							}
						}
					}

					assert(pm.cm.fn == ridgeCount);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);
					//m.cm.Clear();	

					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
					
					int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
					qh_freeqhull(!qh_ALL);  
					qh_memfreeshort (&curlong, &totlong);
					if (curlong || totlong)
						fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
									 totlong, curlong);
					return true;
				}
				else 
					return false;
			}
			case FP_QHULL_VORONOI_FILTERING:
			{
				MeshModel &m=*md.mm();
			    MeshModel &pm =*md.addNewMesh("Voronoi Filtering");
				
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
			    
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */

				float threshold = par.getDynamicFloat("threshold");
 	
				bool result = compute_voronoi(dim,numpoints,m,pm,threshold);
				
				if(result){
					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);

					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);

					return true;
				}

				else return false;
			}
			case FP_QHULL_ALPHA_COMPLEX_AND_SHAPE:
			{
				MeshModel &m=*md.mm();
				
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD)){
					m.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
				}
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD)){
					m.clearDataMask(MeshModel::MM_VERTTEXCOORD);
				}
				
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */

				double alpha = par.getAbsPerc("alpha");

				bool alphashape = false;
        QString name;
				switch(par.getEnum("Filtering"))
				{
					case 0 :	
						alphashape=false;	
            name = QString("Alpha Complex");
						break;
					case 1 :
						alphashape=true;
            name =QString("Alpha Shapes");
						break;
				}

        MeshModel &pm =*md.addNewMesh(qPrintable(name));

				if (!alphashape && !pm.hasDataMask(MeshModel::MM_FACEQUALITY))
				{
					pm.updateDataMask(MeshModel::MM_FACEQUALITY);
				}

				bool result =compute_alpha_shapes(dim,numpoints,m,pm,alpha,alphashape);

				if(result){
					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);

					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);
					Log(GLLogStream::FILTER,"Alpha = %f ",alpha);
					//m.cm.Clear();	

					return true;
				}
				else 
					return false;
			}
			case FP_QHULL_VISIBLE_POINTS:
			{
				MeshModel &m=*md.mm();
				m.updateDataMask(MeshModel::MM_VERTCOLOR);
				m.updateDataMask(MeshModel::MM_VERTFLAGSELECT);

				//Clear old selection
				tri::UpdateSelection<CMeshO>::ClearVertex(m.cm);
				
				int dim= 3;				/* dimension of points */
				int numpoints= m.cm.vn;	/* number of mesh vertices */

				bool usecam = par.getBool("usecamera");
				Point3f viewpoint = par.getPoint3f("viewpoint");
				float threshold = par.getDynamicFloat("radiusThreshold");

				// if usecamera but mesh does not have one
				if( usecam && !m.hasDataMask(MeshModel::MM_CAMERA) ) 
				{
					errorMessage = "Mesh has not a camera that can be used to compute view direction. Please set a view direction."; 
					return false;
				}
				if(usecam)
				{
					viewpoint = m.cm.shot.GetViewPoint();
				}

				MeshModel &pm =*md.addNewMesh("CH Flipped Points");
				
					pm.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
					pm.clearDataMask(MeshModel::MM_VERTTEXCOORD);

				MeshModel &pm2 =*md.addNewMesh("Visible Points Triangulation");
				
					pm2.clearDataMask(MeshModel::MM_WEDGTEXCOORD);
					pm2.clearDataMask(MeshModel::MM_VERTTEXCOORD);

				bool convex_hullFP = par.getBool("convex_hullFP");
				bool triangVP = par.getBool("triangVP");

				int result = visible_points(dim,numpoints,m,pm,pm2,viewpoint,threshold,convex_hullFP,triangVP);

				if(!convex_hullFP)
					md.delMesh(&pm);
				else{
					//Re-orient normals
					bool reorient= par.getBool("reorient");
					if (reorient){
						pm.updateDataMask(MeshModel::MM_FACEFACETOPO);
						bool oriented,orientable;
						
						tri::Clean<CMeshO>::IsOrientedMesh(pm.cm, oriented,orientable);
						vcg::tri::UpdateTopology<CMeshO>::FaceFace(pm.cm);
						vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(pm.cm);
						pm.clearDataMask(MeshModel::MM_FACEFACETOPO);
						//Clear all face because,somewhere, they have been selected
						tri::UpdateSelection<CMeshO>::ClearFace(pm.cm);
					}
					vcg::tri::UpdateBounding<CMeshO>::Box(pm.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm.cm);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm.cm.vn,pm.cm.fn);
				}

				
				if(!triangVP){
					md.delMesh(&pm2);
				}
				else{
					//Re-orient normals
					bool reorient= par.getBool("reorient");
					if (reorient){
						pm2.updateDataMask(MeshModel::MM_FACEFACETOPO);
						bool oriented,orientable;
            if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(pm2.cm)>0) {
                errorMessage = "Mesh has some not 2-manifold faces, Orientability requires manifoldness";
								return false; // can't continue, mesh can't be processed
						}
						
						tri::Clean<CMeshO>::IsOrientedMesh(pm2.cm, oriented,orientable);
						vcg::tri::UpdateTopology<CMeshO>::FaceFace(pm2.cm);
						vcg::tri::UpdateTopology<CMeshO>::TestFaceFace(pm2.cm);
						pm2.clearDataMask(MeshModel::MM_FACEFACETOPO);
						//Clear all face because,somewhere, they have been selected
						tri::UpdateSelection<CMeshO>::ClearFace(pm2.cm);
					}

					vcg::tri::UpdateBounding<CMeshO>::Box(pm2.cm);
					vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(pm2.cm);
					Log(GLLogStream::FILTER,"Successfully created a mesh of %i vert and %i faces",pm2.cm.vn,pm2.cm.fn);
				}

				if(result>=0){
					Log(GLLogStream::FILTER,"Selected %i visible points", result);
					return true;
				}

				else return false;
			}
	}

  assert(0);
  return false;
}
Q_EXPORT_PLUGIN(QhullPlugin)
