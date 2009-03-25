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

using namespace std;
using namespace vcg;

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

void compute_voronoi(int dim, int numpoints, MeshModel &m, MeshModel &pm)
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

	//ATTENTO I VERTICI DEVONO ESSERE TUTTI!!!!
	int convexNumVert = qh_setsize(qh_facetvertices (qh facet_list, NULL, false));

	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the convex hull */
		vertexT *vertex;
		double *voronoi_vertex;
		double *first_pole;
		double *second_pole;
		double max_dist=0;

		setT* poles_set= qh_settemp(qh num_facets); //ogni facet ha un vertice di voronoi abbondiamo
		

		int poles_num =0;
		//Voronoi vertices of the region being considered
		vector<double*> voronoi_vertices;
		FORALLvertices {
			voronoi_vertices.clear();
		    //int site_id = qh_pointid (vertex->point);
		    if (qh hull_dim == 3)
			   qh_order_vertexneighbors(vertex);
		    int infinity_seen = 0;
		    bool is_on_convexhull =false;
		    facetT *neighbor, **neighborp;
		    FOREACHneighbor_(vertex) {
			   if (neighbor->upperdelaunay) {
				 if (!infinity_seen) {
				   infinity_seen = 1;
				   is_on_convexhull =true;
				   //... process a Voronoi vertex "at infinity" ...
				 }
			   }else {
				   voronoi_vertex = neighbor->center;
				   voronoi_vertices.push_back(voronoi_vertex);
				   double dist = qh_pointdist(vertex->point, voronoi_vertex,3);
				   if(dist>max_dist){
					   max_dist=dist;
					   first_pole=voronoi_vertex;
				 }
			   }
		    }

		    if(is_on_convexhull){
			 	//Calcola media normali
		    }
		    else{
				assert(first_pole!=NULL);
				if(first_pole!=NULL)
					qh_setunique(&poles_set, first_pole);

			    //vector sp1
			    double* sp1= new double[3];
			    double* sp2= new double[3];
			    for(int i=0; i<3;i++)
				    sp1[i]= first_pole[i] - vertex->point[i];

			    double max_dist2=0;
			    for(int i=0; i<voronoi_vertices.size();i++){
				    if(voronoi_vertices[i]!= first_pole){
				   	    double dist = qh_pointdist(vertex->point, voronoi_vertices[i],3); 
					    for(int j=0; j<3;j++)
						   sp2[j]= (voronoi_vertices[i])[j] - vertex->point[j];
					    //in realtà fa prodotto scalare
					    double dotProd = qh_getangle(sp1,sp2);
						//max_dist è la distanza sp1
					    //double cos_angle = dotProd/(dist*max_dist);
					    if(dotProd<=0 && dist>max_dist2){
						    max_dist2=dist;
						    second_pole = voronoi_vertices[i];
					    }   			
				    }
				}
				assert(second_pole!=NULL);
				if(second_pole!=NULL)
					qh_setunique(&poles_set, second_pole);
			}
		}
		//S U V
		int numpoles = qh_setsize(poles_set);
		int tot_newpoints = numpoints + numpoles;
		coordT * newpoints = (coordT*)malloc(tot_newpoints*3*sizeof(coordT));
		int i=0;
		FORALLvertices {
			for(int j=0;j<3;j++){
				newpoints[i]= vertex->point[j];
				i++;
			}
		}

		assert(i<=tot_newpoints*3);
		double *pole, **polep;
		FOREACHsetelement_(double,poles_set,pole){
			for(int j=0;j<3;j++)
				newpoints[i++]= pole[j];
		}

		//clean old triangulation
		qh_freeqhull(!qh_ALL); 
		
	    char flags2[]= "qhull d QJ Tcv";	/* option flags for qhull, see qh_opt.htm */
	    exitcode= qh_new_qhull (dim, tot_newpoints, newpoints, ismalloc,flags2, outfile, errfile);
		
		if (!exitcode) { /* if no error */ 
			/* 'qh facet_list' contains the convex hull */

			//convexNumVert sono i vertici iniziali
			tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);
			
			/*ivp length is 'qh num_vertices' because each vertex is accessed through its ID whose range is 
			  0<=qh_pointid(vertex->point)<qh num_vertices*/
			vector<tri::Allocator<CMeshO>::VertexPointer> ivp(qh num_vertices);
			int sdsss = qh num_vertices;
			int vertex_n, vertex_i;
			int i=0;
			FOREACHvertex_i_(qh_facetvertices (qh facet_list, NULL, false)){	
				if ((*vertex).point && qh_pointid(vertex->point)<numpoints){
					pm.cm.vert[i].P()[0] = (*vertex).point[0];
					pm.cm.vert[i].P()[1] = (*vertex).point[1];
					pm.cm.vert[i].P()[2] = (*vertex).point[2];
					ivp[qh_pointid(vertex->point)] = &pm.cm.vert[i];
					i++;
					assert(i<=convexNumVert);
				}
			}

			//prendi solo quei triangoli coi vertici non in S
			facetT *facet, **facetp,  *neighbor;
			FORALLfacet_(qh facet_list) {
				if (!facet->upperdelaunay) {
					facet->visitid= qh visit_id;
					qh_makeridges(facet);
					ridgeT *ridge, **ridgep;
					FOREACHridge_(facet->ridges) {
						neighbor= otherfacet_(ridge, facet);
						if (neighbor->visitid != qh visit_id) {
							tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
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
		int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
		qh_freeqhull(!qh_ALL); 
		qh_memfreeshort (&curlong, &totlong);
		if (curlong || totlong)
			fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
					 totlong, curlong);
	}
}

/*
	The alpha complex is a subcomplex of the Delaunay triangulation. 
	For a given value of 'alpha', the alpha complex includes all the simplices in the Delaunay 
	triangulation which have an empty circumsphere with radius equal or smaller than 'alpha'. 
	Note that at 'alpha' = 0, the alpha complex consists just of the set P, and for sufficiently large 'alpha', 
	the alpha complex is the Delaunay triangulation D(P) of P.

	If "alphashape" is true, it computes the alpha shape of the points.
	If it's false it computes the alpha complex
	
*/

void compute_alpha_shapes(int dim, int numpoints, MeshModel &m, MeshModel &pm, double alpha, bool alphashape){

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

	exitcode= qh_new_qhull (dim, numpoints, points, ismalloc,
							flags, outfile, errfile);
	
	//set facet->center as the Voronoi center
	qh_setvoronoi_all();

	//ATTENTO I VERTICI DEVONO ESSERE TUTTI!!!!
	int convexNumVert = qh_setsize(qh_facetvertices (qh facet_list, NULL, false));

	tri::Allocator<CMeshO>::AddVertices(pm.cm,convexNumVert);
	//RICORDATI DI AGGIUNGERE L'ATTRIBUTO ALPHA ANCHE CON L'ALPHA SHAPE!!!!!!!!!!!!!
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
	
	int ridgeCount=0;
    setT* set= qh_settemp(4* qh num_facets); //abbondiamo
	if (!exitcode) { /* if no error */ 
		/* 'qh facet_list' contains the delaunay triangulation */
		//For all facets computes the radius of the empy circumsphere calculating 
		//the distance between the circumcenter and a vertex of the facet
		facetT *facet, **facetp,  *neighbor;
		qh visit_id++;
		int numFacets=0;
		FORALLfacet_(qh facet_list) {
			numFacets++;
			//Scorro tutte le facce della triangolazione di Delaunay
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
				int delridge=0;
				if (radius>alpha) // if the facet is not good I consider the ridges
				{
					//Compute each ridge (triangle) once and test the cironference radius with alpha
					//Marco la faccia come non visibile
					if(alphashape){
						facet->good=false;
					}
					facet->visitid= qh visit_id;
					qh_makeridges(facet);
					ridgeT *ridge, **ridgep;
					int goodTriangles=0;
					FOREACHridge_(facet->ridges) {
						neighbor= otherfacet_(ridge, facet);
						if (( neighbor->visitid != qh visit_id)){ //||(alphashape && neighbor->visitid != qh visit_id && (neighbor->upperdelaunay || !test(neighbor,ridge,alpha)))) {				
							//Calcola il raggio della circosfera
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
								//se è alphacomplex costruisco direttamente la nuova mesh
								if(!alphashape){
									goodTriangles++;
									tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
									ridgeCount++;
									alphaHandle[fi] = radius;
									FOREACHvertex_i_(ridge->vertices)
										(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];	
								}
								else
									qh_setappend (&set, ridge);
							}
							else if (alphashape){
								    delridge++;
							}
						}
					}
					if(delridge>0)
						int i=0;
					if(alphashape && goodTriangles==4)
						facet->good=true;
					
				}
				else //the facet is good. Put all the triangles of the tetraedro in the mesh
				{
					//Compute each ridge (triangle) once
					facet->visitid= qh visit_id;
					facet->good=true;
					qh_makeridges(facet);
					ridgeT *ridge, **ridgep;
					FOREACHridge_(facet->ridges) {
						neighbor= otherfacet_(ridge, facet);
						if ((neighbor->visitid != qh visit_id)){//||(alphashape && neighbor->visitid != qh visit_id && (neighbor->upperdelaunay || !test(neighbor,ridge, alpha)))){
							//Se è alphacomplex la aggiungo direttamente alla mesh
							if(!alphashape){
								tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
								alphaHandle[fi] = radius;
								ridgeCount++;
								FOREACHvertex_i_(ridge->vertices)
									(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
							}
							else
								qh_setappend (&set, ridge);
						}	
					}
				}
			}
		}
		assert(numFacets== qh num_facets);

		//numFacets=0;
		//if(alphashape){
		//	//costruisci la mesh
		//	qh visit_id++;
		//	
		//	FORALLfacet_(qh facet_list) {
		//		numFacets++;
		//		if (!facet->upperdelaunay) {
		//			facet->visitid= qh visit_id;
		//			qh_makeridges(facet);
		//			ridgeT *ridge, **ridgep;
		//			int ridgenumebr = qh_setsize(facet->ridges);
		//			FOREACHridge_(facet->ridges) {
		//				neighbor= otherfacet_(ridge, facet);
		//				if ((!neighbor->good || !facet->good) /*&& ridge->seen==true*/ && neighbor->visitid != qh visit_id){//||(alphashape && neighbor->visitid != qh visit_id && (neighbor->upperdelaunay || !test(neighbor,ridge, alpha)))){
		//					tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
		//					ridgeCount++;
		//					FOREACHvertex_i_(ridge->vertices)
		//						(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
		//				}
		//			}
		//		}
		//	}
		//	assert(numFacets== qh num_facets);

		if(alphashape){
			//costruisci la mesh
			
			ridgeT *ridge, **ridgep;
			FOREACHridge_(set) {
				if ((!ridge->top->good || !ridge->bottom->good || ridge->top->upperdelaunay || ridge->bottom->upperdelaunay)){//||(alphashape && neighbor->visitid != qh visit_id && (neighbor->upperdelaunay || !test(neighbor,ridge, alpha)))){
					tri::Allocator<CMeshO>::FaceIterator fi=tri::Allocator<CMeshO>::AddFaces(pm.cm,1);
					ridgeCount++;
					FOREACHvertex_i_(ridge->vertices)
						(*fi).V(vertex_i)= ivp[qh_pointid(vertex->point)];
				}
			}



		}		
	}

	assert(pm.cm.fn == ridgeCount);

	int curlong, totlong;	  /* memory remaining after qh_memfreeshort */
	qh_freeqhull(!qh_ALL);  
	qh_memfreeshort (&curlong, &totlong);
	if (curlong || totlong)
		fprintf (stderr, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", 
					 totlong, curlong);
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
	int ridgeCount=0;
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
		//			ridgeCount++;
		//		}
		//	}
		//if(ridgeCount==3)
		//	return true;
		//else
			return false;
	}
	else return true;
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
