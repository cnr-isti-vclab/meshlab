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

#include "qhull_tools.h"

#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/color.h>

#include <fstream>
#include <iostream>


using namespace std;
using namespace vcg;

#define TestMode 0

//Internal prototype 
coordT *qh_readpointsFromMesh(int *numpoints, int *dimension, MeshModel &m);
double calculate_circumradius(pointT* p0,pointT* p1,pointT* p2, int dim);


/***************************************************************************/
/*                                                                         */
/* qhull conventions:													   */
/* coplanar point - a nearly incident point to a hyperplane				   */
/* point - d coordinates												   */
/* vertex - extreme point of the input set								   */
/* ridge - d-1 vertices between two neighboring facets					   */
/* facet - a facet with vertices, ridges, coplanar points, neighboring     */
/*			facets, and hyperplane										   */
/* simplicial facet - a facet with d vertices, d ridges, and d neighbors   */
/* non-simplicial facet - a facet with more than d vertices                */                                               
/*                                                                         */
/*                                                                         */
/***************************************************************************/

/*	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 

	compute_convex_hull(int dim, int numpoints, MeshModel &m)
		build convex hull from a set of vertices of a mesh with Qhull library (http://www.qhull.org/html/qconvex.htm).

	returns 
		the convex hull as a list of simplicial (triangulated) facets, if there are no errors;
		NULL otherwise.
*/
facetT *compute_convex_hull(int dim, int numpoints, MeshModel &m)
{  
	coordT *points;					/* array of coordinates for each point*/
	boolT ismalloc= True;			/* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull Tcv";		/* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;			/* output from qh_produce_output()			
									   use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;			/* error messages from qhull code */ 
	int exitcode;					/* 0 if no error from qhull */
		
    /* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);

	//By default, Qhull merges coplanar facets. So, it's necessary to triangulate the convex hull.
	//If you call qh_triangulate() method , all facets will be simplicial (e.g., triangles in 2-d)
	//In theory calling qh_triangulate() or using option 'Qt' should give the same result, but,
	//in this case, option Qt does not triangulate the output because coplanar faces are still merged.
	qh_triangulate();
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */
		return qh facet_list;
	}

	return NULL;
};

/*	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 

	compute_delaunay(int dim, int numpoints, MeshModel &m)
		build Delauanay triangulation from a set of vertices of a mesh with Qhull library (http://www.qhull.org/html/qdelaun.htm).
			
		The Delaunay triangulation DT(P) of a set of points P in d-dimensional spaces is a triangulation of the convex hull 
		such that no point in P is inside the circum-sphere of any simplex in DT(P).

		By default, qdelaunay merges regions with cocircular or cospherical input sites. 
		If you want a simplicial triangulation use triangulated output ('Qt') or joggled input ('QJ'). 

		Option 'QJ' joggle the input to avoid cospherical and coincident sites. The result is triangulated.
		
		Same result could be achieved with options Qz Qt. 
		Option 'Qz' adds a point above the paraboloid of lifted sites for a Delaunay triangulation.
		It allows the Delaunay triangulation of cospherical sites.
		Option 'Qt' triangulates all non-simplicial facets before generating results

	returns 
		the Delauanay triangulation as a list of tetrahedral facets, if there are no errors. Each face of the tetrahedron is a triangle.
		NULL otherwise.
*/
facetT *compute_delaunay(int dim, int numpoints, MeshModel &m)
{  
	coordT *points;						 /* array of coordinates for each point*/
	boolT ismalloc= True;				 /* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull d QJ Tcv";	     /* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;				 /* output from qh_produce_output()			
											use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;				 /* error messages from qhull code */ 
	int exitcode;						 /* 0 if no error from qhull */
	
	/* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);

	//qh_triangulate();
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */
		return qh facet_list;
	}

	return NULL;
};

/*	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 
	pm --> new mesh
	threshold --> factor that, multiplied to the bbox diagonal, set a threshold used to discard the voronoi vertices too far from the origin.
				  They can cause problems to the qhull library.
				  Growing values of 'threshold' will add more voronoi vertices for a better surface reconstruction.

	compute_voronoi(int dim, int numpoints, MeshModel &m, MeshModel &pm)
		Implements a Voronoi filtering (Amenta and Bern 1998) with Qhull library (http://www.qhull.org/). 
	    The algorithm computes a piecewise-linear approximation of a smooth surface from a finite set of sample points
	    It uses a subset of the Voronoi vertices to remove triangles from the Delaunay triangulation.

	    After computing the Voronoi diagram, foreach sample point it chooses the two farthest opposite Voronoi vertices.
	    Then computes a Delaunay triangulation of the sample points and the selected Voronoi vertices, and keep
	    only those triangles in witch all three vertices are sample points. 

		Options 'Qt' (triangulated output) and 'QJ' (joggled input) may produce unexpected results.

		Option 'Qt' triangulated output. If a Voronoi vertex is defined by cospherical data, Qhull duplicates 
		the vertex. For example, if the data contains a square, the output will contain two copies of the Voronoi vertex.

		Option 'QJ' joggle the input to avoid Voronoi vertices defined by more than dim+1 points. 
		It is less accurate than triangulated output ('Qt').

		Qhull returns the Delauanay triangulation as a list of tetrahedral facets.

	returns 
		true if no errors occurred;
		false otherwise.
		
*/
bool compute_voronoi(int dim, int numpoints, MeshModel &m, MeshModel &pm, float threshold)
{  
	coordT *points;						/* array of coordinates for each point*/
	boolT ismalloc= True;				/* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull d QJ Tcv";	    /* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;				/* output from qh_produce_output()			
										   use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;				/* error messages from qhull code */ 
	int exitcode;						/* 0 if no error from qhull */
		
    /* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);
	coordT* copypoints= (coordT*)malloc(numpoints*dim*sizeof(coordT));
	for(int i=0;i<numpoints*dim;i++)
		copypoints[i] = points[i];

	//First Delaunay Triangulation
	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);

	#if(TestMode)
		ofstream firstTest;
		firstTest.open("firstTest.txt",  ios::out);
		firstTest << "Poli infiniti da non prendere:\n ";
		
		ofstream secondTest;
		secondTest.open("secondTest.txt",  ios::out);
		secondTest << "Punti di input:\n ";

		ofstream thirdTest;
		thirdTest.open("testAfter.txt",  ios::out);
		thirdTest << "Punti di output:\n ";

	#endif

	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */

		//Sets Voronoi vertex as facet center
		qh_setvoronoi_all();

		double *voronoi_vertex;

		//poles_set contains the selected Voronoi vertices.
		//Every facet has a Voronoi vertex; so poles_set size is at most qh num_facets
		setT* poles_set= qh_settemp(qh num_facets); 	
		
		vector<double*> voronoi_vertices; //Voronoi vertices of the region being considered
		vector<double*> normals;          //vector of the outer normals of the convex hull facets that are
										  //neighbor to the point being considered 

		vertexT *vertex;
		FORALLvertices {
			double *first_pole;
			double *second_pole;
			voronoi_vertices.clear();
			normals.clear();
			double max_dist=0; //distance from first_pole to vertex

		    if (qh hull_dim == 3)
			   qh_order_vertexneighbors(vertex);

		    bool is_on_convexhull =false;

			//Considering the neighboring facets of the vertex in order to 
			//compute the Voronoi region for that vertex 
		    facetT *neighbor, **neighborp;

			//Finding first_pole.
		    FOREACHneighbor_(vertex) {
				if (neighbor->upperdelaunay)
					is_on_convexhull =true;
				else {
					voronoi_vertex = neighbor->center;
				    voronoi_vertices.push_back(neighbor->center);

					if (neighbor->toporient)
						normals.push_back(neighbor->normal);
					double* vertex1= vertex->point;
					double* vertex2= voronoi_vertex;
				    double dist = qh_pointdist(vertex1, vertex2,dim);
				    if(dist>max_dist){
						max_dist=dist;
					    first_pole=voronoi_vertex;
				    }
				}
			}

		    if(is_on_convexhull){ //This is a Voronoy vertex at infinity
			 	//Compute normals average
				double* avg_normal= new double[3];
				for(int i=0;i<3;i++){
					avg_normal[i]=0;
					for(size_t j=0; j< normals.size();j++)
						avg_normal[i]+= (normals[j])[i];
				}
				first_pole=avg_normal;

				#if(TestMode)
					firstTest << first_pole[0]<< " " << first_pole[1]<<" " << first_pole[2]<<"\n ";
				#endif

		    }
		    else{ 
				assert(first_pole!=NULL);
				if(first_pole!=NULL){

					//Test if the Voronoi vertex is too far from the origin. 
					//It can cause problems to the qhull library.
					bool discard=false;
					for(int i =0;i<3;i++)
					{
						double* bbCenter = new double[dim];
						double* pole = first_pole;
						for(int i=0;i<dim;i++)
							bbCenter[i] = pm.cm.bbox.Center()[i];
						if(qh_pointdist(bbCenter,pole,dim)>(threshold*pm.cm.bbox.Diag()))
							discard=true;
					}
					if(!discard)
						qh_setunique(&poles_set, first_pole);
				}
			}

		    //vector vertex-first_pole
		    double* sp1= new double[3];
			//vector vertex-second_pole
		    double* sp2= new double[3];
		    for(int i=0; i<3;i++)
			    sp1[i]= first_pole[i] - vertex->point[i];

		    //Finding second_pole
			double max_dist2=0;
		    for(size_t i=0; i<voronoi_vertices.size();i++){
			    if(voronoi_vertices[i]!= first_pole){
					double* vertex1= vertex->point;
					double* vertex2= voronoi_vertices[i];
			   	    double dist = qh_pointdist(vertex1, vertex2,dim); 

					//vector vertex-second_pole
				    for(int j=0; j<dim;j++)
					   sp2[j]= (voronoi_vertices[i])[j] - vertex->point[j];

					//Calculate dot-product between vector vertex-first_pole and vector vertex-second_pole
					double dotProd=0;
					for(int k= 0; k<3;k++ )
						dotProd += sp1[k] * sp2[k];

					//Test if first_pole and second_pole are opposite and if second pole is the farthest from vertex
				    if(dotProd<=0 && dist>max_dist2){
					    max_dist2=dist;
					    second_pole = voronoi_vertices[i];
				    }   			
			    }
			}
			assert(second_pole!=NULL);

			if(second_pole!=NULL){
				//Test if the Voronoi vertex is too far from the origin. 
				//It can cause problems to the qhull library.
				bool discard=false;
				for(int i =0;i<3;i++)
				{
					double* bbCenter = new double[dim];
					double* pole = second_pole;
					for(int i=0;i<dim;i++)
						bbCenter[i] = pm.cm.bbox.Center()[i];
					if(qh_pointdist(bbCenter,pole,dim)>(threshold*pm.cm.bbox.Diag()))
						discard=true;
				}
				if(!discard)
					qh_setunique(&poles_set, second_pole);
			}
			
		}

		int numpoles = qh_setsize(poles_set);
		int tot_newpoints = numpoints + numpoles;

		//Union of the sample points and the selected Voronoi vertices
		coordT * newpoints = (coordT*)malloc(tot_newpoints*dim*sizeof(coordT));
		
		int i=0;
		for(i=0;i<numpoints*3;i++){
			newpoints[i]= copypoints[i];
		}

		double *pole, **polep;
		FOREACHsetelement_(double,poles_set,pole){
			for(int j=0;j<3;j++)
				newpoints[i++]= pole[j];
		}

		#if(TestMode)
			for(int u=0;u<tot_newpoints*3;u+=3)
				secondTest << newpoints[u]<< " " << newpoints[u+1]<<" "<< newpoints[u+2]<<"\n";
		#endif

		//clean old triangulation
		int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
		qh_freeqhull(!qh_ALL); 
		qh_memfreeshort (&curlong, &totlong);
		if (curlong || totlong) 
			fprintf (errfile, "qhull internal warning (user_eg, #1): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);

		
	    //Second Delaunay Triangulation
		char flags2[]= "qhull d QJ Tcv";	/* option flags for qhull, see qh_opt.htm */
	    exitcode= qh_new_qhull (dim, tot_newpoints, newpoints, ismalloc,flags2, outfile, errfile);

		
		if (!exitcode) { /* if no error */ 
			/* 'qh facet_list' contains the convex hull */

			//qh num_vertices-numpoles=numpoints
			tri::Allocator<CMeshO>::AddVertices(pm.cm,qh num_vertices-numpoles);
			
			/*ivp length is 'qh num_vertices' because each vertex is accessed through its ID whose range is 
			  0<=qh_pointid(vertex->point)<qh num_vertices*/
			vector<tri::Allocator<CMeshO>::VertexPointer> ivp(qh num_vertices);

			vertexT* vertex;
			int i=0;
			FORALLvertices{	
				if ((*vertex).point && qh_pointid(vertex->point)<numpoints){
					pm.cm.vert[i].P()[0] = (*vertex).point[0];
					pm.cm.vert[i].P()[1] = (*vertex).point[1];
					pm.cm.vert[i].P()[2] = (*vertex).point[2];
					ivp[qh_pointid(vertex->point)] = &pm.cm.vert[i];
					i++;

					#if(TestMode)
						thirdTest << (*vertex).point[0]<< " " << (*vertex).point[1]<<" " << (*vertex).point[2]<<"\n ";
					#endif

				}
			}

			//Take only the triangles in which all three vertices are sample points
			facetT *facet, *neighbor;
			qh visit_id++;
			FORALLfacet_(qh facet_list) {
				if (!facet->upperdelaunay) {
					facet->visitid= qh visit_id;
					qh_makeridges(facet);
					ridgeT *ridge, **ridgep;
					FOREACHridge_(facet->ridges) {
						neighbor= otherfacet_(ridge, facet);
						if (neighbor->visitid != qh visit_id) {
							tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
							vertexT *vertex;
							int vertex_n, vertex_i;
							FOREACHvertex_i_(ridge->vertices){
								//Test if the facet has only sample points as vertices
								if(qh_pointid(vertex->point)<numpoints)
									(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
								else{
									tri::Allocator<CMeshO>::DeleteFace(pm.cm,*fi);
									break;
								}
							}
						}
					}	
				}
			}
		}
	}

	int curlong, totlong;	  /* memory remaining after qh_memfreeshort */	
	qh_freeqhull(!qh_ALL); 
	qh_memfreeshort (&curlong, &totlong);
	if (curlong || totlong)
		fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
				 totlong, curlong);
	if(!exitcode)
		return true;
	else
		return false;
}

/*
	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 
	pm --> new mesh
	alpha --> upper bound for the radius of the empty circumsphere of each simplex 
	alphashape --> true to calculate alpha shape, false alpha complex

	compute_alpha_shapes(int dim, int numpoints, MeshModel &m, MeshModel &pm, double alpha, bool alphashape)
		build alpha complex or alpha shapes (Edelsbrunner and P.Mucke 1994)from a set of vertices of a mesh with Qhull library (http://www.qhull.org/
		Insert the minimum value of alpha (the circumradius of the triangle) in attribute Quality foreach face.

		The Alpha Shape is the boundary of the alpha complex, that is a subcomplex of the Delaunay triangulation. 
		For a given value of 'alpha', the alpha complex includes all the simplices in the Delaunay 
		triangulation which have an empty circumsphere with radius equal or smaller than 'alpha'. 
		Note that for 'alpha' = 0, the alpha complex consists just of the set P, and for sufficiently large 'alpha', 
		the alpha complex is the Delaunay triangulation DT(P) of P.

		Qhull returns the Delauanay triangulation as a list of tetrahedral facets.

	returns 
		true if no errors occurred;
		false otherwise.
*/

bool compute_alpha_shapes(int dim, int numpoints, MeshModel &m, MeshModel &pm, double alpha, bool alphashape){

	coordT *points;						/* array of coordinates for each point*/
	boolT ismalloc= True;				/* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull d QJ Tcv";	    /* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;				/* output from qh_produce_output()			
										   use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;				/* error messages from qhull code */ 
	int exitcode;						/* 0 if no error from qhull */
		
    /* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);

	int ridgesCount=0;

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);
	
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the delaunay triangulation */

		//Set facet->center as the Voronoi center
		qh_setvoronoi_all();

		int convexNumVert = qh_setsize(qh_facetvertices (qh facet_list, NULL, false));

		//Insert all the sample points, because, even with alpha=0, the alpha shape/alpha complex will
		//contain them.
		tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);

		/*ivp length is 'qh num_vertices' because each vertex is accessed through its ID whose range is 
		  0<=qh_pointid(vertex->point)<qh num_vertices*/
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
		
		//Set of alpha complex triangles for alphashape filtering
		setT* set= qh_settemp(4* qh num_facets); 

		facetT *facet, *neighbor;
		qh visit_id++;
		int numFacets=0;
		FORALLfacet_(qh facet_list) {
			numFacets++;
			if (!facet->upperdelaunay) {
				//For all facets (that are tetrahedrons)calculate the radius of the empty circumsphere considering 
				//the distance between the circumcenter and a vertex of the facet
				vertexT* vertex = (vertexT *)(facet->vertices->e[0].p);
				double* center = facet->center;
				double radius =  qh_pointdist(vertex->point,center,dim);

				if (radius>alpha) // if the facet is not good consider the ridges
				{
					//if calculating the alphashape, unmark the facet ('good' is used as 'marked'). 
					if(alphashape)
						facet->good=false;

					//Compute each ridge (triangle) once and test the cironference radius with alpha
					facet->visitid= qh visit_id;
					qh_makeridges(facet);
					ridgeT *ridge, **ridgep;
					int goodTriangles=0;
					FOREACHridge_(facet->ridges) {
						neighbor= otherfacet_(ridge, facet);
						if (( neighbor->visitid != qh visit_id)){ 			
							//Calculate the radius of the circumference 
							pointT* p0 = ((vertexT*) (ridge->vertices->e[0].p))->point;
							pointT* p1 = ((vertexT*) (ridge->vertices->e[1].p))->point;
							pointT* p2 = ((vertexT*) (ridge->vertices->e[2].p))->point;

							radius = calculate_circumradius(p0,p1,p2, dim);
							
							if(radius <=alpha){
								goodTriangles++;
								//if calculating alpha complex, build the mesh. It needs no filtering.
								if(!alphashape){
									tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
									ridgesCount++;

									//Store the circumradius of the face in face quality
									(*fi).Q() = radius;

									int vertex_n, vertex_i;
									FOREACHvertex_i_(ridge->vertices)
										(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];	
								}
								else
									//if calculating alpha shape, save the triangle (ridge) for subsequent filtering
									qh_setappend(&set, ridge); 
							}
						}
					}

					//If calculating the alphashape, mark the facet('good' is used as 'marked'). 
					//This facet will have some triangles hidden by the facet's neighbor.
					if(alphashape && goodTriangles==4)
						facet->good=true;
					
				}
				else //the facet is good. Put all the triangles of the tetrahedron in the mesh
				{
					//Compute each ridge (triangle) once
					facet->visitid= qh visit_id;
					//If calculating the alphashape, mark the facet('good' is used as 'marked').
					//This facet will have some triangles hidden by the facet's neighbor.
					if(alphashape)
						facet->good=true;
					qh_makeridges(facet);
					ridgeT *ridge, **ridgep;
					FOREACHridge_(facet->ridges) {
						neighbor= otherfacet_(ridge, facet);
						if ((neighbor->visitid != qh visit_id)){
							//if calculating alphacomplex build mesh. It needs no filtering
							if(!alphashape){
								tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
								
								pointT* p0 = ((vertexT*) (ridge->vertices->e[0].p))->point;
								pointT* p1 = ((vertexT*) (ridge->vertices->e[1].p))->point;
								pointT* p2 = ((vertexT*) (ridge->vertices->e[2].p))->point;
								radius = calculate_circumradius(p0,p1,p2, dim);

								//Store the circumradius of the face in face quality
								(*fi).Q() = radius; 

								ridgesCount++;
								int vertex_n, vertex_i;
								FOREACHvertex_i_(ridge->vertices)
									(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
							}
							else
								//if calculating alpha shapes, save the triangle for subsequent filtering	
								qh_setappend(&set, ridge);
						}	
					}
				}
			}
		}
		assert(numFacets== qh num_facets);

		if(alphashape){
			//Filter the triangles (only the ones on the boundary of the alpha complex) and build the mesh
			
			ridgeT *ridge, **ridgep;
			FOREACHridge_(set) {
				if ((!ridge->top->good || !ridge->bottom->good || ridge->top->upperdelaunay || ridge->bottom->upperdelaunay)){
					tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
					ridgesCount++;
					int vertex_n, vertex_i;
					FOREACHvertex_i_(ridge->vertices)
						(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
				}
			}
		}		
	}
	assert(pm.cm.fn == ridgesCount);

	int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
	qh_freeqhull(!qh_ALL);  
	qh_memfreeshort (&curlong, &totlong);
	if (curlong || totlong)
		fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
					 totlong, curlong);

	if(!exitcode)
		return true;
	else
		return false;
}

/*
	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh
	pm --> new mesh that is the convex hull of the flipped points, if convex_hullFP is true
	pm2 --> new mesh that is the convex hull of the non flipped points, if triangVP is true
	viewpoint -> the viewpoint
	threshold -> bounds the radius of the sphere used to select visible points
	convex_hullFP --> true if you want to show the partial Convex Hull of the transformed points cloud
					false otherwise
	triangVP --> true if you want to show a triangulation of the visible points 
				 false otherwise

	bool visible_points(int dim, int numpoints, MeshModel &m, MeshModel &pm,MeshModel &pm2, vcg::Point3f viewpointP,float threshold,bool convex_hullFP,bool triangVP)
		Select the visible points in a point cloud, as viewed from a given viewpoint.
	    It uses the Qhull library (http://www.qhull.org/.
	    The algorithm used (Katz, Tal and Basri 2007) determines visibility without reconstructing a surface or estimating normals.
	    A point is considered visible if its transformed point lies on the convex hull of a trasformed points cloud from the original mesh points.

	returns 
		the number of visible points if no errors occurred;
		-1 otherwise.
*/

int visible_points(int dim, int numpoints, MeshModel &m, MeshModel &pm,MeshModel &pm2, vcg::Point3f viewpointP,float threshold,bool convex_hullFP,bool triangVP){
	boolT ismalloc= True;			/* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull Tcv";		/* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;			/* output from qh_produce_output()			
									   use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;			/* error messages from qhull code */ 
	int exitcode;					/* 0 if no error from qhull */
		
    double* viewpoint = new double[dim];
	for(int i=0;i<3;i++)
		viewpoint[i] = viewpointP[i];

	//Every point in flipped_points and points share the same index
	//Flipped points are numpoints+1 because they include the viewpoint too.
	coordT *flipped_points = (coordT*)malloc((numpoints+1)*(dim)*sizeof(coordT));
	coordT *points = (coordT*)malloc((numpoints)*(dim)*sizeof(coordT));
	//Positions in dist are the indexes of the points 
	double *dist = (double*)malloc((numpoints)*sizeof(double));

	double *point = new double[dim];
	double radius = 0;
	vector<tri::Allocator<CMeshO>::VertexPointer> ivp(numpoints); //Vector of pointers to the input points	
	CMeshO::VertexIterator vi;
	int cnt=0;
	//Compute the radius as the distance between the viewpoint ant its farthest point 
	for(vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); ++vi){
		if(!(*vi).IsD()){
			ivp[cnt]=&(*vi);
			//Move the point so that viewpoint is the origin 
			for(int ii=0;ii<dim;++ii)
				points[3*cnt + ii] = point[ii]=(*vi).P()[ii]- viewpoint[ii];	 
			double distance = 0;
			for(int k= 0; k<dim;k++ )
				distance += point[k] * point[k];
			dist[cnt] = sqrt(distance);
			if(dist[cnt]>radius)
				radius=dist[cnt];
			cnt++;
		}
	}
	assert(cnt==numpoints);

	//Adjust radius according to user param.
	radius = radius * pow(10,threshold);

	//Calculate the flipped points
	for (int j=0; j<numpoints;j++){
		double k= 2*(radius-dist[j])/dist[j];
		for(int ii=0;ii<dim;++ii)
			flipped_points[3*j + ii]=points[3*j + ii]+ k*points[3*j + ii];
	}


	//Attach viewpoint
	for(int ii=0;ii<dim;++ii)
		flipped_points[3*numpoints + ii]= viewpoint[ii];


	//Make a convex hull from the flipped points and the viewpoint
	exitcode= qh_new_qhull (dim, numpoints+1, flipped_points, ismalloc,
							flags, outfile, errfile);

	//By default, Qhull merges coplanar facets. So, it's necessary to triangulate the convex hull.
	//If you call qh_triangulate() method , all facets will be simplicial (e.g., triangles in 2-d)
	//In theory calling qh_triangulate() or using option 'Qt' should give the same result, but,
	//in this case, option Qt does not triangulate the output because coplanar faces are still merged.
	qh_triangulate();

	int selected=0;
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */


		/*ivp length is 'numpoints' because each vertex is accessed through its ID whose range is 
		  0<=qh_pointid(vertex->point)<numpoints. qh num_vertices is < numpoints*/
		vector<tri::Allocator<CMeshO>::VertexPointer> ivp_flipped(numpoints); //Vector of pointers to the flipped points
		vector<tri::Allocator<CMeshO>::VertexPointer> ivp_non_flipped(numpoints); //Vector of pointers to the non flipped points 

		vertexT *vertex;
		selected=qh num_vertices;
		//Eliminate the viewpoint
		FORALLvertices{	
			if ((*vertex).point && qh_pointid(vertex->point)>=numpoints)
				selected--;
		}

		if(convex_hullFP)
			tri::Allocator<CMeshO>::AddVertices(pm.cm,selected);

		if(triangVP)
			tri::Allocator<CMeshO>::AddVertices(pm2.cm,selected);

		int i=0;
		int j=0;
		FORALLvertices{	
			//Do not add the viewpoint
			if ((*vertex).point && qh_pointid(vertex->point)<numpoints){
				//mark the vertex in the mesh that is correspondent to the flipped one (share the same index)		
				ivp[qh_pointid(vertex->point)]->SetS();
				
				if(convex_hullFP){ //Add flipped points to the new mesh
					pm.cm.vert[i].P()[0] = (*vertex).point[0];
					pm.cm.vert[i].P()[1] = (*vertex).point[1];
					pm.cm.vert[i].P()[2] = (*vertex).point[2];
					ivp_flipped[qh_pointid(vertex->point)] = &pm.cm.vert[i];
					i++;
				}
				if(triangVP){ //Add visible points to the new mesh
					pm2.cm.vert[j].P()[0] = (*ivp[qh_pointid(vertex->point)]).P()[0];
					pm2.cm.vert[j].P()[1] = (*ivp[qh_pointid(vertex->point)]).P()[1];
					pm2.cm.vert[j].P()[2] = (*ivp[qh_pointid(vertex->point)]).P()[2];
					ivp_non_flipped[qh_pointid(vertex->point)] = &pm2.cm.vert[j];
					j++;
				}
			}
		}
		vcg::tri::UpdateColor<CMeshO>::VertexSelected(m.cm);

		if(convex_hullFP){
			//Add to the new mesh only the faces of the convex hull whose vertices aren't the viewpoint 
			facetT *facet;
			FORALLfacet_(qh facet_list){		
				vertexT *vertex;
				int vertex_n, vertex_i;
				tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);		
				FOREACHvertex_i_((*facet).vertices){
					if(qh_pointid(vertex->point)<numpoints)
						(*fi).V(vertex_i)= ivp_flipped[qh_pointid(vertex->point)];	
					else{
						tri::Allocator<CMeshO>::DeleteFace(pm.cm,*fi);
						break;
					}
				}
			}
		}
		if(triangVP){
			//Add to the new mesh only the faces whose vertices are visible points, discard the viewpoint
			facetT *facet;
			FORALLfacet_(qh facet_list){		
				vertexT *vertex;
				int vertex_n, vertex_i;
				tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm2.cm,1);		
				FOREACHvertex_i_((*facet).vertices){
					if(qh_pointid(vertex->point)<numpoints)
						(*fi).V(vertex_i)= ivp_non_flipped[qh_pointid(vertex->point)];	
					else{
						tri::Allocator<CMeshO>::DeleteFace(pm2.cm,*fi);
						break;
					}
				}
			}
		}
	}

	int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
	qh_freeqhull(!qh_ALL);  
	qh_memfreeshort (&curlong, &totlong);
	if (curlong || totlong)
		fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
					 totlong, curlong);

	if (!exitcode)
		return selected;
	else 
		return -1;
}

/* dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 

	coordT *qh_readpointsFromMesh(int *numpoints, int *dimension, MeshModel &m)
		build an array of coordinates from the vertices of the mesh m. 
		Each triplet of coordinates rapresents a 3d vertex.

	returns
		the array of coordinates.
*/

coordT *qh_readpointsFromMesh(int *numpoints, int *dimension, MeshModel &m) {
	coordT *points, *coords;

	coords= points= 
	(coordT*)malloc((*numpoints)*(*dimension)*sizeof(coordT));

	int cnt=0;
	CMeshO::VertexIterator vi;
	for(vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); ++vi)
	if(!(*vi).IsD()){
		for(int ii=0;ii<*dimension;++ii)
			*(coords++)=(*vi).P()[ii];
		++cnt;
	}
	assert(cnt==m.cm.vn);

	return(points);
}

/*
	double calculate_circumradius(pointT* p0,pointT* p1,pointT* p2, int dim)
		calculate the radius of the circumference passing through p0, p1, p2.
*/


double calculate_circumradius(pointT* p0,pointT* p1,pointT* p2, int dim){
	coordT a = qh_pointdist(p0,p1,dim);
	coordT b = qh_pointdist(p1,p2,dim);
	coordT c = qh_pointdist(p2,p0,dim);

	coordT sum =(a + b + c)*0.5;
	coordT area = sum*(a+b-sum)*(a+c-sum)*(b+c-sum);
	return (double) (a*b*c)/(4*sqrt(area));
}