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
        typeList << FP_ZIPPERING;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
 QString FilterZippering::filterName(FilterIDType filterId) const
{
  switch(filterId) {
                case FP_ZIPPERING :  return QString("Zippering");
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterZippering::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
                case FP_ZIPPERING :  return QString("Merge two meshes, a mesh with holes and a mesh-patch, into a single one; hole is filled using patch's triangles. Based on <b>Zippered Polygon Meshes from Range Images,</b> by G.Turk, M.Levoy, Siggraph 1994");
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

 int FilterZippering::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_ZIPPERING:  return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTCOLOR;
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
                case FP_ZIPPERING :  return MeshFilterInterface::Remeshing;
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
                case FP_ZIPPERING :
								//Improvements needed
                                foreach (target, md.meshList) {
                                    if ( target->cm.bbox.Diag() > maxVal ) maxVal = target->cm.bbox.Diag();
                                    if (target != md.mm())  break;
                                }
                                parlst.addParam( new RichMesh("FirstMesh", md.mm(), &md, "Mesh (with holes)", "The mesh with holes.") );
                                parlst.addParam( new RichMesh("SecondMesh", target, &md, "Patch", "The mesh that will be used as patch.") );
                                parlst.addParam( new RichAbsPerc("distance", maxVal*0.01, 0, maxVal, "Max distance", "Max distance between mesh and path") );
								parlst.addParam( new RichBool("FaceQuality", false, "Use face quality to select redundant face", "If selected, previously computed face quality will be used in order to select redundant faces.") );
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
    int i; for (i=0; i<3 && !vcg::face::IsBorder(*face, i); i++) {}   //i-edge on patch border
    if (i == 3)  for (i = 0; i < 3 && !(face->FFp(i)->IsD()); i++) {}
    assert( i<3 );
	int samplePerEdge = SAMPLES_PER_EDGE;

    //samples edge in uniform way
    std::vector< vcg::Point3< CMeshO::ScalarType > > edge_samples;
    vcg::Point3< CMeshO::ScalarType > edge_dir = face->P1(i) - face->P(i); edge_dir.Normalize();
    float step = 1.0/(samplePerEdge+1); //step length
    for( int j = 0; j <= samplePerEdge; j++ ) {
        edge_samples.push_back( face->P(i) + edge_dir * (j * step) );
    }

    for ( unsigned int j = 0; j < edge_samples.size(); j ++ ) {
        CMeshO::FacePointer nearestF = 0;
        vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); tri::UnMarkAll(m->cm);
        vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
        MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
        //Search closest point on A
        nearestF =  grid.GetClosest(PDistFunct, markerFunctor, edge_samples[j], max_dist, dist, closest);

        if ( nearestF == 0 )                    return false;   //no face within given range
        if ( isOnBorder(closest, nearestF ) )   return false;   //closest point on border
		if ( nearestF->IsD() )					return false;
    }

    //check if V2(i) has a closest point on border of m
    CMeshO::FacePointer nearestF = 0;
    vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm);
    vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
    MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
    nearestF =  grid.GetClosest(PDistFunct, markerFunctor, face->P2(i), max_dist, dist, closest);
    if ( nearestF == 0 )					return false;    //no face within given range
    if ( isOnBorder( closest, nearestF ) )	return false;    //closest point on border
	if ( nearestF->IsD() )					return false;

    //check if edges are completely projectable on m
    for ( int j = (i+1)%3; j != i; j = (j+1)%3 ) {
		samplePerEdge = SAMPLES_PER_EDGE;
        edge_samples.clear(); edge_dir = face->P1(j) - face->P(j); edge_dir.Normalize();
        for( int k = 0; k <= samplePerEdge; k++ )  edge_samples.push_back( face->P(j) + (face->P1(j) - face->P(j)) * (k * step) ); //campionamento
        // samples on A
        for ( int k = 0; k < edge_samples.size(); k ++ ) {
            CMeshO::FacePointer nearestF = 0;
            vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); tri::UnMarkAll(m->cm);
            vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
            MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
            //Search closest point on A
            nearestF =  grid.GetClosest(PDistFunct, markerFunctor, edge_samples[k], max_dist, dist, closest);
            if ( nearestF == 0 )                    return false;   //no face within given range
            if ( isOnBorder(closest, nearestF ) )   return false;   //closest point on border
			if ( nearestF->IsD() )					return false;
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

	vcg::Point3<CMeshO::ScalarType> qp = vcg::Barycenter(*f); //f barycenter
	//search for max_edge
	float max_edge = std::max( vcg::Distance<float>(f->P(0),f->P(1)), std::max( vcg::Distance<float>(f->P(1),f->P(2)), vcg::Distance<float>(f->P(2),f->P(0)) ) );
	float dist = max_dist; CMeshO::FacePointer nearestF = 0; vcg::Point3<CMeshO::ScalarType> closest;
  vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); UnMarkAll(m->cm);
    vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
	nearestF =  grid.GetClosest(PDistFunct, markerFunctor, qp, max_dist, dist, closest);
	if (nearestF == 0) return false;	//too far away
	float min_q = std::min( nearestF->V(0)->Q(), std::min( nearestF->V(1)->Q(), nearestF->V(2)->Q() ) ); //min distance of nearestF's vertices from M-border
	float max_q = std::max( f->V(0)->Q(), std::max( f->V(1)->Q(), f->V(2)->Q() ) );						 //max distance of F's vertices from A-border
	if ( min_q <= max_edge ) return false;
	if (test) if ( min_q <= max_q ) return false;
	return true;
}

/* Check if point is on border of face f.
 * @param point The query-point
 * @param f     Face containing point
 * @return      true if point lies on a border edge of f, false otherwise
 */
bool FilterZippering::isOnBorder( CMeshO::CoordType point, CMeshO::FacePointer f )  {
    // TODO: bar. coords?
    // for each edge, calculates distance point-edge
	if ( f == 0 ) return false;	//null face
    int cnt = 0;
    for ( int i = 0; i < 3; i ++ ) {
       vcg::Line3<CMeshO::ScalarType> line( f->P(i), f->P1(i) - f->P(i) );
       if ( vcg::Distance( line, point ) <= eps && vcg::face::IsBorder( *f, i ) ) { //lying on border edge
            cnt ++;
       }
    }
    assert( cnt < 3 );
    if ( cnt > 0 ) return true; //it's on border edge
    // it isn't on border edge, so check if it's coinciding with a border-vertex
    cnt = 0;
    for ( int i = 0; i < 3; i ++ )
        if ( vcg::Distance( point, f->P(i) ) <= eps ) //it' s a vertex
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
bool FilterZippering::isOnEdge( CMeshO::CoordType point, CMeshO::FacePointer f ) {
	// TODO: bar. coords?
    // for each edge, calculates distance point-edge
	if ( f == 0 ) return false;	//null face
    int cnt = 0;
    for ( int i = 0; i < 3; i ++ ) {
       vcg::Line3<CMeshO::ScalarType> line( f->P(i), f->P1(i) - f->P(i) );
       if ( vcg::Distance( line, point ) <= eps ) cnt ++;
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

    vcg::face::Pos<CMeshO::FaceType> p( f, i, f->V(i) );
    //loop
    do {
        if ( vcg::face::IsBorder( *p.F(), p.E() ) ) return true;
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
                                    vcg::Point3<CMeshO::ScalarType> N,                          //face normal (useful for proiection)
                                    std::vector<CMeshO::CoordType> &coords,                     //output coords
                                    std::vector<int> &pointers ) {                              //output triangles
	// rotation matrix (will be used for projection on plane)
    vcg::Matrix44<CMeshO::ScalarType> rot_matrix;
    rot_matrix.SetRotateRad( vcg::Angle<CMeshO::ScalarType>( N, vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) ), N ^ vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) );
	//border refinement - brand new method?
	//Remove intersecating border
	for (int i = 0; i < info.border.size(); i ++) {
		for ( int j = 0; j < info.border[i].edges.size(); j++) {
			//project one border segment on face
			vcg::Segment2f s(   vcg::Point2f((rot_matrix * info.border[i].edges[j].P0()).X(), (rot_matrix * info.border[i].edges[j].P0()).Y()),
								vcg::Point2f((rot_matrix * info.border[i].edges[j].P1()).X(), (rot_matrix * info.border[i].edges[j].P1()).Y()) );	//projects edge on plane
			vcg::Point2f x;
			for ( int k = i+1; k < info.border.size(); k++) {
				for ( int h = 0; h < info.border[k].edges.size(); h++) {
					vcg::Segment2f t(   vcg::Point2f((rot_matrix * info.border[k].edges[h].P0()).X(), (rot_matrix * info.border[k].edges[h].P0()).Y()),
										vcg::Point2f((rot_matrix * info.border[k].edges[h].P1()).X(), (rot_matrix * info.border[k].edges[h].P1()).Y()) );	//projects edge on plane
					if ( vcg::SegmentSegmentIntersection( s, t, x ) ) {
						h = info.border[k].edges.size();
						info.border.erase(info.border.begin() + k); 
						k--;
					}
				}
			}
		}
	}
	//Split border if necessary
	for (int i = 0; i < info.border.size(); i ++) {
		for ( int j = 1; j < info.border[i].edges.size(); j++ ) {
			//check if P0 lies on trash edge, then split border (skip first edge)
			for ( int k = 0; k < info.trash[0].edges.size(); k++ ) {
				if ( vcg::SquaredDistance<float>( info.trash[0].edges[k], info.border[i].edges[j].P0() ) == 0.0f ) { //approxim
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
    for (int i = 0; i < info.border.size(); i ++) {
        //search for component intersecated by border and split it into two or more components
        bool conn = true; int c = searchComponent( info, info.border[i].edges.front().P0(), info.border[i].edges.back().P1(), conn ); polyline current;
        if ( conn ) current = info.conn[c]; else current = info.trash[c];
        info.AddCComponent( cutComponent( current, info.border[i], rot_matrix ) );
		polyline rev_border = info.border[i]; std::reverse( rev_border.edges.begin(), rev_border.edges.end() );
        for ( int k = 0; k < rev_border.edges.size(); k ++) rev_border.edges[k].Flip();
        info.AddTComponent( cutComponent( current, rev_border, rot_matrix ) );
        if ( conn ) info.RemoveCComponent( c ); else info.RemoveTComponent( c );
    }
	//No border; triangulation of the whole face
	if ( info.border.size() == 0 ) {
        info.conn = info.trash;
        for ( int  i = 0; i < info.conn.size(); i ++ ) {
            reverse( info.conn[i].edges.begin(), info.conn[i].edges.end() );
            for ( int j = 0; j < info.conn[i].edges.size(); j ++ ) info.conn[i].edges[j].Flip();
            reverse( info.conn[i].verts.begin(), info.conn[i].verts.end() );
            for ( int j = 0; j < info.conn[i].verts.size(); j ++ ) info.conn[i].verts[j] = std::make_pair(info.conn[i].verts[j].second, info.conn[i].verts[j].first);
        }
    }
    //triangulation of Ccomponent
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        std::vector< vcg::Point3<CMeshO::ScalarType> > points;  //coords vector
        std::vector< int > vertices;          //vertices vector
        for ( int j = 0; j < info.conn[i].edges.size(); j ++ )  {
            points.push_back( info.conn[i].edges[j].P0() );
            vertices.push_back( info.conn[i].verts[j].first );
        }
        if ( points.size() < 3 ) continue;
        std::vector< int > indices; int iters = 0;
        std::vector< std::vector< vcg::Point3<CMeshO::ScalarType> > > outlines; outlines.push_back( points );
        while ( indices.size() == 0 && ++iters < MAX_LOOP ) {
            vcg::glu_tesselator::tesselate( outlines, indices );    //glu tessellator
            if ( indices.size() == 0 )
                for ( int k = 0; k < outlines[0].size(); k ++ ) outlines[0][k] = outlines[0][k] * 10.0; //glu tessellator doesn't work properly for close points, so we scale coords in order to obtain a triangulation, if needed
        }
        for ( int k = 0; k < indices.size(); k ++ )  pointers.push_back( vertices[indices[k]] );    //save indices, in order to create new faces
    }
}

/* Split a component into two component, using border as separating line. Discard component external to border polyline.
 * @param comp To-be-split component
 * @param border Border edges
 * @param rot_mat Rotation matrix, needed to project comp points on z=k plane
 */

polyline FilterZippering::cutComponent( polyline comp,                                    //Component to be cut
                                        polyline border,                                  //border
                                        vcg::Matrix44<CMeshO::ScalarType> rot_mat ) {     //Rotation matrix

	vcg::Point3<CMeshO::ScalarType> startpoint = border.edges.front().P0();
    vcg::Point3<CMeshO::ScalarType> endpoint = border.edges.back().P1();
	vcg::Point2<CMeshO::ScalarType> startpoint2D ( (rot_mat * startpoint).X(), (rot_mat * startpoint).Y() );
    vcg::Point2<CMeshO::ScalarType> endpoint2D ( (rot_mat * endpoint).X(), (rot_mat * endpoint).Y() );
    int startedge = 0, endedge = 0; float min_dist_s = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[0], startpoint ), min_dist_e = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[0], endpoint );
    bool v_start = false, v_end = false;
    // search where startpoint and endpoint lie
    for ( int i = 0; i < comp.edges.size(); i ++ ) {
		if ( !v_start && vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ) <= min_dist_s ) { startedge = i; min_dist_s = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ); }
        if ( !v_end && vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ) <= min_dist_e ) { endedge = i; min_dist_e = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ); }
        if ( comp.edges[i].P1() == startpoint ) { startedge = i; v_start = true; }  //lies on a vertex
        if ( comp.edges[i].P0() == endpoint ) { endedge = i; v_end = true; }        //lies on a vertex
    }
    polyline p;
	// border edges will be edges of new comp
    p.edges.insert( p.edges.begin(), border.edges.begin(), border.edges.end() );
    p.verts.insert( p.verts.begin(), border.verts.begin(), border.verts.end() );
    // startedge == endedge
    if ( startedge == endedge && !vcg::Convex<CMeshO::ScalarType>( startpoint2D, vcg::Point2<CMeshO::ScalarType> ( (rot_mat * border.edges.front().P1()).X(), (rot_mat * border.edges.front().P1()).Y() ) , endpoint2D ) ) {
        vcg::Segment3<CMeshO::ScalarType> join( endpoint, startpoint );
        p.edges.push_back( join ); p.verts.push_back( std::make_pair( border.verts.back().second, border.verts.front().first ) ); //Vertex pointers
        return p;
    }
	
    // startedge!=endedge
    // search point on the right, create oriented segment and go on
    int step = -1;
    vcg::Point3<CMeshO::ScalarType> c0 = border.edges.back().P0();
    std::vector< vcg::Segment3<CMeshO::ScalarType> >::iterator edge_it = border.edges.end(); edge_it--;
    //too short segment; not reliable
    while ( vcg::Distance<float>( rot_mat * c0, rot_mat * endpoint ) <= 5.0 * eps ) {
        //previous
        c0 = (*edge_it).P0();
        if (edge_it != border.edges.begin()) edge_it--; else break;
    }
    if ( v_end ) {
        if ( !vcg::Convex<CMeshO::ScalarType>( vcg::Point2<CMeshO::ScalarType> ( (rot_mat * c0).X(), (rot_mat * c0).Y() ), endpoint2D,
                                               vcg::Point2<CMeshO::ScalarType> ( (rot_mat * comp.edges[(endedge+comp.edges.size()-1)%comp.edges.size()].P0()).X(), (rot_mat * comp.edges[(endedge+comp.edges.size()-1)%comp.edges.size()].P0()).Y() ) ) ) {
            step = comp.edges.size() - 1;
        }
        else {
            step = comp.edges.size() + 1;
            vcg::Segment3<CMeshO::ScalarType> s( comp.edges[endedge].P0(), comp.edges[endedge].P1() );
            p.edges.push_back( s ); step = comp.edges.size() + 1;
            p.verts.push_back( std::make_pair( comp.verts[endedge].first, comp.verts[endedge].second ) );
        }
    }
    else {
        if ( !vcg::Convex<CMeshO::ScalarType>(   vcg::Point2<CMeshO::ScalarType> ( (rot_mat * c0).X(), (rot_mat * c0).Y() ), endpoint2D,
                                                 vcg::Point2<CMeshO::ScalarType> ( (rot_mat * comp.edges[endedge].P0()).X(), (rot_mat * comp.edges[endedge].P0()).Y() ) ) ) {
            vcg::Segment3<CMeshO::ScalarType> s( endpoint, comp.edges[endedge].P0() );
            p.edges.push_back( s ); step = comp.edges.size() - 1;
            p.verts.push_back( std::make_pair(border.verts.back().second, comp.verts[endedge].first ) );
        }
        else {
            vcg::Segment3<CMeshO::ScalarType> s( endpoint, comp.edges[endedge].P1() );
            p.edges.push_back( s ); step = comp.edges.size() + 1;
            p.verts.push_back( std::make_pair(border.verts.back().second, comp.verts[endedge].second ) );
        }
    }
    for ( int i = (endedge + step)%(comp.edges.size()); i != startedge; i = (i + step)%(comp.edges.size()) ) {
        p.edges.push_back( comp.edges[i] );
        std::pair<int, int> vs( comp.verts[i] );
        if ( (p.edges[p.edges.size()-2].P0() == p.edges.back().P0()) ||  (p.edges[p.edges.size()-2].P1() == p.edges.back().P1()) ) {
            p.edges.back().Flip();  //change direction
            vs = std::make_pair( comp.verts[i].second, comp.verts[i].first );   //change direction
        }
        p.verts.push_back( vs );
    }

    //last segment
    if ( v_start ) {
        if ( p.edges.back().P1() == comp.edges[startedge].P0() ) {
            vcg::Segment3<CMeshO::ScalarType> s( comp.edges[startedge].P0() , comp.edges[startedge].P1() );
            p.edges.push_back( s );
            p.verts.push_back( std::make_pair ( comp.verts[startedge].first , comp.verts[startedge].second ) );
        }
    }
    else {
        vcg::Segment3<CMeshO::ScalarType> s( p.edges.back().P1() , startpoint );
        p.edges.push_back( s );
        p.verts.push_back( std::make_pair ( p.verts.back().second, border.verts.front().first ) );
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
												vcg::Point3<CMeshO::ScalarType> P0,			//Start border point
												vcg::Point3<CMeshO::ScalarType> P1,			//End border point
												bool &conn ) {
	int nearestC = -1; int nearestT = -1;
	float distanceC = 100000*eps; float distanceT = 100000*eps;

	for ( int i = 0; i < info.nCComponent(); i ++ ) {
		//for each ccon search for edges nearest to P0 and P1
		float distP0 = 200000*eps; float distP1 = 200000*eps;
		for ( int j = 0; j < info.conn[i].edges.size(); j ++ ) {
			if ( vcg::SquaredDistance<float>( info.conn[i].edges[j], P0 ) < distP0 ) distP0 = vcg::SquaredDistance<float>( info.conn[i].edges[j], P0 );
			if ( vcg::SquaredDistance<float>( info.conn[i].edges[j], P1 ) < distP1 ) distP1 = vcg::SquaredDistance<float>( info.conn[i].edges[j], P1 );
		}
		if ( distP0 + distP1 < distanceC ) { distanceC = distP0 + distP1; nearestC = i; }
	}

	for ( int i = 0; i < info.nTComponent(); i ++ ) {
		//for each trash search for edges nearest to P0 and P1
		float distP0 = 200000*eps; float distP1 = 200000*eps;
		for ( int j = 0; j < info.trash[i].edges.size(); j ++ ) {
			if ( vcg::SquaredDistance<float>( info.trash[i].edges[j], P0 ) < distP0 ) distP0 = vcg::SquaredDistance<float>( info.trash[i].edges[j], P0 );
			if ( vcg::SquaredDistance<float>( info.trash[i].edges[j], P1 ) < distP1 ) distP1 = vcg::SquaredDistance<float>( info.trash[i].edges[j], P1 );
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
										 vcg::Segment3<float> edge,					//edge
										 int last_split,							//previously splitted edge
										 int &splitted_edge,						//currently splitted edge
										 vcg::Point3<CMeshO::ScalarType> &hit ) {	//approximate intersection point
	splitted_edge = -1;
    vcg::Plane3<CMeshO::ScalarType> plane; plane.Init( currentF->P(0), currentF->N() ); //projection plane
    vcg::Matrix44<CMeshO::ScalarType> rot_m; vcg::Point2f pt;	//matrix
    rot_m.SetRotateRad( vcg::Angle<CMeshO::ScalarType>( currentF->N(), vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) ), currentF->N() ^ vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) );
    vcg::Segment2f s(   vcg::Point2f((rot_m * plane.Projection(edge.P0())).X(), (rot_m * plane.Projection(edge.P0())).Y()),
                        vcg::Point2f((rot_m * plane.Projection(edge.P1())).X(), (rot_m * plane.Projection(edge.P1())).Y()) );	//projects edge on plane
	for ( int e = 0; e < 3; e ++ ) {
        if ( e != last_split && vcg::SegmentSegmentIntersection( s, vcg::Segment2f( vcg::Point2f( (rot_m * currentF->P(e)).X(), (rot_m * currentF->P(e)).Y() ),
                                                                                    vcg::Point2f( (rot_m * currentF->P1(e)).X(), (rot_m * currentF->P1(e)).Y() ) ), pt ) ) {
            splitted_edge = e; break;
        }
    }
    if (splitted_edge == -1) return false;	//No intersection!
	// search intersection point (approximation)
    vcg::Segment3<CMeshO::ScalarType> b_edge( currentF->P(splitted_edge), currentF->P1(splitted_edge) );
	int sampleNum = SAMPLES_PER_EDGE; float step = 1.0 / (sampleNum + 1);
    vcg::Point3<CMeshO::ScalarType> closest;    float min_dist = b_edge.Length();
    for ( int k = 0; k <= sampleNum; k ++ ) {
        vcg::Point3<CMeshO::ScalarType> currentP = edge.P0() + (edge.P1() - edge.P0())*(k*step);
        if ( vcg::SquaredDistance( b_edge, currentP ) < min_dist ) {
            closest = currentP; min_dist = vcg::SquaredDistance( b_edge, closest );
        }
    }
    if ( min_dist >= b_edge.Length() ) return false; //point not found
    hit = vcg::ClosestPoint(b_edge, closest); //projection on edge
	return true;
}

/* Zippering of two meshes (Turk approach)
 * Given two mesh, a mesh with one or more holes (A) and a second mesh, a patch (B), fill a hole onto m surface
 * using faces of patch. Algorithm const of three steps:
 * - CheckRedundancy: delete face of patch that can be projected completely on A's surface
 * - PatchRefinement: patch vertices are projected on A, and border edges are splitted;
 * - MeshRefinement: faces where patch vertices lie are re-triangulated
 * Based on Zippered Polygon Meshes from Range Images, by G.Turk, M.Levoy, Siggraph 1994
 */
bool FilterZippering::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
     if ( md.size() == 1 )   {
        Log(GLLogStream::FILTER,"Please add a second mesh");
        return false;
    }
    MeshModel *a = par.getMesh("FirstMesh");
    MeshModel *b = par.getMesh("SecondMesh");
	clock_t start, t1, t2, t3;

	/*Distance from border*/
	a->cm.face.EnableVFAdjacency(); a->cm.vert.EnableVFAdjacency(); 
	b->cm.face.EnableVFAdjacency(); b->cm.vert.EnableVFAdjacency(); 
	a->cm.face.EnableQuality();
	b->cm.face.EnableQuality();
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(a->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(a->cm);
	vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(a->cm);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(b->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(b->cm);
	vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(b->cm);
	a->cm.face.DisableVFAdjacency(); 
	b->cm.face.DisableVFAdjacency(); 
	/*end*/

    a->cm.face.EnableFFAdjacency();   vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm); 
    a->cm.face.EnableMark(); tri::UnMarkAll(a->cm);
    b->cm.face.EnableFFAdjacency();      vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm); 
    b->cm.face.EnableMark(); tri::UnMarkAll(b->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
    CMeshO::ScalarType epsilon  = par.getAbsPerc("distance");
 
	//Search for face on patch border
    int limit = a->cm.fn;
    std::vector< vcg::tri::Hole<CMeshO>::Info > ccons, ccons_a, ccons_b; 
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    a->cm.face.EnableColor();
    b->cm.face.EnableColor();//Useful for debug
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);
    vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);

    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm ); eps = a->cm.bbox.Diag() / 1000000;

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
	vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons_a ); 
	MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid on A
	MeshFaceGrid grid_b; grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid on B
	vcg::tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b ); //bordi di B
	std::vector< std::pair<CMeshO::FacePointer,char> > remove_faces; int scr=0;
	//i bordi di A sono in ccons_a
	for ( int i = 0; i < ccons_a.size(); i ++ ) { 
		vcg::face::Pos<CMeshO::FaceType> p = ccons_a[i].p;
		do {
			if ( !p.F()->IsD() && simpleCheckRedundancy( p.F(), b, grid_b, epsilon, true ) ) { limit--; scr++; remove_faces.push_back( std::make_pair(p.F(),'A') );}
			p.NextB();
		} while ( p.F() != ccons_a[i].p.F() );
	}
	while ( !remove_faces.empty() ){ 
			CMeshO::FacePointer currentF = remove_faces.back().first; 
			char choose = remove_faces.back().second;
			remove_faces.pop_back();
			if ( currentF->IsD() ) continue;	//no op
			if ( simpleCheckRedundancy( currentF, b, grid_b, epsilon, true ) ) { scr++; limit--;
					vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *currentF ); 
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0); //topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					remove_faces.push_back( std::make_pair(currentF->FFp(0),'A') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					remove_faces.push_back( std::make_pair(currentF->FFp(1),'A') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					remove_faces.push_back( std::make_pair(currentF->FFp(2),'A') );
			}
	}
	/*Update distance from border*/
	a->cm.face.EnableVFAdjacency(); 
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(a->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(a->cm);
	vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(a->cm);
	a->cm.face.DisableVFAdjacency(); 
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	/*end*/

	//repeat for B
	for ( int i = 0; i < ccons_b.size(); i ++ ) { 
		vcg::face::Pos<CMeshO::FaceType> p = ccons_b[i].p;
		do {
			if ( !p.F()->IsD() && simpleCheckRedundancy( p.F(), a, grid_a, epsilon, false ) )  { scr++; remove_faces.push_back( std::make_pair(p.F(),'B') ); }
			p.NextB();
		} while ( p.F() != ccons_b[i].p.F() );
	}
	while(!remove_faces.empty()) {
			CMeshO::FacePointer currentF = remove_faces.back().first; 
			char choose = remove_faces.back().second;
			remove_faces.pop_back();
			if ( currentF->IsD() ) continue;	//no op
			if ( simpleCheckRedundancy( currentF, a, grid_a, epsilon, false ) ) { scr++;
				vcg::tri::Allocator<CMeshO>::DeleteFace( b->cm, *currentF );
				currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0);	//topology update
				currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
				remove_faces.push_back( std::make_pair(currentF->FFp(0),'B') );
				currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
				currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
				remove_faces.push_back( std::make_pair(currentF->FFp(1),'B') );
				currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
				currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
				remove_faces.push_back( std::make_pair(currentF->FFp(2),'B') );
			}
	}
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
	Log(GLLogStream::DEBUG, "scr value %d", scr);
	//Calculate new border
	ccons_a.clear(); ccons_b.clear();
	vcg::tri::UpdateFlags<CMeshO>::Clear(a->cm);
	vcg::tri::UpdateFlags<CMeshO>::Clear(b->cm);
	vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
	vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons_a ); 
	vcg::tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b ); 
#endif 
#ifndef PREERODE
	MeshFaceGrid grid_a; 
	MeshFaceGrid grid_b; 
	std::vector< std::pair<CMeshO::FacePointer,char> > remove_faces;
	vcg::tri::Hole<CMeshO>::GetInfo( b->cm, false, ccons_b ); 
#endif

	/* STEP 1 - Removing Redundant Surfaces
     * Repeat until mesh surface remain unchanged:
     *   - Remove redundant triangles on the boundary of patch
     */
	start = clock();
	grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid on A
	grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid on B
	bool changed; vcg::face::Pos<CMeshO::FaceType> p; int c_faces = 0;
    std::vector< CMeshO::FacePointer > split_faces;  std::vector< CMeshO::VertexPointer > new_faces; 
	std::priority_queue< std::pair<CMeshO::FacePointer,char>, std::vector< std::pair<CMeshO::FacePointer,char> >, compareFaceQuality > faces_pqueue;
	remove_faces.clear();
	
	if (par.getBool("FaceQuality")) {
		Log(GLLogStream::FILTER, "Using Face Quality...");
		for ( int i = 0; i < ccons_a.size(); i ++ ) { 
			vcg::face::Pos<CMeshO::FaceType> p = ccons_a[i].p;
			do {
				if ( !p.F()->IsD() && checkRedundancy( p.F(), b, grid_b, epsilon ) ) faces_pqueue.push( std::make_pair(p.F(),'A') );
				p.NextB();
			} while ( p.F() != ccons_a[i].p.F() );
		}

		for ( int i = 0; i < ccons_b.size(); i ++ ) { 
			vcg::face::Pos<CMeshO::FaceType> p = ccons_b[i].p;
			do {
				if ( !p.F()->IsD() && checkRedundancy( p.F(), a, grid_a, epsilon ) )  faces_pqueue.push( std::make_pair(p.F(),'B') );
				p.NextB();
			} while ( p.F() != ccons_b[i].p.F() );
		}
		
		while ( !faces_pqueue.empty() ) {
			CMeshO::FacePointer currentF = faces_pqueue.top().first; 
			char choose = faces_pqueue.top().second;
			faces_pqueue.pop();
			if ( currentF->IsD() ) continue;	//no op
			if (choose == 'A') {
				if ( checkRedundancy( currentF, b, grid_b, epsilon ) ) {
					vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0); //topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					faces_pqueue.push( std::make_pair(currentF->FFp(0),'A') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					faces_pqueue.push( std::make_pair(currentF->FFp(1),'A') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					faces_pqueue.push( std::make_pair(currentF->FFp(2),'A') );
				}
			}
			else {
				if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
					vcg::tri::Allocator<CMeshO>::DeleteFace( b->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0);	//topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					faces_pqueue.push( std::make_pair(currentF->FFp(0),'B') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					faces_pqueue.push( std::make_pair(currentF->FFp(1),'B') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					faces_pqueue.push( std::make_pair(currentF->FFp(2),'B') );
				}
			}
		}
	} 
	else {	//do not use face quality
		Log(GLLogStream::FILTER, "Using Standard predicate...");	
		for ( int i = 0; i < ccons_a.size(); i ++ ) { 
			vcg::face::Pos<CMeshO::FaceType> p = ccons_a[i].p;
			if ( p.F()->IsD() ) continue;
			do {
				if ( !p.F()->IsD() && checkRedundancy( p.F(), b, grid_b, epsilon ) ) {limit--; remove_faces.push_back( std::make_pair(p.F(),'A') );}
				p.NextB();
			} while ( p.F() != ccons_a[i].p.F() );
		}

		for ( int i = 0; i < ccons_b.size(); i ++ ) { 
			vcg::face::Pos<CMeshO::FaceType> p = ccons_b[i].p;
			if ( p.F()->IsD() ) continue;
			do {
				if ( !p.F()->IsD() && checkRedundancy( p.F(), a, grid_a, epsilon ) )  remove_faces.push_back( std::make_pair(p.F(),'B') );
				p.NextB();
			} while ( p.F() != ccons_b[i].p.F() );
		}
	
		while ( !remove_faces.empty() ) {
			CMeshO::FacePointer currentF = remove_faces.back().first; 
			char choose = remove_faces.back().second;
			remove_faces.pop_back();
			if ( currentF->IsD() ) continue;	//no op
			if (choose == 'A') {
				if ( checkRedundancy( currentF, b, grid_b, epsilon ) ) { limit--;
					vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0); //topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					remove_faces.push_back( std::make_pair(currentF->FFp(0),'A') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					remove_faces.push_back( std::make_pair(currentF->FFp(1),'A') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					remove_faces.push_back( std::make_pair(currentF->FFp(2),'A') );
				}
			}
			else {
				if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
					vcg::tri::Allocator<CMeshO>::DeleteFace( b->cm, *currentF ); c_faces++;
					currentF->FFp(0)->FFp(currentF->FFi(0)) = currentF->FFp(0);	//topology update
					currentF->FFp(0)->FFi(currentF->FFi(0)) = currentF->FFi(0);
					remove_faces.push_back( std::make_pair(currentF->FFp(0),'B') );
					currentF->FFp(1)->FFp(currentF->FFi(1)) = currentF->FFp(1);
					currentF->FFp(1)->FFi(currentF->FFi(1)) = currentF->FFi(1);
					remove_faces.push_back( std::make_pair(currentF->FFp(1),'B') );
					currentF->FFp(2)->FFp(currentF->FFi(2)) = currentF->FFp(2);
					currentF->FFp(2)->FFi(currentF->FFi(2)) = currentF->FFi(2);
					remove_faces.push_back( std::make_pair(currentF->FFp(2),'B') );
				}
			}
		}
	}//end else

    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
	vcg::tri::Append<CMeshO, CMeshO>::Mesh( a->cm, b->cm );
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceClear(a->cm); 

	ccons.clear(); vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons );  std::vector<std::pair<int,int> > b_pos;
	for ( int i = 0; i < ccons.size(); i ++ ) {
		if ( vcg::tri::Index(a->cm, ccons[i].p.F()) >= limit ) b_pos.push_back( std::make_pair( vcg::tri::Index(a->cm, ccons[i].p.F()), ccons[i].p.E() ) );
	}
	
	for ( int i = 0; i < b_pos.size(); i ++ ) {
		vcg::face::Pos<CMeshO::FaceType> p; p.Set( &(a->cm.face[b_pos[i].first]), b_pos[i].second, a->cm.face[b_pos[i].first].V(b_pos[i].second) );
		CMeshO::FacePointer start = p.F(); 
		if ( vcg::face::BorderCount(*start) == 3 ) { vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *start ); continue; }	
		do {
			if ( !p.F()->IsD() && vcg::face::BorderCount(*p.F()) >= 2 ) {
				
				vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
				vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::FacePointer> fpu;
				CMeshO::FaceIterator f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 4, fpu ); if ( fpu.NeedUpdate() ) { fpu.Update( p.F() ); fpu.Update( start ); }
				CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
				int j; for (j=0; j<3 && vcg::face::IsBorder(*p.F(), j); j++); assert( j < 3 ); //split non border edge
				CMeshO::FacePointer opp_f = p.F()->FFp(j); int opp_j = p.F()->FFi(j);
				(*v).P() = (p.F()->P(j) + p.F()->P1(j))/2.0;
				CMeshO::FacePointer f1 = &*f; f1->V(0) = p.F()->V(j);		f1->V(1) = &(*v);				f1->V(2) = p.F()->V2(j); ++f;
				CMeshO::FacePointer f2 = &*f; f2->V(0) = p.F()->V2(j);		f2->V(1) = &(*v);				f2->V(2) = p.F()->V1(j); ++f;
				CMeshO::FacePointer f3 = &*f; f3->V(0) = p.F()->V(j);		f3->V(1) = opp_f->V2(opp_j);	f3->V(2) = &(*v); ++f;
				CMeshO::FacePointer f4 = &*f; f4->V(0) = opp_f->V2(opp_j);	f4->V(1) = p.F()->V1(j);		f4->V(2) = &(*v); 
				//update topology manually
				f1->FFp(0) = f3; f1->FFp(1) = f2; f1->FFp(2) = f1; f1->FFi(0) = 2; f1->FFi(1) = 0; f1->FFi(2) = 2;
				f2->FFp(0) = f1; f2->FFp(1) = f4; f2->FFp(2) = f2; f2->FFi(0) = 1; f2->FFi(1) = 1; f2->FFi(2) = 2;
				f3->FFp(0) = vcg::face::IsBorder(*opp_f, (opp_j+1)%3)? f3 : opp_f->FFp((opp_j+1)%3); f3->FFp(1) = f4; f3->FFp(2) = f1; f3->FFi(0) = vcg::face::IsBorder(*opp_f, (opp_j+1)%3)? 0 : opp_f->FFi((opp_j+1)%3); f3->FFi(1) = 2; f3->FFi(2) = 0;
				f4->FFp(0) = vcg::face::IsBorder(*opp_f, (opp_j+2)%3)? f4 : opp_f->FFp((opp_j+2)%3); f4->FFp(1) = f2; f4->FFp(2) = f3; f4->FFi(0) = vcg::face::IsBorder(*opp_f, (opp_j+2)%3)? 0 : opp_f->FFi((opp_j+2)%3); f4->FFi(1) = 1; f4->FFi(2) = 1;
				if( !vcg::face::IsBorder(*f3, 0) ) { f3->FFp(0)->FFp(f3->FFi(0)) = f3; f3->FFp(0)->FFi(f3->FFi(0)) = 0; }	//border
				if( !vcg::face::IsBorder(*f4, 0) ) { f4->FFp(0)->FFp(f4->FFi(0)) = f4; f4->FFp(0)->FFi(f4->FFi(0)) = 0; }	//border
				vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *(p.F()->FFp(j)) );
				vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *p.F() );
				if ( p.F() == start ) start = f1;
				if ( (p.F()->FFp(j)) == start && vcg::face::IsBorder(*f3,0) ) start = f3;
				if ( (p.F()->FFp(j)) == start && vcg::face::IsBorder(*f4,0) ) start = f4;
				if ( vcg::face::BorderCount(*(p.F())) == 2 && vcg::face::BorderCount(*(p.F()->FFp(j))) == 2 ) break;	//square
			}
			p.NextB();
		} 
		while( p.F() != start );
	}
	t1 = clock();
    /* End Step 1 */
#ifdef REDUNDANCY_ONLY
    Log(GLLogStream::DEBUG, "Removed %d redundant faces", c_faces);
    return true;
#endif
	new_faces.clear(); split_faces.clear(); remove_faces.clear(); ccons.clear();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	//vcg::tri::UpdateFlags<CMeshO>::FaceClear(a->cm); 
	vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons );
	vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
    grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //reset grid on A
	
    /* STEP 2 - Project patch points on mesh surface
     * and ricorsively subdivide face in smaller triangles until each patch's face has border vertices
     * lying in adiacent or coincident faces. Also collect informations for triangulation of mesh faces.
     */
    //Add optional attribute
    CMeshO::PerFaceAttributeHandle<aux_info> info = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<aux_info>  (a->cm, std::string("aux_info"));
	CMeshO::PerFaceAttributeHandle<bool> visited = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<bool> (a->cm); //check for already visited face
	for ( int i = 0; i < a->cm.face.size(); i ++) visited[i] = false;	//no face previously visited
    std::vector< CMeshO::FacePointer > tbt_faces;   //To Be Triangulated
    std::vector< CMeshO::FacePointer > tbr_faces;   //To Be Removed
    std::vector< int > verts;
	for ( int c = 0; c < ccons.size(); c ++ ) {
		if ( vcg::tri::Index( a->cm, ccons[c].p.F() ) < limit ) continue;
        if ( visited[ccons[c].p.F()] || ccons[c].p.F()->IsD() ) continue; 
		vcg::face::Pos<CMeshO::FaceType> p = ccons[c].p; p.FlipV();//CCW order
		do {

			if ( visited[p.F()] || p.F()->IsD()) { p.NextB(); continue; }  //Already deleted or visited, step over
			visited[p.F()] = true;
			CMeshO::FacePointer nearestF = 0, nearestF1 = 0;
			assert( vcg::face::BorderCount( *p.F() ) > 0 ); assert( vcg::face::IsBorder( *p.F(), p.E() ) );  //Check border correctness
            vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
            vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
            MeshFaceGrid::ScalarType  dist = 2*epsilon;  MeshFaceGrid::CoordType closest, closest1;
            nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), epsilon, dist, closest);
			if ( fabs(dist) >= fabs(epsilon) ) nearestF = 0;
            dist = 2*epsilon;
            nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor,  p.F()->P1(p.E()), epsilon, dist, closest1);
			if ( fabs(dist) >= fabs(epsilon) ) nearestF1 = 0;
            int cnt = 0;    //counter (inf. loop)
			std::vector < std::pair< int, int > > stack;
            std::vector < std::pair< CMeshO::FacePointer, CMeshO::FacePointer > > stack_faces;
			int end_v = vcg::tri::Index(a->cm, p.F()->V2(p.E())); 
            // Not-adjacent; recursive split using mid-point of edge border
            stack.push_back( std::make_pair( vcg::tri::Index( a->cm, p.F()->V(p.E()) ),
                                             vcg::tri::Index( a->cm, p.F()->V1(p.E()) ) ) );    //Edge border
            assert( vcg::face::IsBorder( *p.F(), p.E() ) );
            stack_faces.push_back( std::make_pair(nearestF, nearestF1) );   //Nearest Faces

			CMeshO::FacePointer actualF = p.F(); int actualE = p.E(); p.NextB();
			
            while ( !stack.empty() ) {
				if ( cnt++ > 2*MAX_LOOP ) {
					Log(GLLogStream::DEBUG, "Loop"); stack.clear(); continue;
				} //go to next face in case of inf. loop
				
				std::pair< int, int > border_edge = stack.back(); stack.pop_back();   //vertex indices
                CMeshO::FacePointer start = stack_faces.back().first; CMeshO::FacePointer end = stack_faces.back().second; //nearestF
                stack_faces.pop_back();

				if ( start == 0 && end == 0 ) continue;

				vcg::Point3<CMeshO::ScalarType> closestStart, closestEnd; float distStart, distEnd;
				grid_a.GetClosest(PDistFunct, markerFunctor, a->cm.vert[border_edge.first].P(), 2*epsilon, distStart, closestStart); //closest point on mesh
				grid_a.GetClosest(PDistFunct, markerFunctor, a->cm.vert[border_edge.second].P(), 2*epsilon, distEnd, closestEnd); //closest point on mesh
				
				if (start != 0) { info[start].SetEps( eps ); info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) )  ); } //init
				if (end != 0)	{info[end].SetEps( eps ); info[end].Init( *end, vcg::tri::Index( a->cm, end->V(0) ), vcg::tri::Index( a->cm, end->V(1) ), vcg::tri::Index( a->cm, end->V(2) ) ); }
                        
				if ( isOnBorder( closestStart, start ) && isOnBorder( closestEnd, end ) ) {
					//if closest point is really closest to the border, split border face
					if ( distStart <= eps ) info[start].addVertex( &(a->cm.vert[border_edge.first]), border_edge.first ); 
					if ( distEnd <= eps )   info[end].addVertex( &(a->cm.vert[border_edge.second]), border_edge.second );
					//Verify if the whole segment is on border
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1); bool border = true;
                    vcg::Point3<CMeshO::ScalarType> closestP;   float dist = 2*epsilon;
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        vcg::Point3<CMeshO::ScalarType> currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
						CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*epsilon, dist, closestP); //closest point on mesh
						if ( !isOnBorder( closestP, closestFace ) ) {	//not completely on border: split it!
							border = false;
							vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
							CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
							(*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
							if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
							CMeshO::FacePointer currentF = 0; CMeshO::CoordType closest;
							currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
							stack.push_back( std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
							stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
							stack.push_back( std::make_pair( v - a->cm.vert.begin(), border_edge.second ) );
							stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
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
					if ( distStart <= eps ) info[start].addVertex( &(a->cm.vert[border_edge.first]), border_edge.first ); 
					//Verify if the whole segment is on border
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1); bool border = true;
                    vcg::Point3<CMeshO::ScalarType> closestP;   float dist = 2*epsilon;
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        vcg::Point3<CMeshO::ScalarType> currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
						CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*epsilon, dist, closestP); //closest point on mesh
						if ( !isOnBorder( closestP, closestFace ) ) {	//not completely on border: split it!
							border = false;
							vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
							CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
							(*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
							if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
							CMeshO::FacePointer currentF = 0; CMeshO::CoordType closest;
							currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
							stack.push_back( std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
							stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
							stack.push_back( std::make_pair( v - a->cm.vert.begin(), border_edge.second ) );
							stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
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
					if ( distEnd <= eps )   info[end].addVertex( &(a->cm.vert[border_edge.second]), border_edge.second );
					//Verify if the whole segment is on border
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1); bool border = true;
                    vcg::Point3<CMeshO::ScalarType> closestP;   float dist = 2*epsilon;
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        vcg::Point3<CMeshO::ScalarType> currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
						CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*epsilon, dist, closestP); //closest point on mesh
						if ( !isOnBorder( closestP, closestFace ) ) {	//not completely on border: split it!
							border = false;
							vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
							CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
							(*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
							if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
							CMeshO::FacePointer currentF = 0; CMeshO::CoordType closest;
							currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
							stack.push_back( std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
							stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
							stack.push_back( std::make_pair( v - a->cm.vert.begin(), border_edge.second ) );
							stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
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
					if ( distStart <= eps ) info[start].addVertex( &(a->cm.vert[border_edge.first]), border_edge.first ); 
					a->cm.vert[border_edge.second].P() = closestEnd;
					tbr_faces.push_back( actualF );
					int last_split = -1; CMeshO::FacePointer currentF = end; bool stop = false; 
					CMeshO::VertexPointer startV = &a->cm.vert[border_edge.second];
					CMeshO::VertexPointer endV = &a->cm.vert[border_edge.first];
					CMeshO::VertexPointer thirdV = actualF->V2(actualE); int cnt = 0;
					do {
						int tosplit; vcg::Point3<CMeshO::ScalarType> closest;  cnt++;
						if (!findIntersection( currentF, vcg::Segment3<CMeshO::ScalarType>(a->cm.vert[border_edge.first].P(),a->cm.vert[border_edge.second].P()), last_split, tosplit, closest )) {
							stop = true; //no op
						}
						else {  
							last_split = currentF->FFi( tosplit );
							vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
							CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
							if ( vpu.NeedUpdate() )  { vpu.Update( p.V() ); vpu.Update(startV); vpu.Update(endV); vpu.Update(thirdV); }
							info[currentF].SetEps( eps ); info[currentF].Init( *currentF, vcg::tri::Index(a->cm, currentF->V(0)), vcg::tri::Index(a->cm, currentF->V(1)), vcg::tri::Index(a->cm, currentF->V(2)) );
							startV->P(); (*v).P();
							if ( info[currentF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), startV->P() ),
															 std::make_pair( v - a->cm.vert.begin(), vcg::tri::Index(a->cm, startV ) ) ) ) {
									tbt_faces.push_back( currentF ); 
									verts.push_back( vcg::tri::Index(a->cm, startV) ); verts.push_back( vcg::tri::Index(a->cm, thirdV) ); verts.push_back( v - a->cm.vert.begin() );
									
							}
							startV = &(*v);
							if ( vcg::face::IsBorder( *currentF, tosplit ) )  { //border reached
								verts.push_back( vcg::tri::Index( a->cm, thirdV ) ); verts.push_back( vcg::tri::Index( a->cm, endV ) ); verts.push_back( v - a->cm.vert.begin() );
								stack.push_back( std::make_pair( vcg::tri::Index(a->cm, endV), v - a->cm.vert.begin() ) ); stack_faces.push_back( std::make_pair(start, currentF) );
								stop = true;
							}
							else currentF = currentF->FFp(tosplit);
						}
					} while (!stop && cnt < MAX_LOOP);
					continue;
				}

				if ( end == 0 || isOnBorder( closestEnd, end ) ) {
					//if closest point is really closest to the border, split border face
					if ( distEnd <= eps )   info[end].addVertex( &(a->cm.vert[border_edge.second]), border_edge.second );
					a->cm.vert[border_edge.first].P() = closestStart;
					tbr_faces.push_back( actualF );
					int last_split = -1; CMeshO::FacePointer currentF = start; bool stop = false; 
					CMeshO::VertexPointer startV = &a->cm.vert[border_edge.first];
					CMeshO::VertexPointer endV = &a->cm.vert[border_edge.second];
					CMeshO::VertexPointer thirdV = actualF->V2(actualE); int cnt = 0;
					do {
						int tosplit; vcg::Point3<CMeshO::ScalarType> closest; cnt++;
						if (!findIntersection( currentF, vcg::Segment3<CMeshO::ScalarType>(a->cm.vert[border_edge.first].P(),a->cm.vert[border_edge.second].P()), last_split, tosplit, closest )) {
							break;
						}
						last_split = currentF->FFi( tosplit );
						vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
						CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
						if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
						info[currentF].SetEps( eps ); info[currentF].Init( *currentF, vcg::tri::Index(a->cm, currentF->V(0)), vcg::tri::Index(a->cm, currentF->V(1)), vcg::tri::Index(a->cm, currentF->V(2)) );
						if ( info[currentF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( startV->P(), (*v).P() ),
														 std::make_pair( vcg::tri::Index(a->cm, startV ), v - a->cm.vert.begin() ) ) ) {
								tbt_faces.push_back( currentF ); verts.push_back( vcg::tri::Index(a->cm, thirdV) ); verts.push_back( vcg::tri::Index(a->cm, startV) ); verts.push_back( v - a->cm.vert.begin() );
							}
						startV = &(*v);
						if ( vcg::face::IsBorder( *currentF, tosplit ) )  { //last triangle
							//verts.push_back( vcg::tri::Index( a->cm, endV ) ); verts.push_back( vcg::tri::Index( a->cm, thirdV ) ); verts.push_back( v - a->cm.vert.begin() );
							stack.push_back( std::make_pair( v - a->cm.vert.begin(), vcg::tri::Index(a->cm, endV) ) );
							stack_faces.push_back( std::make_pair(currentF, end) );
							stop = true;
						}
						else currentF = currentF->FFp(tosplit);
					} while (!stop && cnt < MAX_LOOP);
					continue;
				}

				a->cm.vert[border_edge.first].P() = closestStart;
				a->cm.vert[border_edge.second].P() = closestEnd;

                if ( start == end ) {
                    info[start].SetEps( eps ); info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) )  );
                    if ( info[start].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                  std::make_pair( border_edge.first, border_edge.second ) ) ) {
                        tbt_faces.push_back( start ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                    }
                    continue;
                }

				tbr_faces.push_back( actualF );
				if ( isAdjacent( start, end ) ) {
                    //calc. intersection point (approximate) and split edge
                    int shared; for ( int k = 0; k < 3; k ++ ) if ( start->FFp(k) == end ) shared = k;
                    vcg::Segment3<CMeshO::ScalarType> shared_edge( start->P(shared), start->P1(shared) );
					int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1);
                    vcg::Point3<CMeshO::ScalarType> closest;    float min_dist = shared_edge.Length();
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        vcg::Point3<CMeshO::ScalarType> currentP = a->cm.vert[border_edge.first].P() + ( a->cm.vert[border_edge.second].P() - a->cm.vert[border_edge.first].P() ) * (k*step);
                        if ( vcg::SquaredDistance( shared_edge, currentP ) < min_dist ) {
                            closest = currentP; min_dist = vcg::SquaredDistance( shared_edge, closest );
                        }
                    }
                    assert( vcg::SquaredDistance( shared_edge, closest ) < shared_edge.Length() );
                    closest = vcg::ClosestPoint(shared_edge, closest); //intersection point
                    //merge close vertices
                    if ( vcg::Distance<float>( closest, a->cm.vert[border_edge.first].P() ) < eps ) {
                        a->cm.vert[border_edge.first].P() = closest;
                        info[end].SetEps( eps ); info[end].Init( *end, vcg::tri::Index( a->cm, end->V(0) ), vcg::tri::Index( a->cm, end->V(1) ), vcg::tri::Index( a->cm, end->V(2) ) );
                        if ( info[end].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                    std::make_pair( border_edge.first, border_edge.second ) ) ) {
                            tbt_faces.push_back( end ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                        }
                        continue;
                    }
                    //merge close vertices
                    if ( vcg::Distance<float>( closest, a->cm.vert[border_edge.second].P() ) < eps ) {
                        a->cm.vert[border_edge.second].P() = closest;
                        info[start].SetEps( eps ); info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) ) );
                        if ( info[start].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                      std::make_pair( border_edge.first, border_edge.second ) ) ) {
                            tbt_faces.push_back( start ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                        }
                        continue;
                    }
                    //no close vertices, add information to faces
                    vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                    CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
                    if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
					info[start].SetEps( eps );  info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) ) );
                    info[end].SetEps( eps );    info[end].Init( *end, vcg::tri::Index( a->cm, end->V(0) ), vcg::tri::Index( a->cm, end->V(1) ), vcg::tri::Index( a->cm, end->V(2) ) );
                    if ( info[start].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), (*v).P() ),
                                                  std::make_pair( border_edge.first, v - a->cm.vert.begin() ) ) ) {
                        tbt_faces.push_back( start ); verts.push_back(v - a->cm.vert.begin()); verts.push_back( end_v ); verts.push_back( border_edge.first );   //new triangle
                    }
                    if ( info[end].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), a->cm.vert[border_edge.second].P() ),
                                                std::make_pair( v - a->cm.vert.begin(), border_edge.second ) ) ) {
                        tbt_faces.push_back( end ); verts.push_back(v - a->cm.vert.begin()); verts.push_back( border_edge.second ); verts.push_back( end_v );    //new triangle
                    }
                    continue;
                }
				//check if start and end share a vertex
                int w = sharesVertex( start, end ); vcg::Segment3<CMeshO::ScalarType> s( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() );
                if ( w != -1 && (cnt == MAX_LOOP || vcg::SquaredDistance<float>( s, start->P(w) ) <= eps) ) {
					vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                    CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = start->P(w);
					if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
                    info[start].SetEps( eps ); info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) )  );
                    info[end].SetEps( eps ); info[end].Init( *end, vcg::tri::Index( a->cm, end->V(0) ), vcg::tri::Index( a->cm, end->V(1) ), vcg::tri::Index( a->cm, end->V(2) )  );
                    if ( info[start].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), (*v).P() ),
                                                  std::make_pair( border_edge.first, v - a->cm.vert.begin() ) ) ) {
                        tbt_faces.push_back(start); verts.push_back( border_edge.first ); verts.push_back( v - a->cm.vert.begin() ); verts.push_back( end_v );
                    }
                    if ( info[end].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( (*v).P(),  a->cm.vert[border_edge.second].P() ),
                                                std::make_pair( v - a->cm.vert.begin(), border_edge.second ) ) ) {
                        tbt_faces.push_back(end); verts.push_back( v - a->cm.vert.begin() ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                    }
                    continue;
                }
				vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
                (*v).P() = (a->cm.vert[border_edge.first].P() + a->cm.vert[border_edge.second].P())/2.00;
                if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
                CMeshO::FacePointer currentF = 0; CMeshO::CoordType closest;
                currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoint on A
                stack.push_back( std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
				stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
                stack.push_back( std::make_pair( v - a->cm.vert.begin(), border_edge.second ) );
				stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
				assert( stack.size() == stack_faces.size() );
			}
        } while ( p.F() != ccons[c].p.F() );
    }//end for

    for ( int i = 0; i < tbr_faces.size(); i++) {
        if (!tbr_faces[i]->IsD()) vcg::tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbr_faces[i]);
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	t2 = clock();
    /* End of Step 2 */
#ifdef REFINE_PATCH_ONLY
    for ( int k = 0; k < verts.size(); k += 3 ) {
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        (*f).V(0) = &(a->cm.vert[verts[k]]); (*f).V(1) = &(a->cm.vert[verts[k+1]]); (*f).V(2) = &(a->cm.vert[verts[k+2]]);
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	Log(GLLogStream::DEBUG,"Created %d faces", verts.size()/3);
    return true;
#endif
    /* STEP 3
     * Faces are sorted by index, than each face is triangulated using auxiliar information about
     * vertices and edges
     */

	std::sort( tbt_faces.begin(), tbt_faces.end() ); std::vector<CMeshO::FacePointer>::iterator itr = std::unique( tbt_faces.begin(), tbt_faces.end() ); tbt_faces.resize(itr - tbt_faces.begin() );
    std::vector< CMeshO::CoordType > coords; int patch_verts = verts.size();
	for ( int i = 0; i < tbt_faces.size(); i ++ ) {
		if ( !tbt_faces[i]->IsD() ) {
            handleBorder( info[tbt_faces[i]], tbt_faces[i]->N(), coords, verts );
            vcg::tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbt_faces[i]);
        }
    }
	//remove user-defined attribute
	vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<bool>(a->cm, visited);
	vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<aux_info>(a->cm, info);

	// Add new faces
    int n_faces = a->cm.face.size(); int t_faces = 0;
	CMeshO::FaceIterator fn = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, verts.size()/3 );
    for ( int k = 0; k < verts.size(); k += 3 ) { 
		CMeshO::VertexPointer v0 = &(a->cm.vert[verts[k]]);
		CMeshO::VertexPointer v1 = &(a->cm.vert[verts[k+1]]);
		CMeshO::VertexPointer v2 = &(a->cm.vert[verts[k+2]]);
		//avoid null faces; could originate holes - is that really necessary?
		if ( vcg::Angle<float>( v1->P() - v0->P(), v2->P() - v0->P() ) == 0.0f ) continue;
		if ( vcg::Angle<float>( v0->P() - v1->P(), v2->P() - v1->P() ) == 0.0f ) continue;
		if ( vcg::Angle<float>( v0->P() - v2->P(), v1->P() - v2->P() ) == 0.0f ) continue;
		//CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
		if ( k < patch_verts ) {
            (*fn).V(0) = v0; (*fn).V(1) = v1; (*fn).V(2) = v2; (*fn).N() = ( (*fn).P(0) - (*fn).P(2) )^( (*fn).P(1)-(*fn).P(2) );
        }
        else {
            (*fn).V(0) = v0; (*fn).V(2) = v1; (*fn).V(1) = v2; (*fn).N() = ( (*fn).P(0) - (*fn).P(2) )^( (*fn).P(1)-(*fn).P(2) );
        }
		(*fn).V(0)->C() = vcg::Color4b::DarkGray; (*fn).V(1)->C() = vcg::Color4b::DarkGray; (*fn).V(2)->C() = vcg::Color4b::DarkGray; 
		(*fn).C() = vcg::Color4b::Magenta;
		n_faces++; t_faces++; ++fn;
    }
	a->cm.face.resize( n_faces );
	t3 = clock();	
	//post-processing refinement
    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm );
    vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex( a->cm );
    vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex( a->cm );
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    a->cm.face.DisableColor();
	Log(GLLogStream::DEBUG, "**********************" );
    Log(GLLogStream::FILTER, "End - Remove %d faces from patch - Created %d new faces - Timing T1 %d T2 %d T3 %d", c_faces, t_faces, t1-start, t2-t1, t3-t2);

    return true;
}

Q_EXPORT_PLUGIN(FilterZippering)
