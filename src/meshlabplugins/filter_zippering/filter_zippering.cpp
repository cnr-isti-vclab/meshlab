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

#include <QtGui>
#include "filter_zippering.h"

#include <vcg/simplex/face/topology.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/hole.h>
#include <vcg/space/intersection2.h>
#include <wrap/gl/glu_tesselator.h>


using namespace vcg;
using namespace std;
//#define JUMP_STEP1 1
//#define REDUNDANCY_ONLY 1
//#define REFINE_PATCH_ONLY 1
#define MAX_LOOP 150
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterZippering::FilterZippering() 
{ 
        typeList <<  FP_REDUNDANCY <<  FP_ZIPPERING;
  
	foreach(FilterIDType tt , types()) actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString FilterZippering::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_REDUNDANCY :  return QString("Select Redundant Faces");
        case FP_ZIPPERING :  return QString("Zippering");
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterZippering::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_REDUNDANCY:  return QString("Remove redundant faces from one mesh or from both of them, starting from borders.");
		case FP_ZIPPERING :  return QString("Merge two triangle meshes into a single one. This method doesn't provide check on redundancy. Based on <b>Controlledand Adaptive Mesh Zippering,</b> by S.Marras, F.Ganovelli, P.Cignoni.");
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

 int FilterZippering::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_REDUNDANCY:  return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTCOLOR;
    case FP_ZIPPERING :  return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTCOLOR;
    default: assert(0);
  }
  return 0;
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
 FilterZippering::FilterClass FilterZippering::getClass(QAction *a)
{
  switch(ID(a))
  {
		case FP_REDUNDANCY: return MeshFilterInterface::Selection;
        case FP_ZIPPERING : return MeshFilterInterface::Remeshing;
		default : assert(0); 
  }
  return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterZippering::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst)
{
         MeshModel *target;
         float maxVal = 0.0;
         switch(ID(action))	 {
			case FP_REDUNDANCY:
								//get diagonals
								foreach (target, md.meshList) {
								if ( target->cm.bbox.Diag() > maxVal ) maxVal = target->cm.bbox.Diag();
								if (target != md.mm())  break;
								}
								parlst.addParam( new RichMesh("FirstMesh", md.mm(), &md, "Mesh (with holes)", "The mesh with holes.") );
								parlst.addParam( new RichMesh("SecondMesh", md.mm(), &md, "Patch", "The mesh that will be used as patch.") );
								parlst.addParam( new RichAbsPerc("distance", maxVal*0.01, 0, maxVal, "Max distance", "Max distance between mesh and path") );
								parlst.addParam( new RichBool("UseQuality", false, "Use quality to select redundant face", "If selected, previously computed face quality will be used in order to select redundant faces.") );
								parlst.addParam( new RichBool("FastErosion", false, "Use fast erosion algorithm", "If selected, improves the speed-up of algorithm (results may be not accurate). Useful for large meshes.") );
								parlst.addParam( new RichBool("FullProcessing", false, "Process the whole Mesh", "If selected, redundancy test is performed over the whole surface of the mesh") );
								break;
								
            case FP_ZIPPERING :
								//get diagonals
								foreach (target, md.meshList) {
									if ( target->cm.bbox.Diag() > maxVal ) maxVal = target->cm.bbox.Diag();
									if (target != md.mm())  break;
								}
								parlst.addParam( new RichMesh("FirstMesh", md.mm(), &md, "Mesh (with holes)", "The mesh with holes.") );
								parlst.addParam( new RichMesh("SecondMesh", target, &md, "Patch", "The mesh that will be used as patch.") );
								parlst.addParam( new RichBool("FaceQuality", false, "Use face quality to select redundant face", "If selected, previously computed face quality will be used in order to select redundant faces.") );
								parlst.addParam( new RichBool("RedundancyOnly", false, "Remove redundant faces only", "If selected, remove redundant faces only without performing other operations.") );
								break;
				default: break; // do not add any parameter for the other filters
        }
}

/* Given the mesh m (let's assume it's mesh with holes) and a face on another mesh (let's assume it's
 * the patch), check if the face can be completely projected on to mesh m. "Completely projected" means
 * that edges' projection completely lies on one or more faces of m, and does not intersect edge
 * border of mesh m.
 * @param face  The query face (from patch)
 * @param m     The mesh with holes
 * @param grid  A face-grid created using the faces of mesh m
 * @param max_dist Max Distance allowed between m and patch; if distance between face and m is higher than max_dist, face will be discarded
 * @return true if face can be completely projected on m (redundant), false otherwise.
 */
bool FilterZippering::checkRedundancy(  CMeshO::FacePointer face,
                                        MeshModel *m,
                                        MeshFaceGrid &grid,
                                        CMeshO::ScalarType max_dist )
{
    // Step1: check if border edge can be projected on m
    
	//search for border edge
	//edge adjacent to a selected face can be considered as border edge
	int i;
	for ( i = 0; i < 3; i ++ ) {
		if ( face::IsBorder(*face, i) ) break;
		if ( face->FFp(i)->IsS() ) break;
	}
    assert( i<3 );
	size_t samplePerEdge = SAMPLES_PER_EDGE;

    //samples edge in uniform way
    vector< Point3< CMeshO::ScalarType > > edge_samples;
    Point3< CMeshO::ScalarType > edge_dir = face->P1(i) - face->P(i); edge_dir.Normalize();
    float step = 1.0/(samplePerEdge+1); //step length
    for( size_t j = 0; j <= samplePerEdge; j++ ) {
        edge_samples.push_back( face->P(i) + edge_dir * (j * step) );
    }

    for ( unsigned int j = 0; j < edge_samples.size(); j ++ ) {
        CMeshO::FacePointer nearestF = 0;
        tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); tri::UnMarkAll(m->cm);
        face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
        MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
        //Search closest point on A
        nearestF =  grid.GetClosest(PDistFunct, markerFunctor, edge_samples[j], max_dist, dist, closest);

        if ( nearestF == 0 )                    return false;   //no face within given range
        if ( isOnBorder(closest, nearestF ) )   return false;   //closest point on border
		if ( nearestF->IsD() )					return false;
		if ( nearestF->IsS() )					return false;	//face is selected (will be deleted)
    }

    //check if V2(i) has a closest point on border of m
    CMeshO::FacePointer nearestF = 0;
    tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm);
    face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
    MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
    nearestF =  grid.GetClosest(PDistFunct, markerFunctor, face->P2(i), max_dist, dist, closest);
    if ( nearestF == 0 )					return false;    //no face within given range
    if ( isOnBorder( closest, nearestF ) )	return false;    //closest point on border
	if ( nearestF->IsD() )					return false;
	if ( nearestF->IsS() )					return false;

    //check if edges are completely projectable on m
    for ( int j = (i+1)%3; j != i; j = (j+1)%3 ) {
		samplePerEdge = SAMPLES_PER_EDGE;
        edge_samples.clear(); edge_dir = face->P1(j) - face->P(j); edge_dir.Normalize();
        for( size_t k = 0; k <= samplePerEdge; k++ )  edge_samples.push_back( face->P(j) + (face->P1(j) - face->P(j)) * (k * step) ); //campionamento
        // samples on A
        for ( size_t k = 0; k < edge_samples.size(); k ++ ) {
            CMeshO::FacePointer nearestF = 0;
            tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); tri::UnMarkAll(m->cm);
            face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
            MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
            //Search closest point on A
            nearestF =  grid.GetClosest(PDistFunct, markerFunctor, edge_samples[k], max_dist, dist, closest);
            if ( nearestF == 0 )                    return false;   //no face within given range
            if ( isOnBorder(closest, nearestF ) )   return false;   //closest point on border
			if ( nearestF->IsD() )					return false;
			if ( nearestF->IsS() )					return false;
        }
    }
    // redundant
    return true;
}

/*
 * Perform a simple test on a face in order to check redundancy (F.Ganovelli improvement of redundancy test)
 * CheckRedundancy procedure uses a number of sampled points; instead, this function performs redundancy test
 * using only the barycenter of face f. Then, after that nearest face is found, function verifies if
 * vertices' distance from border is greater than max edge length of f; in this case, f is considered redundant.
 * Alternatively, it can be used in order to check if face is too distance from border of M.
 * @param face  The query face (from patch)
 * @param m     The mesh with holes (Note that distance from border must be previously calculated)
 * @param grid  A face-grid created using the faces of mesh m
 * @param max_dist Max Distance allowed between m and patch; if distance between face and m is higher than max_dist, face will be discarded
 * @param test parameted used to determine the type of test
 * @return true if face can be considered redundant, false otherwise
*/
bool FilterZippering::simpleCheckRedundancy(   CMeshO::FacePointer f,   //face
                                MeshModel *m,            //mesh A
                                MeshFaceGrid &grid,      //grid A
								CMeshO::ScalarType max_dist,
								bool test) {   //Max search distance

  Point3f qp = Barycenter(*f); //f barycenter
	//search for max_edge
  float max_edge = max( Distance<float>(f->P(0),f->P(1)), max( Distance<float>(f->P(1),f->P(2)), Distance<float>(f->P(2),f->P(0)) ) );
  float dist = max_dist; CMeshO::FacePointer nearestF = 0; Point3f closest;
  tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); UnMarkAll(m->cm);
    face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
	nearestF =  grid.GetClosest(PDistFunct, markerFunctor, qp, max_dist, dist, closest);
	if (nearestF == 0) return false;	//too far away
  float min_q = min( nearestF->V(0)->Q(), min( nearestF->V(1)->Q(), nearestF->V(2)->Q() ) ); //min distance of nearestF's vertices from M-border
  float max_q = max( f->V(0)->Q(), max( f->V(1)->Q(), f->V(2)->Q() ) );						 //max distance of F's vertices from A-border
	if ( min_q <= max_edge ) return false;
	if (test) if ( min_q <= max_q ) return false;
	return true;
}

/* Check if point is on border of face f.
 * @param point The query-point
 * @param f     Face containing point
 * @return      true if point lies on a border edge of f, false otherwise
 */
bool FilterZippering::isOnBorder( Point3f point, CMeshO::FacePointer f )  {
    // TODO: bar. coords?
    // for each edge, calculates distance point-edge
	if ( f == 0 ) return false;	//null face
    int cnt = 0;
    for ( int i = 0; i < 3; i ++ ) {
       Line3<CMeshO::ScalarType> line( f->P(i), f->P1(i) - f->P(i) );
       if ( Distance( line, point ) <= eps && ( face::IsBorder( *f, i ) || f->FFp(i)->IsS() ) ) { //lying on border edge
            cnt ++;
       }
    }
    assert( cnt < 3 );
    if ( cnt > 0 ) return true; //it's on border edge
    // it isn't on border edge, so check if it's coinciding with a border-vertex
    cnt = 0;
    for ( int i = 0; i < 3; i ++ )
        if ( Distance( point, f->P(i) ) <= eps ) //it' s a vertex
            return isBorderVert( f, i );
    // no border-edge, no border-vertex, stop
    return false;
}

/* Check if vertex point on face f belongs to a edge (not necessary border edge).
 * @param point quesy point
 * @param f face
 * @return true if point lies on edge of f, false otherwise
 * DEPRECATED
 */
bool FilterZippering::isOnEdge( Point3f point, CMeshO::FacePointer f ) {
	// TODO: bar. coords?
    // for each edge, calculates distance point-edge
	if ( f == 0 ) return false;	//null face
    int cnt = 0;
    for ( int i = 0; i < 3; i ++ ) {
       Line3<CMeshO::ScalarType> line( f->P(i), f->P1(i) - f->P(i) );
       if ( Distance( line, point ) <= eps ) cnt ++;
    }
    if ( cnt > 0 ) return true; //it's on edge
    return false;
}

/* Check if vertex i on face f belong to a border edge.
 * @param f face
 * @param i index of vertex
 * @return true if f->V(i) lies on border-edge, false otherwise
 */
bool FilterZippering::isBorderVert( CMeshO::FacePointer f, int i ) {

    face::Pos<CMeshO::FaceType> p( f, i, f->V(i) );
    //loop
    do {
		//border
        if ( face::IsBorder( *p.F(), p.E() ) ) return true;
		//adj to selected face
		if ( p.F()->FFp(p.E())->IsS() ) return true;
        p.FlipE(); p.FlipF();
    } while(p.F() != f);
    return false;
}

/* Check adjiacency between two faces
 * @param f1 first face
 * @param f2 second face
 * return true if f1 is adjacent to f2, false otherwise
 */
bool FilterZippering::isAdjacent( CMeshO::FacePointer f1, CMeshO::FacePointer f2 ) {
    if ( f1 == f2 ) return false;
    return ((f1 == f2->FFp(0)) ||  (f1 == f2->FFp(1)) || (f1 == f2->FFp(2)));
}

/* Using auxiliar informations, replace a face with a new triangulation, discarding part of the originale face
 * and tesselating the other part.
 * @param info  Auxiliar information from face
 * @param N     Face normal
 * @param coords    Output coords of vertices
 * @param pointers  Output vertex indices, will be used for creation of new faces
 */
void FilterZippering::handleBorder( aux_info &info,                                             //Auxiliar information for triangulatio
                                    Point3f N,                          //face normal (useful for proiection)
                                    vector<Point3f> &/*coords*/,                     //output coords
                                    vector<int> &pointers ) {                              //output triangles
	// rotation matrix (will be used for projection on plane)
    Matrix44f rot_matrix;
    rot_matrix.SetRotateRad( Angle<CMeshO::ScalarType>( N, Point3f(0.0, 0.0, 1.0) ), N ^ Point3f(0.0, 0.0, 1.0) );
	//border refinement - brand new method?
	//Remove intersecating border
  for (size_t i = 0; i < info.border.size(); i ++) {
    for ( size_t j = 0; j < info.border[i].edges.size(); j++) {
			//project one border segment on face
      Segment2f s(   Point2f((rot_matrix * info.border[i].edges[j].P0()).X(), (rot_matrix * info.border[i].edges[j].P0()).Y()),
                Point2f((rot_matrix * info.border[i].edges[j].P1()).X(), (rot_matrix * info.border[i].edges[j].P1()).Y()) );	//projects edge on plane
      Point2f x;
      for ( size_t k = i+1; k < info.border.size(); k++) {
        for ( size_t h = 0; h < info.border[k].edges.size(); h++) {
          Segment2f t(   Point2f((rot_matrix * info.border[k].edges[h].P0()).X(), (rot_matrix * info.border[k].edges[h].P0()).Y()),
                    Point2f((rot_matrix * info.border[k].edges[h].P1()).X(), (rot_matrix * info.border[k].edges[h].P1()).Y()) );	//projects edge on plane
          if ( SegmentSegmentIntersection( s, t, x ) ) {
						h = info.border[k].edges.size();
						info.border.erase(info.border.begin() + k); 
						k--;
					}
				}
			}
		}
	}
	//Split border if necessary
  for (size_t i = 0; i < info.border.size(); i ++) {
    for ( size_t j = 1; j < info.border[i].edges.size(); j++ ) {
			//check if P0 lies on trash edge, then split border (skip first edge)
      for ( size_t k = 0; k < info.trash[0].edges.size(); k++ ) {
        if ( SquaredDistance<float>( info.trash[0].edges[k], info.border[i].edges[j].P0() ) == 0.0f ) { //approxim
					//Split is needed
					polyline newborder; newborder.edges.insert( newborder.edges.begin(), info.border[i].edges.begin() + j, info.border[i].edges.end() );
					newborder.verts.insert( newborder.verts.begin(), info.border[i].verts.begin() + j, info.border[i].verts.end() );
					info.border[i].edges.erase( info.border[i].edges.begin() + j, info.border[i].edges.end() );
					info.border[i].verts.erase( info.border[i].verts.begin() + j, info.border[i].verts.end() );
					info.border.push_back( newborder );
					break;
				}
			}
		}
	}

    // For each border...
    for (size_t i = 0; i < info.border.size(); i ++) {
        //search for component intersecated by border and split it into two or more components
        bool conn = true; int c = searchComponent( info, info.border[i].edges.front().P0(), info.border[i].edges.back().P1(), conn ); polyline current;
        if ( conn ) current = info.conn[c]; else current = info.trash[c];
        info.AddCComponent( cutComponent( current, info.border[i], rot_matrix ) );
    polyline rev_border = info.border[i]; reverse( rev_border.edges.begin(), rev_border.edges.end() );
        for ( size_t k = 0; k < rev_border.edges.size(); k ++) rev_border.edges[k].Flip();
        info.AddTComponent( cutComponent( current, rev_border, rot_matrix ) );
        if ( conn ) info.RemoveCComponent( c ); else info.RemoveTComponent( c );
    }
	//No border; triangulation of the whole face
	if ( info.border.size() == 0 ) {
        info.conn = info.trash;
        for ( size_t  i = 0; i < info.conn.size(); i ++ ) {
            reverse( info.conn[i].edges.begin(), info.conn[i].edges.end() );
            for ( size_t j = 0; j < info.conn[i].edges.size(); j ++ ) info.conn[i].edges[j].Flip();
            reverse( info.conn[i].verts.begin(), info.conn[i].verts.end() );
            for ( size_t j = 0; j < info.conn[i].verts.size(); j ++ ) info.conn[i].verts[j] = make_pair(info.conn[i].verts[j].second, info.conn[i].verts[j].first);
        }
    }
    //triangulation of Ccomponent
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        vector< Point3f > points;  //coords vector
        vector< int > vertices;          //vertices vector
        for ( size_t j = 0; j < info.conn[i].edges.size(); j ++ )  {
            points.push_back( info.conn[i].edges[j].P0() );
            vertices.push_back( info.conn[i].verts[j].first );
        }
        if ( points.size() < 3 ) continue;
        vector< int > indices; int iters = 0;
        vector< vector< Point3f > > outlines; outlines.push_back( points );
        while ( indices.size() == 0 && ++iters < MAX_LOOP ) {
            glu_tesselator::tesselate( outlines, indices );    //glu tessellator
            if ( indices.size() == 0 )
                for ( size_t k = 0; k < outlines[0].size(); k ++ ) outlines[0][k] = outlines[0][k] * 10.0; //glu tessellator doesn't work properly for close points, so we scale coords in order to obtain a triangulation, if needed
        }
        for ( size_t k = 0; k < indices.size(); k ++ )  pointers.push_back( vertices[indices[k]] );    //save indices, in order to create new faces
    }
}

/* Split a component into two component, using border as separating line. Discard component external to border polyline.
 * @param comp To-be-split component
 * @param border Border edges
 * @param rot_mat Rotation matrix, needed to project comp points on z=k plane
 */

polyline FilterZippering::cutComponent( polyline comp,                                    //Component to be cut
                                        polyline border,                                  //border
                                        Matrix44f rot_mat ) {     //Rotation matrix

  Point3f startpoint = border.edges.front().P0();
    Point3f endpoint = border.edges.back().P1();
  Point2<CMeshO::ScalarType> startpoint2D ( (rot_mat * startpoint).X(), (rot_mat * startpoint).Y() );
    Point2<CMeshO::ScalarType> endpoint2D ( (rot_mat * endpoint).X(), (rot_mat * endpoint).Y() );
    int startedge = 0, endedge = 0; float min_dist_s = SquaredDistance<CMeshO::ScalarType>( comp.edges[0], startpoint ), min_dist_e = SquaredDistance<CMeshO::ScalarType>( comp.edges[0], endpoint );
    bool v_start = false, v_end = false;
    // search where startpoint and endpoint lie
    for ( size_t i = 0; i < comp.edges.size(); i ++ ) {
    if ( !v_start && SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ) <= min_dist_s ) { startedge = i; min_dist_s = SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ); }
        if ( !v_end && SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ) <= min_dist_e ) { endedge = i; min_dist_e = SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ); }
        if ( comp.edges[i].P1() == startpoint ) { startedge = i; v_start = true; }  //lies on a vertex
        if ( comp.edges[i].P0() == endpoint ) { endedge = i; v_end = true; }        //lies on a vertex
    }
    polyline p;
	// border edges will be edges of new comp
    p.edges.insert( p.edges.begin(), border.edges.begin(), border.edges.end() );
    p.verts.insert( p.verts.begin(), border.verts.begin(), border.verts.end() );
    // startedge == endedge
    if ( startedge == endedge && !Convex<CMeshO::ScalarType>( startpoint2D, Point2<CMeshO::ScalarType> ( (rot_mat * border.edges.front().P1()).X(), (rot_mat * border.edges.front().P1()).Y() ) , endpoint2D ) ) {
        Segment3<CMeshO::ScalarType> join( endpoint, startpoint );
        p.edges.push_back( join ); p.verts.push_back( make_pair( border.verts.back().second, border.verts.front().first ) ); //Vertex pointers
        return p;
    }
	
    // startedge!=endedge
    // search point on the right, create oriented segment and go on
    int step = -1;
    Point3f c0 = border.edges.back().P0();
    vector< Segment3<CMeshO::ScalarType> >::iterator edge_it = border.edges.end(); edge_it--;
    //too short segment; not reliable
    while ( Distance<float>( rot_mat * c0, rot_mat * endpoint ) <= 5.0 * eps ) {
        //previous
        c0 = (*edge_it).P0();
        if (edge_it != border.edges.begin()) edge_it--; else break;
    }
    if ( v_end ) {
        if ( !Convex<CMeshO::ScalarType>( Point2<CMeshO::ScalarType> ( (rot_mat * c0).X(), (rot_mat * c0).Y() ), endpoint2D,
                                               Point2<CMeshO::ScalarType> ( (rot_mat * comp.edges[(endedge+comp.edges.size()-1)%comp.edges.size()].P0()).X(), (rot_mat * comp.edges[(endedge+comp.edges.size()-1)%comp.edges.size()].P0()).Y() ) ) ) {
            step = comp.edges.size() - 1;
        }
        else {
            step = comp.edges.size() + 1;
            Segment3<CMeshO::ScalarType> s( comp.edges[endedge].P0(), comp.edges[endedge].P1() );
            p.edges.push_back( s ); step = comp.edges.size() + 1;
            p.verts.push_back( make_pair( comp.verts[endedge].first, comp.verts[endedge].second ) );
        }
    }
    else {
        if ( !Convex<CMeshO::ScalarType>(   Point2<CMeshO::ScalarType> ( (rot_mat * c0).X(), (rot_mat * c0).Y() ), endpoint2D,
                                                 Point2<CMeshO::ScalarType> ( (rot_mat * comp.edges[endedge].P0()).X(), (rot_mat * comp.edges[endedge].P0()).Y() ) ) ) {
            Segment3<CMeshO::ScalarType> s( endpoint, comp.edges[endedge].P0() );
            p.edges.push_back( s ); step = comp.edges.size() - 1;
            p.verts.push_back( make_pair(border.verts.back().second, comp.verts[endedge].first ) );
        }
        else {
            Segment3<CMeshO::ScalarType> s( endpoint, comp.edges[endedge].P1() );
            p.edges.push_back( s ); step = comp.edges.size() + 1;
            p.verts.push_back( make_pair(border.verts.back().second, comp.verts[endedge].second ) );
        }
    }
    for ( int i = (endedge + step)%(comp.edges.size()); i != startedge; i = (i + step)%(comp.edges.size()) ) {
        p.edges.push_back( comp.edges[i] );
        pair<int, int> vs( comp.verts[i] );
        if ( (p.edges[p.edges.size()-2].P0() == p.edges.back().P0()) ||  (p.edges[p.edges.size()-2].P1() == p.edges.back().P1()) ) {
            p.edges.back().Flip();  //change direction
            vs = make_pair( comp.verts[i].second, comp.verts[i].first );   //change direction
        }
        p.verts.push_back( vs );
    }

    //last segment
    if ( v_start ) {
        if ( p.edges.back().P1() == comp.edges[startedge].P0() ) {
            Segment3<CMeshO::ScalarType> s( comp.edges[startedge].P0() , comp.edges[startedge].P1() );
            p.edges.push_back( s );
            p.verts.push_back( make_pair ( comp.verts[startedge].first , comp.verts[startedge].second ) );
        }
    }
    else {
        Segment3<CMeshO::ScalarType> s( p.edges.back().P1() , startpoint );
        p.edges.push_back( s );
        p.verts.push_back( make_pair ( p.verts.back().second, border.verts.front().first ) );
    }
	//comp
    return p;
}

/* Search for component intersected by points P0 and P1, which are respectively first and last point of a border polyline.
 * @param info auxiliar information containing data about components
 * @param P0 first point of border
 * @param conn output parameter, true if resulting component is CC, false if it is trashC
 * @return index of component
 */
int FilterZippering::searchComponent(			aux_info &info,								//Auxiliar info
                        Point3f P0,			//Start border point
                        Point3f P1,			//End border point
												bool &conn ) {
	int nearestC = -1; int nearestT = -1;
	float distanceC = 100000*eps; float distanceT = 100000*eps;

	for ( int i = 0; i < info.nCComponent(); i ++ ) {
		//for each ccon search for edges nearest to P0 and P1
		float distP0 = 200000*eps; float distP1 = 200000*eps;
    for ( size_t j = 0; j < info.conn[i].edges.size(); j ++ ) {
      if ( SquaredDistance<float>( info.conn[i].edges[j], P0 ) < distP0 ) distP0 = SquaredDistance<float>( info.conn[i].edges[j], P0 );
      if ( SquaredDistance<float>( info.conn[i].edges[j], P1 ) < distP1 ) distP1 = SquaredDistance<float>( info.conn[i].edges[j], P1 );
		}
		if ( distP0 + distP1 < distanceC ) { distanceC = distP0 + distP1; nearestC = i; }
	}

  for ( size_t i = 0; i < info.nTComponent(); i ++ ) {
		//for each trash search for edges nearest to P0 and P1
		float distP0 = 200000*eps; float distP1 = 200000*eps;
    for ( size_t j = 0; j < info.trash[i].edges.size(); j ++ ) {
      if ( SquaredDistance<float>( info.trash[i].edges[j], P0 ) < distP0 ) distP0 = SquaredDistance<float>( info.trash[i].edges[j], P0 );
      if ( SquaredDistance<float>( info.trash[i].edges[j], P1 ) < distP1 ) distP1 = SquaredDistance<float>( info.trash[i].edges[j], P1 );
		}
		if ( distP0 + distP1 < distanceT ) { distanceT = distP0 + distP1; nearestT = i; }
	}

	if ( distanceC <= distanceT ) { conn = true; return nearestC; }
	conn = false; return nearestT;
}

/* Check if face f1 and f2 have a common vertex.
 * @param f1 face#1
 * @param f2 face#2
 * @return true if faces share a vertex, false otherwise
 */
int  FilterZippering::sharesVertex( CMeshO::FacePointer f1, CMeshO::FacePointer f2 ) {
    for (int i = 0; i < 3; i ++) {
        for (int k = 0; k < 3; k ++)
            if ( f1->V(i) == f2->V(k) ) return i;
    }
    return -1;
}

/* Given a face and an edge, check if edge projection intersect one of face edge.
 * last_split edge is not checked.
 * @param currentF Query face
 * @param edge	   Query edge
 * @param last_split Discard this edge during check
 * @param splitted_edge Contains index of splitted edge
 * @param hit	   Approximated intersection point
 * @return true if there's intersection, false otherwise
 */
bool FilterZippering::findIntersection(  CMeshO::FacePointer currentF,				//face
                     Segment3<float> edge,					//edge
										 int last_split,							//previously splitted edge
										 int &splitted_edge,						//currently splitted edge
                     Point3f &hit ) {	//approximate intersection point
	splitted_edge = -1;
    Plane3<CMeshO::ScalarType> plane; plane.Init( currentF->P(0), currentF->N() ); //projection plane
    Matrix44f rot_m; Point2f pt;	//matrix
    rot_m.SetRotateRad( Angle<CMeshO::ScalarType>( currentF->N(), Point3f(0.0, 0.0, 1.0) ), currentF->N() ^ Point3f(0.0, 0.0, 1.0) );
    Segment2f s(   Point2f((rot_m * plane.Projection(edge.P0())).X(), (rot_m * plane.Projection(edge.P0())).Y()),
                        Point2f((rot_m * plane.Projection(edge.P1())).X(), (rot_m * plane.Projection(edge.P1())).Y()) );	//projects edge on plane
	for ( int e = 0; e < 3; e ++ ) {
        if ( e != last_split && SegmentSegmentIntersection( s, Segment2f( Point2f( (rot_m * currentF->P(e)).X(), (rot_m * currentF->P(e)).Y() ),
                                                                                    Point2f( (rot_m * currentF->P1(e)).X(), (rot_m * currentF->P1(e)).Y() ) ), pt ) ) {
            splitted_edge = e; break;
        }
    }
    if (splitted_edge == -1) return false;	//No intersection!
	// search intersection point (approximation)
    Segment3<CMeshO::ScalarType> b_edge( currentF->P(splitted_edge), currentF->P1(splitted_edge) );
	int sampleNum = SAMPLES_PER_EDGE; float step = 1.0 / (sampleNum + 1);
    Point3f closest;    float min_dist = b_edge.Length();
    for ( int k = 0; k <= sampleNum; k ++ ) {
        Point3f currentP = edge.P0() + (edge.P1() - edge.P0())*(k*step);
        if ( SquaredDistance( b_edge, currentP ) < min_dist ) {
            closest = currentP; min_dist = SquaredDistance( b_edge, closest );
        }
    }
    if ( min_dist >= b_edge.Length() ) return false; //point not found
    hit = ClosestPoint(b_edge, closest); //projection on edge
	return true;
}

/**
 * Initialize queue for the selection of redundant faces.
 * @param queue The queue (unsorted)
 * @param m00   First mesh
 * @param m01   Second mesh
 * @param fullProcess If true, insert all the faces in the queue
 */
bool FilterZippering::Init_q( vector< pair<CMeshO::FacePointer,char> >& queue,
						    MeshModel* a,
							MeshModel* b,
							bool fullProcess ) {
	//full process mode: store all faces in the queue
	if ( fullProcess ) {
		// all the faces from A
		for ( CMeshO::FaceIterator fi = a->cm.face.begin(); fi != a->cm.face.end(); ++fi ) 
			queue.push_back( std::make_pair(&*fi, 'A') );
		// all the faces from B
		for ( CMeshO::FaceIterator fi = b->cm.face.begin(); fi != b->cm.face.end(); ++fi ) 
			queue.push_back( std::make_pair(&*fi, 'B') );
		return true;
	}

	//normal mode: store only border faces
	vector< tri::Hole<CMeshO>::Info > a_border;
	vector< tri::Hole<CMeshO>::Info > b_border;
	//get information about border of the mesh
	tri::Hole<CMeshO>::GetInfo( a->cm, false, a_border );
	tri::Hole<CMeshO>::GetInfo( b->cm, false, b_border );

	if ( a_border.empty() && b_border.empty() ) {
		Log( "No border face, exiting" );
		return false;
	}
	//face from A-border
	for ( size_t i = 0; i < a_border.size(); i ++ ) {
		face::Pos<CMeshO::FaceType> p = a_border[i].p;
		if ( p.F()->IsD() ) continue;
		do {
			if ( !p.F()->IsD())  queue.push_back( make_pair(p.F(),'A') );	//label 'A'
			p.NextB();
		} while ( p.F() != a_border[i].p.F() ); //visit the whole border
	}
	//face from B-border
    for ( size_t i = 0; i < b_border.size(); i ++ ) {
		face::Pos<CMeshO::FaceType> p = b_border[i].p;
		if ( p.F()->IsD() ) continue;
		do {
			if ( !p.F()->IsD() )  queue.push_back( make_pair(p.F(),'B') );	//label 'B'
			p.NextB();
		} while ( p.F() != b_border[i].p.F() );
	}
	//check if queue is empty
	if (queue.empty()) return false;
	//queue is not empty, return true
	return true;
}

/**
 * Initialize queue for the selection of redundant faces (overload for priority queue)
 * @param queue The queue (unsorted)
 * @param m00   First mesh
 * @param m01   Second mesh
 * @param fullProcess If true, insert all the faces in the queue
 */
bool FilterZippering::Init_pq( std::priority_queue< std::pair<CMeshO::FacePointer,char>, std::vector< std::pair<CMeshO::FacePointer,char> >, compareFaceQuality >& queue,
							MeshModel* a,
							MeshModel* b,
							bool fullProcess ) {
	//full process mode: store all faces in the queue
	if ( fullProcess ) {
		// all the faces from A
		for ( CMeshO::FaceIterator fi = a->cm.face.begin(); fi != a->cm.face.end(); ++fi ) 
			queue.push( std::make_pair(&*fi, 'A') );
		// all the faces from B
		for ( CMeshO::FaceIterator fi = b->cm.face.begin(); fi != b->cm.face.end(); ++fi ) 
			queue.push( std::make_pair(&*fi, 'B') );
		return true;
	}

	//normal mode: store only border faces
	vector< tri::Hole<CMeshO>::Info > a_border;
	vector< tri::Hole<CMeshO>::Info > b_border;
	//get information about border of the mesh
	tri::Hole<CMeshO>::GetInfo( a->cm, false, a_border );
	tri::Hole<CMeshO>::GetInfo( b->cm, false, b_border );

	if ( a_border.empty() && b_border.empty() ) {
		Log( "No border face, exiting" );
		return false;
	}
	//face from A-border
	for ( size_t i = 0; i < a_border.size(); i ++ ) {
		face::Pos<CMeshO::FaceType> p = a_border[i].p;
		if ( p.F()->IsD() ) continue;
		do {
			if ( !p.F()->IsD())  queue.push( make_pair(p.F(),'A') );	//label 'A'
			p.NextB();
		} while ( p.F() != a_border[i].p.F() ); //visit the whole border
	}
	//face from B-border
    for ( size_t i = 0; i < b_border.size(); i ++ ) {
		face::Pos<CMeshO::FaceType> p = b_border[i].p;
		if ( p.F()->IsD() ) continue;
		do {
			if ( !p.F()->IsD() )  queue.push( make_pair(p.F(),'B') );	//label 'B'
			p.NextB();
		} while ( p.F() != b_border[i].p.F() );
	}
	//check if queue is empty
	if (queue.empty()) return false;
	//queue is not empty, return true
	return true;
}


/**
 * Select redundant faces from meshes A and B. A face is said to be redundant if
 * a number of samples of the face project on the surface of the other mesh.
 * @param queue Unsorted queue containing face-pointers from both meshes
 * @param a, b the meshes involved in the process
 * @param epsilon Maximum search distance
 */
void FilterZippering::selectRedundant( std::vector< std::pair<CMeshO::FacePointer,char> >& queue,	//queue
							  MeshModel* a,													//mesh A
							  MeshModel* b,													//mesh B
							  float epsilon ) {												//max search distance
	//create grid on the meshes (needed for nearest point search)
	MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid A
	MeshFaceGrid grid_b; grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid B
	//clear selection on both meshes
	tri::UpdateSelection<CMeshO>::Clear( a->cm );
	tri::UpdateSelection<CMeshO>::Clear( b->cm );

	//process face once at the time until queue is not empty
	while ( !queue.empty() ) {
		//extract face from the queue
		CMeshO::FacePointer currentF = queue.back().first;  char choose = queue.back().second; queue.pop_back();
		if ( currentF->IsD() || currentF->IsS() ) continue;	//no op if face is deleted or selected (already tested)
		//face from mesh A, test redundancy with respect to B
		if (choose == 'A') {
			if ( checkRedundancy( currentF, b, grid_b, epsilon ) ) {
				//if face is redundant, remove it from A and put new border faces at the top of the queue
				//in order to guarantee that A and B will be tested alternatively
				currentF->SetS();
				//insert adjacent faces at the beginning of the queue 
				queue.insert( queue.begin(), make_pair(currentF->FFp(0),'A') );
				queue.insert( queue.begin(), make_pair(currentF->FFp(1),'A') );
				queue.insert( queue.begin(), make_pair(currentF->FFp(2),'A') );
			}
		}
		//face is from mesh B, test redundancy with respect to A
		else {
			if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
				//if face is redundant, remove it from B and put new border faces at the top of the queue
				//in order to guarantee that A and B will be tested alternatively
				currentF->SetS();
				//insert adjacent faces at the beginning of the queue 
				queue.insert( queue.begin(), make_pair(currentF->FFp(0),'B') );
				queue.insert( queue.begin(), make_pair(currentF->FFp(1),'B') );
				queue.insert( queue.begin(), make_pair(currentF->FFp(2),'B') );
			}
		}
	}
}
	

/* Zippering of two meshes (Turk approach)
 * Given two mesh, a mesh with one or more holes (A) and a second mesh, a patch (B), fill a hole onto m surface
 * using faces of patch. Algorithm const of three steps:
 * - CheckRedundancy: delete face of patch that can be projected completely on A's surface
 * - PatchRefinement: patch vertices are projected on A, and border edges are splitted;
 * - MeshRefinement: faces where patch vertices lie are re-triangulated
 * Based on Zippered Polygon Meshes from Range Images, by G.Turk, M.Levoy, Siggraph 1994
 */
bool FilterZippering::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, CallBackPos */*cb*/)
{

	
    MeshModel* a = par.getMesh("FirstMesh");
    MeshModel* b = par.getMesh("SecondMesh");

    if ( a == b )   {
          errorMessage="Please add a second mesh";
          return false;
      }

	clock_t start, t1, t2, t3;

	//enable FF adjacency, mark, compute normals for face (both on A and B)
	a->updateDataMask(MeshModel::MM_FACEFACETOPO + MeshModel::MM_FACEMARK + MeshModel::MM_FACEQUALITY + MeshModel::MM_FACECOLOR);
	b->updateDataMask(MeshModel::MM_FACEFACETOPO + MeshModel::MM_FACEMARK + MeshModel::MM_FACEQUALITY + MeshModel::MM_FACECOLOR);
	tri::UnMarkAll(a->cm);
	tri::UnMarkAll(b->cm);

	tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);
	tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);
	tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
	tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);
	tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);
	tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
    //epsilon - search threshold
	CMeshO::ScalarType epsilon  = par.getAbsPerc("distance");

	vector< pair<CMeshO::FacePointer,char> > generic_queue; //unsorted queue
	priority_queue< pair<CMeshO::FacePointer,char>, vector< pair<CMeshO::FacePointer,char> >, compareFaceQuality > priority_queue;	//priority queue

	/**
	 * Filter Redundancy: Select redundant faces from the surface of the meshes.
	 */
	if (ID(filter) == FP_REDUNDANCY) {
		//if user chooses to usequality, initialize priority queue... 
		if ( par.getBool("UseQuality") )  {
			if ( !Init_pq( priority_queue, a, b, par.getBool("FullProcessing") ) ) {
				Log("Meshes haven't border faces - Please select Full Processing option");
				return false;
			}
		}
		//...else initialize unsorted queue
		else  {
			if ( !Init_q( generic_queue, a, b, par.getBool("FullProcessing") ) ) {
				Log("Meshes haven't border faces - Please select Full Processing option");
				return false;
			}
		}
		//TODO: visitare la coda, per ogni faccia della coda esaminare il redundancy usando
		//non solo i buchi ma anche le facce selezionate, aggiornare la coda se necessario
		//NESSUNA RIMOZIONE

		CMeshO::ScalarType epsilon  = par.getAbsPerc("distance");

		//if ( par.getBool("FastErosion")  && par.getBool("UseQuality") ) selectRedundantFast_pq();
		//if ( par.getBool("FastErosion")  && !par.getBool("UseQuality") ) selectRedundantFast();
		//if ( !par.getBool("FastErosion") && par.getBool("UseQuality") ) selectRedundant_pq();
		if ( !par.getBool("FastErosion") && !par.getBool("UseQuality") ) 
			selectRedundant( generic_queue, a, b, epsilon );

		return true;
	}
 
	//Search for face on patch border
  vector< tri::Hole<CMeshO>::Info > ccons, ccons_a, ccons_b; //info (will contain info aboud borders of A and B)
  eps = a->cm.bbox.Diag() / 1000000;
  tri::UpdateQuality<CMeshO>::FaceConstant(a->cm,0);
  tri::UpdateQuality<CMeshO>::FaceConstant(b->cm,0);

#ifdef PREERODE
	/* Pre-Erosion step (F.Ganovelli improvement)
	 * Calculate distance from border for both A and B vertices;
	 * Pust A-border faces in a stack S
	 * While (s is not empty)
	 *		pop face f
	 *      calculate Sa as maximum length of edge of f
	 *		search for nearest point on B for each vertex of f and choose the nearest one
	 *		if distance between choosen vertex is greater than Sa and distance of choosen vertex from A-border is greater than
	 *		distance between nearest point and B-border, set f as redundant and remove it from A.
	 *		update distance from border of A-vertices
	 * end while
	 * repeat using B
	 */
		//Distance from border quality
		a->cm.face.EnableVFAdjacency(); a->cm.vert.EnableVFAdjacency(); 
		b->cm.face.EnableVFAdjacency(); b->cm.vert.EnableVFAdjacency(); 
		a->cm.face.EnableQuality();
		b->cm.face.EnableQuality();
    tri::UpdateTopology<CMeshO>::VertexFace(a->cm);
    tri::UpdateFlags<CMeshO>::FaceBorderFromVF(a->cm);
    tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(a->cm);
    tri::UpdateTopology<CMeshO>::VertexFace(b->cm);
    tri::UpdateFlags<CMeshO>::FaceBorderFromVF(b->cm);
    tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(b->cm);
		a->cm.face.DisableVFAdjacency(); 
		b->cm.face.DisableVFAdjacency(); 
		// end -- needed only for pre-erosion step

  tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons_a );
	MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid on A
	MeshFaceGrid grid_b; grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid on B
  tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b ); //bordi di B
  vector< pair<CMeshO::FacePointer,char> > remove_faces; int scr=0;
	//i bordi di A sono in ccons_a
	for ( int i = 0; i < ccons_a.size(); i ++ ) { 
    face::Pos<CMeshO::FaceType> p = ccons_a[i].p;
		do {
      if ( !p.F()->IsD() && simpleCheckRedundancy( p.F(), b, grid_b, epsilon, true ) ) { limit--; scr++; remove_faces.push_back( make_pair(p.F(),'A') );}
			p.NextB();
		} while ( p.F() != ccons_a[i].p.F() );
	}
	while ( !remove_faces.empty() ){ 
			CMeshO::FacePointer currentF = remove_faces.back().first; 
			char choose = remove_faces.back().second;
			remove_faces.pop_back();
			if ( currentF->IsD() ) continue;	//no op
			if ( simpleCheckRedundancy( currentF, b, grid_b, epsilon, true ) ) { scr++; limit--;
          tri::Allocator<CMeshO>::DeleteFace( a->cm, *currentF );
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0); //topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
          remove_faces.push_back( make_pair(currentF->FFp(0),'A') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
          remove_faces.push_back( make_pair(currentF->FFp(1),'A') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
          remove_faces.push_back( make_pair(currentF->FFp(2),'A') );
			}
	}
	/*Update distance from border*/
	a->cm.face.EnableVFAdjacency(); 
  tri::UpdateTopology<CMeshO>::VertexFace(a->cm);
  tri::UpdateFlags<CMeshO>::FaceBorderFromVF(a->cm);
  tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(a->cm);
	a->cm.face.DisableVFAdjacency(); 
  tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	/*end*/

	//repeat for B
	for ( int i = 0; i < ccons_b.size(); i ++ ) { 
    face::Pos<CMeshO::FaceType> p = ccons_b[i].p;
		do {
      if ( !p.F()->IsD() && simpleCheckRedundancy( p.F(), a, grid_a, epsilon, false ) )  { scr++; remove_faces.push_back( make_pair(p.F(),'B') ); }
			p.NextB();
		} while ( p.F() != ccons_b[i].p.F() );
	}
	while(!remove_faces.empty()) {
			CMeshO::FacePointer currentF = remove_faces.back().first; 
			char choose = remove_faces.back().second;
			remove_faces.pop_back();
			if ( currentF->IsD() ) continue;	//no op
			if ( simpleCheckRedundancy( currentF, a, grid_a, epsilon, false ) ) { scr++;
        tri::Allocator<CMeshO>::DeleteFace( b->cm, *currentF );
				currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0);	//topology update
				currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
        remove_faces.push_back( make_pair(currentF->FFp(0),'B') );
				currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
				currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
        remove_faces.push_back( make_pair(currentF->FFp(1),'B') );
				currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
				currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
        remove_faces.push_back( make_pair(currentF->FFp(2),'B') );
			}
	}
  tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
  tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
	Log(GLLogStream::DEBUG, "scr value %d", scr);
	//Calculate new border
	ccons_a.clear(); ccons_b.clear();
  tri::UpdateFlags<CMeshO>::Clear(a->cm);
  tri::UpdateFlags<CMeshO>::Clear(b->cm);
  tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
  tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons_a );
  tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b );
#endif 
#ifndef PREERODE
	MeshFaceGrid grid_a; //grid A
	MeshFaceGrid grid_b; //grid B
  vector< pair<CMeshO::FacePointer,char> > remove_faces; //priority queue
  tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons_a );
  tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b );		 //info about borders
#endif

	/* STEP 1 - Removing Redundant Surfaces
     * Repeat until mesh surface remain unchanged:
     *   - Remove redundant triangles on the boundary of patch
     */
	start = clock();
	grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid on A
	grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid on B
  //bool changed;
  face::Pos<CMeshO::FaceType> p; int c_faces = 0;
  std::priority_queue< std::pair<CMeshO::FacePointer,char>, std::vector< std::pair<CMeshO::FacePointer,char> >, compareFaceQuality > faces_pqueue;	//the queue
  remove_faces.clear();
	
	if (par.getBool("FaceQuality")) {
		Log( "Using Face Quality...");
		//insert in the PQueue faces from A border
    for ( size_t i = 0; i < ccons_a.size(); i ++ ) {
      face::Pos<CMeshO::FaceType> p = ccons_a[i].p;
			do {
				if ( !p.F()->IsD() && checkRedundancy( p.F(), b, grid_b, epsilon ) ) 
          faces_pqueue.push( make_pair(p.F(),'A') );
				p.NextB();
			} while ( p.F() != ccons_a[i].p.F() );
		}
		//insert in the PQueue faces from B border
    for ( size_t i = 0; i < ccons_b.size(); i ++ ) {
      face::Pos<CMeshO::FaceType> p = ccons_b[i].p;
			do {
				if ( !p.F()->IsD() && checkRedundancy( p.F(), a, grid_a, epsilon ) )  
          faces_pqueue.push( make_pair(p.F(),'B') );
				p.NextB();
			} while ( p.F() != ccons_b[i].p.F() );
		}
		//while queue is not empty...
		while ( !faces_pqueue.empty() ) {
			CMeshO::FacePointer currentF = faces_pqueue.top().first; 
			char choose = faces_pqueue.top().second;
			faces_pqueue.pop();
			if ( currentF->IsD() ) continue;	//no op, face is already deleted
			if (choose == 'A') {
				if ( checkRedundancy( currentF, b, grid_b, epsilon ) ) {
					//face has to be removed, so we delete and update topology
          tri::Allocator<CMeshO>::DeleteFace( a->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0); //topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
          faces_pqueue.push( make_pair(currentF->FFp(0),'A') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
          faces_pqueue.push( make_pair(currentF->FFp(1),'A') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
          faces_pqueue.push( make_pair(currentF->FFp(2),'A') );
				}
			}
			else {
				if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
					//face is redundant, so we delete and update topology
          tri::Allocator<CMeshO>::DeleteFace( b->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0);	//topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
          faces_pqueue.push( make_pair(currentF->FFp(0),'B') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
          faces_pqueue.push( make_pair(currentF->FFp(1),'B') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
          faces_pqueue.push( make_pair(currentF->FFp(2),'B') );
				}
			}
		}
	} 
	else {	//do not use face quality
		Log( "Using Standard predicate...");	
    for ( size_t i = 0; i < ccons_a.size(); i ++ ) {
      face::Pos<CMeshO::FaceType> p = ccons_a[i].p;
			if ( p.F()->IsD() ) continue;
			do {
        if ( !p.F()->IsD() && checkRedundancy( p.F(), b, grid_b, epsilon ) ) remove_faces.push_back( make_pair(p.F(),'A') );
				p.NextB();
			} while ( p.F() != ccons_a[i].p.F() );
		}

    for ( size_t i = 0; i < ccons_b.size(); i ++ ) {
      face::Pos<CMeshO::FaceType> p = ccons_b[i].p;
			if ( p.F()->IsD() ) continue;
			do {
        if ( !p.F()->IsD() && checkRedundancy( p.F(), a, grid_a, epsilon ) )  remove_faces.push_back( make_pair(p.F(),'B') );
				p.NextB();
			} while ( p.F() != ccons_b[i].p.F() );
		}
	
		while ( !remove_faces.empty() ) {
			CMeshO::FacePointer currentF = remove_faces.back().first; 
			char choose = remove_faces.back().second;
			remove_faces.pop_back();
			if ( currentF->IsD() ) continue;	//no op
			if (choose == 'A') {
				if ( checkRedundancy( currentF, b, grid_b, epsilon ) ) {
					//if face is redundant, remove it from A and put new border faces at the top of the queue
					//in order to guarantee that A and B will be tested alternatively
          tri::Allocator<CMeshO>::DeleteFace( a->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0); //topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					//insert as first element
          remove_faces.insert( remove_faces.begin(), make_pair(currentF->FFp(0),'A') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					//insert as first element
          remove_faces.insert( remove_faces.begin(), make_pair(currentF->FFp(1),'A') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					//insert as first element
          remove_faces.insert( remove_faces.begin(), make_pair(currentF->FFp(2),'A') );
				}
			}
			else {
				if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
					//if face is redundant, remove it from B and put new border faces at the top of the queue
					//in order to guarantee that A and B will be tested alternatively
          tri::Allocator<CMeshO>::DeleteFace( b->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0);	//topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					//insert as first element
          remove_faces.insert( remove_faces.begin(), make_pair(currentF->FFp(0),'B') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					//insert as first element
          remove_faces.insert( remove_faces.begin(), make_pair(currentF->FFp(1),'B') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					//insert as first element
          remove_faces.insert( remove_faces.begin(), make_pair(currentF->FFp(2),'B') );
				}
			}
		}
	}//end else

	//update topology 
    tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
  tri::UpdateTopology<CMeshO>::FaceFace(b->cm);

	//Redundancy Only selected -> exit
	if ( par.getBool("RedundancyOnly") ) {
		Log(GLLogStream::DEBUG, "Removed %d redundant faces", c_faces);
		//post-processing
    tri::UpdateBounding<CMeshO>::Box( a->cm );
    tri::Clean<CMeshO>::RemoveUnreferencedVertex( a->cm );
    tri::Clean<CMeshO>::RemoveDuplicateVertex( a->cm );
    tri::Clean<CMeshO>::RemoveUnreferencedVertex( b->cm );
    tri::Clean<CMeshO>::RemoveDuplicateVertex( b->cm );
		a->cm.face.DisableColor();
		b->cm.face.DisableColor();
		return true;
	}
	t1 = clock();
	/*End step 1*/

	/*****************
	 * Border Refinement
	 * In order to guarantee that a face has only one border edge, search for faces with two border edges
	 * and store their indices in a vector.
	 */
	//clear flags and search for new border faces
  tri::UpdateFlags<CMeshO>::FaceClear(a->cm);
  tri::UpdateFlags<CMeshO>::FaceClear(b->cm);

	ccons_b.clear();											//clear info
  tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b );   //retrieve info about border of B
  vector<pair<int,int> > b_pos;						//pos container
	
	//store pos information for each border of B
  for ( size_t i = 0; i < ccons_b.size(); i ++ )
    b_pos.push_back( make_pair( tri::Index(b->cm, ccons_b[i].p.F()), ccons_b[i].p.E() ) );
	
	//use previously stored information in order to explore the whole border
  for ( size_t i = 0; i < b_pos.size(); i ++ ) {
    face::Pos<CMeshO::FaceType> p;
		p.Set( &(b->cm.face[b_pos[i].first]), b_pos[i].second, b->cm.face[b_pos[i].first].V(b_pos[i].second) );
		CMeshO::FacePointer start = p.F(); 
		//spurious face, we remove it
    if ( face::BorderCount(*start) == 3 ) { tri::Allocator<CMeshO>::DeleteFace( b->cm, *start ); continue; }
		do {
			//we found a face previously not deleted and with 2 (or more) edge border
      if ( !p.F()->IsD() && face::BorderCount(*p.F()) >= 2 ) {
				//split face F into 4 faces, using new vertex v
        tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
        tri::Allocator<CMeshO>::PointerUpdater<CMeshO::FacePointer> fpu;
        CMeshO::FaceIterator f = tri::Allocator<CMeshO>::AddFaces( b->cm, 4, fpu );
				if ( fpu.NeedUpdate() ) { 
					fpu.Update( p.F() ); 
					fpu.Update( start ); 
				}
        CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( b->cm, 1, vpu );
				if ( vpu.NeedUpdate() ) 
					vpu.Update( p.V() );
        int j; for (j=0; j<3 && face::IsBorder(*p.F(), j); j++); assert( j < 3 ); //split non border edge
				CMeshO::FacePointer opp_f = p.F()->FFp(j); int opp_j = p.F()->FFi(j);
				(*v).P() = (p.F()->P(j) + p.F()->P1(j))/2.0;
				//create new faces
				CMeshO::FacePointer f1 = &*f; f1->V(0) = p.F()->V(j);		f1->V(1) = &(*v);				f1->V(2) = p.F()->V2(j); ++f;
				CMeshO::FacePointer f2 = &*f; f2->V(0) = p.F()->V2(j);		f2->V(1) = &(*v);				f2->V(2) = p.F()->V1(j); ++f;
				CMeshO::FacePointer f3 = &*f; f3->V(0) = p.F()->V(j);		f3->V(1) = opp_f->V2(opp_j);	f3->V(2) = &(*v); ++f;
				CMeshO::FacePointer f4 = &*f; f4->V(0) = opp_f->V2(opp_j);	f4->V(1) = p.F()->V1(j);		f4->V(2) = &(*v); 
				//update topology manually
				f1->FFp(0) = f3; f1->FFp(1) = f2; f1->FFp(2) = f1; f1->FFi(0) = 2; f1->FFi(1) = 0; f1->FFi(2) = 2;
				f2->FFp(0) = f1; f2->FFp(1) = f4; f2->FFp(2) = f2; f2->FFi(0) = 1; f2->FFi(1) = 1; f2->FFi(2) = 2;
        f3->FFp(0) = face::IsBorder(*opp_f, (opp_j+1)%3)? f3 : opp_f->FFp((opp_j+1)%3); f3->FFp(1) = f4; f3->FFp(2) = f1; f3->FFi(0) = face::IsBorder(*opp_f, (opp_j+1)%3)? 0 : opp_f->FFi((opp_j+1)%3); f3->FFi(1) = 2; f3->FFi(2) = 0;
        f4->FFp(0) = face::IsBorder(*opp_f, (opp_j+2)%3)? f4 : opp_f->FFp((opp_j+2)%3); f4->FFp(1) = f2; f4->FFp(2) = f3; f4->FFi(0) = face::IsBorder(*opp_f, (opp_j+2)%3)? 0 : opp_f->FFi((opp_j+2)%3); f4->FFi(1) = 1; f4->FFi(2) = 1;
        if( !face::IsBorder(*f3, 0) ) {
					f3->FFp(0)->FFp(f3->FFi(0)) = f3; f3->FFp(0)->FFi(f3->FFi(0)) = 0; 
				}	//border
        if( !face::IsBorder(*f4, 0) ) {
					f4->FFp(0)->FFp(f4->FFi(0)) = f4; f4->FFp(0)->FFi(f4->FFi(0)) = 0; 
				}	//border
        tri::Allocator<CMeshO>::DeleteFace( b->cm, *(p.F()->FFp(j)) );
        tri::Allocator<CMeshO>::DeleteFace( b->cm, *p.F() );
				if ( p.F() == start ) start = f1;
        if ( (p.F()->FFp(j)) == start && face::IsBorder(*f3,0) ) start = f3;
        if ( (p.F()->FFp(j)) == start && face::IsBorder(*f4,0) ) start = f4;
        if ( face::BorderCount(*(p.F())) == 2 && face::BorderCount(*(p.F()->FFp(j))) == 2 ) break;//square
				p.Set( f1, 2, p.V() );		//we deleted two faces, so we set a new pos in order to avoid errors
			}
			p.NextB();
		} while( p.F() != start );
	}

	//before append, remove deleted faces from mesh A 
  tri::Allocator<CMeshO>::CompactFaceVector( a->cm );
  tri::Clean<CMeshO>::RemoveUnreferencedVertex( a->cm );
  tri::Clean<CMeshO>::RemoveDuplicateVertex( a->cm );
  tri::Allocator<CMeshO>::CompactVertexVector( a->cm );
  size_t limit = a->cm.face.size();	//save number of faces of a
	//before append, remove deleted faces from mesh B 
  tri::Allocator<CMeshO>::CompactFaceVector( b->cm );
  tri::Clean<CMeshO>::RemoveUnreferencedVertex( b->cm );
  tri::Clean<CMeshO>::RemoveDuplicateVertex( b->cm );
  tri::Allocator<CMeshO>::CompactVertexVector( b->cm );
	//append B to A
  tri::Append<CMeshO, CMeshO>::Mesh( a->cm, b->cm );
  tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
  tri::UpdateFlags<CMeshO>::FaceClear(a->cm);
    /* End Border Refinement */
	
	/* Pre-Refinement steps
	 * Search information about borders on the new mesh.
	 */
	remove_faces.clear(); ccons.clear();
    tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
  tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons );
  tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
    grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //compute grid on the original part of A
	
    /* STEP 2.1 - REFINEMENT
	 * Project patch points on mesh surface
     * and ricorsively subdivide face in smaller triangles until each patch's face has border vertices
     * lying in adiacent or coincident faces. Also collect informations for triangulation of mesh faces.
     */
    //Add optional attribute
    CMeshO::PerFaceAttributeHandle<bool> visited = tri::Allocator<CMeshO>::AddPerFaceAttribute<bool> (a->cm); //check for already visited face
  map<CMeshO::FacePointer, aux_info> map_info;
	aux_info dummy;			//dummy info
  pair<map<CMeshO::FacePointer, aux_info>::iterator,bool> ret_val; //return value of insert
  for ( size_t i = 0; i < a->cm.face.size(); i ++) visited[i] = false;	//no face previously visited
    vector< CMeshO::FacePointer > tbt_faces;   //To Be Triangulated
    vector< CMeshO::FacePointer > tbr_faces;   //To Be Removed
    vector< int > verts;
  for ( size_t c = 0; c < ccons.size(); c ++ ) {
    if ( tri::Index( a->cm, ccons[c].p.F() ) < limit ) continue;
        if ( visited[ccons[c].p.F()] || ccons[c].p.F()->IsD() ) continue; 
    face::Pos<CMeshO::FaceType> p = ccons[c].p; p.FlipV();//CCW order
		do {

			if ( visited[p.F()] || p.F()->IsD()) { p.NextB(); continue; }  //Already deleted or visited, step over
			visited[p.F()] = true;
			CMeshO::FacePointer nearestF = 0, nearestF1 = 0;
      assert( face::BorderCount( *p.F() ) > 0 ); assert( face::IsBorder( *p.F(), p.E() ) );  //Check border correctness
            tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
            face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
            MeshFaceGrid::ScalarType  dist = 2*epsilon;  MeshFaceGrid::CoordType closest, closest1;
            nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), epsilon, dist, closest);
			if ( fabs(dist) >= fabs(epsilon) ) nearestF = 0;
            dist = 2*epsilon;
            nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor,  p.F()->P1(p.E()), epsilon, dist, closest1);
			if ( fabs(dist) >= fabs(epsilon) ) nearestF1 = 0;
            int cnt = 0;    //counter (inf. loop)
      vector < pair< int, int > > stack;
            vector < pair< CMeshO::FacePointer, CMeshO::FacePointer > > stack_faces;
      int end_v = tri::Index(a->cm, p.F()->V2(p.E()));
            // Not-adjacent; recursive split using mid-point of edge border
            stack.push_back( make_pair( tri::Index( a->cm, p.F()->V(p.E()) ),
                                             tri::Index( a->cm, p.F()->V1(p.E()) ) ) );    //Edge border
            assert( face::IsBorder( *p.F(), p.E() ) );
            stack_faces.push_back( make_pair(nearestF, nearestF1) );   //Nearest Faces

			CMeshO::FacePointer actualF = p.F(); int actualE = p.E(); p.NextB();
			
            while ( !stack.empty() ) {
				if ( cnt++ > 2*MAX_LOOP ) {
					Log(GLLogStream::DEBUG, "Loop"); stack.clear(); continue;
				} //go to next face in case of inf. loop
				
        pair< int, int > border_edge = stack.back(); stack.pop_back();   //vertex indices
                CMeshO::FacePointer start = stack_faces.back().first; CMeshO::FacePointer end = stack_faces.back().second; //nearestF
                stack_faces.pop_back();

				if ( start == 0 && end == 0 ) continue;

        Point3f closestStart, closestEnd; float distStart, distEnd;
				grid_a.GetClosest(PDistFunct, markerFunctor, a->cm.vert[border_edge.first].P(), 2*epsilon, distStart, closestStart); //closest point on mesh
				grid_a.GetClosest(PDistFunct, markerFunctor, a->cm.vert[border_edge.second].P(), 2*epsilon, distEnd, closestEnd); //closest point on mesh
				
				if (start != 0) { 
          map_info.insert( make_pair(start, dummy) );
					map_info[start].SetEps( eps ); 
          map_info[start].Init( *start, tri::Index( a->cm, start->V(0) ), tri::Index( a->cm, start->V(1) ), tri::Index( a->cm, start->V(2) )  );
				} //init
				if (end != 0)	{
          map_info.insert( make_pair(end, dummy) );
					map_info[end].SetEps( eps );
          map_info[end].Init( *end, tri::Index( a->cm, end->V(0) ), tri::Index( a->cm, end->V(1) ), tri::Index( a->cm, end->V(2) ) );
				}
                        
				if ( isOnBorder( closestStart, start ) && isOnBorder( closestEnd, end ) ) {
					//if closest point is really closest to the border, split border face
					if ( distStart <= eps ) {
            map_info.insert( make_pair(start, dummy) );
						map_info[start].addVertex( &(a->cm.vert[border_edge.first]), border_edge.first ); 
					}
					if ( distEnd <= eps )   {
            map_info.insert( make_pair(end, dummy) );
						map_info[end].addVertex( &(a->cm.vert[border_edge.second]), border_edge.second );
					}
					//Verify if the whole segment is on border
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1); bool border = true;
                    Point3f closestP;   float dist = 2*epsilon;
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        Point3f currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
						CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*epsilon, dist, closestP); //closest point on mesh
						if ( !isOnBorder( closestP, closestFace ) ) {	//not completely on border: split it!
							border = false;
              tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
              CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
							(*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
							if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
              CMeshO::FacePointer currentF = 0; Point3f closest;
							currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
              stack.push_back( make_pair( border_edge.first, v - a->cm.vert.begin() ) );
              stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
              stack.push_back( make_pair( v - a->cm.vert.begin(), border_edge.second ) );
              stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
							break;
						}
					}

					if ( border ) {
						//actualFace: no operation needed
						if ( a->cm.vert[border_edge.first].P() == actualF->P(actualE) &&
							 a->cm.vert[border_edge.second].P() == actualF->P1(actualE) ) ;
						else {//create new triangle to file small hole
							verts.push_back(border_edge.second); verts.push_back( end_v ); verts.push_back( border_edge.first );   //new triangle
						}
					}
					continue;
				}

				if ( isOnBorder( closestStart, start ) && end == 0 ) {
					//if closest point is really closest to the border, split border face
					if ( distStart <= eps ) {
            map_info.insert( make_pair( start, dummy ) );
						map_info[start].addVertex( &(a->cm.vert[border_edge.first]), border_edge.first ); 
					}
					//Verify if the whole segment is on border
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1); bool border = true;
                    Point3f closestP;   float dist = 2*epsilon;
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        Point3f currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
						CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*epsilon, dist, closestP); //closest point on mesh
						if ( !isOnBorder( closestP, closestFace ) ) {	//not completely on border: split it!
							border = false;
              tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
              CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
							(*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
							if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
              CMeshO::FacePointer currentF = 0; Point3f closest;
							currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
              stack.push_back( make_pair( border_edge.first, v - a->cm.vert.begin() ) );
              stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
              stack.push_back( make_pair( v - a->cm.vert.begin(), border_edge.second ) );
              stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
							break;
						}
					}

					if ( border ) {
						//actualFace: no operation needed
						if ( a->cm.vert[border_edge.first].P() == actualF->P(actualE) &&
							 a->cm.vert[border_edge.second].P() == actualF->P1(actualE) ) ;
						else {//create new triangle to file small hole
							verts.push_back(border_edge.second); verts.push_back( end_v ); verts.push_back( border_edge.first );   //new triangle
						}
					}
					continue;
				}

				if ( isOnBorder( closestEnd, end ) && start == 0 ) {
					//if closest point is really closest to the border, split border face
					if ( distEnd <= eps )   {
            map_info.insert( make_pair(end, dummy) );
						map_info[end].addVertex( &(a->cm.vert[border_edge.second]), border_edge.second );
					}
					//Verify if the whole segment is on border
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1); bool border = true;
                    Point3f closestP;   float dist = 2*epsilon;
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        Point3f currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
						CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*epsilon, dist, closestP); //closest point on mesh
						if ( !isOnBorder( closestP, closestFace ) ) {	//not completely on border: split it!
							border = false;
              tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
              CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
							(*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
							if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
              CMeshO::FacePointer currentF = 0; Point3f closest;
							currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
              stack.push_back( make_pair( border_edge.first, v - a->cm.vert.begin() ) );
              stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
              stack.push_back( make_pair( v - a->cm.vert.begin(), border_edge.second ) );
              stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
							break;
						}
					}

					if ( border ) {
						//actualFace: no operation needed
						if ( a->cm.vert[border_edge.first].P() == actualF->P(actualE) &&
							 a->cm.vert[border_edge.second].P() == actualF->P1(actualE) ) ;
						else {//create new triangle to file small hole
							verts.push_back(border_edge.second); verts.push_back( end_v ); verts.push_back( border_edge.first );   //new triangle
						}
					}
					continue;
				}
				
				if ( start == 0 || isOnBorder( closestStart, start ) ) {
					//if closest point is really closest to the border, split border face
					if ( distStart <= eps ) {
            map_info.insert( make_pair(start, dummy) );
						map_info[start].addVertex( &(a->cm.vert[border_edge.first]), border_edge.first );
					}
					a->cm.vert[border_edge.second].P() = closestEnd;
					tbr_faces.push_back( actualF );
					int last_split = -1; CMeshO::FacePointer currentF = end; bool stop = false; 
					CMeshO::VertexPointer startV = &a->cm.vert[border_edge.second];
					CMeshO::VertexPointer endV = &a->cm.vert[border_edge.first];
					CMeshO::VertexPointer thirdV = actualF->V2(actualE); int cnt = 0;
					do {
            int tosplit; Point3f closest;  cnt++;
            if (!findIntersection( currentF, Segment3<CMeshO::ScalarType>(a->cm.vert[border_edge.first].P(),a->cm.vert[border_edge.second].P()), last_split, tosplit, closest )) {
							stop = true; //no op
						}
						else {  
							last_split = currentF->FFi( tosplit );
              tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
              CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
							if ( vpu.NeedUpdate() )  { vpu.Update( p.V() ); vpu.Update(startV); vpu.Update(endV); vpu.Update(thirdV); }
              map_info.insert( make_pair(currentF, dummy) );
              map_info[currentF].Init( *currentF, tri::Index(a->cm, currentF->V(0)), tri::Index(a->cm, currentF->V(1)), tri::Index(a->cm, currentF->V(2)) );
							startV->P(); (*v).P();
              if ( map_info[currentF].AddToBorder( Segment3<CMeshO::ScalarType> ( (*v).P(), startV->P() ),
                               make_pair( v - a->cm.vert.begin(), tri::Index(a->cm, startV ) ) ) ) {
									tbt_faces.push_back( currentF ); 
                  verts.push_back( tri::Index(a->cm, startV) ); verts.push_back( tri::Index(a->cm, thirdV) ); verts.push_back( v - a->cm.vert.begin() );
									
							}
							startV = &(*v);
              if ( face::IsBorder( *currentF, tosplit ) )  { //border reached
                verts.push_back( tri::Index( a->cm, thirdV ) ); verts.push_back( tri::Index( a->cm, endV ) ); verts.push_back( v - a->cm.vert.begin() );
                stack.push_back( make_pair( tri::Index(a->cm, endV), v - a->cm.vert.begin() ) ); stack_faces.push_back( make_pair(start, currentF) );
								stop = true;
							}
							else currentF = currentF->FFp(tosplit);
						}
					} while (!stop && cnt < MAX_LOOP);
					continue;
				}

				if ( end == 0 || isOnBorder( closestEnd, end ) ) {
					//if closest point is really closest to the border, split border face
					if ( distEnd <= eps )   {
            map_info.insert( make_pair(end, dummy) );
						map_info[end].addVertex( &(a->cm.vert[border_edge.second]), border_edge.second );
					}
					a->cm.vert[border_edge.first].P() = closestStart;
					tbr_faces.push_back( actualF );
					int last_split = -1; CMeshO::FacePointer currentF = start; bool stop = false; 
					CMeshO::VertexPointer startV = &a->cm.vert[border_edge.first];
					CMeshO::VertexPointer endV = &a->cm.vert[border_edge.second];
					CMeshO::VertexPointer thirdV = actualF->V2(actualE); int cnt = 0;
					do {
            int tosplit; Point3f closest; cnt++;
            if (!findIntersection( currentF, Segment3<CMeshO::ScalarType>(a->cm.vert[border_edge.first].P(),a->cm.vert[border_edge.second].P()), last_split, tosplit, closest )) {
							break;
						}
						last_split = currentF->FFi( tosplit );
            tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
            CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
						if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
            map_info.insert( make_pair(currentF, dummy) );
						map_info[currentF].SetEps( eps ); 
            map_info[currentF].Init( *currentF, tri::Index(a->cm, currentF->V(0)), tri::Index(a->cm, currentF->V(1)), tri::Index(a->cm, currentF->V(2)) );
            if ( map_info[currentF].AddToBorder( Segment3<CMeshO::ScalarType> ( startV->P(), (*v).P() ),
                             make_pair( tri::Index(a->cm, startV ), v - a->cm.vert.begin() ) ) ) {
                tbt_faces.push_back( currentF ); verts.push_back( tri::Index(a->cm, thirdV) ); verts.push_back( tri::Index(a->cm, startV) ); verts.push_back( v - a->cm.vert.begin() );
							}
						startV = &(*v);
            if ( face::IsBorder( *currentF, tosplit ) )  { //last triangle
              //verts.push_back( tri::Index( a->cm, endV ) ); verts.push_back( tri::Index( a->cm, thirdV ) ); verts.push_back( v - a->cm.vert.begin() );
              stack.push_back( make_pair( v - a->cm.vert.begin(), tri::Index(a->cm, endV) ) );
              stack_faces.push_back( make_pair(currentF, end) );
							stop = true;
						}
						else currentF = currentF->FFp(tosplit);
					} while (!stop && cnt < MAX_LOOP);
					continue;
				}

				a->cm.vert[border_edge.first].P() = closestStart;
				a->cm.vert[border_edge.second].P() = closestEnd;

                if ( start == end ) {
          map_info.insert( make_pair(start, dummy) );
                    map_info[start].SetEps( eps ); 
          map_info[start].Init( *start, tri::Index( a->cm, start->V(0) ), tri::Index( a->cm, start->V(1) ), tri::Index( a->cm, start->V(2) )  );
                    if ( map_info[start].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                  make_pair( border_edge.first, border_edge.second ) ) ) {
                        tbt_faces.push_back( start ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                    }
                    continue;
                }

				tbr_faces.push_back( actualF );
				if ( isAdjacent( start, end ) ) {
                    //calc. intersection point (approximate) and split edge
                    int shared; for ( int k = 0; k < 3; k ++ ) if ( start->FFp(k) == end ) shared = k;
                    Segment3<CMeshO::ScalarType> shared_edge( start->P(shared), start->P1(shared) );
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1);
                    Point3f closest;    float min_dist = shared_edge.Length();
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        Point3f currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
                        if ( SquaredDistance( shared_edge, currentP ) < min_dist ) {
                            closest = currentP; min_dist = SquaredDistance( shared_edge, closest );
                        }
                    }
                    assert( SquaredDistance( shared_edge, closest ) < shared_edge.Length() );
                    closest = ClosestPoint(shared_edge, closest); //intersection point
                    //merge close vertices
                    if ( Distance<float>( closest, a->cm.vert[border_edge.first].P() ) < eps ) {
                        a->cm.vert[border_edge.first].P() = closest;
            map_info.insert( make_pair( end, dummy ) );
                        map_info[end].SetEps( eps ); 
            map_info[end].Init( *end, tri::Index( a->cm, end->V(0) ), tri::Index( a->cm, end->V(1) ), tri::Index( a->cm, end->V(2) ) );
                        if ( map_info[end].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                    make_pair( border_edge.first, border_edge.second ) ) ) {
                            tbt_faces.push_back( end ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                        }
                        continue;
                    }
                    //merge close vertices
                    if ( Distance<float>( closest, a->cm.vert[border_edge.second].P() ) < eps ) {
                        a->cm.vert[border_edge.second].P() = closest;
            map_info.insert( make_pair( start, dummy ) );
                        map_info[start].SetEps( eps ); 
            map_info[start].Init( *start, tri::Index( a->cm, start->V(0) ), tri::Index( a->cm, start->V(1) ), tri::Index( a->cm, start->V(2) ) );
                        if ( map_info[start].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                      make_pair( border_edge.first, border_edge.second ) ) ) {
                            tbt_faces.push_back( start ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                        }
                        continue;
                    }
                    //no close vertices, add information to faces
                    tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                    CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
                    if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
          map_info.insert( make_pair( start, dummy ) );
					map_info[start].SetEps( eps );  
          map_info[start].Init( *start, tri::Index( a->cm, start->V(0) ), tri::Index( a->cm, start->V(1) ), tri::Index( a->cm, start->V(2) ) );
                    map_info.insert( make_pair( end, dummy ) );
					map_info[end].SetEps( eps );    
          map_info[end].Init( *end, tri::Index( a->cm, end->V(0) ), tri::Index( a->cm, end->V(1) ), tri::Index( a->cm, end->V(2) ) );
                    if ( map_info[start].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), (*v).P() ),
                                                  make_pair( border_edge.first, v - a->cm.vert.begin() ) ) ) {
                        tbt_faces.push_back( start ); verts.push_back(v - a->cm.vert.begin()); verts.push_back( end_v ); verts.push_back( border_edge.first );   //new triangle
                    }
                    if ( map_info[end].AddToBorder( Segment3<CMeshO::ScalarType> ( (*v).P(), a->cm.vert[border_edge.second].P() ),
                                                make_pair( v - a->cm.vert.begin(), border_edge.second ) ) ) {
                        tbt_faces.push_back( end ); verts.push_back(v - a->cm.vert.begin()); verts.push_back( border_edge.second ); verts.push_back( end_v );    //new triangle
                    }
                    continue;
                }
				//check if start and end share a vertex
                int w = sharesVertex( start, end ); Segment3<CMeshO::ScalarType> s( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() );
                if ( w != -1 && (cnt == MAX_LOOP || SquaredDistance<float>( s, start->P(w) ) <= eps) ) {
          tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                    CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = start->P(w);
					if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
          map_info.insert( make_pair( start, dummy ) );
                    map_info[start].SetEps( eps ); 
          map_info[start].Init( *start, tri::Index( a->cm, start->V(0) ), tri::Index( a->cm, start->V(1) ), tri::Index( a->cm, start->V(2) )  );
                    map_info.insert( make_pair( end, dummy ) );
					map_info[end].SetEps( eps ); 
          map_info[end].Init( *end, tri::Index( a->cm, end->V(0) ), tri::Index( a->cm, end->V(1) ), tri::Index( a->cm, end->V(2) )  );
                    if ( map_info[start].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), (*v).P() ),
                                                  make_pair( border_edge.first, v - a->cm.vert.begin() ) ) ) {
                        tbt_faces.push_back(start); verts.push_back( border_edge.first ); verts.push_back( v - a->cm.vert.begin() ); verts.push_back( end_v );
                    }
                    if ( map_info[end].AddToBorder( Segment3<CMeshO::ScalarType> ( (*v).P(),  a->cm.vert[border_edge.second].P() ),
                                                make_pair( v - a->cm.vert.begin(), border_edge.second ) ) ) {
                        tbt_faces.push_back(end); verts.push_back( v - a->cm.vert.begin() ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                    }
                    continue;
                }
        tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
                (*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
                if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
                CMeshO::FacePointer currentF = 0; Point3f closest;
                currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
                stack.push_back( make_pair( border_edge.first, v - a->cm.vert.begin() ) );
        stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
                stack.push_back( make_pair( v - a->cm.vert.begin(), border_edge.second ) );
        stack_faces.push_back( pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
				assert( stack.size() == stack_faces.size() );
			}
        } while ( p.F() != ccons[c].p.F() );
    }//end for

    for ( size_t i = 0; i < tbr_faces.size(); i++) {
        if (!tbr_faces[i]->IsD()) tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbr_faces[i]);
    }
    tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	t2 = clock();
    /* End of Step 2.1 */


    /* STEP 2.2 - RETRIANGULATION
     * Faces are sorted by index, than each face is triangulated using auxiliar information about
     * vertices and edges
     */
  sort( tbt_faces.begin(), tbt_faces.end() ); vector<CMeshO::FacePointer>::iterator itr = unique( tbt_faces.begin(), tbt_faces.end() ); tbt_faces.resize(itr - tbt_faces.begin() );
    vector< Point3f > coords; size_t patch_verts = verts.size();
  for ( size_t i = 0; i < tbt_faces.size(); i ++ ) {
		if ( !tbt_faces[i]->IsD() ) {
            handleBorder( map_info[tbt_faces[i]], tbt_faces[i]->N(), coords, verts );
            tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbt_faces[i]);
        }
    }
	//remove user-defined attribute
  tri::Allocator<CMeshO>::DeletePerFaceAttribute<bool>(a->cm, visited);

	// Add new faces
    int n_faces = a->cm.face.size(); int t_faces = 0;
  CMeshO::FaceIterator fn = tri::Allocator<CMeshO>::AddFaces( a->cm, verts.size()/3 );
    for ( size_t k = 0; k < verts.size(); k += 3 ) {
		CMeshO::VertexPointer v0 = &(a->cm.vert[verts[k]]);
		CMeshO::VertexPointer v1 = &(a->cm.vert[verts[k+1]]);
		CMeshO::VertexPointer v2 = &(a->cm.vert[verts[k+2]]);
		//avoid null faces; could originate holes - is that really necessary?
    if ( Angle<float>( v1->P() - v0->P(), v2->P() - v0->P() ) == 0.0f ) continue;
    if ( Angle<float>( v0->P() - v1->P(), v2->P() - v1->P() ) == 0.0f ) continue;
    if ( Angle<float>( v0->P() - v2->P(), v1->P() - v2->P() ) == 0.0f ) continue;
		if ( k < patch_verts ) {
            (*fn).V(0) = v0; (*fn).V(1) = v1; (*fn).V(2) = v2; (*fn).N() = ( (*fn).P(0) - (*fn).P(2) )^( (*fn).P(1)-(*fn).P(2) );
        }
        else {
            (*fn).V(0) = v0; (*fn).V(2) = v1; (*fn).V(1) = v2; (*fn).N() = ( (*fn).P(0) - (*fn).P(2) )^( (*fn).P(1)-(*fn).P(2) );
        }
    //(*fn).V(0)->C() = Color4b::DarkGray; (*fn).V(1)->C() = Color4b::DarkGray; (*fn).V(2)->C() = Color4b::DarkGray;
    (*fn).C() = Color4b::Magenta;
		(*fn).SetS();
		n_faces++; t_faces++; ++fn;
    }
	t3 = clock();	
	//post-processing refinement
  tri::Clean<CMeshO>::RemoveDuplicateVertex( a->cm );
  tri::Clean<CMeshO>::RemoveDuplicateFace( a->cm );
	//compact vectors and update topology and bbox
  tri::Allocator<CMeshO>::CompactFaceVector( a->cm );
  tri::Allocator<CMeshO>::CompactVertexVector( a->cm );
  tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
  tri::UpdateBounding<CMeshO>::Box( a->cm );
	//disabling colors
    a->cm.face.DisableColor();
	b->cm.face.DisableColor();
	/*End of step 2.2 */
	/*Select faces in order to perform future remeshing operations */
  tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(a->cm);
  tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(a->cm);

	Log(GLLogStream::DEBUG, "**********************" );
    Log( "End - Remove %d faces from patch - Created %d new faces - Timing T1 %d T2 %d T3 %d", c_faces, t_faces, t1-start, t2-t1, t3-t2);

    return true;
}

Q_EXPORT_PLUGIN(FilterZippering)
