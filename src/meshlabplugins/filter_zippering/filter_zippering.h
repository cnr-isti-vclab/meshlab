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

****************************************************************************/

#ifndef FILTERZIPPERING_H
#define FILTERZIPPERING_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/grid_static_ptr.h>

#define SAMPLES_PER_EDGE 100

// Polyline (set of consecutive segments)
struct polyline {
    std::vector< vcg::Segment3<CMeshO::ScalarType> > edges;                         //polyline edges
    std::vector< std::pair<int, int> > verts;
};

//Auxiliar info for triangulation
struct aux_info {
    std::vector< polyline > conn;   //Close components (to be triangulated)
    std::vector< polyline > trash;  //Close components (to be deleted)
    std::vector< polyline > border; //Segment intersecting components
    float eps;                      //epsilon
    //Add segment c to border
    virtual bool AddToBorder( vcg::Segment3<CMeshO::ScalarType> c, std::pair<int, int> v  ) {
        /****Insert new segment****/
        //Search for segment S having S.P0() == c.P1 or S.P1 == c.P0()
        if ( v.first == v.second ) return false;
        if ( c.Length() < eps ) { c.P1() = c.P0(); v.second = v.first; }
        //check if c doesn't lie on existing edges
        //if it does, split edge
        for ( int i = 0; i < trash.size(); i ++ ) {
            for ( int j = 0; j < trash[i].edges.size(); j ++ ) { //Only one trash component
                vcg::Segment3<CMeshO::ScalarType> a, b;
                a = trash[i].edges[j]; b = c;
                float tx0 =  (float) (b.P0().X() - a.P0().X())/(a.P1().X() - a.P0().X());
                float ty0 =  (float) (b.P0().Y() - a.P0().Y())/(a.P1().Y() - a.P0().Y());
                float tz0 =  (float) (b.P0().Z() - a.P0().Z())/(a.P1().Z() - a.P0().Z());
                if ( (fabs(tx0 - ty0) < eps ) && (fabs(ty0 - tz0) < eps) ) {
                    float tx1 =  (float) (b.P1().X() - a.P0().X())/(a.P1().X() - a.P0().X());
                    float ty1 =  (float) (b.P1().Y() - a.P0().Y())/(a.P1().Y() - a.P0().Y());
                    float tz1 =  (float) (b.P1().Z() - a.P0().Z())/(a.P1().Z() - a.P0().Z());
                    if ( (fabs(tx1 - ty1) < eps) && (fabs(ty1 - tz1) < eps) ) {
                        if ( (tx0 < 0.0f + eps) && (tx1 > 1.0f - eps) ) return false;   //external
                        if ( (tx1 < 0.0f + eps) && (tx0 > 1.0f - eps) ) return false;   //external
                        if ( (tx0 >= 0.0f + eps) && (tx0 <= 1.0f - eps) && (tx1 >= 0.0f + eps) && (tx1 <= 1.0f - eps) ) {
                            //double split
                            if ( tx0 > tx1 ) { c.Flip(); v = std::make_pair( v.second, v.first ); }
                            //insert new edges
                            trash[i].edges.insert( trash[i].edges.begin() + j + 1, vcg::Segment3<CMeshO::ScalarType>( trash[i].edges[j].P0(), c.P0() ) );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 1, std::make_pair( trash[i].verts[j].first, v.first ) );
                            trash[i].edges.insert( trash[i].edges.begin() + j + 2, c );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 2, v );
                            trash[i].edges.insert( trash[i].edges.begin() + j + 3, vcg::Segment3<CMeshO::ScalarType>( c.P1(), trash[i].edges[j].P1() ) );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 3, std::make_pair( v.second, trash[i].verts[j].second ) );
                            //erase old one
                            trash[i].edges.erase( trash[i].edges.begin() + j );
                            trash[i].verts.erase( trash[i].verts.begin() + j );
                            return true;
                        }
                        if ( (tx0 >= 0.0f + eps) && (tx0 <= 1.0f - eps) ) {
                            //single split in P0
                            //insert new edges
                            trash[i].edges.insert( trash[i].edges.begin() + j + 1, vcg::Segment3<CMeshO::ScalarType>( trash[i].edges[j].P0(), c.P0() ) );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 1, std::make_pair( trash[i].verts[j].first, v.first ) );
                            trash[i].edges.insert( trash[i].edges.begin() + j + 2, vcg::Segment3<CMeshO::ScalarType>( c.P0(), trash[i].edges[j].P1() ) );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 2, std::make_pair( v.first, trash[i].verts[j].second ) );
                            //erase old one
                            trash[i].edges.erase( trash[i].edges.begin() + j );
                            trash[i].verts.erase( trash[i].verts.begin() + j );
                            return true;
                        }
                        if ( (tx1 >= 0.0f + eps) && (tx1 <= 1.0f - eps) ) {
                            //single split in P1
                            //insert new edges
                            trash[i].edges.insert( trash[i].edges.begin() + j + 1, vcg::Segment3<CMeshO::ScalarType>( trash[i].edges[j].P0(), c.P1() ) );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 1, std::make_pair( trash[i].verts[j].first, v.second ) );
                            trash[i].edges.insert( trash[i].edges.begin() + j + 2, vcg::Segment3<CMeshO::ScalarType>( c.P1(), trash[i].edges[j].P1() ) );
                            trash[i].verts.insert( trash[i].verts.begin() + j + 2, std::make_pair( v.second, trash[i].verts[j].second ) );
                            //erase old one
                            trash[i].edges.erase( trash[i].edges.begin() + j );
                            trash[i].verts.erase( trash[i].verts.begin() + j );
                            return true;
                        }
                    }
                }
            }
        }

        bool found = false;
        for ( unsigned int j = 0; j < border.size(); j ++ ) {
            for ( int i = 0; i < border[j].verts.size() && !found; i ++ ) {
                if ( border[j].verts[i].first == v.second ) { found = true; border[j].edges.insert( border[j].edges.begin() + i, c ); border[j].verts.insert( border[j].verts.begin() + i, v ); }           //insert before i-th element
                if ( border[j].verts[i].second == v.first ) { found = true; border[j].edges.insert( border[j].edges.begin() + i + 1, c ); border[j].verts.insert( border[j].verts.begin() + i + 1, v ); }   //insert after i-th element
            }
        }
        if (!found) {   //Create a new polyline ad add it to the border list
            polyline nwpoly; nwpoly.edges.push_back( c ); nwpoly.verts.push_back( v ); border.push_back( nwpoly );
        } else {
            //Merge consecutive polylines into a single one
            for ( unsigned int j = 0; j < border.size(); j ++ )
                for ( unsigned int i = j+1; i < border.size(); i ++ ) {
                    if ( border[j].verts.front().first == border[i].verts.back().second ) {
                        border[j].edges.insert( border[j].edges.begin(), border[i].edges.begin(), border[i].edges.end() );
                        border[j].verts.insert( border[j].verts.begin(), border[i].verts.begin(), border[i].verts.end() );
                        border.erase(border.begin() + i);
                    } 
					else if ( border[j].verts.back().second == border[i].verts.front().first ) {
							border[j].edges.insert( border[j].edges.end(), border[i].edges.begin(), border[i].edges.end() );
							border[j].verts.insert( border[j].verts.end(), border[i].verts.begin(), border[i].verts.end() );
							border.erase(border.begin() + i);
						 }
                }
        }//end if (!found)

        for ( int k = 0; k < border.size(); k ++) {
            for ( int i = 0; i < trash.size(); i ++ ) {
                for ( int j = 0; j < trash[i].verts.size(); j ++ ) { //Only one trash component
                    if ( trash[i].verts[j].first == border[k].verts.back().second ) {
                        trash[i].edges[j].P0() = border[k].edges.back().P1();
                        //trash[i].edges.erase( trash[i].edges.begin() + j ); trash[i].verts.erase( trash[i].verts.begin() + j )
                    }
                    if ( trash[i].verts[j].first == border[k].verts.front().first ) {
                        trash[i].edges[j].P0() = border[k].edges.front().P0();
                        //trash[i].edges.erase( trash[i].edges.begin() + j ); trash[i].verts.erase( trash[i].verts.begin() + j )
                    }
                }
            }
        }
        return true;
    }//end AddToBorder

    // Add c.component
    virtual void AddCComponent( polyline c ) {
        conn.push_back(c);
    }
    // Add t.component
    virtual void AddTComponent( polyline t ) {
        trash.push_back(t);
    }

    //Set eps
    virtual void SetEps( float e ) {
        eps = e;
    }

    // Set initial t.component
    virtual void Init( CMeshO::FaceType f, int a, int b, int c ) {
        if (!trash.empty()) return;
        polyline tri;   tri.edges.push_back( vcg::Segment3<CMeshO::ScalarType>(f.P(0), f.P(1)) );
                        tri.edges.push_back( vcg::Segment3<CMeshO::ScalarType>(f.P(1), f.P(2)) );
                        tri.edges.push_back( vcg::Segment3<CMeshO::ScalarType>(f.P(2), f.P(0)) );
                        tri.verts.push_back( std::make_pair(a, b) );
                        tri.verts.push_back( std::make_pair(b, c) );
                        tri.verts.push_back( std::make_pair(c, a) );
        AddTComponent( tri );
    }

    // Remove c.component
    virtual void RemoveCComponent( int i ) {
        conn.erase( conn.begin() + i );
    }

    // Remove t.component
    virtual void RemoveTComponent( int i ) {
        trash.erase( trash.begin() + i );
    }

    // Number of c.component
    virtual int nCComponent( ) {
        return conn.size();
    }

    // Number of t.component
    virtual int nTComponent( ) {
        return trash.size();
    }

};//end struct


class FilterZippering : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

        typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MeshFaceGrid;
        typedef vcg::GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > MeshVertGrid;



public:
        enum { FP_ZIPPERING  };

        FilterZippering();
	
        virtual const QString filterName(FilterIDType filter) const;
        virtual const QString filterInfo(FilterIDType filter) const;
	virtual bool autoDialog(QAction *) {return true;}
        virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
        const int getRequirements(QAction *action);
        virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
        virtual bool applyFilter(QAction *filter, MeshModel &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) {return true;}
	const FilterClass getClass(QAction *a);
	virtual int postCondition( QAction *a ) const { return MeshModel::MM_FACEFACETOPO|MeshModel::MM_VERTNORMAL; }



private:
        bool checkRedundancy(   CMeshO::FacePointer f,   //face
                                MeshModel *a,            //mesh A
                                MeshFaceGrid &grid,      //grid A
                                CMeshO::ScalarType max_dist );   //Max search distance
        bool isBorderVert( CMeshO::FacePointer f, int i);
        bool isOnBorder( CMeshO::CoordType point, CMeshO::FacePointer f );
        bool isAdjacent( CMeshO::FacePointer f1, CMeshO::FacePointer f2 );
        int  sharesVertex( CMeshO::FacePointer f1, CMeshO::FacePointer f2 );
        void handleBorder( aux_info &info,                                            //Auxiliar information for triangulatio
                           vcg::Point3<CMeshO::ScalarType> N,                        //face normal (useful for proiection)
                           std::vector<CMeshO::CoordType> &coords,          //output coords
                           std::vector<int> &output ); //output v. pointers
        polyline cutComponent(  polyline comp,                                   //Component to be cut
                                polyline border,                                 //border
                                vcg::Matrix44<CMeshO::ScalarType> rot_mat );     //Rotation matrix
        bool debug_v;
        int searchComponent( aux_info &info,                            //Auxiliar info
                             vcg::Segment3<CMeshO::ScalarType> s,       //query segment
                             bool &conn );
        float eps;
};

#endif
