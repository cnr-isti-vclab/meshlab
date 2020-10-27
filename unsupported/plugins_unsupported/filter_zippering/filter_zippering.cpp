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

#include "filter_zippering.h"

#include <vcg/simplex/face/topology.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/jumping_pos.h>
#include <vcg/complex/append.h>
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/hole.h>
#include <vcg/complex/algorithms/geodesic.h>
#include <vcg/space/intersection2.h>
#include <wrap/gl/glu_tesselator.h>


#include <ctime>

using namespace vcg;
using namespace std;

#define MAX_LOOP 150
//function added to map call sto distance
template <class ScalarType>
ScalarType FilterZippering::SquaredDistance( vcg::Segment3<ScalarType> &s, vcg::Point3<ScalarType> &p)
{
	Point3<ScalarType> clos;
	ScalarType dist;
	vcg::SegmentPointDistance<CMeshO::ScalarType>(s, p, clos,dist );
	return dist;
}

template <class ScalarType>
vcg::Point3<ScalarType> FilterZippering::ClosestPoint( vcg::Segment3<ScalarType> &s, vcg::Point3<ScalarType> &p)
{
	Point3<ScalarType> clos;
	ScalarType dist;
	vcg::SegmentPointDistance<CMeshO::ScalarType>(s, p, clos,dist );
	return clos;
}

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
    case FP_REDUNDANCY:  return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY;
    case FP_ZIPPERING :  return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEMARK | MeshModel::MM_VERTCOLOR | MeshModel::MM_VERTQUALITY;
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
                                parlst.addParam( new RichMesh("FirstMesh", md.mm(), &md, "Source Mesh", "The mesh with holes.") );
                                parlst.addParam( new RichMesh("SecondMesh", md.mm(), &md, "Target Mesh", "The mesh that will be used as patch.") );
                                parlst.addParam( new RichAbsPerc("distance", maxVal*0.01, 0, maxVal, "Max distance", "Max distance between mesh and path") );
                                parlst.addParam( new RichBool("UseQuality", false, "Use quality to select redundant face", "If selected, previously computed face quality will be used in order to select redundant faces.") );
                                //parlst.addParam( new RichBool("FastErosion", false, "Use fast erosion algorithm", "If selected, improves the speed-up of algorithm (results may be not accurate). Useful for large meshes.") );
                                parlst.addParam( new RichBool("FullProcessing", false, "Process the whole Target Mesh", "If selected, redundancy test is performed over the whole surface of the mesh") );
                                break;

			case FP_ZIPPERING :
								//get diagonals
								foreach (target, md.meshList) {
									if ( target->cm.bbox.Diag() > maxVal ) maxVal = target->cm.bbox.Diag();
									if (target != md.mm())  break;
								}
								parlst.addParam( new RichMesh("FirstMesh", md.mm(), &md, "Mesh (with holes)", "The mesh with holes.") );
								parlst.addParam( new RichMesh("SecondMesh", target, &md, "Patch", "The mesh that will be used as patch.") );
								parlst.addParam( new RichAbsPerc("distance", maxVal*0.01, 0, maxVal, "Max distance", "Max distance between mesh and path") );
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
	//no border edge find; check edge 0
	if ( i == 3 ) i = 0;

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
        if ( nearestF->IsD() )					return false;	//face is deleted
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
    if ( nearestF->IsD() )					return false;	 //face has been removed
    if ( nearestF->IsS() )					return false;	 //face has been selected

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
            if ( nearestF->IsD() )					return false;	//face has been removed
            if ( nearestF->IsS() )					return false;	//face has been selected
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

	Point3m qp = Barycenter(*f); //f barycenter
	//search for max_edge
	Scalarm max_edge = max( Distance<Scalarm>(f->P(0),f->P(1)), max( Distance<Scalarm>(f->P(1),f->P(2)), Distance<Scalarm>(f->P(2),f->P(0)) ) );
	Scalarm dist = max_dist; 
    CMeshO::FacePointer nearestF = 0; 
    Point3m closest;
	tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); UnMarkAll(m->cm);
	face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
	nearestF =  grid.GetClosest(PDistFunct, markerFunctor, qp, max_dist, dist, closest);
	if (nearestF == 0) return false;	//too far away
	Scalarm min_q = min( nearestF->V(0)->Q(), min( nearestF->V(1)->Q(), nearestF->V(2)->Q() ) ); //min distance of nearestF's vertices from M-border
	Scalarm max_q = max( f->V(0)->Q(), max( f->V(1)->Q(), f->V(2)->Q() ) );					   //max distance of F's vertices from A-border
	if ( min_q <= max_edge ) return false;
	if (test) if ( min_q <= max_q ) return false;
	return true;
}

/* Check if point is on border of face f.
 * @param point The query-point
 * @param f     Face containing point
 * @return      true if point lies on a border edge or vertex of f, false otherwise
 */
bool FilterZippering::isOnBorder( Point3<CMeshO::ScalarType> point, CMeshO::FacePointer f )  {
	// for each edge, calculates distance point-edge
	if ( f == 0 ) return false;	//null face

    //compute barycentric coords
  Point3m bc;
  InterpolationParameters<CMeshO::FaceType, CMeshO::ScalarType>( *f, f->N(), point, bc );
    //search for max and min
  int min_el = min_element(&bc[0], &bc[0]+3) - &bc[0];
  int max_el = max_element(&bc[0], &bc[0]+3) - &bc[0];
    //coords of max el = 1.0f -> check vertex
    if ( bc[max_el] >= 1.0f - eps ) return isBorderVert( f, max_el );
    //coords of min_el = 0.0f -> check edge
    if ( bc[min_el] <= 0.0f + eps ) return ( face::IsBorder( *f, (min_el+1)%3 ) || f->FFp((min_el+1)%3)->IsS() );

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
                                    Point3<CMeshO::ScalarType> N,                          //face normal (useful for proiection)
                                    vector<Point3< CMeshO::ScalarType> > &/*coords*/,                     //output coords
                                    vector<int> &pointers ) {                              //output triangles
    // rotation matrix (will be used for projection on plane)
    Matrix44m rot_matrix;
    rot_matrix.SetRotateRad( Angle<CMeshO::ScalarType>( N, Point3m(0.0, 0.0, 1.0) ), N ^ Point3m(0.0, 0.0, 1.0) );
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
                Scalarm EPSILON=info.trash[0].edges[k].Length()/100.f;
                Point3m clos;
                Scalarm dist;
                vcg::SegmentPointDistance( info.trash[0].edges[k],info.border[i].edges[j].P0(),clos,dist);
                if (dist<EPSILON)
                {
        //if ( SquaredDistance<float>( info.trash[0].edges[k], info.border[i].edges[j].P0() ) == 0.0f ) { //approxim
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
        if ( c == -1 ) //no intersection
            continue;
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
        vector< Point3m > points;  //coords vector
        vector< int > vertices;          //vertices vector
        for ( size_t j = 0; j < info.conn[i].edges.size(); j ++ )  {
            points.push_back( info.conn[i].edges[j].P0() );
            vertices.push_back( info.conn[i].verts[j].first );
        }
        if ( points.size() < 3 ) continue;
        vector< int > indices; int iters = 0;
        vector< vector< Point3m > > outlines; outlines.push_back( points );
        while ( indices.size() == 0 && ++iters < MAX_LOOP ) {
            glu_tesselator::tesselate( outlines, indices );    //glu tessellator
            if ( indices.size() == 0 )
                for ( size_t k = 0; k < outlines[0].size(); k ++ ) outlines[0][k] = outlines[0][k] * 10.0; //glu tessellator doesn't work properly for close points, so we scale coords in order to obtain a triangulation, if needed
        }
        for ( size_t k = 0; k < indices.size(); k += 3 )  {
            if ( (vertices[indices[k]] != vertices[indices[k+1]]) &&
                 (vertices[indices[k]] != vertices[indices[k+2]]) &&
                 (vertices[indices[k+1]] != vertices[indices[k+2]] ) ) {
                pointers.push_back( vertices[indices[k]] );    //save indices, in order to create new faces
                pointers.push_back( vertices[indices[k+1]] );
                pointers.push_back( vertices[indices[k+2]] );
            }
        }
    }
}

/* Split a component into two component, using border as separating line. Discard component external to border polyline.
 * @param comp To-be-split component
 * @param border Border edges
 * @param rot_mat Rotation matrix, needed to project comp points on z=k plane
 */

polyline FilterZippering::cutComponent( polyline comp,                                    //Component to be cut
                                        polyline border,                                  //border
                                        Matrix44<CMeshO::ScalarType> rot_mat ) {     //Rotation matrix

  Point3m startpoint = border.edges.front().P0();
    Point3m endpoint = border.edges.back().P1();
  Point2m startpoint2D ( (rot_mat * startpoint).X(), (rot_mat * startpoint).Y() );
    Point2m endpoint2D ( (rot_mat * endpoint).X(), (rot_mat * endpoint).Y() );
    //int startedge = 0, endedge = 0; float min_dist_s = SquaredDistance<CMeshO::ScalarType>( comp.edges[0], startpoint ), min_dist_e = SquaredDistance<CMeshO::ScalarType>( comp.edges[0], endpoint );
        int startedge = 0, endedge = 0;
        Point3m clos;
        Scalarm dist;
        Scalarm min_dist_s; vcg::SegmentPointDistance<CMeshO::ScalarType>( comp.edges[0], startpoint, clos,min_dist_s );
        Scalarm min_dist_e; vcg::SegmentPointDistance<CMeshO::ScalarType>( comp.edges[0], endpoint, clos,min_dist_e );
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
    if ( startedge == endedge && !Convex<CMeshO::ScalarType>( startpoint2D, Point2m ( (rot_mat * border.edges.front().P1()).X(), (rot_mat * border.edges.front().P1()).Y() ) , endpoint2D ) ) {
        Segment3m join( endpoint, startpoint );
        p.edges.push_back( join ); p.verts.push_back( make_pair( border.verts.back().second, border.verts.front().first ) ); //Vertex pointers
        return p;
    }

    // startedge!=endedge
    // search point on the right, create oriented segment and go on
    int step = -1;
    Point3m c0 = border.edges.back().P0();
    vector< Segment3m >::iterator edge_it = border.edges.end(); edge_it--;
    //too short segment; not reliable
    while ( Distance<Scalarm>( rot_mat * c0, rot_mat * endpoint ) <= 5.0 * eps ) {
        //previous
        c0 = (*edge_it).P0();
        if (edge_it != border.edges.begin()) edge_it--; else break;
    }
    if ( v_end ) {
        if ( !Convex<CMeshO::ScalarType>( Point2m ( (rot_mat * c0).X(), (rot_mat * c0).Y() ), endpoint2D,
                                               Point2m ( (rot_mat * comp.edges[(endedge+comp.edges.size()-1)%comp.edges.size()].P0()).X(), (rot_mat * comp.edges[(endedge+comp.edges.size()-1)%comp.edges.size()].P0()).Y() ) ) ) {
            step = comp.edges.size() - 1;
        }
        else {
            step = comp.edges.size() + 1;
            Segment3m s( comp.edges[endedge].P0(), comp.edges[endedge].P1() );
            p.edges.push_back( s ); step = comp.edges.size() + 1;
            p.verts.push_back( make_pair( comp.verts[endedge].first, comp.verts[endedge].second ) );
        }
    }
    else {
        if ( !Convex<CMeshO::ScalarType>(   Point2m ( (rot_mat * c0).X(), (rot_mat * c0).Y() ), endpoint2D,
                                                 Point2m ( (rot_mat * comp.edges[endedge].P0()).X(), (rot_mat * comp.edges[endedge].P0()).Y() ) ) ) {
            Segment3m s( endpoint, comp.edges[endedge].P0() );
            p.edges.push_back( s ); step = comp.edges.size() - 1;
            p.verts.push_back( make_pair(border.verts.back().second, comp.verts[endedge].first ) );
        }
        else {
            Segment3m s( endpoint, comp.edges[endedge].P1() );
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
            Segment3m s( comp.edges[startedge].P0() , comp.edges[startedge].P1() );
            p.edges.push_back( s );
            p.verts.push_back( make_pair ( comp.verts[startedge].first , comp.verts[startedge].second ) );
        }
    }
    else {
        Segment3m s( p.edges.back().P1() , startpoint );
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
                        Point3<CMeshO::ScalarType> P0,			//Start border point
                        Point3<CMeshO::ScalarType> P1,			//End border point
                                                bool &conn ) {
    int nearestC = -1; int nearestT = -1;
    Scalarm distanceC = 100000*eps; 
    Scalarm distanceT = 100000*eps;

	for ( int i = 0; i < info.nCComponent(); i ++ ) {
		//for each ccon search for edges nearest to P0 and P1
		Scalarm distP0 = 200000*eps; 
        Scalarm distP1 = 200000*eps;
	for ( size_t j = 0; j < info.conn[i].edges.size(); j ++ ) {
	  //if ( SquaredDistance<float>( info.conn[i].edges[j], P0 ) < distP0 ) distP0 = SquaredDistance<float>( info.conn[i].edges[j], P0 );
			Scalarm dist_test;
			Point3m clos;
			vcg::SegmentPointSquaredDistance(info.conn[i].edges[j],P0,clos,dist_test);
			if (dist_test<distP0)
				distP0=dist_test;

	  //if ( SquaredDistance<float>( info.conn[i].edges[j], P1 ) < distP1 ) distP1 = SquaredDistance<float>( info.conn[i].edges[j], P1 );
			vcg::SegmentPointSquaredDistance(info.conn[i].edges[j],P1,clos,dist_test);
			if (dist_test<distP1)
				distP1=dist_test;
		}
		if ( distP0 + distP1 < distanceC ) { distanceC = distP0 + distP1; nearestC = i; }
	}

  for ( size_t i = 0; i < info.nTComponent(); i ++ ) {
        //for each trash search for edges nearest to P0 and P1
        Scalarm distP0 = 200000*eps; 
        Scalarm distP1 = 200000*eps;
    for ( size_t j = 0; j < info.trash[i].edges.size(); j ++ ) {
      if ( SquaredDistance<Scalarm>( info.trash[i].edges[j], P0 ) < distP0 ) 
          distP0 = SquaredDistance<Scalarm>( info.trash[i].edges[j], P0 );
      if ( SquaredDistance<Scalarm>( info.trash[i].edges[j], P1 ) < distP1 ) 
          distP1 = SquaredDistance<Scalarm>( info.trash[i].edges[j], P1 );
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
										 Segment3<CMeshO::ScalarType> edge,					//edge
										 int last_split,							//previously splitted edge
										 int &splitted_edge,						//currently splitted edge
										 Point3<CMeshO::ScalarType> &hit ) {	//approximate intersection point
	if ( currentF == NULL ) return false;
	splitted_edge = -1;
	Plane3<CMeshO::ScalarType> plane; plane.Init( currentF->P(0), currentF->N() ); //projection plane
	Matrix44m rot_m; Point2m pt;	//matrix
	rot_m.SetRotateRad( Angle<CMeshO::ScalarType>( currentF->N(), Point3m(0.0, 0.0, 1.0) ), currentF->N() ^ Point3m(0.0, 0.0, 1.0) );
	Segment2m s(   Point2m((rot_m * plane.Projection(edge.P0())).X(), (rot_m * plane.Projection(edge.P0())).Y()),
				   Point2m((rot_m * plane.Projection(edge.P1())).X(), (rot_m * plane.Projection(edge.P1())).Y()) );	//projects edge on plane
	for ( int e = 0; e < 3; e ++ ) {
		if ( e != last_split && SegmentSegmentIntersection( s, Segment2m( Point2m( (rot_m * currentF->P(e)).X(), (rot_m * currentF->P(e)).Y() ),
																		  Point2m( (rot_m * currentF->P1(e)).X(), (rot_m * currentF->P1(e)).Y() ) ), pt ) ) {
			splitted_edge = e; break;
		}
	}
	if (splitted_edge == -1) return false;	//No intersection!
	// search intersection point (approximation)
	Segment3<CMeshO::ScalarType> b_edge( currentF->P(splitted_edge), currentF->P1(splitted_edge) );
	int sampleNum = SAMPLES_PER_EDGE; 
    Scalarm step = 1.0 / (sampleNum + 1);
	Point3m closest;    
    Scalarm min_dist = b_edge.Length();
	for ( int k = 0; k <= sampleNum; k ++ ) {
		Point3m currentP = edge.P0() + (edge.P1() - edge.P0())*(k*step);
		if ( SquaredDistance( b_edge, currentP ) < min_dist ) {
			closest = currentP; min_dist = SquaredDistance( b_edge, closest );
		}
	}
	if ( min_dist >= b_edge.Length() ) return false; //point not found
	hit = ClosestPoint<Scalarm>(b_edge, closest); //projection on edge
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
	//full process mode: store all faces of target mesh in the in the queue
	if ( fullProcess ) {
		//store all the faces from B
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

int FilterZippering::preProcess (vector< std::pair<CMeshO::FacePointer,char> >& queue,	//queue
								 MeshModel* a,
								 MeshModel* b,
								 MeshFaceGrid grid_a,									//grid on A
								 MeshFaceGrid grid_b,									//grid on A
								 float max_dist ) {										//max dist search

	//face count
	int fc = 0;

	// update distance from border (A)
	a->updateDataMask(MeshModel::MM_VERTFACETOPO);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(a->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(a->cm);
	//vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(a->cm);
	vcg::tri::Geodesic<CMeshO>::DistanceFromBorder(a->cm);
	a->updateDataMask(-MeshModel::MM_VERTFACETOPO);	//is that correct?
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	// update distance from border (B)
	b->updateDataMask(MeshModel::MM_VERTFACETOPO);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(b->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(b->cm);
//	vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(b->cm);
	vcg::tri::Geodesic<CMeshO>::DistanceFromBorder(b->cm);
	b->updateDataMask(-MeshModel::MM_VERTFACETOPO); //is that correct?
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
	//perform simpleCheckRedundancy on faces of the queue
	for ( int i = 0; i < queue.size(); i ++ ) {
		if ( queue[i].second == 'B' ) {
			if ( simpleCheckRedundancy( queue[i].first, a, grid_a, max_dist, true ) ) {
				queue[i].first->SetS(); fc++;
			}
		}
		if ( queue[i].second == 'A' ) {
			if ( simpleCheckRedundancy( queue[i].first, b, grid_b, max_dist, true ) ) {
				queue[i].first->SetS(); fc++;
			}
		}
	}
	return fc;
}

int FilterZippering::preProcess_pq ( std::priority_queue< std::pair<CMeshO::FacePointer,char>, std::vector< std::pair<CMeshO::FacePointer,char> >, compareFaceQuality >& queue,	//the queue
									MeshModel* a,
									MeshModel* b,
									MeshFaceGrid grid_a,									//grid on A
									MeshFaceGrid grid_b,									//grid on A
									float max_dist ) {										//max dist search

	//face count
	int fc = 0;

	// update distance from border (A)
	a->updateDataMask(MeshModel::MM_VERTFACETOPO);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(a->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(a->cm);
//	vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(a->cm);
	vcg::tri::Geodesic<CMeshO>::DistanceFromBorder(a->cm);

	a->updateDataMask(-MeshModel::MM_VERTFACETOPO);	//is that correct?
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	// update distance from border (B)
	b->updateDataMask(MeshModel::MM_VERTFACETOPO);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(b->cm);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromVF(b->cm);
//	vcg::tri::UpdateQuality<CMeshO>::VertexGeodesicFromBorder(b->cm);
	vcg::tri::Geodesic<CMeshO>::DistanceFromBorder(b->cm);
	b->updateDataMask(-MeshModel::MM_VERTFACETOPO); //is that correct?
	vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm);

	//tmp queue
	vector< std::pair<CMeshO::FacePointer,char> > tmp_queue;
	//copy data in a support vector
	while (!queue.empty()) {
		tmp_queue.push_back( queue.top() );
		queue.pop();
	}
	//perform simpleCheckRedundancy on faces of the vector
	for ( int i = 0; i < tmp_queue.size(); i ++ ) {
		if ( tmp_queue[i].second == 'B' ) {
			if ( simpleCheckRedundancy( tmp_queue[i].first, a, grid_a, max_dist, true ) ) {
				tmp_queue[i].first->SetS(); fc++;
			}
			//store non-redundant faces for future check
			else queue.push( tmp_queue[i] );
		}
		if ( tmp_queue[i].second == 'A' ) {
			if ( simpleCheckRedundancy( tmp_queue[i].first, b, grid_b, max_dist, true ) ) {
				tmp_queue[i].first->SetS(); fc++;
			}
			else queue.push( tmp_queue[i] );
		}
	}

	return fc;
}

/**
 * Select redundant faces from meshes A and B. A face is said to be redundant if
 * a number of samples of the face project on the surface of the other mesh.
 * @param queue Unsorted queue containing face-pointers from both meshes
 * @param a, b the meshes involved in the process
 * @param epsilon Maximum search distance
 */
int FilterZippering::selectRedundant( std::vector< std::pair<CMeshO::FacePointer,char> >& queue,	//queue
							  MeshModel* a,													//mesh A
							  MeshModel* b,													//mesh B
							  float epsilon ) {												//max search distance
	//create grid on the meshes (needed for nearest point search)
	MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid A
	MeshFaceGrid grid_b; grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid B
	//clear selection on both meshes
	tri::UpdateSelection<CMeshO>::Clear( a->cm );
	tri::UpdateSelection<CMeshO>::Clear( b->cm );
	//count selected faces
	int sf = 0;
	//fast pre processing
	sf = preProcess( queue, a, b, grid_a, grid_b, epsilon );

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
				currentF->SetS(); sf++;
				//insert adjacent faces at the beginning of the queue
				queue.push_back( make_pair(currentF->FFp(0),'A') );
				queue.push_back( make_pair(currentF->FFp(1),'A') );
				queue.push_back( make_pair(currentF->FFp(2),'A') );
			}
		}
		//face is from mesh B, test redundancy with respect to A
		else {
			if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
				//if face is redundant, remove it from B and put new border faces at the top of the queue
				//in order to guarantee that A and B will be tested alternatively
				currentF->SetS(); sf++;
				//insert adjacent faces at the beginning of the queue
				queue.push_back( make_pair(currentF->FFp(0),'B') );
				queue.push_back( make_pair(currentF->FFp(1),'B') );
				queue.push_back( make_pair(currentF->FFp(2),'B') );
			}
		}
	}
	//return number of selected faces
	return sf;
}


/**
 * Select redundant faces from meshes A and B. A face is said to be redundant if
 * a number of samples of the face project on the surface of the other mesh.
 * @param queue priority queue containing face-pointers from both meshes ordered by quality
 * @param a, b the meshes involved in the process
 * @param epsilon Maximum search distance
 */
int FilterZippering::selectRedundant_pq( std::priority_queue< std::pair<CMeshO::FacePointer,char>, std::vector< std::pair<CMeshO::FacePointer,char> >, compareFaceQuality >& queue,	//the queue
										  MeshModel* a,													//mesh A
										  MeshModel* b,													//mesh B
										  float epsilon ) {												//max search distance
	//create grid on the meshes (needed for nearest point search)
	MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.end() );  //Grid A
	MeshFaceGrid grid_b; grid_b.Set( b->cm.face.begin(), b->cm.face.end() );  //Grid B
	//clear selection on both meshes
	tri::UpdateSelection<CMeshO>::Clear( a->cm );
	tri::UpdateSelection<CMeshO>::Clear( b->cm );
	//count selected faces
	int sf = 0;
	//fast pre processing
	sf = preProcess_pq( queue, a, b, grid_a, grid_b, epsilon );

	//process face once at the time until queue is not empty
	while ( !queue.empty() ) {
		//extract face from the queue
		CMeshO::FacePointer currentF = queue.top().first;  char choose = queue.top().second; queue.pop();
		if ( currentF->IsD() || currentF->IsS() ) continue;	//no op if face is deleted or selected (already tested)
		//face from mesh A, test redundancy with respect to B
		if (choose == 'A') {
			if ( checkRedundancy( currentF, b, grid_b, epsilon ) ) {
				//if face is redundant, set is as Selectedand put new border faces in of the queue
				currentF->SetS(); sf++;
				//insert adjacent faces at the beginning of the queue
				queue.push( make_pair(currentF->FFp(0),'A') );
				queue.push( make_pair(currentF->FFp(1),'A') );
				queue.push( make_pair(currentF->FFp(2),'A') );
			}
		}
		//face is from mesh B, test redundancy with respect to A
		else {
			if ( checkRedundancy( currentF, a, grid_a, epsilon ) ) {
				//if face is redundant, remove it from B and put new border faces at the top of the queue
				//in order to guarantee that A and B will be tested alternatively
				currentF->SetS(); sf++;
				//insert adjacent faces at the beginning of the queue
				queue.push( make_pair(currentF->FFp(0),'B') );
				queue.push( make_pair(currentF->FFp(1),'B') );
				queue.push( make_pair(currentF->FFp(2),'B') );
			}
		}
	}
	//return number of selected faces
	return sf;
}


/**
 * Search border-faces of the mesh having two or more border edges, then split them
 * if they have only two border edges or delete them if they have three border edges.
 */
int FilterZippering::refineBorder( MeshModel* m ) {
	//clear flags
	tri::UpdateFlags<CMeshO>::FaceClear(m->cm);
	//recover information about the border of the mesh
	vector< tri::Hole<CMeshO>::Info > border;
	tri::Hole<CMeshO>::GetInfo( m->cm, false, border );
	//pos container
	vector<pair<int,int> > border_pos;
	//counter
	int sf = 0;
	//store pos information (index of face and of edge) for each border of the mesh
	for ( size_t i = 0; i < border.size(); i ++ )
		border_pos.push_back( make_pair( tri::Index(m->cm, border[i].p.F()), border[i].p.E() ) );
	//explore the border
	for ( size_t i = 0; i < border_pos.size(); i ++ )  {
		//create and set a pos for the current border
		face::Pos<CMeshO::FaceType> pos;
		pos.Set( &(m->cm.face[border_pos[i].first]), //face pointer
				 border_pos[i].second,				 //edge index
				 m->cm.face[border_pos[i].first].V(border_pos[i].second) );	//vertex pointer
		CMeshO::FacePointer start = pos.F();			//keep pointer to the first
		//first case: start has three border face (spurious triangle). Remove it from m and step over
		if ( face::BorderCount(*start) == 3 ) {
			tri::Allocator<CMeshO>::DeleteFace( m->cm, *start );
			continue;
		}
		//second case: start has ore or two border edges, so we visit the whole border
		do {
			//current face has not been deleted and has two border edges
			if ( !pos.F()->IsD() && face::BorderCount(*pos.F()) >= 2 ) {
				sf++;
				//pointer updater
				tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
				tri::Allocator<CMeshO>::PointerUpdater<CMeshO::FacePointer> fpu;
				//add 4 new faces that will replace p.F() and an adjacent face
				CMeshO::FaceIterator f = tri::Allocator<CMeshO>::AddFaces( m->cm, 4, fpu );
				//update face pointers if necessary
				if ( fpu.NeedUpdate() ) { fpu.Update( pos.F() ); fpu.Update( start ); }
				//add 1 new vertex
				CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( m->cm, 1, vpu );
				//update vertex pointer if necessary
				if ( vpu.NeedUpdate() )
					vpu.Update( pos.V() );
				//search non-border edge;
				int j; for (j=0; j<3 && face::IsBorder(*pos.F(), j); j++); assert( j < 3 );
				//opposite face (vill be removed) and half-edge
				CMeshO::FacePointer opp_f = pos.F()->FFp(j); int opp_j = pos.F()->FFi(j);
				//place new vertex in the midpoint of the non-border edge
				(*v).P() = (pos.F()->P(j) + pos.F()->P1(j))/2.0;
				//create new faces
				CMeshO::FacePointer f1 = &*f; f1->V(0) = pos.F()->V(j);			f1->V(1) = &(*v);				f1->V(2) = pos.F()->V2(j); ++f;
				CMeshO::FacePointer f2 = &*f; f2->V(0) = pos.F()->V2(j);		f2->V(1) = &(*v);				f2->V(2) = pos.F()->V1(j); ++f;
				CMeshO::FacePointer f3 = &*f; f3->V(0) = pos.F()->V(j);			f3->V(1) = opp_f->V2(opp_j);	f3->V(2) = &(*v); ++f;
				CMeshO::FacePointer f4 = &*f; f4->V(0) = opp_f->V2(opp_j);		f4->V(1) = pos.F()->V1(j);		f4->V(2) = &(*v);
				//attach face f1 - Not working, why?
				//face::Attach<CMeshO::FaceType>( f1, 0, f3, 2 );
				//face::Attach<CMeshO::FaceType>( f1, 1, f2, 0 );
				//face::Attach<CMeshO::FaceType>( f1, 2, f1, 2 );
				//create topology for the new faces
				f1->FFp(0) = f3; f1->FFp(1) = f2; f1->FFp(2) = f1; f1->FFi(0) = 2; f1->FFi(1) = 0; f1->FFi(2) = 2;
				f2->FFp(0) = f1; f2->FFp(1) = f4; f2->FFp(2) = f2; f2->FFi(0) = 1; f2->FFi(1) = 1; f2->FFi(2) = 2;
				f3->FFp(0) = face::IsBorder(*opp_f, (opp_j+1)%3)? f3 : opp_f->FFp((opp_j+1)%3); f3->FFp(1) = f4; f3->FFp(2) = f1; f3->FFi(0) = face::IsBorder(*opp_f, (opp_j+1)%3)? 0 : opp_f->FFi((opp_j+1)%3); f3->FFi(1) = 2; f3->FFi(2) = 0;
				f4->FFp(0) = face::IsBorder(*opp_f, (opp_j+2)%3)? f4 : opp_f->FFp((opp_j+2)%3); f4->FFp(1) = f2; f4->FFp(2) = f3; f4->FFi(0) = face::IsBorder(*opp_f, (opp_j+2)%3)? 0 : opp_f->FFi((opp_j+2)%3); f4->FFi(1) = 1; f4->FFi(2) = 1;
				//f3 is border-face on the 0 edge?
				if( !face::IsBorder(*f3, 0) ) {
					f3->FFp(0)->FFp(f3->FFi(0)) = f3; f3->FFp(0)->FFi(f3->FFi(0)) = 0;
				}
				//f4 is border-face on the 0 edge?
				if( !face::IsBorder(*f4, 0) ) {
					f4->FFp(0)->FFp(f4->FFi(0)) = f4; f4->FFp(0)->FFi(f4->FFi(0)) = 0;
				}
				//store pointers to face that must be deleted
				CMeshO::FacePointer del_face_00 = pos.F();
				CMeshO::FacePointer del_face_01 = pos.F()->FFp(j);
				//troubleshooting
				//if we split start face, we have to set a new start in order to avoid infinite loop
				if ( pos.F() == start )
					start = f1;
				if ( (pos.F()->FFp(j)) == start && face::IsBorder(*f3,0) )
					start = f3;
				if ( (pos.F()->FFp(j)) == start && face::IsBorder(*f4,0) )
					start = f4;
				//it's a square, break the loop and go on
				bool square = face::BorderCount(*(pos.F())) == 2 && face::BorderCount(*(pos.F()->FFp(j))) == 2;
				//we deleted two faces, so we set a new pos using new faces
				pos.Set( f1, 2, f1->V(0) );
				//finally remove faces
				tri::Allocator<CMeshO>::DeleteFace( m->cm, *del_face_00 );
				tri::Allocator<CMeshO>::DeleteFace( m->cm, *del_face_01 );
				//it's a square, break the loop and go on
				if (square) break;
			}
			pos.NextB();
		} while( pos.F() != start );

	}
	//return number of splitted faces
	return sf;
}


/**
 * Project a face from the mesh B on the surface of the mesh A. In order to project, we need
 * to find nearest points of border-vertices of the face (we use the grid).
 * Faces that must be retriangulated or deleted are stored in two separate vectors.
 */
void FilterZippering::projectFace( CMeshO::FacePointer f,							//pointer to the face that will be projected
								   MeshModel* a,									//mesh A
								   MeshFaceGrid grid_a,								//grid on A
								   float max_dist,									//max dist search
								   map< CMeshO::FacePointer, aux_info >& map_info,	//map with auxiliar information
								   vector< CMeshO::FacePointer >& tbt_faces,		//vector to-be-triangulated faces
								   vector< CMeshO::FacePointer >& tbr_faces,		//vector to-be-removed faces
								   vector< int >& verts ) {							//indices of vertices that will define new faces

	assert( face::BorderCount( *f ) > 0 );
	int e; for ( e = 0; e < 3; e ++ ) if ( face::IsBorder(*f, e) ) break;
	assert( face::IsBorder<CMeshO::FaceType>( *f, e ) );  //Check border correctness
	//iteration counter
	int cnt = 0;
	//stack of border edges
	vector < pair< int, int > > stack;
	aux_info dummy;			//dummy info
	//store current border edge
	stack.push_back( make_pair( tri::Index( a->cm, f->V(e) ),
								tri::Index( a->cm, f->V1(e) ) ) );    //indices of border vertices
	//while there are border edges...
	while ( !stack.empty() ) {

		//avoid infinite loop
		if ( cnt++ > 2*MAX_LOOP ) {
			Log(GLLogStream::DEBUG, "Loop");
			stack.clear();
			continue;
		}

		pair< int, int > current_edge = stack.back(); stack.pop_back();   //vertex indices
		assert( current_edge.first != current_edge.second );
		tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
		face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
		MeshFaceGrid::ScalarType  dist = 2*max_dist;  MeshFaceGrid::CoordType closestStart, closestEnd;
		//search for nearest face of vertex e
		CMeshO::FacePointer startF = grid_a.GetClosest(PDistFunct, markerFunctor, a->cm.vert[current_edge.first].P(), max_dist, dist, closestStart);
		if ( fabs(dist) >= fabs(max_dist) ) startF = 0;
		dist = 2*max_dist;
		//search for nearest face of vertex e+1
		CMeshO::FacePointer endF = grid_a.GetClosest(PDistFunct, markerFunctor,  a->cm.vert[current_edge.second].P(), max_dist, dist, closestEnd);
		if ( fabs(dist) >= fabs(max_dist) ) endF = 0;

		//check if current edge projection fits together with an edge of the face
			if ( startF != 0 && endF != 0 ) {
			bool fit = false;
			int vert = -1;
			for ( int i = 0; i < 3; i ++ ) {
				if ( vcg::Distance<Scalarm>(closestStart, startF->P(i)) < eps )
					 vert = i;
			}
			if ( vert != -1 ) {
				//search around vert
				face::JumpingPos<CMeshO::FaceType> p;
				p.Set( startF, vert, startF->V(vert) );

				do {
					if ( vcg::Distance<Scalarm>(closestStart, p.V()->P()) < eps &&
						 vcg::Distance<Scalarm>(closestEnd, p.F()->P1(p.E())) < eps &&
						 face::IsBorder( *(p.F()), p.E() ) )
						 fit = true;
					p.FlipF();
					p.FlipE();
				}while (p.F() != startF);
				//we found the face!
				if (fit)
					continue;
			}
		}

		//case 00: startF and endF are null faces: no op
		if ( startF == 0 && endF == 0 ) continue;

		//case 04 (special) : vertices project on border edges.
		if ( ( isOnBorder( closestStart, startF ) && isOnBorder( closestEnd, endF ) ) ||
			 ( isOnBorder( closestStart, startF ) && endF == 0 )  ||
			 ( isOnBorder( closestEnd, endF ) && startF == 0 ) ) {
			//if closest point is really close to the border, then split border face
			if ( startF != 0 && Distance<CMeshO::ScalarType>(a->cm.vert[current_edge.first].P(), closestStart) <= eps ) {
				map_info.insert( make_pair(startF, dummy) );
				map_info[startF].addVertex( &(a->cm.vert[current_edge.first]), current_edge.first );
				tbt_faces.push_back( startF );
			}
			//check endF and eventually split it as seen before
			if ( endF != 0 && Distance<CMeshO::ScalarType>(a->cm.vert[current_edge.second].P(), closestEnd) <= eps )   {
				map_info.insert( make_pair(endF, dummy) );
				map_info[endF].addVertex( &(a->cm.vert[current_edge.second]), current_edge.second );
				tbt_faces.push_back( endF );
			}
			//check if the current_edge projects completely on border
			//if true, no operation is needed
			if ( handleBorderEdgeBB( current_edge, a, grid_a, max_dist, startF, endF, f, map_info, stack, verts ) )
				continue;
			//false: current edge will be tested later
			tbr_faces.push_back( f );
		}

		//case 05 (special): one of the border vertices doesn't project on the surface
		if ( ( startF == 0 || isOnBorder( closestStart, startF ) ) ||
			 ( endF == 0 || isOnBorder( closestEnd, endF ) ) ) {
			//if closest point is really closest to the border, split border face
			if ( startF != 0 && Distance<CMeshO::ScalarType>(a->cm.vert[current_edge.first].P(), closestStart) <= eps ) {
				map_info[startF].SetEps( eps );
				map_info[startF].Init( *startF, tri::Index( a->cm, startF->V(0) ), tri::Index( a->cm, startF->V(1) ), tri::Index( a->cm, startF->V(2) )  );
				map_info.insert( make_pair(startF, dummy) );
				map_info[startF].addVertex( &(a->cm.vert[current_edge.first]), current_edge.first );
				tbt_faces.push_back( startF );
			}
			//if closest point is really closest to the border, split border face
			if ( endF != 0 && Distance<CMeshO::ScalarType>(a->cm.vert[current_edge.second].P(), closestEnd) <= eps ) {
				map_info[endF].SetEps( eps );
				map_info[endF].Init( *endF, tri::Index( a->cm, endF->V(0) ), tri::Index( a->cm, endF->V(1) ), tri::Index( a->cm, endF->V(2) )  );
				map_info.insert( make_pair(endF, dummy) );
				map_info[endF].addVertex( &(a->cm.vert[current_edge.second]), current_edge.second );
				tbt_faces.push_back( endF );
			}

			//project one of the two vertices on the mesh
			if ( startF != 0 ) a->cm.vert[current_edge.first].P() = closestStart;
			if ( endF != 0 )a->cm.vert[current_edge.second].P() = closestEnd;
			//we're going to split current face
			tbr_faces.push_back( f );

			//choose direction of splitting (from start to end or viceversa) according to which
			//vertex doesn't project
			int dir = 0;
			if ( startF == 0 || isOnBorder( closestStart, startF ) )  dir = 1;
			//split face
			handleBorderEdgeOB( current_edge, dir, a, grid_a, max_dist, startF, endF, f, map_info, stack, tbt_faces, verts );

			continue;
		}
		//end of special cases

		//initialize info on faces (if info is already initialized, no operation is done)
		map_info.insert( make_pair(startF, dummy) );	map_info[startF].SetEps( eps ); map_info[startF].Init( *startF, tri::Index( a->cm, startF->V(0) ), tri::Index( a->cm, startF->V(1) ), tri::Index( a->cm, startF->V(2) )  );
		map_info.insert( make_pair(endF, dummy) );		map_info[endF].SetEps( eps );   map_info[endF].Init( *endF, tri::Index( a->cm, endF->V(0) ), tri::Index( a->cm, endF->V(1) ), tri::Index( a->cm, endF->V(2) )  );

		//project points on the surface of the mesh
		a->cm.vert[current_edge.first].P() = closestStart;
		a->cm.vert[current_edge.second].P() = closestEnd;

		//case 01 : startF and endF are the same face
		if ( startF == endF ) {
			//start face will be retriangulated
			tbt_faces.push_back( startF );
			//add information to startF
			handleBorderEdgeSF( current_edge, a, startF, endF, f, map_info, stack, verts );
			continue;
		}

		//case 02: startF is adjacent to endF
		if ( isAdjacent( startF, endF ) ) {
			//start face and end face will be retriangulated
			tbt_faces.push_back( startF );
			tbt_faces.push_back( endF );
			//also, current face will be splitted and replaced by other two faces
			tbr_faces.push_back( f );
			//split face and update information
			handleBorderEdgeAF( current_edge, a, startF, endF, f, map_info, stack, verts );
			continue;

		}

		//case 03: startF is not adjacent to endF
		if ( !isAdjacent( startF, endF ) ) {
			//start face and end face will be retriangulated
			tbt_faces.push_back( startF );
			tbt_faces.push_back( endF );
			//also, current face will be splitted and replaced by other two faces
			tbr_faces.push_back( f );
			//split face and add edges to the stack
			handleBorderEdgeNF( current_edge, a, startF, endF, f, map_info, stack, verts );
			continue;
		}

	}


}

//case 01: vertices of border edge project on the same face
void FilterZippering::handleBorderEdgeSF ( pair< int, int >& current_edge,					//current border edge
										   MeshModel* a,									//mesh A
										   CMeshO::FacePointer startF,						//face where first vertex lies
										   CMeshO::FacePointer endF,						//face where second vertex lies (=startF)
										   CMeshO::FacePointer splittingF,					//splitting face
										   map< CMeshO::FacePointer, aux_info >& map_info,	//map with auxiliar information
										   vector < pair< int, int > >& stack,				//stack containing border edges
										   vector< int >& verts )  {						//vector of indices

	//border edge
	int e; for ( e = 0; e < 3; e ++ ) if ( face::IsBorder(*splittingF, e) ) break;

	//add information to startF
	if( map_info[startF].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[current_edge.first].P(), a->cm.vert[current_edge.second].P() ),
									  make_pair( current_edge.first, current_edge.second ) ) ) {
		//avoid duplicate faces
		if ( current_edge.first == tri::Index( a->cm, splittingF->V(e) ) &&
			 current_edge.second == tri::Index( a->cm, splittingF->V1(e) ) ) return;
		//store indices of vertices of the new face
		verts.push_back( current_edge.first );
		verts.push_back( current_edge.second );
		verts.push_back( tri::Index( a->cm, splittingF->V2(e) ) );
	}
}



//case 02: vertices of border edge project on adjacent faces
void FilterZippering::handleBorderEdgeAF ( pair< int, int >& current_edge,						//current border edge
										   MeshModel* a,										//mesh A
										   CMeshO::FacePointer startF,							//face where first vertex lies
										   CMeshO::FacePointer endF,							//face where second vertex lies
										   CMeshO::FacePointer splittingF,						//splitting face
										   map< CMeshO::FacePointer, aux_info >& map_info,		//map with auxiliar information
										   vector < pair< int, int > >& stack,					//stack containing border edges
										   vector< int >& verts )  {							//vector of indices

        //calc. intersection point (approximate) and split edge
        int shared; for ( int k = 0; k < 3; k ++ ) if ( startF->FFp(k) == endF ) shared = k;
        Segment3<CMeshO::ScalarType> shared_edge( startF->P(shared), startF->P1(shared) );
        int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1);
        Point3m closest;    
        Scalarm min_dist = shared_edge.Length();
        //subsample the current border edge and search for point which is closest to the edge shared by startF and endF
        for ( int k = 0; k <= sampleNum; k ++ ) 
        {
            Point3m currentP = a->cm.vert[current_edge.first].P() + ( a->cm.vert[current_edge.second].P() - a->cm.vert[current_edge.first].P() ) * (k*step);
            if ( SquaredDistance( shared_edge, currentP ) < min_dist ) 
            {
                closest = currentP;
                min_dist = SquaredDistance( shared_edge, closest );
            }
        }
        //have we found something?
        assert( SquaredDistance( shared_edge, closest ) < shared_edge.Length() );

        //closest (pseudo-intersection) point
        closest = ClosestPoint(shared_edge, closest);

		//border edge
		int e; for ( e = 0; e < 3; e ++ ) if ( face::IsBorder(*splittingF, e) ) break;

        //no close vertices, add information to faces
        tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
        CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
        if ( map_info[startF].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[current_edge.first].P(), (*v).P() ),
                                           make_pair( current_edge.first, v - a->cm.vert.begin() ) ) ) {
            verts.push_back( v - a->cm.vert.begin() );
            verts.push_back( tri::Index( a->cm, splittingF->V2(e) ) );
            verts.push_back( current_edge.first );   //new triangle
        }
        if ( map_info[endF].AddToBorder( Segment3<CMeshO::ScalarType> ( (*v).P(), a->cm.vert[current_edge.second].P() ),
                                         make_pair( v - a->cm.vert.begin(), current_edge.second ) ) ) {
            verts.push_back(v - a->cm.vert.begin());
            verts.push_back( current_edge.second );
            verts.push_back( tri::Index( a->cm, splittingF->V2(e) ) );	   //new triangle
        }
}

//case 03: vertices of border edge project on non-adjacent faces
void FilterZippering::handleBorderEdgeNF ( pair< int, int >& current_edge,						//current border edge
										   MeshModel* a,										//mesh A
										   CMeshO::FacePointer startF,							//face where first vertex lies
										   CMeshO::FacePointer endF,							//face where second vertex lies
										   CMeshO::FacePointer splittingF,						//splitting face
										   map< CMeshO::FacePointer, aux_info >& map_info,		//map with auxiliar information
										   vector < pair< int, int > >& stack,					//stack containing border edges
										   vector< int >& verts )  {
	//border edge
	int e; for ( e = 0; e < 3; e ++ ) if ( face::IsBorder(*splittingF, e) ) break;

    //startF and endF are not adjacent, but they could share a vertex...
    int w = sharesVertex( startF, endF );
    int cnt = 0;
    Segment3<CMeshO::ScalarType> s( a->cm.vert[current_edge.first].P(), a->cm.vert[current_edge.second].P() );
    //if they share a vertex and border edge pass trough the vertex, we split current face using the vertex
    if ( w != -1 && (cnt++ == MAX_LOOP || SquaredDistance<Scalarm>( s, startF->P(w) ) <= eps) ) {
        //too short and it's a vertex, do nothing
        if ( s.Length() < eps && vcg::Distance<Scalarm>( s.P0(), startF->P(w) ) < eps )
            return;
        tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
        CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = startF->P(w);
        //add information to startF
        if ( map_info[startF].AddToBorder( Segment3<CMeshO::ScalarType> ( a->cm.vert[current_edge.first].P(), (*v).P() ),
                                           make_pair( current_edge.first, v - a->cm.vert.begin() ) ) ) {
            verts.push_back( current_edge.first );
            verts.push_back( v - a->cm.vert.begin() );
            verts.push_back( tri::Index( a->cm, splittingF->V2(e) ) );
        }
        //add information to endF
        if ( map_info[endF].AddToBorder( Segment3<CMeshO::ScalarType> ( (*v).P(),  a->cm.vert[current_edge.second].P() ),
                                    make_pair( v - a->cm.vert.begin(), current_edge.second ) ) ) {
             verts.push_back( v - a->cm.vert.begin() );
             verts.push_back( current_edge.second );
             verts.push_back( tri::Index( a->cm, splittingF->V2(e) ) );
        }
        //stop
        return;
    }

	//add new vertex on the mid-point
	tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
	CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
	(*v).P() = (a->cm.vert[current_edge.first].P() + a->cm.vert[current_edge.second].P())/2.00;
	stack.push_back( make_pair( current_edge.first, v - a->cm.vert.begin() ) );
	stack.push_back( make_pair( v - a->cm.vert.begin(), current_edge.second ) );
}

//case 04: both vertices of border edge project on border edge
//return true if the whole current_edge project on border edge (no split needed)
bool FilterZippering::handleBorderEdgeBB ( std::pair< int, int >& current_edge,					//current border edge
										   MeshModel* a,										//mesh A
										   MeshFaceGrid grid_a,									//grid on A (needed for sampling)
										   float max_dist,										//max search dist (needed for sampling)
										   CMeshO::FacePointer startF,							//face where first vertex lies
										   CMeshO::FacePointer endF,							//face where second vertex lies
										   CMeshO::FacePointer splittingF,						//splitting face
										   std::map< CMeshO::FacePointer, aux_info >& map_info,	//map with auxiliar information
										   std::vector < std::pair< int, int > >& stack,		//stack containing border edges
										   std::vector< int >& verts ) {						//vector of indices

	//Verify if the whole segment is on border
	tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
	face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
	int sampleNum = SAMPLES_PER_EDGE; 
    Scalarm step = 1.0/(sampleNum+1); 
    bool border = true;
	Point3m closestP;   
    Scalarm dist = 2*max_dist;
	for ( int k = 0; k <= sampleNum; k ++ ) 
    {
		Point3m currentP = a->cm.vert[current_edge.first].P() + ( a->cm.vert[current_edge.second].P() - a->cm.vert[current_edge.first].P() ) * (k*step);
		CMeshO::FacePointer closestFace = grid_a.GetClosest(PDistFunct, markerFunctor, currentP, 2*max_dist, dist, closestP); //closest point on mesh
		if ( !isOnBorder( closestP, closestFace ) ) return false;	//not completely on border; will be splitted later
	}

	//border edge of splittingF
	int e; for ( e = 0; e < 3; e ++ ) if ( face::IsBorder(*splittingF, e) ) break;

	//actualFace: no operation needed
	if ( a->cm.vert[current_edge.first].P() == splittingF->P(e) &&
		 a->cm.vert[current_edge.second].P() == splittingF->P1(e) ) {}
	else {//create new triangle to file small hole
			verts.push_back( current_edge.second );
			verts.push_back( tri::Index( a->cm, splittingF->V2(e) ) );
			verts.push_back( current_edge.first );   //new triangle
	}

	return true;
}



//case 05: one of the vertices doesn't project on the surface of the mesh
void FilterZippering::handleBorderEdgeOB ( std::pair< int, int >& current_edge,						//current border edge
										   int direction,											//split direction (1 from start to end, 0 from end to start)
										   MeshModel* a,											//mesh A
										   MeshFaceGrid grid_a,										//grid on A (needed for sampling)
										   float max_dist,											//max search dist (needed for sampling)
										   CMeshO::FacePointer startF,								//face where first vertex lies
										   CMeshO::FacePointer endF,								//face where second vertex lies
										   CMeshO::FacePointer splittingF,							//splitting face
										   map< CMeshO::FacePointer, aux_info >& map_info,			//map with auxiliar information
										   vector < pair< int, int > >& stack,						//stack containing border edges
										   vector< CMeshO::FacePointer >& tbt_faces,			    //stack containing pointers to face that wille be retriangulated
										   vector< int >& verts ) {									//vector of indices

		assert( direction == 1 || direction == 0 );
		//index of last splitted edge
		int last_split = -1;
		//face currently examinated
		CMeshO::FacePointer currentF = direction? startF : endF;
		//stop flag
		bool stop = false;

		//border edge of splittingF
		int e; for ( e = 0; e < 3; e ++ ) if ( face::IsBorder(*splittingF, e) ) break;

		//indices of vertices involved in the process
		int startV = direction ? current_edge.first : current_edge.second;
		int endV = direction ? current_edge.second : current_edge.first;
		int thirdV = tri::Index( a->cm, splittingF->V2(e) );

		int cnt = 0;
		aux_info dummy;

		do {
			int tosplit;
			Point3m closest;
			cnt++;
			if (!findIntersection( currentF, Segment3<CMeshO::ScalarType>(a->cm.vert[current_edge.first].P(),a->cm.vert[current_edge.second].P()), last_split, tosplit, closest )) {
				stop = true; //no op
			}
			else {
				//we found an intersection, so split the face using intersection point
				last_split = currentF->FFi( tosplit );
				tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
				//new vertex on the splitting point
				CMeshO::VertexIterator v = tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
				//init currentF aux_information
				map_info.insert( make_pair(currentF, dummy) );
				map_info[currentF].Init( *currentF, tri::Index(a->cm, currentF->V(0)), tri::Index(a->cm, currentF->V(1)), tri::Index(a->cm, currentF->V(2)) );
				//add information to currentF
				if ( map_info[currentF].AddToBorder( Segment3<CMeshO::ScalarType> ( (*v).P(), a->cm.vert[startV].P() ), make_pair( v - a->cm.vert.begin(), startV ) ) ) {
						tbt_faces.push_back( currentF );
						verts.push_back( startV );
						verts.push_back( thirdV );
						verts.push_back( v - a->cm.vert.begin() );

				}
				//startV = v - a->cm.vert.begin(); //why?
				if ( face::IsBorder( *currentF, tosplit ) )  { //border reached
					verts.push_back( thirdV );
					verts.push_back( endV );
					verts.push_back( v - a->cm.vert.begin() );

					stack.push_back( make_pair( endV, v - a->cm.vert.begin() ) );

					stop = true;
				}
				else currentF = currentF->FFp(tosplit);
			}
		} while (!stop && cnt < MAX_LOOP);
}


/* Zippering of two meshes
 * Given two mesh, a mesh with one or more holes (A) and a second mesh, a patch (B), fill a hole onto m surface
 * using faces of patch. Algorithm const of three steps:
 * - CheckRedundancy: delete face of patch that can be projected completely on A's surface
 * - PatchRefinement: patch vertices are projected on A, and border edges are splitted;
 * - MeshRefinement: faces where patch vertices lie are re-triangulated
 * Based on Controlled and Adaptive mesh zippering
 */
bool FilterZippering::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, CallBackPos /**cb*/)
{
    //get the meshes
    MeshModel* a = par.getMesh("FirstMesh");
    MeshModel* b = par.getMesh("SecondMesh");

    if ( a == b )   {
          errorMessage="Please add a second mesh";
          return false;
      }

	//enable FF adjacency, mark, compute normals for face (both on A and B)
	a->updateDataMask(MeshModel::MM_FACEFACETOPO + MeshModel::MM_FACEMARK + MeshModel::MM_FACEQUALITY + MeshModel::MM_FACECOLOR + MeshModel::MM_VERTQUALITY);
	b->updateDataMask(MeshModel::MM_FACEFACETOPO + MeshModel::MM_FACEMARK + MeshModel::MM_FACEQUALITY + MeshModel::MM_FACECOLOR + MeshModel::MM_VERTQUALITY);
	tri::UnMarkAll(a->cm);
	tri::UnMarkAll(b->cm);

	tri::UpdateNormal<CMeshO>::PerFaceNormalized(a->cm);
	tri::UpdateNormal<CMeshO>::PerVertexNormalized(a->cm);
	tri::UpdateNormal<CMeshO>::PerFaceNormalized(b->cm);
	tri::UpdateNormal<CMeshO>::PerVertexNormalized(b->cm);
	//fixed eps
	eps = 0.00001f;

	clock_t t1, t2;
	t1 = clock();

	/**
	 * Filter Redundancy: Select redundant faces from the surface of the meshes.
	 */
	if (ID(filter) == FP_REDUNDANCY) {
		//queues
		vector< pair<CMeshO::FacePointer,char> > generic_queue; //unsorted queue
		priority_queue< pair<CMeshO::FacePointer,char>, vector< pair<CMeshO::FacePointer,char> >, compareFaceQuality > priority_queue;	//priority queue

		//if user chooses to usequality, initialize priority queue...
		if ( par.getBool("UseQuality") )  {
			if ( !Init_pq( priority_queue, a, b, par.getBool("FullProcessing") ) ) {
				Log("Target mesh has no border faces - Please select Full Processing option");
				return false;
			}
		}
		//...else initialize unsorted queue
		else  {
			if ( !Init_q( generic_queue, a, b, par.getBool("FullProcessing") ) ) {
				Log("Target mesh has no border faces - Please select Full Processing option");
				return false;
			}
		}
		Log( "elapsed time: %f", (clock()-t1)/CLOCKS_PER_SEC );
		CMeshO::ScalarType epsilon  = par.getAbsPerc("distance");
		int sf;
		//if ( par.getBool("FastErosion")  && par.getBool("UseQuality") ) selectRedundantFast_pq();
		//if ( par.getBool("FastErosion")  && !par.getBool("UseQuality") ) selectRedundantFast();
		if ( /*!par.getBool("FastErosion") &&*/ par.getBool("UseQuality") ) {
			Log( "Quality-based Redundancy" );
			sf = selectRedundant_pq( priority_queue, a, b, epsilon );
		}
		if ( /*!par.getBool("FastErosion") &&*/ !par.getBool("UseQuality") ) {
			Log( "Standard Redunancy" );
			sf = selectRedundant( generic_queue, a, b, epsilon );
			Log( "elapsed time: %f", (t2-t1)/CLOCKS_PER_SEC );
		}

		Log( "Selected %i redundant faces", sf );
		return true;
	} //end FP_REDUNDANCY

	/**
	 * Filter zippering
	 * Assuming that redundant faces has already been removed,
	 * we project the border of mesh B on the surface of mesh A.
	 */
	if (ID(filter) == FP_ZIPPERING) {
		//pre-processing step: visit the border of B and split faces with two border edges
		refineBorder( b );
		//clean flags
		tri::UpdateFlags<CMeshO>::FaceClear(a->cm);
		tri::UpdateFlags<CMeshO>::FaceClear(b->cm);
		//before append, clean mesh A
		tri::Clean<CMeshO>::RemoveUnreferencedVertex( a->cm );
		tri::Clean<CMeshO>::RemoveDuplicateVertex( a->cm );
		//before append, clean mesh B
		tri::Clean<CMeshO>::RemoveUnreferencedVertex( b->cm );
		tri::Clean<CMeshO>::RemoveDuplicateVertex( b->cm );
		//store face number
		size_t fn_limit = a->cm.fn;
		//append B to A and update flags
		tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
		tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
		tri::Append<CMeshO, CMeshO>::Mesh( a->cm, b->cm );
		tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
		tri::UpdateFlags<CMeshO>::FaceClear(a->cm);
		tri::UpdateNormal<CMeshO>::PerFaceNormalized(a->cm);
		tri::UpdateNormal<CMeshO>::PerVertexNormalized(a->cm);
		//create grid on mesh A
		MeshFaceGrid grid;
		grid.Set( a->cm.face.begin(), a->cm.face.begin() + fn_limit );  //compute grid on the original part of A
		//recover information about border of B
		vector< tri::Hole<CMeshO>::Info > border;
		tri::Hole<CMeshO>::GetInfo( a->cm, false, border );

		//Add optional attribute (true if face has been visited)
		CMeshO::PerFaceAttributeHandle<bool> visited = tri::Allocator<CMeshO>::AddPerFaceAttribute<bool> (a->cm); //check for already visited face
		for ( size_t i = 0; i < a->cm.face.size(); i ++) visited[i] = false;	//no face initially visited

		//create map between faces and auxiliar information
		map<CMeshO::FacePointer, aux_info> map_info;
		vector< CMeshO::FacePointer > tbt_faces;   //To Be Triangulated
		vector< CMeshO::FacePointer > tbr_faces;   //To Be Removed
		vector< int > verts;					   //vector containing indices of vertices of the new faces
		//for each border of the mesh B, select a face and projected it on the surface of A
		//add information to the faces of A when needed
		for ( size_t c = 0; c < border.size(); c ++ ) {
			//create pos on the surface
			face::Pos<CMeshO::FaceType> p = border[c].p; p.FlipV();//CCW order
			//face is from A border, go over
			if ( tri::Index( a->cm, p.F() ) < fn_limit ) continue;
			//face has already been deleted or visiter,go over
			if ( visited[p.F()] || p.F()->IsD() ) continue;
			do {
				//Already deleted or visited, step over
				if ( visited[p.F()] || p.F()->IsD()) { p.NextB(); continue; }
				visited[p.F()] = true;
				//store vertex index (avoid crash)
				int v_ind = tri::Index( a->cm, p.V() );
				//project current face on the surface of A
				projectFace( p.F(), a, grid, par.getFloat("distance"), map_info, tbt_faces, tbr_faces, verts );
				//restore vertex pointer
				p.V() = &a->cm.vert[v_ind];
				p.NextB();
			} while ( p.F() != border[c].p.F() );
		}

		//remove splitted faces
		for ( size_t i = 0; i < tbr_faces.size(); i++) {
			if (!tbr_faces[i]->IsD()) tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbr_faces[i]);
		}

		//remove duplicates from vector to-be-triangulated
		sort( tbt_faces.begin(), tbt_faces.end() );  int k;
		vector<CMeshO::FacePointer>::iterator itr = unique( tbt_faces.begin(), tbt_faces.end() );
		tbt_faces.resize(itr - tbt_faces.begin() );
		//retriangulation of the faces and removal of the remaining part
		vector< Point3m > coords; size_t patch_verts = verts.size();
		for ( size_t i = 0; i < tbt_faces.size(); i ++ )	 {
			if ( !tbt_faces[i]->IsD() ) {
				handleBorder( map_info[tbt_faces[i]], tbt_faces[i]->N(), coords, verts );
				tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbt_faces[i]);
			}
		}

		//remove user-defined attribute
		int debcnt = 0;
		tri::Allocator<CMeshO>::DeletePerFaceAttribute<bool>(a->cm, visited);
		//add new faces and remove old ones
		CMeshO::FaceIterator fn = tri::Allocator<CMeshO>::AddFaces( a->cm, verts.size()/3 );
		for ( size_t k = 0; k < verts.size(); k += 3 ) {
			CMeshO::VertexPointer v0 = &(a->cm.vert[verts[k]]);
			CMeshO::VertexPointer v1 = &(a->cm.vert[verts[k+1]]);
			CMeshO::VertexPointer v2 = &(a->cm.vert[verts[k+2]]);
			if ( v0 == v1 || v1 == v2 || v0 == v2 )
				continue;
			//correct orientation of face normals
			if ( k < patch_verts ) {
				(*fn).V(0) = v0; (*fn).V(1) = v1; (*fn).V(2) = v2; (*fn).N() = ( (*fn).P(0) - (*fn).P(2) )^( (*fn).P(1)-(*fn).P(2) );
			}
			else {
				(*fn).V(0) = v0; (*fn).V(2) = v1; (*fn).V(1) = v2; (*fn).N() = ( (*fn).P(0) - (*fn).P(2) )^( (*fn).P(1)-(*fn).P(2) );
			}
			(*fn).SetS();
			++fn;
		}

		//remove degenerate faces
		tri::Clean<CMeshO>::RemoveDegenerateFace(a->cm);
		tri::Clean<CMeshO>::RemoveDuplicateVertex(a->cm);
		//compact vectors and update topology
		tri::Allocator<CMeshO>::CompactFaceVector( a->cm );
		tri::Allocator<CMeshO>::CompactVertexVector( a->cm );
		tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
		tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(a->cm);
		tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(a->cm);
		tri::UpdateBounding<CMeshO>::Box( a->cm );

		return true;
	} //end FP_ZIPPERING

	assert(0);
	Log(GLLogStream::DEBUG, "**********************" );

	return true;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterZippering)
