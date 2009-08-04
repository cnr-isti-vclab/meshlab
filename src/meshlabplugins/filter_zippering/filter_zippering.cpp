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
#include <cmath>
#include <time.h>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/simplex/face/topology.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/hole.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/space/intersection2.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/glu_tesselator.h>

#include "filter_zippering.h"

//#define REDUNDANCY_ONLY 1
//#define REFINE_PATCH_ONLY 1
#define MAX_LOOP 50

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
const QString FilterZippering::filterName(FilterIDType filterId) const
{
  switch(filterId) {
                case FP_ZIPPERING :  return QString("Zippering");
		default : assert(0); 
	}
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
const QString FilterZippering::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
                case FP_ZIPPERING :  return QString("Merge two meshes, a mesh with holes and a mesh-patch, into a single one; hole is filled using patch's triangles. Based on <b>Zippered Polygon Meshes from Range Images,</b> by G.Turk, M.Levoy, Siggraph 1994");
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

const int FilterZippering::getRequirements(QAction *action)
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
const FilterZippering::FilterClass FilterZippering::getClass(QAction *a)
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
void FilterZippering::initParameterSet(QAction *action, MeshDocument &md, FilterParameterSet & parlst)
{
         MeshModel *target;
         float maxVal = 0.0;
         switch(ID(action))	 {
                case FP_ZIPPERING :
                                foreach (target, md.meshList) {
                                    if ( target->cm.bbox.Diag() > maxVal ) maxVal = target->cm.bbox.Diag();
                                    if (target != md.mm())  break;
                                }
                                parlst.addMesh ("FirstMesh", md.mm(), "Mesh (with holes)", "The mesh with holes.");
                                parlst.addMesh ("SecondMesh", target, "Patch", "The mesh that will be used as patch.");
                                parlst.addAbsPerc("distance", maxVal*0.01, 0, maxVal, "Max distance", "Max distance between mesh and path");
                break;
                default : assert(0);
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
    int i; for (i=0; i<3 && !vcg::face::IsBorder(*face, i); i++);   //i-edge on patch border
    if (i == 3)  for (i = 0; i < 3 && !(face->FFp(i)->IsD()); i++);
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
        vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); m->cm.UnMarkAll();
        vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
        MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
        //Search closest point on A
        nearestF =  grid.GetClosest(PDistFunct, markerFunctor, edge_samples[j], max_dist, dist, closest);

        if ( nearestF == 0 )                    return false;   //no face within given range
        if ( isOnBorder(closest, nearestF ) )   return false;   //closest point on border
    }

    //check if V2(i) has a closest point on border of m
    CMeshO::FacePointer nearestF = 0;
    vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm);
    vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
    MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
    nearestF =  grid.GetClosest(PDistFunct, markerFunctor, face->P2(i), max_dist, dist, closest);
    if ( nearestF == 0 ) return false;           //no face within given range
    if ( isOnBorder( closest, nearestF ) ) return false;    //closest point on border

    //check if edges are completely projectable on m
    for ( int j = (i+1)%3; j != i; j = (j+1)%3 ) {
        edge_samples.clear(); edge_dir = face->P1(j) - face->P(j); edge_dir.Normalize();
        for( int k = 0; k <= samplePerEdge; k++ )  edge_samples.push_back( face->P(j) + (face->P1(j) - face->P(j)) * (k * step) ); //campionamento
        // samples on A
        for ( int k = 0; k < edge_samples.size(); k ++ ) {
            CMeshO::FacePointer nearestF = 0;
            vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm); m->cm.UnMarkAll();
            vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
            MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
            //Search closest point on A
            nearestF =  grid.GetClosest(PDistFunct, markerFunctor, edge_samples[k], max_dist, dist, closest);
            if ( nearestF == 0 )                    return false;   //no face within given range
            if ( isOnBorder(closest, nearestF ) )   return false;   //closest point on border
        }
    }
    // redundant
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

    // For each border...
    for (int i = 0; i < info.border.size(); i ++) {
        //search for component intersecated by border and split it into two or more components
        bool conn = true; int c = searchComponent( info, info.border[i].edges.front(), conn ); polyline current;
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
        std::vector< int > indices;
        std::vector< std::vector< vcg::Point3<CMeshO::ScalarType> > > outlines; outlines.push_back( points );
        while ( indices.size() == 0 ) {
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

/* Search for component intersected by segment s.
 * @param info auxiliar information containing data about components
 * @param s query segment
 * @param conn output parameter, true if resulting component is CC, false if it is trashC
 * @return index of component
 */
int FilterZippering::searchComponent(   aux_info &info,                            //Auxiliar info
                                        vcg::Segment3<CMeshO::ScalarType> s,       //query segment
                                        bool &conn ) {
    float min_dist = 1000*eps; conn = true; int index = -1;
    // for each CC search component check which CC is nearest to segment s
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        polyline current = info.conn[i];
        for ( int j = 0; j < current.edges.size(); j ++ ) {
            // proximity is measured using distance between CC-edge and extrema of s
            if ( vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P0() ) <= min_dist ) {
                min_dist = vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P0() ); index = i;
            }
            if ( vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P1() ) <= min_dist ) {
                min_dist = vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P1() ); index = i;
            }
        }
    }
    // for each TrashC search component check which TrashC is nearest to segment s
    for ( int i = 0; i < info.nTComponent(); i ++ ) {
        polyline current = info.trash[i];
        for ( int j = 0; j < current.edges.size(); j ++ ) {
            //proximity as seen before
            if ( vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P0() ) <= min_dist ) {
                min_dist = vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P0() ); index = i; conn = false;
            }
            if ( vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P1() ) <= min_dist ) {
                min_dist = vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P1() ); index = i; conn = false;
            }
        }
    }
    assert( index != -1 );
    return index;

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

/* Zippering of two meshes (Turk approach)
 * Given two mesh, a mesh with one or more holes (A) and a second mesh, a patch (B), fill a hole onto m surface
 * using faces of patch. Algorithm const of three steps:
 * - CheckRedundancy: delete face of patch that can be projected completely on A's surface
 * - PatchRefinement: patch vertices are projected on A, and border edges are splitted;
 * - MeshRefinement: faces where patch vertices lie are re-triangulated
 * Based on Zippered Polygon Meshes from Range Images, by G.Turk, M.Levoy, Siggraph 1994
 */
bool FilterZippering::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
     if ( md.size() == 1 )   {
        Log(GLLogStream::FILTER,"Please add a second mesh");
        return false;
    }
    MeshModel *a = par.getMesh("FirstMesh");
    MeshModel *b = par.getMesh("SecondMesh");

    a->cm.face.EnableFFAdjacency();   vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    a->cm.face.EnableMark(); a->cm.UnMarkAll();
    b->cm.face.EnableFFAdjacency();      vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
    b->cm.face.EnableMark(); b->cm.UnMarkAll();
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);
    CMeshO::ScalarType epsilon  = par.getAbsPerc("distance");

    //Search for face on patch border
    int limit = a->cm.fn;
    std::vector< vcg::tri::Hole<CMeshO>::Info > ccons; vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons ); ccons.clear();
    vcg::tri::Append<CMeshO, CMeshO>::Mesh( a->cm, b->cm );
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    a->cm.face.EnableColor();
    b->cm.face.EnableColor();//Useful for debug
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);
    vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    debug_v = false;

    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm ); eps = a->cm.bbox.Diag() / 1000000;
    Log(GLLogStream::DEBUG, "eps value %f", eps);

    /* STEP 1 - Removing Redundant Surfaces
     * Repeat until mesh surface remain unchanged:
     *   - Remove redundant triangles on the boundary of patch
     */
    MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //Grid on A
    bool changed; vcg::face::Pos<CMeshO::FaceType> p; int c_faces = 0;
    std::vector< CMeshO::FacePointer > split_faces;  std::vector< CMeshO::VertexPointer > new_faces; std::vector< CMeshO::FacePointer > remove_faces;
    vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons );
    for ( int i = 0; i < ccons.size(); i ++ ) {
        int start = remove_faces.size();
        vcg::face::Pos<CMeshO::FaceType> p = ccons[i].p;
        do {
            if ( !p.F()->IsD() && checkRedundancy( p.F(), a, grid_a, epsilon ) )  {
            vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *(p.F()) ); remove_faces.push_back( p.F() );
            } else
                if ( !p.F()->IsD() ) split_faces.push_back( p.F() );
            p.NextB();
        } while ( p.F() != ccons[i].p.F() );

        int num_iter = remove_faces.size() - start;  int count = 0;

        do {
            changed = false; count = 0;
            for ( int i = start; i < start + num_iter; i ++ ) {
                for ( int j = 0; j < 3; j ++ ) {
                    if ( !remove_faces[i]->FFp(j)->IsD() && checkRedundancy( remove_faces[i]->FFp(j), a, grid_a, epsilon ) ) {
                        changed = true; count++; c_faces++; remove_faces.push_back( remove_faces[i]->FFp(j) );
                        vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *(remove_faces[i]->FFp(j)) );
                    } else {
                        if ( !remove_faces[i]->FFp(j)->IsD() ) split_faces.push_back( remove_faces[i]->FFp(j) );
                    }
                }
            }
            start = start + num_iter; num_iter = count;
        } while (changed);
    }
    //remove redundant faces
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);

    //split faces with two border edges
    for ( int i = 0; i < split_faces.size(); i ++ ) {
        if ( vcg::face::BorderCount( *split_faces[i] ) == 3 ) {
            if (!split_faces[i]->IsD())  vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *split_faces[i] );
            if (split_faces[i]->IsD()) { split_faces.erase( split_faces.begin() + i ); --i; }
        }
        if ( vcg::face::BorderCount( *split_faces[i] ) <= 1 ) {
            split_faces.erase( split_faces.begin() + i ); --i;
        }
    }
    CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, split_faces.size() );
    for ( int i = 0; i < split_faces.size(); i ++ ) {
        if ( split_faces[i]->IsD() ) continue;
        int j; for (j=0; j<3 && vcg::face::IsBorder(*split_faces[i], j); j++); assert( j < 3 ); //split non border edge
        (*v).P() = (split_faces[i]->P(j) + split_faces[i]->P1(j))/2.0;
        new_faces.push_back( split_faces[i]->V(j) ); new_faces.push_back( &*v );  new_faces.push_back( split_faces[i]->V2(j) );
        new_faces.push_back( &*v ); new_faces.push_back( split_faces[i]->V1(j) ); new_faces.push_back( split_faces[i]->V2(j) );
        CMeshO::FacePointer opp_face = split_faces[i]->FFp(j);  int opp_edge = split_faces[i]->FFi(j);
        new_faces.push_back( opp_face->V1(opp_edge) ); new_faces.push_back(opp_face->V2(opp_edge));  new_faces.push_back( &*v );
        new_faces.push_back( &*v ); new_faces.push_back( opp_face->V2(opp_edge) ); new_faces.push_back( opp_face->V(opp_edge) );
        vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *split_faces[i] );
        if ( !opp_face->IsD() )vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *opp_face ); ++v;
        //TODO Manual update
    }
    // Add new faces
    for ( int k = 0; k < new_faces.size(); k += 3 ) {
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        (*f).V(0) = new_faces[k]; (*f).V(1) = new_faces[k+1]; (*f).V(2) = new_faces[k+2];
        (*f).N() = ( (*f).P(0) - (*f).P(2) )^( (*f).P(1)-(*f).P(2) );
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	vcg::tri::Clean<CMeshO>::RemoveSmallConnectedComponentsSize( a->cm, b->cm.fn/20 );
    /* End Step 1 */
#ifdef REDUNDANCY_ONLY
    Log(GLLogStream::DEBUG, "Removed %d redundant faces", c_faces);
    return true;
#endif
	new_faces.clear(); split_faces.clear(); remove_faces.clear();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //reset grid on A
    vcg::tri::UpdateSelection<CMeshO>::ClearFace(a->cm);
    /* STEP 2 - Project patch points on mesh surface
     * and ricorsively subdivide face in smaller triangles until each patch's face has border vertices
     * lying in adiacent or coincident faces. Also collect informations for triangulation of mesh faces.
     */
    //Add optional attribute
    CMeshO::PerFaceAttributeHandle<aux_info> info = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<aux_info>  (a->cm, std::string("aux_info"));
    vcg::tri::Hole<CMeshO>::GetInfo( a->cm, false, ccons );
    std::vector< CMeshO::FacePointer > tbt_faces;   //To Be Triangulated
    std::vector< CMeshO::FacePointer > tbr_faces;   //To Be Removed
    std::vector< int > verts; debug_v = true; 
	for ( int c = 0; c < ccons.size(); c ++ ) {
        if (ccons[c].p.F()->IsS() || ccons[c].p.F()->IsD() ) continue;
        vcg::face::Pos<CMeshO::FaceType> p = ccons[c].p; debug_v = false; p.FlipV();//CCW order
		do {
            p.F()->SetS();
            if (p.F()->IsD()) { p.NextB(); continue; }  //Already deleted, step over
			p.F()->C() = vcg::Color4b::LightGray; 
			CMeshO::FacePointer nearestF = 0, nearestF1 = 0;
			assert( vcg::face::BorderCount( *p.F() ) > 0 ); assert( vcg::face::IsBorder( *p.F(), p.E() ) );  //Check border correctness
            vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
            vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
            MeshFaceGrid::ScalarType  dist = 2*epsilon;  MeshFaceGrid::CoordType closest, closest1;
            nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), epsilon, dist, closest);
            dist = 2*epsilon;
            nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor,  p.F()->P1(p.E()), epsilon, dist, closest1);
			//Both vertices are too far from mesh
            if (nearestF == 0 && nearestF1 == 0) { p.NextB(); continue; }   //jump to next border
            //One vertex is too far away, the other one is not
            if ( nearestF == 0 || nearestF1 == 0 ) {
                CMeshO::FacePointer currentF = nearestF ? nearestF : nearestF1;
                CMeshO::VertexPointer startV = nearestF ? p.F()->V(p.E()) : p.F()->V1(p.E());
                CMeshO::VertexPointer endV = nearestF ? p.F()->V1(p.E()) : p.F()->V(p.E());
                bool inv = nearestF? false : true;
                if (nearestF)  p.F()->P(p.E()) = closest;
                else p.F()->P1(p.E()) = closest1;
                int thirdV = vcg::tri::Index( a->cm, p.F()->V2(p.E()) );
                int last_split = -1; vcg::Point2f pt; bool stop = false;
                //search for splitting edge
                do {
                    int tosplit = -1;
                    vcg::Plane3<CMeshO::ScalarType> plane; plane.Init( currentF->P(0), currentF->N() );
                    vcg::Matrix44<CMeshO::ScalarType> rot_m;
                    rot_m.SetRotateRad( vcg::Angle<CMeshO::ScalarType>( currentF->N(), vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) ), currentF->N() ^ vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) );
                    vcg::Segment2f s(   vcg::Point2f((rot_m * plane.Projection( startV->P() )).X(), (rot_m * plane.Projection( startV->P() )).Y()),
                                        vcg::Point2f((rot_m * plane.Projection( endV->P() )).X(), (rot_m * plane.Projection( endV->P() )).Y()) );

                    for ( int e = 0; e < 3; e ++ ) {
                        if ( e != last_split && vcg::SegmentSegmentIntersection( s, vcg::Segment2f( vcg::Point2f( (rot_m * currentF->P(e)).X(), (rot_m * currentF->P(e)).Y() ),
                                                                                                    vcg::Point2f( (rot_m * currentF->P1(e)).X(), (rot_m * currentF->P1(e)).Y() ) ), pt ) ) {
                            tosplit = e; break;
                        }
                    }
                    assert(tosplit!=-1); last_split = currentF->FFi(tosplit);
                    // search intersection point (approximation)
                    vcg::Segment3<CMeshO::ScalarType> edge( currentF->P(tosplit), currentF->P1(tosplit) );
                    int sampleNum = SAMPLES_PER_EDGE; float step = 1.0 / (sampleNum + 1);
                    vcg::Point3<CMeshO::ScalarType> closest;    float min_dist = edge.Length();
                    for ( int k = 0; k <= sampleNum; k ++ ) {
                        vcg::Point3<CMeshO::ScalarType> currentP = startV->P() + ( endV->P() - startV->P() ) * (k*step);
                        if ( vcg::SquaredDistance( edge, currentP ) < min_dist ) {
                            closest = currentP; min_dist = vcg::SquaredDistance( edge, closest );
                        }
                    }

                    assert( vcg::SquaredDistance( edge, closest ) < edge.Length() );    //point found
                    closest = vcg::ClosestPoint(edge, closest); //projection on edge
                    vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                    CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
                    if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
                    info[currentF].SetEps( eps ); info[currentF].Init( *currentF, vcg::tri::Index(a->cm, currentF->V(0)), vcg::tri::Index(a->cm, currentF->V(1)), vcg::tri::Index(a->cm, currentF->V(2)) );
                    if (!inv) {
                        if ( info[currentF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( startV->P(), (*v).P() ),
                                                         std::make_pair( vcg::tri::Index( a->cm, startV ), v - a->cm.vert.begin() ) ) ) {
                            tbt_faces.push_back( currentF ); verts.push_back( vcg::tri::Index( a->cm, startV ) ); verts.push_back( thirdV ); verts.push_back( v - a->cm.vert.begin() );
                        }
                    }
                    else {
                        if ( info[currentF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), startV->P() ),
                                                         std::make_pair( v - a->cm.vert.begin(), vcg::tri::Index( a->cm, startV ) ) ) ) {
                            tbt_faces.push_back( currentF ); verts.push_back( thirdV ); verts.push_back( vcg::tri::Index( a->cm, startV ) ); verts.push_back( v - a->cm.vert.begin() );
                        }
                    }
                    startV = &(*v);
                    if ( vcg::face::IsBorder( *currentF, tosplit ) )  { //last triangle
                        if ( !inv ) { verts.push_back( vcg::tri::Index( a->cm, endV ) ); verts.push_back( thirdV ); verts.push_back( v - a->cm.vert.begin() ); }
                        else { verts.push_back( vcg::tri::Index( a->cm, endV ) ); verts.push_back( v - a->cm.vert.begin() ); verts.push_back( thirdV ); }
                        stop = true;
                    }
                    else currentF = currentF->FFp(tosplit);
                } while (!stop);
                tbr_faces.push_back( p.F() ); p.NextB(); continue; //remove face and jump over
            }
            p.F()->P(p.E()) = closest;  p.F()->P1(p.E()) = closest1;
            std::vector < std::pair< int, int > > stack;
            std::vector < std::pair< CMeshO::FacePointer, CMeshO::FacePointer > > stack_faces;
            int end_v = vcg::tri::Index(a->cm, p.F()->V2(p.E())); 
            //Check if nearest faces are coincident
            if ( nearestF == nearestF1 ) {
                info[nearestF].SetEps( eps ); 
				info[nearestF].Init( *nearestF, vcg::tri::Index(a->cm, nearestF->V(0)), vcg::tri::Index(a->cm, nearestF->V(1)), vcg::tri::Index(a->cm, nearestF->V(2)) );
                //ct++;
				if ( info[nearestF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( p.F()->P(p.E()), p.F()->P1(p.E()) ),
                                                 std::make_pair( vcg::tri::Index(a->cm, p.F()->V(p.E())), vcg::tri::Index(a->cm, p.F()->V1(p.E())) ) ) )
                    tbt_faces.push_back( nearestF );
                p.NextB(); continue; //next border
            }
			//Check if nearest faces are adjacent
            if ( isAdjacent( nearestF, nearestF1 ) ) {
                //search for shared edge
                int shared; for ( int k = 0; k < 3; k ++ ) if ( nearestF->FFp(k) == nearestF1 ) shared = k;
                //search for closest point
                vcg::Segment3<CMeshO::ScalarType> edge( nearestF->P(shared), nearestF->P1(shared) );
                int sampleNum = SAMPLES_PER_EDGE; float step = 1.0/(sampleNum+1);
                vcg::Point3<CMeshO::ScalarType> closest;    float min_dist = edge.Length();
                for ( int k = 0; k <= sampleNum; k ++ ) {
                    vcg::Point3<CMeshO::ScalarType> currentP = p.F()->P(p.E()) + ( p.F()->P1(p.E()) - p.F()->P(p.E()) ) * (k*step);
                    if ( vcg::SquaredDistance( edge, currentP ) < min_dist ) {
                        closest = currentP; min_dist = vcg::SquaredDistance( edge, closest );
                    }
                }
                assert( vcg::SquaredDistance( edge, closest ) < edge.Length() );    //point found
                closest = vcg::ClosestPoint(edge, closest); //projection on edge
                //merge close vertices
                if ( vcg::Distance<float>( closest, p.F()->P(p.E()) ) < eps ) {
                    p.F()->P(p.E()) = closest;
                    info[nearestF1].SetEps( eps ); info[nearestF1].Init( *nearestF1, vcg::tri::Index( a->cm, nearestF1->V(0) ), vcg::tri::Index( a->cm, nearestF1->V(1) ), vcg::tri::Index( a->cm, nearestF1->V(2) ) );
                    if ( info[nearestF1].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( p.F()->P(p.E()), p.F()->P1(p.E()) ),
                                                      std::make_pair( vcg::tri::Index(a->cm, p.F()->V(p.E())), vcg::tri::Index(a->cm, p.F()->V1(p.E())) ) ) )
                        tbt_faces.push_back( nearestF1 );
                    p.NextB(); continue;
                }
                if ( vcg::Distance<float>( closest, p.F()->P1(p.E()) ) < eps ) {
                    p.F()->P1(p.E()) = closest;
                    info[nearestF].SetEps( eps ); info[nearestF].Init( *nearestF, vcg::tri::Index( a->cm, nearestF->V(0) ), vcg::tri::Index( a->cm, nearestF->V(1) ), vcg::tri::Index( a->cm, nearestF1->V(2) ) );
                    if ( info[nearestF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( p.F()->P(p.E()), p.F()->P1(p.E()) ),
                                                     std::make_pair( vcg::tri::Index(a->cm, p.F()->V(p.E())), vcg::tri::Index(a->cm, p.F()->V1(p.E())) ) ) )
                        tbt_faces.push_back( nearestF );
                    p.NextB(); continue;
                }
                tbr_faces.push_back( p.F() ); //remove face, it will be overwritten by new faces
                //Add new vertices
                vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
                if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
                info[nearestF].SetEps( eps );  info[nearestF].Init( *nearestF, vcg::tri::Index(a->cm, nearestF->V(0)), vcg::tri::Index(a->cm, nearestF->V(1)), vcg::tri::Index(a->cm, nearestF->V(2)) );
                info[nearestF1].SetEps( eps ); info[nearestF1].Init( *nearestF1, vcg::tri::Index(a->cm, nearestF1->V(0)), vcg::tri::Index(a->cm, nearestF1->V(1)), vcg::tri::Index(a->cm, nearestF1->V(2)) );
                if ( info[nearestF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( p.F()->P(p.E()), (*v).P() ),
                                                 std::make_pair( vcg::tri::Index(a->cm, p.F()->V(p.E())), v - a->cm.vert.begin() ) ) ) {
                    tbt_faces.push_back( nearestF ); verts.push_back(v - a->cm.vert.begin()); verts.push_back(end_v); verts.push_back(vcg::tri::Index(a->cm, p.F()->V(p.E())));   //new triangle
                 }
                if ( info[nearestF1].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), p.F()->P1(p.E()) ),
                                                   std::make_pair( v - a->cm.vert.begin(), vcg::tri::Index(a->cm, p.F()->V1(p.E())) ) ) ) {
                    tbt_faces.push_back( nearestF1 ); verts.push_back(v - a->cm.vert.begin());   verts.push_back(vcg::tri::Index(a->cm, p.F()->V1(p.E())));  verts.push_back(end_v);  //First triangle
                }
				p.NextB(); continue;
            }
            tbr_faces.push_back( p.F() ); //remove face, it will be overwritten by new faces
            int cnt = 0;    //counter (inf. loop)
            // Not-adjacent; recursive split using mid-point of edge border
            stack.push_back( std::make_pair( vcg::tri::Index( a->cm, p.F()->V(p.E()) ),
                                             vcg::tri::Index( a->cm, p.F()->V1(p.E()) ) ) );    //Edge border
            assert( vcg::face::IsBorder( *p.F(), p.E() ) );
            stack_faces.push_back( std::make_pair(nearestF, nearestF1) );   //Nearest Faces
            CMeshO::FacePointer actualF = p.F(); int actualE = p.E(); p.NextB();
            while ( !stack.empty() ) {
                assert( ++cnt < MAX_LOOP );  //stop in case of inf. loop
                std::pair< int, int > border_edge = stack.back(); stack.pop_back();   //vertex indices
                CMeshO::FacePointer start = stack_faces.back().first; CMeshO::FacePointer end = stack_faces.back().second; //facce di A, non richiedono update
                stack_faces.pop_back();
                if ( start == end ) {
                    info[start].SetEps( eps ); info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) )  );
                    if ( info[start].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                                  std::make_pair( border_edge.first, border_edge.second ) ) ) {
                        tbt_faces.push_back( start ); verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                    }
                    continue;
                }

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
                if ( w != -1 && vcg::SquaredDistance<float>( s, start->P(w) ) <= eps ) {
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
                currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //proj. midpoin on A
                assert( currentF != 0 ); (*v).P() = closest;
                stack.push_back( std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
                stack.push_back( std::make_pair( v - a->cm.vert.begin(), border_edge.second ) );
                stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
                stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
                assert( stack.size() == stack_faces.size() );
            }
        } while ( p.F() != ccons[c].p.F() );
    }//end for

    for ( int i = 0; i < tbr_faces.size(); i++) {
        if (!tbr_faces[i]->IsD()) vcg::tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbr_faces[i]);
    }

    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    
    /* End of Step 2 */
#ifdef REFINE_PATCH_ONLY
    for ( int k = 0; k < verts.size(); k += 3 ) {
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        (*f).V(0) = &(a->cm.vert[verts[k]]); (*f).V(1) = &(a->cm.vert[verts[k+1]]); (*f).V(2) = &(a->cm.vert[verts[k+2]]);
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    return true;
#endif
    /* STEP 3
     * Faces are sorted by index, than each face is triangulated using auxiliar information about
     * vertices and edges
     */
    std::sort( tbt_faces.begin(), tbt_faces.end() ); std::vector<CMeshO::FacePointer>::iterator itr = std::unique( tbt_faces.begin(), tbt_faces.end() ); tbt_faces.resize(itr - tbt_faces.begin() );
    std::vector< CMeshO::CoordType > coords; int patch_verts = verts.size(); debug_v = false; int d=-1;
    for ( int i = 0; i < tbt_faces.size(); i ++ ) {
        if ( !tbt_faces[i]->IsD() ) {
            handleBorder( info[tbt_faces[i]], tbt_faces[i]->N(), coords, verts );
            vcg::tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbt_faces[i]);
        }
    }
    vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<aux_info>(a->cm, info); //no more useful
    // Add new faces
    int n_faces = 0;
    for ( int k = 0; k < verts.size(); k += 3 ) { n_faces++;
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        if ( k < patch_verts ) {
            (*f).V(0) = &(a->cm.vert[verts[k]]); (*f).V(1) = &(a->cm.vert[verts[k+1]]); (*f).V(2) = &(a->cm.vert[verts[k+2]]); (*f).N() = ( (*f).P(0) - (*f).P(2) )^( (*f).P(1)-(*f).P(2) );
        }
        else {
            (*f).V(0) = &(a->cm.vert[verts[k]]); (*f).V(2) = &(a->cm.vert[verts[k+1]]); (*f).V(1) = &(a->cm.vert[verts[k+2]]); (*f).N() = ( (*f).P(0) - (*f).P(2) )^( (*f).P(1)-(*f).P(2) );
        }
    }
    //post-processing refinement
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
	vcg::tri::Clean<CMeshO>::RemoveSmallConnectedComponentsSize( a->cm, a->cm.fn / 100 );
    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm );
    vcg::tri::Clean<CMeshO>::RemoveUnreferencedVertex( a->cm );
    //vcg::tri::Clean<CMeshO>::MergeCloseVertex( a->cm, eps ); //tooooooo slow
    vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex( a->cm );
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    a->cm.face.DisableColor();
    //garbage collector
    vcg::tri::Allocator<CMeshO>::CompactVertexVector(a->cm);
    vcg::tri::Allocator<CMeshO>::CompactFaceVector(a->cm);
    Log(GLLogStream::DEBUG, "**********************" );
    Log(GLLogStream::DEBUG, "End - Remove %d faces from patch - Created %d new faces", c_faces, n_faces);

    return true;
}

Q_EXPORT_PLUGIN(FilterZippering)
