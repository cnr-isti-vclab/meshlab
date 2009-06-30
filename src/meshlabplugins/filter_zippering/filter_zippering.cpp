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
#include <vcg/simplex/face/topology.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/vertex/component_ocf.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/space/intersection2.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/glu_tesselator.h>

#include "filter_zippering.h"

//#define REDUNDANCY_ONLY 1
//#define REFINE_PATCH_ONLY 1


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

    int samplePerEdge = SAMPLES_PER_EDGE;

    //samples edge in uniform way
    std::vector< vcg::Point3< CMeshO::ScalarType > > edge_samples;
    vcg::Point3< CMeshO::ScalarType > edge_dir = face->P1(i) - face->P(i); edge_dir.Normalize();
    float step = 1.0/(samplePerEdge+1); //step length
    for( int j = 0; j <= samplePerEdge; j++ ) {
        edge_samples.push_back( face->P(i) + edge_dir * (j * step) );
    }

    for ( int j = 0; j < edge_samples.size(); j ++ ) {
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
        if ( vcg::face::IsBorder( *p.F(), p.E() ) )
            return true;
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
    //triangulation of Ccomponent
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        std::vector< vcg::Point3<CMeshO::ScalarType> > points;  //coords vector
        std::vector< int > vertices;          //vertices vector
        for ( int j = 0; j < info.conn[i].edges.size(); j ++ )  {
            points.push_back( info.conn[i].edges[j].P0() );
            vertices.push_back( info.conn[i].verts[j].first );
        }
        std::vector< int > indices;
        std::vector< std::vector< vcg::Point3<CMeshO::ScalarType> > > outlines; outlines.push_back( points );
        vcg::glu_tesselator::tesselate( outlines, indices );    //glu tessellator
        for ( int k = 0; k < indices.size(); k ++ ) {
            pointers.push_back( vertices[indices[k]] );         //save indices, in order to create new faces
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
                                        vcg::Matrix44<CMeshO::ScalarType> rot_mat ) {     //Rotation matrix

    vcg::Point3<CMeshO::ScalarType> startpoint = border.edges.front().P0();
    vcg::Point3<CMeshO::ScalarType> endpoint = border.edges.back().P1();
    vcg::Point2<CMeshO::ScalarType> startpoint2D ( (rot_mat * startpoint).X(), (rot_mat * startpoint).Y() );
    vcg::Point2<CMeshO::ScalarType> endpoint2D ( (rot_mat * endpoint).X(), (rot_mat * endpoint).Y() );
    int startedge = 0, endedge = 0; float min_dist_s = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[0], startpoint ), min_dist_e = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[0], endpoint );
    // search where startpoint lies
    for ( int i = 0; i < comp.edges.size(); i ++ ) {
        if ( vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ) <= min_dist_s ) { startedge = i; min_dist_s = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ); }
        if ( vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ) <= min_dist_e ) { endedge = i; min_dist_e = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ); }
    }
    polyline p;
    // border edges will be edges of new comp
    p.edges.insert( p.edges.begin(), border.edges.begin(), border.edges.end() );
    p.verts.insert( p.verts.begin(), border.verts.begin(), border.verts.end() );
    // startedge == endedge
    if ( startedge == endedge && isRight( startpoint2D, vcg::Point2<CMeshO::ScalarType> ( (rot_mat * border.edges.front().P1()).X(), (rot_mat * border.edges.front().P1()).Y() ) , endpoint2D ) ) {
        vcg::Segment3<CMeshO::ScalarType> join( endpoint, startpoint );
        p.edges.push_back( join ); p.verts.push_back( std::make_pair( border.verts.back().second, border.verts.front().first ) ); //Vertex pointers
        return p;
    }

    // startedge!=endedge
    // search point on the right, create oriented segment and go on
    int step = -1;
    if ( isRight(   vcg::Point2<CMeshO::ScalarType> ( (rot_mat * border.edges.back().P0()).X(), (rot_mat * border.edges.back().P0()).Y() ), endpoint2D,
                    vcg::Point2<CMeshO::ScalarType> ( (rot_mat * comp.edges[endedge].P0()).X(), (rot_mat * comp.edges[endedge].P0()).Y() ) ) ) {
        vcg::Segment3<CMeshO::ScalarType> s( endpoint, comp.edges[endedge].P0() );
        p.edges.push_back( s ); step = comp.edges.size() - 1;
        p.verts.push_back( std::make_pair(border.verts.back().second, comp.verts[endedge].first ) );
    }

    if ( isRight(   vcg::Point2<CMeshO::ScalarType> ( (rot_mat * border.edges.back().P0()).X(), (rot_mat * border.edges.back().P0()).Y() ), endpoint2D,
                    vcg::Point2<CMeshO::ScalarType> ( (rot_mat * comp.edges[endedge].P1()).X(), (rot_mat * comp.edges[endedge].P1()).Y() ) ) ) {
        vcg::Segment3<CMeshO::ScalarType> s( endpoint, comp.edges[endedge].P1() );
        p.edges.push_back( s ); step = comp.edges.size() + 1;
        p.verts.push_back( std::make_pair(border.verts.back().second, comp.verts[endedge].second ) );
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
    vcg::Segment3<CMeshO::ScalarType> s( p.edges.back().P1() , startpoint );
    p.edges.push_back( s );
    p.verts.push_back( std::make_pair ( p.verts.back().second, border.verts.front().first ) );
    //comp
    return p;
}

/* Search for component intersected by segment s.
 * @param info auxiliar information containing data about components
 * @param s query segment
 * @param conn output parameter, true if resulting component is CC, false if it is trashC
 * @return index of component
 */
int FilterZippering::searchComponent( aux_info &info,                            //Auxiliar info
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

/* Check if point pn is on the right of segment p1-->p2
 * @param p1 first point of segment (start point)
 * @param p2 second point of segment(end point)
 * @param pn query point
 * @return true if pn lies on the right of p1-->p2, false otherwise
 */
bool FilterZippering::isRight (vcg::Point2< float > p1, vcg::Point2< float > p2, vcg::Point2< float > pn)
{
        if ( ( p1.X() - pn.X() ) * ( p2.Y() - pn.Y() ) - ( p2.X() - pn.X() ) * ( p1.Y() - pn.Y() )  >  0.0000 )
                return false;
        else
                return true;
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
    a->cm.face.EnableMark(); a->cm.UnMarkAll(); a->cm.vert.EnableTexCoord();
    b->cm.face.EnableFFAdjacency();      vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm);
    b->cm.face.EnableMark(); b->cm.UnMarkAll(); a->cm.vert.EnableTexCoord();
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);

    CMeshO::FacePointer patch_border;   //patch border pointer
    CMeshO::ScalarType epsilon  = par.getAbsPerc("distance");

    //Search for face on patch border
    for ( CMeshO::FaceIterator fi = b->cm.face.begin(); fi != b->cm.face.end(); ++fi)
        if ( vcg::face::BorderCount<CMeshO::FaceType>(*fi) > 0 ) { patch_border = &*fi; break; }
    assert( vcg::face::BorderCount<CMeshO::FaceType>(*patch_border) > 0 );
    int patch_border_i = vcg::tri::Index( b->cm, patch_border ) + a->cm.fn; int limit = a->cm.fn;

    a->cm.vert.DisableTexCoord(); b->cm.vert.DisableTexCoord();
    vcg::tri::Append<CMeshO, CMeshO>::Mesh( a->cm, b->cm );
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    a->cm.face.EnableColor();  a->cm.vert.EnableTexCoord();  b->cm.vert.EnableTexCoord();
    b->cm.face.EnableColor();//Useful for debug
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);
    vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    patch_border = &a->cm.face[patch_border_i]; debug_v = false;

    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm ); eps = a->cm.bbox.Diag() / 1000000;
    Log(GLLogStream::DEBUG, "eps value %f", eps);

    /* STEP 1 - Removing Redundant Surfaces
     * Repeat until mesh surface remain unchanged:
     *   - Remove redundant triangles on the boundary of patch
     */
    vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::FacePointer> fpu;
    vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
    MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //Grid on A
    bool changed; vcg::face::Pos<CMeshO::FaceType> p; CMeshO::FacePointer start; int c_faces = 0;
    std::vector< CMeshO::FacePointer > split_faces;  std::vector< CMeshO::VertexPointer > new_faces;
    do {     //Changed loop
        changed = false;
        //Search for a border face on patch
        patch_border = 0;
        for ( CMeshO::FaceIterator fi = a->cm.face.end()-1; fi != a->cm.face.begin(); --fi )
            if ( !(*fi).IsD() && (vcg::face::BorderCount<CMeshO::FaceType>(*fi) > 0) &&
                 (vcg::face::BorderCount<CMeshO::FaceType>(*fi) < 2) ) { patch_border = &*fi; break; }
        assert( patch_border != 0 );
        // Set pos border
        int i; for (i=0; i<3 && !vcg::face::IsBorder(*patch_border, i); i++); assert( i < 3 ); //i-edge on border
        p.Set( patch_border, i, patch_border->V(i) ); CMeshO::FacePointer start = p.F();
        do  {
            if (p.F()->IsD()) { p.NextB(); continue; }  //Already deleted, step over
            if ( checkRedundancy( p.F(), a, grid_a, epsilon ) )  {  //Check face Redundacy
                changed = true; c_faces++;
                vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *(p.F()) );
            }
            if ( !(p.F()->IsD()) && vcg::face::BorderCount( *p.F() ) == 2 ) {   //Split face (simplification)
                if ( std::find( split_faces.begin(), split_faces.end(), p.F() ) == split_faces.end() )
                    split_faces.push_back( p.F() );
            }
            p.NextB();
        } while (p.F() != start);
        //Update topology
        vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    } while (changed);

    //split faces with two border edges
    for ( int i = 0; i < split_faces.size(); i ++ ) {
        if ( vcg::face::BorderCount( *split_faces[i] ) == 3 ) {
            vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *split_faces[i] );
            split_faces.erase( split_faces.begin() + i ); --i;
        }
    }
    CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, split_faces.size() );
    for ( int i = 0; i < split_faces.size(); i ++ ) {
        int j; for (j=0; j<3 && vcg::face::IsBorder(*split_faces[i], j); j++); assert( j < 3 ); //split non border edge
        (*v).P() = (split_faces[i]->P(j) + split_faces[i]->P1(j))/2.0;
        new_faces.push_back( split_faces[i]->V(j) ); new_faces.push_back( &*v );  new_faces.push_back( split_faces[i]->V2(j) );
        new_faces.push_back( &*v ); new_faces.push_back( split_faces[i]->V1(j) ); new_faces.push_back( split_faces[i]->V2(j) );
        CMeshO::FacePointer opp_face = split_faces[i]->FFp(j);  int opp_edge = split_faces[i]->FFi(j);
        new_faces.push_back( opp_face->V1(opp_edge) ); new_faces.push_back(opp_face->V2(opp_edge));  new_faces.push_back( &*v );
        new_faces.push_back( &*v ); new_faces.push_back( opp_face->V2(opp_edge) ); new_faces.push_back( opp_face->V(opp_edge) );
        if ( !split_faces[i]->IsD() )   vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *split_faces[i] );
        if ( !(opp_face->IsD()) )       vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *opp_face ); ++v;

    }
    // Add new faces
    for ( int k = 0; k < new_faces.size(); k += 3 ) {
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        (*f).V(0) = new_faces[k]; (*f).V(1) = new_faces[k+1]; (*f).V(2) = new_faces[k+2];
        (*f).N() = ( (*f).P(0) - (*f).P(2) )^( (*f).P(1)-(*f).P(2) );
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    vcg::tri::Clean<CMeshO>::RemoveSmallConnectedComponentsSize( a->cm, b->cm.fn / 20.00 );      //Remove spurious component
    /* End Step 1 */
#ifdef REDUNDANCY_ONLY
    Log(GLLogStream::DEBUG, "Removed %d redundant faces", c_faces);
    return true;
#endif
    new_faces.clear(); split_faces.clear();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //reset grid on A

    /* STEP 2 - Project patch points on mesh surface
     * and ricorsively subdivide face in smaller triangles until each patch's face has border vertices
     * lying in adiacent or coincident faces. Also collect informations for triangulation of mesh faces.
     */
    //Add optional attribute
    CMeshO::PerFaceAttributeHandle<aux_info> info = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<aux_info>  (a->cm, std::string("aux_info"));
    patch_border = 0;
    for ( CMeshO::FaceIterator fi = a->cm.face.end()-1; fi != a->cm.face.begin(); --fi )
            if ( !(*fi).IsD() && (vcg::face::BorderCount<CMeshO::FaceType>(*fi) > 0) &&
                 (vcg::face::BorderCount<CMeshO::FaceType>(*fi) < 2) ) { patch_border = &*fi; break; }
    assert( patch_border != 0 );
    int i; for (i=0; i<3 && !vcg::face::IsBorder(*patch_border, i); i++); assert( i < 3 );
    p.Set( patch_border, i, patch_border->V1(i) ); start = p.F();
    std::vector< CMeshO::FacePointer > tbt_faces;   //To Be Triangulated
    std::vector< CMeshO::FacePointer > tbr_faces;   //To Be Removed
    std::vector< int > verts;
    do {
        if (p.F()->IsD()) { p.NextB(); continue; }  //Already deleted, step over

        CMeshO::FacePointer nearestF = 0, nearestF1 = 0;
        assert( vcg::face::BorderCount( *p.F() ) > 0 ); assert( vcg::face::IsBorder( *p.F(), p.E() ) );  //Check border correctness
        vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
        vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
        MeshFaceGrid::ScalarType  dist = 2*epsilon;  MeshFaceGrid::CoordType closest, closest1;
        nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), epsilon, dist, closest);
        dist = 2*epsilon;
        nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor,  p.F()->P1(p.E()), epsilon, dist, closest1);
        //Both vertices are too far from mesh
        if (nearestF == 0 && nearestF1 == 0) { p.NextB(); continue; }
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
            //
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
                verts.push_back( vcg::tri::Index( a->cm, startV ) ); verts.push_back( thirdV ); verts.push_back( v - a->cm.vert.begin() );
                if (!inv) info[currentF].AddToBorder(   vcg::Segment3<CMeshO::ScalarType> ( startV->P(), (*v).P() ),
                                                        std::make_pair( vcg::tri::Index( a->cm, startV ), v - a->cm.vert.begin() ) );
                else info[currentF].AddToBorder(vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), startV->P() ),
                                                std::make_pair( v - a->cm.vert.begin(), vcg::tri::Index( a->cm, startV ) ) );
                info[currentF].Init( *currentF, vcg::tri::Index(a->cm, currentF->V(0)), vcg::tri::Index(a->cm, currentF->V(1)), vcg::tri::Index(a->cm, currentF->V(2)) );
                startV = &(*v);     tbt_faces.push_back( currentF );
                if ( vcg::face::IsBorder( *currentF, tosplit ) )  { //last triangle
                    if ( !inv ) {
                        verts.push_back( vcg::tri::Index( a->cm, endV ) ); verts.push_back( thirdV ); verts.push_back( v - a->cm.vert.begin() );
                    }
                    else {
                        verts.push_back( vcg::tri::Index( a->cm, endV ) ); verts.push_back( v - a->cm.vert.begin() ); verts.push_back( thirdV );
                    }
                    stop = true;
                }
                else currentF = currentF->FFp(tosplit);
            } while (!stop);

            //remove face
            tbr_faces.push_back( p.F() );
            p.NextB();
            continue;
        }
        p.F()->P(p.E()) = closest;  p.F()->P1(p.E()) = closest1;
        std::vector < std::pair< int, int > > stack;
        std::vector < std::pair< CMeshO::FacePointer, CMeshO::FacePointer > > stack_faces;
        int end_v = vcg::tri::Index(a->cm, p.F()->V2(p.E())); bool modified = true;
        while ( modified ) {
            modified = false;
            // Handle degenerate case
            // projected points lying on edges of A
            for ( int k = 0; k < 3; k ++ ) {
                if ( vcg::Distance<float>( nearestF->P(k), p.F()->P(p.E()) ) <= eps ) {
                    vcg::Segment3<CMeshO::ScalarType> s( vcg::Barycenter( *(p.F()) ), p.F()->P(p.E()) );
                    p.F()->P(p.E()) = s.P(1.2); nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), 2*epsilon, dist, closest);
                    p.F()->P(p.E()) = closest; modified = true;
                }
                if ( vcg::Distance<float>( nearestF1->P(k), p.F()->P1(p.E()) ) <= eps ) {
                    vcg::Segment3<CMeshO::ScalarType> s( vcg::Barycenter( *(p.F()) ), p.F()->P1(p.E()) );
                    p.F()->P1(p.E()) = s.P(1.2); nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P1(p.E()), 2*epsilon, dist, closest);
                    p.F()->P1(p.E()) = closest; modified = true;
                }
            }
            // projected poinst lying on vertices of A
            for ( int k = 0; k < 3; k ++ ) {
                a->cm.UnMarkAll();
                vcg::Segment3<CMeshO::ScalarType> edge( nearestF->P(k), nearestF->P1(k) );
                if ( vcg::SquaredDistance<float>( edge, p.F()->P(p.E()) ) <= eps ) {
                    vcg::Segment3<CMeshO::ScalarType> s( vcg::Barycenter( *(p.F()) ), p.F()->P(p.E()) );
                    p.F()->P(p.E()) = s.P(0.9); nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), 2*epsilon, dist, closest);
                    if ( isOnBorder( closest, nearestF ) ) {
                        p.F()->P(p.E()) = s.P(1.1); nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), 2*epsilon, dist, closest);
                    }
                    p.F()->P(p.E()) = closest; modified = true;
                }
                a->cm.UnMarkAll();
                edge = vcg::Segment3<CMeshO::ScalarType>( nearestF1->P(k), nearestF1->P1(k) );
                if ( vcg::SquaredDistance<float>( edge, p.F()->P1(p.E()) ) <= eps ) {
                    vcg::Segment3<CMeshO::ScalarType> s( vcg::Barycenter( *(p.F()) ), p.F()->P1(p.E()) );
                    p.F()->P1(p.E()) = s.P(0.9); nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P1(p.E()), 2*epsilon, dist, closest);
                    if ( isOnBorder( closest, nearestF1 ) ) {
                        p.F()->P1(p.E()) = s.P(1.1); nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P1(p.E()), 2*epsilon, dist, closest);
                    }
                    p.F()->P1(p.E()) = closest; modified = true;
                }
            }
        }
        //Check if nearest faces are coincident
        if ( nearestF == nearestF1 ) {
            info[nearestF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( p.F()->P(p.E()), p.F()->P1(p.E()) ),
                                        std::make_pair( vcg::tri::Index(a->cm, p.F()->V(p.E())), vcg::tri::Index(a->cm, p.F()->V1(p.E())) ) );
            info[nearestF].Init( *nearestF, vcg::tri::Index(a->cm, nearestF->V(0)), vcg::tri::Index(a->cm, nearestF->V(1)), vcg::tri::Index(a->cm, nearestF->V(2)) );
            tbt_faces.push_back( nearestF );
            p.NextB(); continue;
        } //concident faces; no op, next loop
        tbr_faces.push_back( p.F() ); //remove face, it will be overwritten by new faces

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

            //Add new vertices
            vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
            CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
            if ( vpu.NeedUpdate() )  vpu.Update( p.V() );

            verts.push_back(v - a->cm.vert.begin());   verts.push_back(vcg::tri::Index(a->cm, p.F()->V1(p.E())));  verts.push_back(end_v);  //First triangle
            verts.push_back(v - a->cm.vert.begin());   verts.push_back(end_v);  verts.push_back(vcg::tri::Index(a->cm, p.F()->V(p.E())));   //Second triangle

            info[nearestF].AddToBorder( vcg::Segment3<CMeshO::ScalarType> ( p.F()->P(p.E()), (*v).P() ),
                                        std::make_pair( vcg::tri::Index(a->cm, p.F()->V(p.E())), v - a->cm.vert.begin() ) );
            info[nearestF1].AddToBorder(    vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), p.F()->P1(p.E()) ),
                                            std::make_pair( v - a->cm.vert.begin(), vcg::tri::Index(a->cm, p.F()->V1(p.E())) ) );
            info[nearestF].Init( *nearestF, vcg::tri::Index(a->cm, nearestF->V(0)), vcg::tri::Index(a->cm, nearestF->V(1)), vcg::tri::Index(a->cm, nearestF->V(2)) );
            info[nearestF1].Init( *nearestF1, vcg::tri::Index(a->cm, nearestF1->V(0)), vcg::tri::Index(a->cm, nearestF1->V(1)), vcg::tri::Index(a->cm, nearestF1->V(2)) );
            tbt_faces.push_back( nearestF );    tbt_faces.push_back( nearestF1 );
            p.NextB();
            continue;
        }

        int cnt = 0;    //counter (inf. loop)
        // Not-adjacent; recursive split using mid-point of edge border
        stack.push_back( std::make_pair( vcg::tri::Index( a->cm, p.F()->V(p.E()) ),
                                         vcg::tri::Index( a->cm, p.F()->V1(p.E()) ) ) );    //Edge border
        assert( vcg::face::IsBorder( *p.F(), p.E() ) );
        stack_faces.push_back( std::make_pair(nearestF, nearestF1) );   //Nearest Faces
        p.NextB();
        while ( !stack.empty() ) {
            std::pair< int, int > border_edge = stack.back(); stack.pop_back();   //Da aggiornare
            CMeshO::FacePointer start = stack_faces.back().first; CMeshO::FacePointer end = stack_faces.back().second; //facce di A, non richiedono update
            stack_faces.pop_back();
            cnt++; assert( cnt < 100 );  //Assert (exit for in case of inf. loop)

            if ( start == end ) {
                tbt_faces.push_back( start );
                info[start].AddToBorder(    vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() ),
                                            std::make_pair( border_edge.first, border_edge.second ) );
                info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) )  );
                verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
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
                vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
                if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
                verts.push_back(v - a->cm.vert.begin());   verts.push_back( border_edge.second );  verts.push_back( end_v );    //first triangle
                verts.push_back(v - a->cm.vert.begin());   verts.push_back( end_v );    verts.push_back( border_edge.first );   //second triangle
                info[start].AddToBorder(    vcg::Segment3<CMeshO::ScalarType> ( a->cm.vert[border_edge.first].P(), (*v).P() ),
                                            std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
                info[start].Init( *start, vcg::tri::Index( a->cm, start->V(0) ), vcg::tri::Index( a->cm, start->V(1) ), vcg::tri::Index( a->cm, start->V(2) ) );
                info[end].AddToBorder(  vcg::Segment3<CMeshO::ScalarType> ( (*v).P(), a->cm.vert[border_edge.second].P() ),
                                        std::make_pair( v - a->cm.vert.begin(), border_edge.second ) );
                info[end].Init( *end, vcg::tri::Index( a->cm, end->V(0) ), vcg::tri::Index( a->cm, end->V(1) ), vcg::tri::Index( a->cm, end->V(2) ) );
                tbt_faces.push_back( start ); tbt_faces.push_back( end );
                continue;
            }

            int w = sharesVertex( start, end ); vcg::Segment3<CMeshO::ScalarType> s( a->cm.vert[border_edge.first].P(), a->cm.vert[border_edge.second].P() );
            if ( w != -1 && vcg::SquaredDistance<float>( s, start->P(w) ) <= eps ) {
                vcg::Segment3<CMeshO::ScalarType> e( p.F()->P2(p.E()), start->P(w) );
                vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu );
                (*v).P() = e.P(1.1);
                if ( vpu.NeedUpdate() )  vpu.Update( p.V() );
                stack.push_back(    std::make_pair( border_edge.first, v - a->cm.vert.begin() ) );
                stack.push_back(    std::make_pair( v - a->cm.vert.begin() , border_edge.second ) );
                CMeshO::FacePointer currentF = grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest);
                (*v).P() = closest;
                stack_faces.push_back( std::pair< CMeshO::FacePointer, CMeshO::FacePointer > (start, currentF) );
                stack_faces.push_back( std::pair< CMeshO::FacePointer, CMeshO::FacePointer > (currentF, end) );
                assert( stack.size() == stack_faces.size() );
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
    } while ( p.F() != start );

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
    std::vector< CMeshO::CoordType > coords; int patch_verts = verts.size();
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

    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    vcg::tri::Clean<CMeshO>::RemoveSmallConnectedComponentsSize( a->cm, a->cm.fn / 100 );
    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm );
    a->cm.face.DisableColor();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    //garbage collector
    vcg::tri::Allocator<CMeshO>::CompactVertexVector(a->cm);
    vcg::tri::Allocator<CMeshO>::CompactFaceVector(a->cm);

    Log(GLLogStream::DEBUG, "**********************" );
    Log(GLLogStream::DEBUG, "End - Remove %d faces from patch - Created %d new faces", c_faces, n_faces);

    return true;
}

Q_EXPORT_PLUGIN(FilterZippering)
