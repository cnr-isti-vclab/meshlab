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
		the convex hull as a list of simplicial (triangulated) facets
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
	facetT *facet;					/* set by FORALLfacets */
		
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

		

	}

	return qh facet_list;
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
		the Delauanay triangulation as a list of tetrahedral facets. Each face of the tetrahedron is a triangle.


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
	facetT *facet;						 /* set by FORALLfacets */

	/* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);

	//II Metodo POI CANCELLALO
	//char hidden_options[]=" d v H Qbb Qf Qg Qm Qr Qu Qv Qx Qz TR E V Fp Gt Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 ";
	//
	//boolT new_ismalloc;
	//static boolT firstcall = True;
	//coordT *new_points;

	//if (firstcall) {
	//	qh_meminit (errfile);
	//	firstcall= False;
	//}
	//if (strncmp (flags,"qhull ", 6)) {
	//	fprintf (errfile, "qh_new_qhull: start qhull_cmd argument with \"qhull \"\n");
	//	exit(1);
	//}
	//qh_initqhull_start (NULL, outfile, errfile);


	//exitcode= setjmp (qh errexit); /* simple statement for CRAY J916 */
	//if (!exitcode) {
 //   qh_option ("delaunay  Qbbound-last", NULL, NULL);
 //   qh DELAUNAY= True;     /* 'd'   */
 //   qh SCALElast= True;    /* 'Qbb' */
 //   qh KEEPcoplanar= True; /* 'Qc', to keep coplanars in 'p' */
 //   qh_checkflags (qh qhull_command, hidden_options);
 //   qh PROJECTdelaunay= True;
 //   qh_initflags (qh qhull_command);
 //   points= qh_readpointsFromMesh(&numpoints, &dim, &ismalloc, m);
 //   if (dim >= 5) {
 //     qh_option ("Qxact_merge", NULL, NULL);
 //     qh MERGEexact= True; /* 'Qx' always */
 //   }
 //   qh_init_B (points, numpoints, dim, ismalloc);
 //   qh_qhull();
 //   qh_check_output();
 //  if (outfile)
 //     qh_produce_output(); 

 //   if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPpoint && !qh STOPcone)
 //     qh_check_points();
 //   exitcode= qh_ERRnone;

	//}


	//qh_triangulate();
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */


	}

	return qh facet_list;
};

/*	dim  --> dimension of points
	numpoints --> number of points
	m --> original mesh 

	compute_voronoi(int dim, int numpoints, MeshModel &m)
		

	The Voronoi diagram is the nearest-neighbor map for a set of points. 
	Each region contains those points that are nearer one input site than any other input site. 
	The Voronoi diagram is the dual of the Delaunay triangulation. 

	Options 'Qt' (triangulated output) and 'QJ' (joggled input) may produce unexpected results.

	Option 'Qt' triangulated output. If a Voronoi vertex is defined by cospherical data, Qhull duplicates 
	the vertex. For example, if the data contains a square, the output will contain two copies of the Voronoi vertex.

	Option 'QJ' joggle the input to avoid Voronoi vertices defined by more than dim+1 points. 
	It is less accurate than triangulated output ('Qt').

	returns 
		
*/

facetT *compute_voronoi(int dim, int numpoints, MeshModel &m)
{  
	coordT *points;						/* array of coordinates for each point*/
	boolT ismalloc= True;				/* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull v QJ Tcv";	/* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;				/* output from qh_produce_output()			
										   use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;				/* error messages from qhull code */ 
	int exitcode;						/* 0 if no error from qhull */
	facetT *facet;						/* set by FORALLfacets */
		
    /* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);
	
	qh_setvoronoi_all();

	//qh_triangulate();
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */

		

	}

	return qh facet_list;
};

/*
	The alpha complex is a subcomplex of the Delaunay triangulation. 
	For a given value of 'alpha', the alpha complex includes all the simplices in the Delaunay 
	triangulation which have an empty circumsphere with radius equal or smaller than 'alpha'. 
	Note that at 'alpha' = 0, the alpha complex consists just of the set P, and for sufficiently large 'alpha', 
	the alpha complex is the Delaunay triangulation D(P) of P.
	
*/

facetT *compute_alpha_shapes(int dim, int numpoints, MeshModel &m, double alpha){

	coordT *points;						/* array of coordinates for each point*/
	boolT ismalloc= True;				/* True if qhull should free points in qh_freeqhull() or reallocation */ 
	char flags[]= "qhull d QJ Tcv";	    /* option flags for qhull, see qh_opt.htm */
	FILE *outfile= NULL;				/* output from qh_produce_output()			
										   use NULL to skip qh_produce_output() */ 
	FILE *errfile= stderr;				/* error messages from qhull code */ 
	int exitcode;						/* 0 if no error from qhull */
	facetT *facet;						/* set by FORALLfacets */
		
    /* initialize points[] here.
	   points is an array of coordinates. Each triplet of coordinates rapresents a 3d vertex */
	points= qh_readpointsFromMesh(&numpoints, &dim, m);

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);
	
	//set facet->center as the Voronoi center
	qh_setvoronoi_all();

	int count =0;
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */
		//For all facets computes the radius of the empy circumsphere calculating 
		//the distance between the circumcenter and a vertex of the facet
		FORALLfacets {
			if (!facet->upperdelaunay) {
				vertexT* vertex = (vertexT *)(facet->vertices->e[0].p);
				double* center = facet->center;
				double px = (*vertex).point[0];
				double qx = *center;
				double py = (*vertex).point[1];
				double qy = *center++;
				double pz = (*vertex).point[2];
				double qz = *center++;
				double radius = sqrt(pow(px-qx,2)+pow(py-qy,2)+pow(pz-qz,2));
				if (radius>alpha){
					qh_removefacet(facet);
					count++;
				}
			}
		}
	}

	return qh facet_list;
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
