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

#include <fstream>
#include <iostream>


using namespace std;
using namespace vcg;

#define TestMode 0

//Internal prototype ALCUNI SONO INTUTILI POI TOGLILI
coordT *qh_readpointsFromMesh(int *numpoints, int *dimension, MeshModel &m);
bool test(facetT* facet, ridgeT* triangle ,double alpha);
void addDelaunay (coordT *points, int numpoints, int numnew, int dim);
coordT pointdist(pointT *point1, pointT *point2, int dim);


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
		build convex hull from a set of vertices of a mesh

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
		build Delauanay triangulation from a set of vertices of a mesh.
			
		The Delaunay triangulation of a set of points in d-dimensional spaces is the projection of the convex 
		hull of the projections of the points onto a (d+1)-dimensional paraboloid.

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

	compute_voronoi(int dim, int numpoints, MeshModel &m)

		Reconstructs the mesh surface starting from its vertices through a double 
		Delaunay triangulation. The second one takes in input some Voronoi vertices too. 

		Options 'Qt' (triangulated output) and 'QJ' (joggled input) may produce unexpected results.

		Option 'Qt' triangulated output. If a Voronoi vertex is defined by cospherical data, Qhull duplicates 
		the vertex. For example, if the data contains a square, the output will contain two copies of the Voronoi vertex.

		Option 'QJ' joggle the input to avoid Voronoi vertices defined by more than dim+1 points. 
		It is less accurate than triangulated output ('Qt').

	returns 
		true if no errors occurred;
		false otherwise.
		
*/
bool compute_voronoi(int dim, int numpoints, MeshModel &m, MeshModel &pm)
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

		vertexT *vertex;
		double *voronoi_vertex;

		//poles_set contains the selected Voronoi vertices.
		//Every facet has a Voronoi vertex; so poles_set size is at most qh num_facets
		setT* poles_set= qh_settemp(qh num_facets); 	

		int poles_num =0;
		
		vector<double*> voronoi_vertices; //Voronoi vertices of the region being considered
		vector<double*> normals;          //vector of the outer normals of the convex hull facets
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
			//compute the Voronoi region of vertex 
		    facetT *neighbor, **neighborp;

			//Finding first_pole.
		    FOREACHneighbor_(vertex) {
				if (neighbor->upperdelaunay)
					is_on_convexhull =true;
				else{
					voronoi_vertex = neighbor->center;
				    voronoi_vertices.push_back(neighbor->center);
					if (neighbor->toporient)
						normals.push_back(neighbor->normal);
					double* vertex1= vertex->point;
					double* vertex2= voronoi_vertex;
				    double dist = qh_pointdist(vertex1, vertex2,4);
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
					for(int j=0; j< normals.size();j++)
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

					//Test if the Voronoi vertex is too far
					bool discard=false;
					for(int i =0;i<3;i++)
					{
						double* bbCenter = new double[dim];
						double* pole = first_pole;
						for(int i=0;i<dim;i++)
							bbCenter[i] = pm.cm.bbox.Center()[i];
						bbCenter[3]=0;
						if(qh_pointdist(bbCenter,pole,dim+1)>(1000*pm.cm.bbox.Diag()))
							discard=true;
					}
					if(!discard)
						qh_setunique(&poles_set, first_pole);
				}
			}

		    //vector vertex-first_pole
		    double* sp1= new double[3];
		    double* sp2= new double[3];
		    for(int i=0; i<3;i++)
			    sp1[i]= first_pole[i] - vertex->point[i];

		    //Finding second_pole
			double max_dist2=0;
		    for(int i=0; i<voronoi_vertices.size();i++){
			    if(voronoi_vertices[i]!= first_pole){
					double* vertex1= vertex->point;
					double* vertex2= voronoi_vertices[i];
			   	    double dist = qh_pointdist(vertex1, vertex2,4); 

					//vector vertex-second_pole
				    for(int j=0; j<4;j++)
					   sp2[j]= (voronoi_vertices[i])[j] - vertex->point[j];

					//Calculate dot-product between vector vertex-first_pole and vector vertex-second_pole
					double dotProd=0;
					for(int k= 0; k<3;k++ )
						dotProd += sp1[k] * sp2[k];

					//Test is first_pole and second_pole are opposite
				    if(dotProd<=0 && dist>max_dist2){
					    max_dist2=dist;
					    second_pole = voronoi_vertices[i];
				    }   			
			    }
			}
			assert(second_pole!=NULL);

			if(second_pole!=NULL){
				//Test if the Voronoi vertex is too far
				bool discard=false;
				for(int i =0;i<3;i++)
				{
					double* bbCenter = new double[dim+1];
					double* pole = second_pole;
					for(int i=0;i<dim;i++)
						bbCenter[i] = pm.cm.bbox.Center()[i];
					bbCenter[3]=0;
					if(qh_pointdist(bbCenter,pole,dim+1)>(1000*pm.cm.bbox.Diag()))
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

			int vertex_n, vertex_i;
			vertexT* vertex;
			int i=0;
			FOREACHvertex_i_(qh_facetvertices (qh facet_list, NULL, false)){	
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
			facetT *facet, **facetp,  *neighbor;
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
		return false;
	else
		return true;
}

/*
	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 
	pm --> new mesh
	alpha --> upper bound for the radius of the empty circumsphere of each simplex 
	alphashape --> true to calculate alpha shape, false alpha complex

	compute_alpha_shapes(int dim, int numpoints, MeshModel &m, MeshModel &pm, double alpha, bool alphashape)
		build alpha complex ora alpha shapes from a set of vertices of a mesh.

		The Alpha Shape is the boundary of the alpha complex, that is a subcomplex of the Delaunay triangulation. 
		For a given value of 'alpha', the alpha complex includes all the simplices in the Delaunay 
		triangulation which have an empty circumsphere with radius equal or smaller than 'alpha'. 
		Note that at 'alpha' = 0, the alpha complex consists just of the set P, and for sufficiently large 'alpha', 
		the alpha complex is the Delaunay triangulation D(P) of P.

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

		tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);

		CMeshO::PerFaceAttributeHandle<double> alphaHandle = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<double>(pm.cm,std::string("Alpha"));							

		/*ivp length is 'qh num_vertices' because each vertex is accessed through its ID whose range is 
		  0<=qh_pointid(vertex->point)<qh num_vertices*/
		vector<tri::Allocator<CMeshO>::VertexPointer> ivp(qh num_vertices);
		vertexT *vertex;
		int     vertex_n, vertex_i;
		FOREACHvertex_i_(qh_facetvertices (qh facet_list, NULL, false)){	
			if ((*vertex).point){
				pm.cm.vert[vertex_i].P()[0] = (*vertex).point[0];
				pm.cm.vert[vertex_i].P()[1] = (*vertex).point[1];
				pm.cm.vert[vertex_i].P()[2] = (*vertex).point[2];
				ivp[qh_pointid(vertex->point)] = &pm.cm.vert[vertex_i];
			}
		}
		
		//Set of alpha complex triangles for alphashape filtering
		setT* set= qh_settemp(4* qh num_facets); 

		facetT *facet, **facetp,  *neighbor;
		qh visit_id++;
		int numFacets=0;
		FORALLfacet_(qh facet_list) {
			numFacets++;
			if (!facet->upperdelaunay) {
				//For all facets calculate the radius of the empty circumsphere considering 
				//the distance between the circumcenter and a vertex of the facet
				vertexT* vertex = (vertexT *)(facet->vertices->e[0].p);
				double* center = facet->center;
				double px = (*vertex).point[0];
				double qx = *center;
				double py = (*vertex).point[1];
				double qy = *center++;
				double pz = (*vertex).point[2];
				double qz = *center++;
				double radius = sqrt(pow(px-qx,2)+pow(py-qy,2)+pow(pz-qz,2));

				if (radius>alpha) // if the facet is not good consider the ridges
				{
					//if calculating the alphashape, mark the facet ('good' is used as 'marked'). It could be open and some triangles visible
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

							coordT a = qh_pointdist(p0,p1,3);
							coordT b = qh_pointdist(p1,p2,3);
							coordT c = qh_pointdist(p2,p0,3);

							coordT sum =(a + b + c)*0.5;
							coordT area = sum*(a+b-sum)*(a+c-sum)*(b+c-sum);
							radius = (double) (a*b*c)/(4*sqrt(area));
							
							if(radius <=alpha){
								goodTriangles++;
								//if calculating alpha complex, build the mesh. It needs no filtering.
								if(!alphashape){
									tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
									ridgesCount++;
									alphaHandle[fi] = radius;
									FOREACHvertex_i_(ridge->vertices)
										(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];	
								}
								else
									//if calculating alpha shapes, save the triangle for subsequent filtering
									qh_setunique (&set, ridge); 
							}
						}
					}

					//if calculating the alphashape, mark the facet('good' is used as 'marked'). 
					//This facet could hide some triangle.
					if(alphashape && goodTriangles==4)
						facet->good=true;
					
				}
				else //the facet is good. Put all the triangles of the tetrahedron in the mesh
				{
					//Compute each ridge (triangle) once
					facet->visitid= qh visit_id;
					//if calculating the alphashape, mark the facet('good' is used as 'marked') . 
					//This facet could hide some triangle.
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
								alphaHandle[fi] = radius; //CORREGGI CON IL RAGGIO DELLA CIRCOSFERA
								ridgesCount++;
								FOREACHvertex_i_(ridge->vertices)
									(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
							}
							else
								//if calculating alpha shapes, save the triangle for subsequent filtering	
								qh_setunique(&set, ridge);
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
		return false;
	else
		return true;
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


bool test(facetT* facet, ridgeT* triangle ,double alpha){
	vertexT* vertex = (vertexT *)(facet->vertices->e[0].p);
	double* center = facet->center;
	double px = (*vertex).point[0];
	double qx = *center;
	double py = (*vertex).point[1];
	double qy = *center++;
	double pz = (*vertex).point[2];
	double qz = *center++;
	double radius = sqrt(pow(px-qx,2)+pow(py-qy,2)+pow(pz-qz,2));
	int ridgesCount=0;
	if (radius>alpha){
		//facet->visitid= qh visit_id;
		//qh_makeridges(facet);
		//ridgeT *ridge, **ridgep;
		//FOREACHridge_(facet->ridges) {
		//	if (ridge!=triangle) {				
		//		//Calcola il raggio della circosfera
		//		pointT* p0 = ((vertexT*) (ridge->vertices->e[0].p))->point;
		//		pointT* p1 = ((vertexT*) (ridge->vertices->e[1].p))->point;
		//		pointT* p2 = ((vertexT*) (ridge->vertices->e[2].p))->point;

		//		coordT a = qh_pointdist(p0,p1,3);
		//		coordT b = qh_pointdist(p1,p2,3);
		//		coordT c = qh_pointdist(p2,p0,3);

		//		coordT sum =(a + b + c)*0.5;
		//		coordT area = sum*(a+b-sum)*(a+c-sum)*(b+c-sum);
		//		radius = (double) (a*b*c)/(4*sqrt(area));
		//		
		//		if(radius <=alpha){
		//			ridgesCount++;
		//		}
		//	}
		//if(ridgesCount==3)
		//	return true;
		//else
			return false;
	}
	else return true;
}

void addDelaunay (coordT *points, int numpoints, int numnew, int dim) {
  int j;
  coordT *point;
  facetT *facet;
  realT bestdist;
  boolT isoutside;

  for (j= 0; j < numnew ; j++) {
    point= points + (numpoints+j)*dim;
    if (points == qh first_point)  /* in case of 'QRn' */
      qh num_points= numpoints+j+1;  
    /* qh num_points sets the size of the points array.  You may
       allocate the point elsewhere.  If so, qh_addpoint records
       the point's address in qh other_points 
    */
    
    qh_setdelaunay (dim, 1, point);
    facet= qh_findbestfacet (point, !qh_ALL, &bestdist, &isoutside);
    if (isoutside) {
      if (!qh_addpoint (point, facet, False))
	break;  /* user requested an early exit with 'TVn' or 'TCn' */
    }
    
    /* qh_produce_output(); */
  }
  if (qh DOcheckmax)
    qh_check_maxout();
  else if (qh KEEPnearinside)
    qh_nearcoplanar();
} /*.addDelaunay.*/

coordT pointdist(pointT *point1, pointT *point2, int dim) {
  coordT dist, diff;
  dist= 0.0;

  for (int k=0;k<dim; k++) {
    diff= *point1++ - *point2++;
    dist += diff * diff;
  }
  return(sqrt(dist));
}