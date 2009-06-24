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
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/topology.h>
#include <vcg/space/intersection2.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/glu_tesselator.h>

#include "filter_zippering.h"
#include "remove_small_cc.h"

//#define DEB 1
//#define REDUNDANCY_ONLY 1
//#define REFINE_PATCH_ONLY 1
#define DFACE -1


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
         switch(ID(action))	 {
                case FP_ZIPPERING :
                                target= md.mm();
                                foreach (target, md.meshList)
                                if (target != md.mm())  break;
                                parlst.addMesh ("FirstMesh", md.mm(), "Mesh (with holes)", "The mesh with holes.");
                                parlst.addMesh ("SecondMesh", target, "Patch", "The mesh that will be used as patch.");
                break;
                default : assert(0);
        }
}

bool FilterZippering::checkRedundancy(  CMeshO::FacePointer face,
                                        MeshModel *m,
                                        MeshFaceGrid &grid,
                                        CMeshO::ScalarType max_dist )
{
    /***********************/
    // Step1: campiona l'edge di bordo su B (uniform); per ogni punto campionato trova il closest Point su A. Se
    // anche un solo punto sta sul bordo, l'algoritmo fallisce
    // Trova l'edge di bordo su face (uno e uno solo)
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

        assert( nearestF != 0 );

        //assert( !isOnBorder( closest, nearestF ) );
        if ( isOnBorder(closest, nearestF ) )  return false;
    }

    //check if V2(i) has a closest point on border of m
    CMeshO::FacePointer nearestF = 0;
    vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&m->cm);
    vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
    MeshFaceGrid::ScalarType  dist = max_dist;  MeshFaceGrid::CoordType closest;
    nearestF =  grid.GetClosest(PDistFunct, markerFunctor, face->P2(i), max_dist, dist, closest);
    if ( max_dist <= fabs( dist ) ) return false;           //non ha punti vicini: assumiamo che "cada" nel buco
    if ( isOnBorder( closest, nearestF ) ) return false;    //Punto piu' vicino che sta sul bordo, non e' ridondante

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
            assert( nearestF != 0 );
            if ( isOnBorder(closest, nearestF ) )  return false;
        }
    }
    // redundant
    return true;
}

// data la faccia f e il punto point, verifica se il punto e' o meno su un edge di bordo
// se il punto cade su un vertice verifica se il vertice incide sul bordo
bool FilterZippering::isOnBorder( CMeshO::CoordType point, CMeshO::FacePointer f )  {
    //TODO: usare le coord bar
    // Parametri interpolanti non sempre buoni...
    // Per ogni edge misuriamo la distanza tra il punto e la retta su cui giace l'edge
    int cnt = 0;
    for ( int i = 0; i < 3; i ++ ) {
       vcg::Line3<CMeshO::ScalarType> line( f->P(i), f->P1(i) - f->P(i) );
       if ( vcg::Distance( line, point ) <= eps && vcg::face::IsBorder( *f, i ) ) { //giace su un edge di bordo
            cnt ++;
       }
    }
    assert( cnt < 3 );
    if ( cnt > 0 ) return true; //Cade su almeno un edge di bordo
    //cnt == 0. Closest potrebbe pero' coincidere con un vertice, per cui si controlla se coincide con un vertice
    //ed eventualmente si verifica se il vertice incide sul bordo
    cnt = 0;
    for ( int i = 0; i < 3; i ++ )
        if ( vcg::Distance( point, f->P(i) ) <= eps ) //Sara' vicino ad al piu' un vertice
            return isBorderVert( f, i );

    // Ne' vertice ne' bordo, va bene cosi
    return false;
}

/* Scoprire perchè non funziona
bool FilterZippering::isOnBorder2( CMeshO::CoordType point, CMeshO::FacePointer f )  {

    vcg::Triangle3<CMeshO::ScalarType> t( f->P(0), f->P(1), f->P(2) );
    float a, b, c;
    vcg::InterpolationParameters<vcg::Triangle3<CMeshO::ScalarType>, CMeshO::ScalarType>( t, point, a, b, c );

    if ( (a <= 1.00000 + EPSIL) && (a >= 1.00000 - EPSIL) ) return isBorderVert( f, 0 );
    if ( (b <= 1.00000 + EPSIL) && (b >= 1.00000 - EPSIL) ) return isBorderVert( f, 1 );
    if ( (c <= 1.00000 + EPSIL) && (c >= 1.00000 - EPSIL) ) return isBorderVert( f, 2 );

    if ( (a <= 0.00000 + EPSIL) && (a >= 0.00000 - EPSIL) ) return vcg::face::IsBorder(*f, 1);
    if ( (b <= 0.00000 + EPSIL) && (b >= 0.00000 - EPSIL) ) return vcg::face::IsBorder(*f, 2);
    if ( (c <= 0.00000 + EPSIL) && (c >= 0.00000 - EPSIL) ) return vcg::face::IsBorder(*f, 0);

    return false;
}*/

/* true se v incide su almeno una faccia di bordo, false altrimenti */
bool FilterZippering::isBorderVert( CMeshO::FacePointer f, int i ) {

    vcg::face::Pos<CMeshO::FaceType> p( f, i, f->V(i) );
    //ciclo
    do {
        if ( vcg::face::IsBorder( *p.F(), p.E() ) )
            return true;
        p.FlipE(); p.FlipF();
    } while(p.F() != f);

    return false;
}

bool FilterZippering::isAdjacent( CMeshO::FacePointer f1, CMeshO::FacePointer f2 ) {
    if ( f1 == f2 ) return false;
    return ((f1 == f2->FFp(0)) ||  (f1 == f2->FFp(1)) || (f1 == f2->FFp(2)));
}

void FilterZippering::handleBorder( aux_info &info,                                            //Auxiliar information for triangulatio
                                    vcg::Point3<CMeshO::ScalarType> N,                        //face normal (useful for proiection)
                                    std::vector<CMeshO::CoordType> &coords,           //output coords
                                    std::vector< CMeshO::VertexPointer > &pointers ) { //output triangles

    // Calc. rotation matrix
    vcg::Matrix44<CMeshO::ScalarType> rot_matrix;
    rot_matrix.SetRotateRad( vcg::Angle<CMeshO::ScalarType>( N, vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) ), N ^ vcg::Point3<CMeshO::ScalarType>(0.0, 0.0, 1.0) );

    // For each border...
    for (int i = 0; i < info.border.size(); i ++) {
        /*DEBUG*/
        bool conn = true; int c = searchComponent( info, info.border[i].edges.front(), conn ); polyline current;
        if ( conn ) current = info.conn[c]; else current = info.trash[c];
        info.AddCComponent( cutComponent( current, info.border[i], rot_matrix ) );
        polyline rev_border = info.border[i]; std::reverse( rev_border.edges.begin(), rev_border.edges.end() );
        for ( int k = 0; k < rev_border.edges.size(); k ++) rev_border.edges[k].Flip();
        info.AddTComponent( cutComponent( current, rev_border, rot_matrix ) );
        if ( conn ) info.RemoveCComponent( c ); else info.RemoveTComponent( c );
    }
#ifdef DEB
    Log(GLLogStream::DEBUG, "**********************");
    Log(GLLogStream::DEBUG, "Ha %d C comp.", info.nCComponent() );
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        Log(GLLogStream::DEBUG, "Compnente %d", i );
        for ( int j = 0; j < info.conn[i].edges.size(); j ++ )
            Log(GLLogStream::DEBUG, "P0 %f %f %f P1 %f %f %f",  info.conn[i].edges[j].P0().X(), info.conn[i].edges[j].P0().Y(), info.conn[i].edges[j].P0().Z(),
                                                                info.conn[i].edges[j].P1().X(), info.conn[i].edges[j].P1().Y(), info.conn[i].edges[j].P1().Z() );
    }
    Log(GLLogStream::DEBUG, "Ha %d T comp.", info.nTComponent() );
    for ( int i = 0; i < info.nTComponent(); i ++ ) {
        Log(GLLogStream::DEBUG, "Compnente %d", i );
        for ( int j = 0; j < info.trash[i].edges.size(); j ++ )
            Log(GLLogStream::DEBUG, "P0 %f %f %f P1 %f %f %f",  info.trash[i].edges[j].P0().X(), info.trash[i].edges[j].P0().Y(), info.trash[i].edges[j].P0().Z(),
                                                                info.trash[i].edges[j].P1().X(), info.trash[i].edges[j].P1().Y(), info.trash[i].edges[j].P1().Z() );
    }
    //debug_v = true;
    return;
#endif
    //assert(0);  //ha concluso con le componenti connesse
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        std::vector< vcg::Point3<CMeshO::ScalarType> > points;//riunisce i vertici in un vector
        std::vector< CMeshO::VertexPointer > vertices;
        for ( int j = 0; j < info.conn[i].verts.size(); j ++ )  {
            points.push_back( info.conn[i].verts[j].first->P() );
            vertices.push_back( info.conn[i].verts[j].first );
        }
        std::vector< int > indices;
        std::vector< std::vector< vcg::Point3<CMeshO::ScalarType> > > outlines; outlines.push_back( points );
        vcg::glu_tesselator::tesselate( outlines, indices );
        for ( int k = 0; k < indices.size(); k ++ ) {
            pointers.push_back( vertices[indices[k]] );
        }
#ifdef DEB
        if (tess.size() == 0) {
            Log(GLLogStream::DEBUG, "Ciclo");
            for ( int j = 0; j < polygon.size(); j ++ )
                Log(GLLogStream::DEBUG, "P%d - %f %f %f", j, polygon[j].X(), polygon[j].Y(), polygon[j].Z() );
        }
        Log(GLLogStream::DEBUG, "**********************");
        Log(GLLogStream::DEBUG, "Ha %d C comp.", info.nCComponent() );
        for ( int i = 0; i < info.nCComponent(); i ++ ) {
            Log(GLLogStream::DEBUG, "Compnente %d", i );
            for ( int j = 0; j < info.conn[i].edges.size(); j ++ )
                Log(GLLogStream::DEBUG, "P0 %f %f %f P1 %f %f %f",  info.conn[i].edges[j].P0().X(), info.conn[i].edges[j].P0().Y(), info.conn[i].edges[j].P0().Z(),
                                                                    info.conn[i].edges[j].P1().X(), info.conn[i].edges[j].P1().Y(), info.conn[i].edges[j].P1().Z() );
        }
#endif
    }

}

polyline FilterZippering::cutComponent( polyline comp,                                    //Component to be cut
                                        polyline border,                                  //border
                                        vcg::Matrix44<CMeshO::ScalarType> rot_mat ) {     //Rotation matrix
#ifdef DEB
    Log(GLLogStream::DEBUG, "BORDER" );
    for ( int j = 0; j < border.edges.size(); j ++ )
         Log(GLLogStream::DEBUG, "Da %f %f %f a %f %f %f",   border.edges[j].P0().X(),border.edges[j].P0().Y(), border.edges[j].P0().Z(),
                                                                border.edges[j].P1().X(),border.edges[j].P1().Y(), border.edges[j].P1().Z() );
#endif


    vcg::Point3<CMeshO::ScalarType> startpoint = border.edges.front().P0();
    vcg::Point3<CMeshO::ScalarType> endpoint = border.edges.back().P1();
    vcg::Point2<CMeshO::ScalarType> startpoint2D ( (rot_mat * startpoint).X(), (rot_mat * startpoint).Y() );
    vcg::Point2<CMeshO::ScalarType> endpoint2D ( (rot_mat * endpoint).X(), (rot_mat * endpoint).Y() );
    int startedge = 0, endedge = 0; float min_dist_s = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[0], startpoint ), min_dist_e = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[0], endpoint );
    // Cerca tra gli edge della componente quello su cui cade lo startpoint
    for ( int i = 0; i < comp.edges.size(); i ++ ) {
        if ( vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ) <= min_dist_s ) { startedge = i; min_dist_s = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], startpoint ); }
        if ( vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ) <= min_dist_e ) { endedge = i; min_dist_e = vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], endpoint ); }
    }
    polyline p;
    // inserisce il bordo nella nuova componente
    p.edges.insert( p.edges.begin(), border.edges.begin(), border.edges.end() );
    p.verts.insert( p.verts.begin(), border.verts.begin(), border.verts.end() );
    // caso particolare: startedge == endedge
    if ( startedge == endedge && isRight( startpoint2D, vcg::Point2<CMeshO::ScalarType> ( (rot_mat * border.edges.front().P1()).X(), (rot_mat * border.edges.front().P1()).Y() ) , endpoint2D ) ) {
        vcg::Segment3<CMeshO::ScalarType> join( endpoint, startpoint );
        p.edges.push_back( join ); p.verts.push_back( std::make_pair( border.verts.back().second, border.verts.front().first ) ); //Vertex pointers
        return p;
    }

    // caso generico: startedge!=endedge
    // cerca il punto a destra di endpoint su endedge, costruisce il segmento e procede oltre
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
        p.edges.push_back( comp.edges[i] ); std::pair<CMeshO::VertexPointer, CMeshO::VertexPointer> vs( comp.verts[i] );
        if ( (p.edges[p.edges.size()-2].P0() == p.edges.back().P0()) ||  (p.edges[p.edges.size()-2].P1() == p.edges.back().P1()) ) {
            p.edges.back().Flip();  //verso corretto
            vs = std::make_pair( comp.verts[i].second, comp.verts[i].first );   //Inverte i puntatori ai vertici
        }
        p.verts.push_back( vs );
    }

    vcg::Segment3<CMeshO::ScalarType> s( p.edges.back().P1() , startpoint );
    p.edges.push_back( s );
    p.verts.push_back( std::pair<CMeshO::VertexPointer, CMeshO::VertexPointer> ( p.verts.back().second, border.verts.front().first ) );

#ifdef DEB
    for ( int j = 0; j < p.edges.size(); j ++ )
         Log(GLLogStream::DEBUG, "Da %f %f %f a %f %f %f", p.edges[j].P0().X(),p.edges[j].P0().Y(), p.edges[j].P0().Z(),
                                                                p.edges[j].P1().X(),p.edges[j].P1().Y(), p.edges[j].P1().Z() );
#endif

    return p;
}

bool FilterZippering::isInside( polyline &comp, vcg::Segment3<CMeshO::ScalarType> s, vcg::Matrix44<CMeshO::ScalarType> rot_mat ) {

    for ( int i = 0; i < comp.edges.size(); i ++ ) {
        if ( vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], s.P0() ) <= eps ) return true;
        if ( vcg::SquaredDistance<CMeshO::ScalarType>( comp.edges[i], s.P1() ) <= eps ) return true;
    }

    return false;
}

int FilterZippering::searchComponent( aux_info &info,                            //Auxiliar info
                             vcg::Segment3<CMeshO::ScalarType> s,       //query segment
                             bool &conn ) {
    float min_dist = 10*eps; conn = true; int index = -1;
    // Verifica le componenti connesse; se ce ne è una in cui la distanza segmento-bordo è
    // inferiore a min_dist, diventa la nuova candidata
    for ( int i = 0; i < info.nCComponent(); i ++ ) {
        polyline current = info.conn[i];
        for ( int j = 0; j < current.edges.size(); j ++ ) {
            if ( vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P0() ) <= min_dist ) {
                min_dist = vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P0() ); index = i;
            }
            if ( vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P1() ) <= min_dist ) {
                min_dist = vcg::SquaredDistance<CMeshO::ScalarType>( current.edges[j], s.P1() ); index = i;
            }
        }
    }

    for ( int i = 0; i < info.nTComponent(); i ++ ) {
        polyline current = info.trash[i];
        for ( int j = 0; j < current.edges.size(); j ++ ) {
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

bool FilterZippering::isRight (vcg::Point2< float > p1, vcg::Point2< float > p2, vcg::Point2< float > pn)
{
        // Verifica il segno del determinante della matrice avente come colonne il vettore [1,1,1]
        // e i due vettori colonna contenenti, rispettivamente, le ascisse e le ordinate dei tre punti p1, p2 e pn.

        // Per ridurre il numero di moltiplicazioni nel calcolo utilizzo la matrice ottenuta sostituendo
        // le prime due righe con opportune combinazioni lineari (risp., riga 1 = riga 1 - riga 3  e  riga 2 = riga 2 - riga 3)
        if ( ( p1.X() - pn.X() ) * ( p2.Y() - pn.Y() ) - ( p2.X() - pn.X() ) * ( p1.Y() - pn.Y() )  >  0.0000 )
                return false;
        else
                return true;
}

int  FilterZippering::sharesVertex( CMeshO::FacePointer f1, CMeshO::FacePointer f2 ) {
    for (int i = 0; i < 3; i ++) {
        for (int k = 0; k < 3; k ++)
            if ( f1->V(i) == f2->V(k) ) return i;
    }
    return -1;
}

/* Zippering of two meshes (Turk approach)
 */
bool FilterZippering::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos *cb)
{
     if ( md.size() == 1 )   {
        Log(GLLogStream::FILTER,"Please add a second mesh");
        return false;
    }
    MeshModel *a = par.getMesh("FirstMesh");
    MeshModel *b = par.getMesh("SecondMesh");

    a->cm.face.EnableFFAdjacency();      vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm); a->cm.face.EnableMark(); a->cm.UnMarkAll();
    b->cm.face.EnableFFAdjacency();      vcg::tri::UpdateTopology<CMeshO>::FaceFace(b->cm); b->cm.face.EnableMark(); b->cm.UnMarkAll();

    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(b->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(b->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(b->cm);

    CMeshO::FacePointer patch_border;   //patch border
    CMeshO::ScalarType epsilon  = a->cm.bbox.Diag() / 30.0; //Su homer non trova niente, capire perche'
                                                            //Rivedere l'epsilon
    //Cerca una faccia di bordo e ne salva l'indice
    for ( CMeshO::FaceIterator fi = b->cm.face.begin(); fi != b->cm.face.end(); ++fi)
        if ( vcg::face::BorderCount<CMeshO::FaceType>(*fi) > 0 ) { patch_border = &*fi; break; }
    assert( vcg::face::BorderCount<CMeshO::FaceType>(*patch_border) > 0 );
    int patch_border_i = vcg::tri::Index( b->cm, patch_border ) + a->cm.fn; int limit = a->cm.fn;

    vcg::tri::Append<CMeshO, CMeshO>::Mesh( a->cm, b->cm ); vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm); a->cm.face.EnableColor(); //Useful for debug
    vcg::tri::UpdateNormals<CMeshO>::PerFaceNormalized(a->cm);   vcg::tri::UpdateFlags<CMeshO>::FaceProjection(a->cm);  vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalized(a->cm);
    patch_border = &a->cm.face[patch_border_i]; /*md.meshList.erase(md.meshList.begin()+1);*/ debug_v = false;
    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm ); eps = a->cm.bbox.Diag() / 20000000;
    Log(GLLogStream::DEBUG, "eps value %f", eps);

    /* STEP 1 - Removing Redundant Surfaces
     * Repeat until mesh surface remain unchanged:
     *   - Remove redundant triangles on the boundary of patch
     */
    vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::FacePointer> fpu;
    vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
    MeshFaceGrid grid_a; grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit );  //Grid on A
    bool changed; vcg::face::Pos<CMeshO::FaceType> p; CMeshO::FacePointer start; int c_faces = 0;

    do {     //Changed loop
        changed = false;
        //Search for a border face on patch
        if ( patch_border->IsD() || vcg::face::BorderCount<CMeshO::FaceType>(*patch_border) == 3 ) {
            if ( vcg::face::BorderCount<CMeshO::FaceType>(*patch_border) == 3 )  {
                if ( !patch_border->IsD() )
                    vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *patch_border );
            }
            patch_border = 0;
            for ( CMeshO::FaceIterator fi = a->cm.face.end()-1; fi != a->cm.face.begin(); --fi )
                if ( !(*fi).IsD() && (vcg::face::BorderCount<CMeshO::FaceType>(*fi) > 0) &&
                     (vcg::face::BorderCount<CMeshO::FaceType>(*fi) < 3) ) { patch_border = &*fi; break; }
            assert( patch_border != 0 );
        }
        // Set pos border
        int i; for (i=0; i<3 && !vcg::face::IsBorder(*patch_border, i); i++); assert( i < 3 ); //i-edge on border
        p.Set( patch_border, i, patch_border->V(i) ); CMeshO::FacePointer start = p.F();
        do  {
            if (p.F()->IsD()) { p.NextB(); continue; }  //Already deleted, step over
            if ( checkRedundancy( p.F(), a, grid_a, epsilon ) )  {  //Check face Redundacy
                changed = true; c_faces++;
                vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *(p.F()) );
                int j; for (j=0; j<3 && vcg::face::IsBorder(*(p.F()), j); j++);   //no border edge
                if (vcg::face::BorderCount<CMeshO::FaceType>( (*p.F()->FFp(j)) ) == 0) patch_border = p.F()->FFp(j);   //new border
            }
            if ( !(p.F()->IsD()) && vcg::face::BorderCount( *p.F() ) == 2 ) {   //Split face (simplification)
                CMeshO::FacePointer split_face = p.F(); p.NextB(); //Set p for next iteration
                int i; for (i=0; i<3 && vcg::face::IsBorder(*split_face, i); i++); assert( i < 3 ); //split non border edge
                CMeshO::FacePointer opp_face = split_face->FFp(i); int opp_edge = split_face->FFi(i);
                vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *split_face );
                if ( !(opp_face->IsD()) ) vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *opp_face );
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); if ( vpu.NeedUpdate() ) vpu.Update( p.V() );
                (*v).P() = (p.F()->P(i) + p.F()->P1(i))/2.0;
                CMeshO::FaceIterator f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 4, fpu ); if ( fpu.NeedUpdate() ) { fpu.Update(split_face); fpu.Update(opp_face); fpu.Update( p.F() ); fpu.Update( start ); fpu.Update( patch_border ); grid_a.Set( a->cm.face.begin(), a->cm.face.begin()+limit ); }
                (*f).V(0) = split_face->V(i);       (*f).V(1) = &*v;                    (*f).V(2) = split_face->V2(i);      ++f;
                (*f).V(0) = &*v;                    (*f).V(1) = split_face->V1(i);      (*f).V(2) = split_face->V2(i);      ++f;
                (*f).V(0) = opp_face->V1(opp_edge); (*f).V(1) = opp_face->V2(opp_edge); (*f).V(2) = &*v;
                (*f).N() = ( (*f).P(1) - (*f).P(2) )^( (*f).P(0)-(*f).P(2) ); (*f).N() *= -1;      ++f;
                (*f).V(0) = &*v;                    (*f).V(1) = opp_face->V2(opp_edge); (*f).V(2) = opp_face->V(opp_edge);
                (*f).N() = ( (*f).P(1) - (*f).P(2) )^( (*f).P(0)-(*f).P(2) ); (*f).N() *= -1;
            }
            else p.NextB();
        } while (p.F() != start);
        //Update topology
        vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    } while (changed);

    vcg::RemoveSmallConnectedComponentsSize<CMeshO>( a->cm, a->cm.fn / 500.00 );      //Remove spurious component
    /* End Step 1 */
#ifdef REDUNDANCY_ONLY
    Log(GLLogStream::DEBUG, "Rimosse %d facce ridondanti", c_faces);
    return true;
#endif
    /*************************/
    /* STEP 2 - Project patch points on mesh surface
     * and ricorsively subdivide face in smaller triangles until each patch's face has border vertices
     * lying in adiacent or coincident faces. Also collect informations for triangulation of mesh faces.
     */
    //Add optional attribute
    CMeshO::PerFaceAttributeHandle<aux_info> info = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<aux_info>  (a->cm, std::string("aux_info"));
    patch_border = 0;
    for ( CMeshO::FaceIterator fi = a->cm.face.end()-1; fi != a->cm.face.begin(); --fi )
            if ( !(*fi).IsD() && (vcg::face::BorderCount<CMeshO::FaceType>(*fi) > 0) &&
                 (vcg::face::BorderCount<CMeshO::FaceType>(*fi) < 3) ) { patch_border = &*fi; break; }
    assert( patch_border != 0 );
    int i; for (i=0; i<3 && !vcg::face::IsBorder(*patch_border, i); i++); assert( i < 3 );
    std::vector< CMeshO::FacePointer > tbt_faces; //To Be Triangulated
    p.Set( patch_border, i, patch_border->V1(i) ); start = p.F();

    std::vector< CMeshO::VertexPointer > verts;
    do {
        if (p.F()->IsD()) { p.NextB(); continue; }  //Already deleted, step over
        CMeshO::FacePointer nearestF = 0, nearestF1 = 0;
        assert( vcg::face::BorderCount( *p.F() ) > 0 ); assert( vcg::face::IsBorder( *p.F(), p.E() ) );  //Check border correctness
        vcg::tri::FaceTmark<CMeshO> markerFunctor; markerFunctor.SetMesh(&a->cm);
        vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
        MeshFaceGrid::ScalarType  dist = 2*epsilon;  MeshFaceGrid::CoordType closest;
        nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), 2*epsilon, dist, closest);
        assert( nearestF != 0 );//Useless?
        p.F()->P(p.E()) = closest; dist = 2*epsilon;
        nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor,  p.F()->P1(p.E()), 2*epsilon, dist, closest);
        assert( nearestF1 != 0 );//Useless?
        p.F()->P1(p.E()) = closest;
        std::vector < std::pair< CMeshO::VertexPointer, CMeshO::VertexPointer > > stack;
        std::vector < std::pair< CMeshO::FacePointer, CMeshO::FacePointer > > stack_faces;
        CMeshO::VertexPointer end_v = p.F()->V2(p.E());
        bool modified = true;
        while ( modified ) {
            modified = false;
            /* PRIMO CASO DEGENERE */
            // Uno dei due closest point giace su un vertice già esistente della mesh A
            // operiamo spostando il closest in modo da  evitare questa sovrapposizione.
            // - Check sul vertice p.P(p.E())
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

            /* SECONDO CASO DEGENERE */
            // Almeno uno dei due closest point giace su un edge della mesh A
            // operiamo come in precedenza
            for ( int k = 0; k < 3; k ++ ) {
                a->cm.UnMarkAll();
                vcg::Segment3<CMeshO::ScalarType> edge( nearestF->P(k), nearestF->P1(k) );
                if ( vcg::SquaredDistance<float>( edge, p.F()->P(p.E()) ) <= eps ) {
                    vcg::Segment3<CMeshO::ScalarType> s( vcg::Barycenter( *(p.F()) ), p.F()->P(p.E()) );
                    p.F()->P(p.E()) = s.P(0.9); nearestF =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P(p.E()), 2*epsilon, dist, closest);
                    //p.F()->P(p.E()) = closest;
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
                    //p.F()->P1(p.E()) = closest;
                    if ( isOnBorder( closest, nearestF1 ) ) {
                        p.F()->P1(p.E()) = s.P(1.1); nearestF1 =  grid_a.GetClosest(PDistFunct, markerFunctor, p.F()->P1(p.E()), 2*epsilon, dist, closest);
                    }
                    p.F()->P1(p.E()) = closest; modified = true;
                }
            }
        }
        //Check if nearest faces are coincident
        if ( nearestF == nearestF1 ) {
            info[nearestF].AddToBorder( std::make_pair( p.F()->V(p.E()), p.F()->V1(p.E())) );
            info[nearestF].Init( *nearestF ); tbt_faces.push_back( nearestF );
            if ( vcg::tri::Index( a->cm, nearestF ) == DFACE ) {
                    Log(GLLogStream::DEBUG, "NEARESTEQ Aggiungo %f %f %f a %f %f %f",p.F()->P(p.E()).X(), p.F()->P(p.E()).Y(), p.F()->P(p.E()).Z(),
                                                                                            closest.X(), closest.Y(), closest.Z() );
            }
            p.NextB(); continue;
        } //concident faces; no op, next loop
        vcg::tri::Allocator<CMeshO>::DeleteFace( a->cm, *p.F() ); //remove face, it will be overwritten by new faces

        //Check if nearest faces are adjacent
        if ( isAdjacent( nearestF, nearestF1 ) ) {
            //search for shared edge
            int shared; for ( int k = 0; k < 3; k ++ ) if ( nearestF->FFp(k) == nearestF1 ) shared = k;
            //search for closest point
            vcg::Segment3<CMeshO::ScalarType> edge( nearestF->P(shared), nearestF->P1(shared) );
            int sampleNum = 150; float step = 1.0/(sampleNum+1);
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
            if ( vpu.NeedUpdate() ) {
                //Aggiorna riferimenti in aux_info
                Log( GLLogStream::DEBUG, "Update");
                for ( int k = 0; k < verts.size(); k ++ ) { vpu.Update( verts[k] ); }
                vpu.Update( p.V() ); vpu.Update( end_v );
                for ( int k = 0; k < tbt_faces.size(); k ++ ) { //update auxiliar info
                    for ( int j = 0; j < info[tbt_faces[k]].border.size(); j ++ ) {
                        //border
                        for ( int i = 0; i < info[tbt_faces[k]].border[j].verts.size(); i ++ ) {
                            vpu.Update( info[tbt_faces[k]].border[j].verts[i].first );
                            vpu.Update( info[tbt_faces[k]].border[j].verts[i].second );
                        }
                    }
                    //trash comp
                    for ( int j = 0; j < info[tbt_faces[k]].trash.size(); j ++ ) {
                        for ( int i = 0; i < info[tbt_faces[k]].trash[j].verts.size(); i ++ ) {
                            vpu.Update( info[tbt_faces[k]].trash[j].verts[i].first );
                            vpu.Update( info[tbt_faces[k]].trash[j].verts[i].second );
                        }
                    }
                }
            }
            CMeshO::FacePointer split_face = p.F();
            verts.push_back(&*v);   verts.push_back(split_face->V1(p.E()));  verts.push_back(end_v);  //First triangle
            verts.push_back(&*v);   verts.push_back(end_v);  verts.push_back(split_face->V(p.E()));   //Second triangle

            info[nearestF].AddToBorder( std::make_pair( split_face->V(p.E()), &*v ) );
            info[nearestF1].AddToBorder( std::make_pair( &*v, split_face->V1(p.E()) ) );
            info[nearestF].Init( *nearestF );   info[nearestF1].Init( *nearestF1 );
            tbt_faces.push_back( nearestF );    tbt_faces.push_back( nearestF1 );
            if ( vcg::tri::Index( a->cm, nearestF ) == DFACE ) {
                    Log(GLLogStream::DEBUG, "NEAREST Aggiungo %f %f %f a %f %f %f",split_face->P(p.E()).X(), split_face->P(p.E()).Y(), split_face->P(p.E()).Z(),
                                                                                            closest.X(), closest.Y(), closest.Z() );
            }
            if ( vcg::tri::Index( a->cm, nearestF1 ) == DFACE ) {
                    Log(GLLogStream::DEBUG, "NEAREST1 Aggiungo %f %f %f a %f %f %f", closest.X(), closest.Y(), closest.Z(),
                                                                                     split_face->P1(p.E()).X(), split_face->P1(p.E()).Y(), split_face->P1(p.E()).Z() );
            }
            p.NextB();
            continue;
        }

        int cnt = 0;    //counter (inf. loop)
        // Not-adjacent; recursive split using mid-point of edge border
        stack.push_back( std::make_pair( p.F()->V(p.E()), p.F()->V1(p.E()) ) );    //Edge border
        assert( vcg::face::IsBorder( *p.F(), p.E() ) );
        stack_faces.push_back( std::make_pair(nearestF, nearestF1) );   //Nearest Faces
        p.NextB();
        while ( !stack.empty() ) {
            std::pair< CMeshO::VertexPointer, CMeshO::VertexPointer > border_edge = stack.back(); stack.pop_back();   //Da aggiornare
            CMeshO::FacePointer start = stack_faces.back().first; CMeshO::FacePointer end = stack_faces.back().second; //facce di A, non richiedono update
            stack_faces.pop_back();
            cnt++; assert( cnt < 100 );  //Assert (exit for in case of inf. loop)

            if ( start == end ) {
                tbt_faces.push_back( start );
                info[start].AddToBorder( std::make_pair( border_edge.first, border_edge.second ) );
                info[start].Init( *start );
                /*if ( vcg::tri::Index( a->cm, start ) == DFACE )  {
                    Log(GLLogStream::DEBUG, "STARTEQ Aggiungo %f %f %f a %f %f %f",tmp.P(0).X(), tmp.P(0).Y(), tmp.P(0).Z(),
                                                                                            tmp.P(1).X(), tmp.P(1).Y(), tmp.P(1).Z() );
                }*/
                verts.push_back( border_edge.first ); verts.push_back( border_edge.second ); verts.push_back( end_v );
                continue;
            }

            if ( isAdjacent( start, end ) ) {
                //Calcola il punto di intersezione (o una sua approssimazione) e splitta la faccia tmp
                int shared; for ( int k = 0; k < 3; k ++ ) if ( start->FFp(k) == end ) shared = k;
                //abbiamo l'indice dell'edge, su start, che separa le due facce
                //costruiamo il segmento associato
                vcg::Segment3<CMeshO::ScalarType> shared_edge( start->P(shared), start->P1(shared) );
                int sampleNum = 150; float step = 1.0/(sampleNum+1);
                vcg::Point3<CMeshO::ScalarType> closest;    float min_dist = shared_edge.Length();
                for ( int k = 0; k <= sampleNum; k ++ ) {
                    vcg::Point3<CMeshO::ScalarType> currentP = border_edge.first->P() + ( border_edge.second->P() - border_edge.first->P() ) * (k*step);
                    if ( vcg::SquaredDistance( shared_edge, currentP ) < min_dist ) {
                        closest = currentP; min_dist = vcg::SquaredDistance( shared_edge, closest );
                    }
                }
                assert( vcg::SquaredDistance( shared_edge, closest ) < shared_edge.Length() );
                //A questo punto abbiamo il closest point. Lo proiettiamo sul segmento e splittiamo tmp
                closest = vcg::ClosestPoint(shared_edge, closest); //Proietta su edge il punto closest
                //SplitFace( tmp, 0, closest, new_faces );    //Salva direttamente su newfaces
                vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = closest;
                if ( vpu.NeedUpdate() ) {
                    //Update references
                    Log( GLLogStream::DEBUG, "Update");
                    vpu.Update( p.V() ); vpu.Update( border_edge.first ); vpu.Update( border_edge.second ); vpu.Update( end_v );
                    for ( int k = 0; k < stack.size(); k ++ ) { vpu.Update( stack[k].first ); vpu.Update( stack[k].second ); }
                    for ( int k = 0; k < verts.size(); k ++ ) { vpu.Update( verts[k] ); }
                    for ( int k = 0; k < tbt_faces.size(); k ++ ) { //update auxiliar info
                        //border
                        for ( int j = 0; j < info[tbt_faces[k]].border.size(); j ++ ) {
                            for ( int i = 0; i < info[tbt_faces[k]].border[j].verts.size(); i ++ ) {
                                vpu.Update( info[tbt_faces[k]].border[j].verts[i].first );
                                vpu.Update( info[tbt_faces[k]].border[j].verts[i].second );
                            }
                        }
                        //trash comp
                        for ( int j = 0; j < info[tbt_faces[k]].trash.size(); j ++ ) {
                            for ( int i = 0; i < info[tbt_faces[k]].trash[j].verts.size(); i ++ ) {
                                vpu.Update( info[tbt_faces[k]].trash[j].verts[i].first );
                                vpu.Update( info[tbt_faces[k]].trash[j].verts[i].second );
                            }
                        }
                    }
                }
                verts.push_back(&*v);   verts.push_back( border_edge.second );  verts.push_back( end_v );    //first triangle
                verts.push_back(&*v);   verts.push_back( end_v );    verts.push_back( border_edge.first );   //second triangle
                info[start].AddToBorder( std::make_pair( border_edge.first, &*v ) );
                info[start].Init( *start );
                if ( vcg::tri::Index( a->cm, start ) == DFACE ) {
                    /*Log(GLLogStream::DEBUG, "START Aggiungo %f %f %f a %f %f %f",tmp.P(0).X(), tmp.P(0).Y(), tmp.P(0).Z(),
                                                                                            closest.X(), closest.Y(), closest.Z() );
                    vcg::Segment3<float> s0( start->P(0), start->P(1) );
                    vcg::Segment3<float> s1( start->P(1), start->P(2) );
                    vcg::Segment3<float> s2( start->P(2), start->P(0) );
                    Log(GLLogStream::DEBUG, "Distance 0: %f", vcg::SquaredDistance( s0, tmp.P(0) ) );
                    Log(GLLogStream::DEBUG, "Distance 1: %f", vcg::SquaredDistance( s1, tmp.P(0) ) );
                    Log(GLLogStream::DEBUG, "Distance 2: %f", vcg::SquaredDistance( s2, tmp.P(0) ) );*/
                }
                info[end].AddToBorder( std::make_pair( &*v, border_edge.second ) );
                info[end].Init( *end );
                if ( vcg::tri::Index( a->cm, end ) == DFACE ) {
                    /*Log(GLLogStream::DEBUG, "END Aggiungo %f %f %f a %f %f %f",  closest.X(), closest.Y(), closest.Z(),
                                                                                            tmp.P(1).X(), tmp.P(1).Y(), tmp.P(1).Z() );*/

                }
                tbt_faces.push_back( start ); tbt_faces.push_back( end );

                if ( vcg::tri::Index( a->cm, start ) == DFACE ) {
                    Log(GLLogStream::DEBUG, "start border" );
                    for (int j = 0; j < info[start].border.size(); j ++) {
                        polyline current = info[start].border[j];
                        for (int w = 0; w < current.edges.size(); w ++)
                            Log(GLLogStream::DEBUG, "Edge %i Da %f %f %f a %f %f %f", j,   current.edges[w].P0().X(), current.edges[w].P0().Y(), current.edges[w].P0().Z(),
                                                                                            current.edges[w].P1().X(), current.edges[w].P1().Y(), current.edges[w].P1().Z() );
                    }
                }
                if ( vcg::tri::Index( a->cm, end ) == DFACE ) {
                    Log(GLLogStream::DEBUG, "end border" );
                    for (int j = 0; j < info[end].border.size(); j ++) {
                        polyline current = info[end].border[j];
                        for (int w = 0; w < current.edges.size(); w ++)
                            Log(GLLogStream::DEBUG, "Edge %i Da %f %f %f a %f %f %f", j,   current.edges[w].P0().X(), current.edges[w].P0().Y(), current.edges[w].P0().Z(),
                                                                                            current.edges[w].P1().X(), current.edges[w].P1().Y(), current.edges[w].P1().Z() );
                    }
                }
                /*************/
                continue;
            }

            int w = sharesVertex( start, end ); vcg::Segment3<CMeshO::ScalarType> s( border_edge.first->P(), border_edge.second->P() );
            if ( w != -1 && vcg::SquaredDistance<float>( s, start->P(w) ) <= eps ) {
                vcg::Segment3<CMeshO::ScalarType> e( p.F()->P2(p.E()), start->P(w) );
                //SplitFace( tmp, 0, e.P(1.1), stack );
                vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
                CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = e.P(1.1);
                if ( vpu.NeedUpdate() ) {
                    //Update references
                    Log( GLLogStream::DEBUG, "Update");
                    vpu.Update( p.V() ); vpu.Update( border_edge.first ); vpu.Update( border_edge.second ); vpu.Update( end_v );
                    for ( int k = 0; k < stack.size(); k ++ ) { vpu.Update( stack[k].first ); vpu.Update( stack[k].second ); }
                    for ( int k = 0; k < verts.size(); k ++ ) { vpu.Update( verts[k] ); }
                    for ( int k = 0; k < tbt_faces.size(); k ++ ) { //update auxiliar info
                        for ( int j = 0; j < info[tbt_faces[k]].border.size(); j ++ ) {
                            //border
                            for ( int i = 0; i < info[tbt_faces[k]].border[j].verts.size(); i ++ ) {
                                vpu.Update( info[tbt_faces[k]].border[j].verts[i].first );
                                vpu.Update( info[tbt_faces[k]].border[j].verts[i].second );
                            }
                        }
                        //trash comp
                        for ( int j = 0; j < info[tbt_faces[k]].trash.size(); j ++ ) {
                            for ( int i = 0; i < info[tbt_faces[k]].trash[j].verts.size(); i ++ ) {
                                vpu.Update( info[tbt_faces[k]].trash[j].verts[i].first );
                                vpu.Update( info[tbt_faces[k]].trash[j].verts[i].second );
                            }
                        }
                    }
                }
                stack.push_back( std::make_pair( border_edge.first, &*v ) ); stack.push_back( std::make_pair( &*v, border_edge.second ) );
                CMeshO::FacePointer currentF = grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest);
                (*v).P() = closest;
                stack_faces.push_back( std::pair< CMeshO::FacePointer, CMeshO::FacePointer > (start, currentF) );
                stack_faces.push_back( std::pair< CMeshO::FacePointer, CMeshO::FacePointer > (currentF, end) );
                assert( stack.size() == stack_faces.size() );
                continue;
            }

            vcg::tri::Allocator<CMeshO>::PointerUpdater<CMeshO::VertexPointer> vpu;
            CMeshO::VertexIterator v = vcg::tri::Allocator<CMeshO>::AddVertices( a->cm, 1, vpu ); (*v).P() = (border_edge.first->P() + border_edge.second->P())/2.00;
            if ( vpu.NeedUpdate() ) {
                //Update references
                Log( GLLogStream::DEBUG, "Update");
                vpu.Update( p.V() ); vpu.Update( border_edge.first ); vpu.Update( border_edge.second ); vpu.Update( end_v );
                for ( int k = 0; k < stack.size(); k ++ ) { vpu.Update( stack[k].first ); vpu.Update( stack[k].second ); }
                for ( int k = 0; k < verts.size(); k ++ ) { vpu.Update( verts[k] ); }
                for ( int k = 0; k < tbt_faces.size(); k ++ ) { //update auxiliar info
                    //border
                    for ( int j = 0; j < info[tbt_faces[k]].border.size(); j ++ ) {
                        for ( int i = 0; i < info[tbt_faces[k]].border[j].verts.size(); i ++ ) {
                            vpu.Update( info[tbt_faces[k]].border[j].verts[i].first );
                            vpu.Update( info[tbt_faces[k]].border[j].verts[i].second );
                        }
                    }
                    //trash comp
                    for ( int j = 0; j < info[tbt_faces[k]].trash.size(); j ++ ) {
                        for ( int i = 0; i < info[tbt_faces[k]].trash[j].verts.size(); i ++ ) {
                            vpu.Update( info[tbt_faces[k]].trash[j].verts[i].first );
                            vpu.Update( info[tbt_faces[k]].trash[j].verts[i].second );
                        }
                    }
                }
            }
            CMeshO::FacePointer currentF = 0; CMeshO::CoordType closest;
            currentF =  grid_a.GetClosest(PDistFunct, markerFunctor, (*v).P(), 2*epsilon, dist, closest); //Proietta il midPoint sulla mesh A
            assert( currentF != 0 ); (*v).P() = closest;
            stack.push_back( std::make_pair( border_edge.first, &*v ) ); stack.push_back( std::make_pair( &*v, border_edge.second ) );
            stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( start, currentF ) );
            stack_faces.push_back( std::pair<CMeshO::FacePointer, CMeshO::FacePointer> ( currentF, end ) );
            assert( stack.size() == stack_faces.size() );
        }
    } while ( p.F() != start );

    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    /* End of Step 2 */
#ifdef REFINE_PATCH_ONLY
    for ( int k = 0; k < verts.size(); k += 3 ) {
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        (*f).V(0) = verts[k]; (*f).V(1) = verts[k+1]; (*f).V(2) = verts[k+2];
    }
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    return true;
#endif
    // triangolazione delle parti delle facce di A che non sono coperte dalle facce di B
    /* STEP 3 */
    /* Faces are sorted by index, than each face is triangulated using auxiliar information about
     * vertices and edges */
    std::sort( tbt_faces.begin(), tbt_faces.end() ); std::vector<CMeshO::FacePointer>::iterator it = std::unique( tbt_faces.begin(), tbt_faces.end() ); tbt_faces.resize(it - tbt_faces.begin() );
    std::vector< CMeshO::CoordType > coords; int patch_verts = verts.size();
    for ( int i = 0; i < tbt_faces.size(); i ++ ) {

        if ( !tbt_faces[i]->IsD() ) {
            handleBorder( info[tbt_faces[i]], tbt_faces[i]->N(), coords, verts );
            vcg::tri::Allocator<CMeshO>::DeleteFace(a->cm, *tbt_faces[i]);
        }
    }
    vcg::tri::Allocator<CMeshO>::DeletePerFaceAttribute<aux_info>(a->cm, info); //no more useful
    Log(GLLogStream::DEBUG, "**********************" );

    /*CMeshO::FacePointer face = tbt_faces[70];
    face->C() = vcg::Color4b::Cyan;*/

    /***********DEBUG**************/
//    for (int j = 0; j < info[face].border.size(); j ++ ) {
//        Log(GLLogStream::DEBUG, "BorderSize: %d", info[face].border[j].edges.size() );
//        for (int f = 0; f < info[face].border[j].edges.size(); f++)
//        Log(GLLogStream::DEBUG, "Bordo %i Da %f %f %f a %f %f %f", j,   info[face].border[j].edges[f].P0().X(),info[face].border[j].edges[f].P0().Y(), info[face].border[j].edges[f].P0().Z(),
//                                                                        info[face].border[j].edges[f].P1().X(),info[face].border[j].edges[f].P1().Y(), info[face].border[j].edges[f].P1().Z() );
//    }

    //Log(GLLogStream::DEBUG, "Faccia %d", vcg::tri::Index( a->cm, face ) );
    //handleBorder( info[face], face->N(), coords, v_pointers );
    //vcg::tri::Allocator<CMeshO>::DeleteFace(a->cm, *face);

    // Add new faces
    int n_faces = 0;
    for ( int k = 0; k < verts.size(); k += 3 ) { n_faces++;
        CMeshO::FaceIterator  f = vcg::tri::Allocator<CMeshO>::AddFaces( a->cm, 1 );
        if ( k < patch_verts ) {
            (*f).V(0) = verts[k]; (*f).V(1) = verts[k+1]; (*f).V(2) = verts[k+2]; (*f).N() = ( (*f).P(0) - (*f).P(2) )^( (*f).P(1)-(*f).P(2) );
        }
        else {
            (*f).V(0) = verts[k]; (*f).V(2) = verts[k+1]; (*f).V(1) = verts[k+2]; (*f).N() = ( (*f).P(0) - (*f).P(2) )^( (*f).P(1)-(*f).P(2) );
        }
    }

    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    vcg::RemoveSmallConnectedComponentsSize<CMeshO>( a->cm, a->cm.fn / 100 );
    vcg::tri::UpdateBounding<CMeshO>::Box( a->cm );
    a->cm.face.DisableColor();
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(a->cm);
    //garbage collector
    vcg::tri::Allocator<CMeshO>::CompactVertexVector(a->cm);
    vcg::tri::Allocator<CMeshO>::CompactFaceVector(a->cm);

    Log(GLLogStream::DEBUG, "Concluso - Remove %d faces from patch - Created %d new faces", c_faces, n_faces);

    return true;
}

Q_EXPORT_PLUGIN(FilterZippering)
