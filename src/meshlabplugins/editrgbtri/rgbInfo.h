#ifndef RGBINFO_H_
#define RGBINFO_H_

/****************************************************************************
* Rgb Triangulations Plugin                                                 *
*                                                                           *
* Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
* Copyright(C) 2007                                                         *
* DISI - Department of Computer Science                                     *
* University of Genova                                                      *
*                                                                           *
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
****************************************************************************/

#include <common/meshmodel.h>
#include <iostream>

namespace rgbt
{
using vcg::Point3f;
using std::list;
/// Represent a sorted pair of vertex
struct VertexPair
{
    VertexPair() {}
    /// Costruct a new pair 
    VertexPair(int v1, int v2)  
    {
        assert(v1 != v2);
        if (v1 <= v2)
        {
            this->v1 = v1;
            this->v2 = v2;            
        }
        else
        {
            this->v1 = v2;
            this->v2 = v1;            
        }
        
    }
    
    friend int operator==(const VertexPair& l, const VertexPair& r)
    {
        return (l.v1 == r.v1 && l.v2 == r.v2);
    }
    /// First vertex
    int v1;
    /// Second vertex
    int v2;
};
/// Additional information for a Vertex needed by the RGB triangulation
class VertexInfo
{
public:
    
    VertexInfo() 
    {
    	resetInfo();
    }
    /// Reset the Vertex at the original status
    inline void resetInfo()
    {
    	level = 0;
    	count = 0;
    	isPinfReady = false;
    	Point3f pzero = Point3f(0,0,0);
    	pl = pzero;
    	pinf = pzero;
    	isMarked = false;
    	isBorder = false;
    }

    /// Set vertex level
    inline void setLevel(short int l)
    {
        level = l;
    }

    /// Get vertex level
    inline const short int getLevel()
    {
        return level;
    }
    /// Get vertex control point at level l
    inline const Point3f& getPl()
    {
    	return pl;
    }
    /// Set vertex control point at level l
    inline void setPl(Point3f& p)
    {
    	pl = p;
    }
    /// Get vertex control point at level inf
    inline const Point3f& getPinf()
    {
    	return pinf;
    }
    /// Set vertex control point at level inf
    inline void setPinf(Point3f& p)
    {
    	pinf = p;
    }
    /// Get number of contribute already given
    inline int getCount()
    {
    	return count;
    }
    /// Set number of contribute already given
    inline void setCount(int c)
    {
    	count = c;
    }
    /// Get the number of incident edges in the base mesh
    inline int getBaseArity()
    {
    	return arity;
    }
    /// Set the number of incident edges in the base mesh
    inline void setBaseArity(int a)
    {
    	arity = a;
    }
    
    /// Return true if Pinf is already computed
    inline bool getIsPinfReady()
    {
    	return isPinfReady;
    }
    /// Return the status of Pinf
    inline void setIsPinfReady(bool c)
    {
    	isPinfReady = c;
    }
    /// Return true if the vertex is Marked
    inline bool getIsMarked()
    {
    	return isMarked;
    }
    /// Set the mark
    inline void setIsMarked(bool c)
    {
    	isMarked = c;
    }
    /// Return true if the vertex is inserted in the current selective refinement
    inline bool getIsNew()
    {
    	return isNew;
    }
    /// Set if the vertex is inserted in the current selective refinement
    inline void setIsNew(bool c)
    {
    	isNew = c;
    }
    
    /// Return true if the vertex is on the border
    inline bool getIsBorder()
    {
        return isBorder;
    }
    /// Set if the vertex is on the border
    inline void setIsBorder(bool c)
    {
        isBorder = c;
    }
    
    /// List that contain the index of all the vertexes that has contributed to Pinf
    inline list<int>& taken()
    {
    	return cTaken;
    }
    /// List that contain the index of all the vertexes that depends on this vertex for Pinf
    inline list<int>& given()
    {
    	return cGiven;
    }
    

private:
	/// Level of vertex
    short int level;
    /// Point at insertion level
    Point3f pl;
    /// Point at inf level
    Point3f pinf;
    /// Count the numbers of p^l accumulated
    int count;
    /// Indicate if Pinf contaian a correct value or only the sum of p^l-1
    bool isPinfReady;
    /// Is used during recursive split of green edges performed during the calculation of control point for an odd vertex
    bool isMarked;
    /// Is used during recursive split of green edges performed during the calculation of control point for an odd vertex
    bool isNew;
    /// Contain the indexes of the vertex that has contributed to Pinf
    list<int> cTaken;
    /// Contain the indexes of the vertex that has this Pl added in the Pinf
    list<int> cGiven;
    /// Indicates if the vertex is on the border of the mesh
    bool isBorder;
    /// Number of incident edge in the base mesh
    int arity;
    
};

/// Additional information for a Face needed by the RGB triangulation
class FaceInfo
{
public:
    FaceInfo() :
        color(FACE_GREEN), level(0)
    {

    }
    
    /// Color of a face
    typedef enum
    {
        FACE_RED_GGR = 1, /*!< RED, starting from the highest level vertex in ccw order the edges are green, green, red*/
        FACE_RED_RGG = 2, /*!< RED, starting from the highest level vertex in ccw order the edges are red, green, green*/
        FACE_GREEN = 0, /*!< GREEN*/
        FACE_BLUE_GGR = 3, /*!< BLUE, starting from the lowest level vertex in ccw order the edges are green, green, red*/
        FACE_BLUE_RGG = 4, /*!< BLUE, starting from the lowest level vertex in ccw order the edges are red, green, green*/
    } FaceColor;

    /// Edge Color
    typedef enum
    {
        EDGE_RED = 0, /*!< RED*/
        EDGE_GREEN = 1 /*!< GREEN*/
    } EdgeColor;

    /// Set the face color
    inline void setColor(FaceColor fc)
    {
        color = fc;
    }
    
    /// Get the face color
    inline const FaceColor getColor()
    {
        return color;
    }

    /// Set the face level
    inline void setLevel(short int l)
    {
        level = l;
    }

    /// Get the face level
    inline const short int getLevel()
    {
        return level;
    }

private:
	/// Face color
    FaceColor color;
    /// Face level
    short int level;
};

/// Container for all the informations needed by a rgb triangulation
class RgbInfo
{
public:
	typedef std::vector<VertexInfo> VERTEXC;
	typedef std::vector<FaceInfo> FACEC;
	/// Allocate information for nvert vertexes and nface faces
    RgbInfo(int nvert, int nface)
    {
        vert.resize(nvert);
        face.resize(nface);
    }
    virtual ~RgbInfo()
    {
    };
    /// Vertex informations
    VERTEXC vert;
    /// Face informations
    FACEC face;
};

template<class TRI_MESH_TYPE> class RgbVertex;
/// Wrapper for RgbInfo and CMeshO that allow a trasparent access to geometrical, topological and rgb informations on a triangle 
template<class TRI_MESH_TYPE> class RgbTriangle
{
public:

    typedef FaceInfo::FaceColor FaceColor;
    typedef FaceInfo::EdgeColor EdgeColor;

    /// The tetrahedral mesh type
    typedef TRI_MESH_TYPE TriMeshType;
    /// The face type
    typedef typename TriMeshType::FaceType FaceType;
    /// The vertex type
    typedef typename FaceType::VertexType VertexType;
    /// The vertex type pointer
    typedef typename FaceType::VertexType* VertexPointer;
    /// Face Pointer
    typedef typename TriMeshType::FacePointer FacePointer;

	typedef typename VertexType::ScalarType ScalarType;
	typedef typename VertexType::CoordType CoordType;


    
    // Standard costructor for use in STL containers
    RgbTriangle() {}
    /// Costruct a new RGBTriangle for the triangle of index TriangleIndex
    RgbTriangle(TriMeshType& M, RgbInfo& Info, int TriangleIndex) :
        m(&M), rgbInfo(&Info), index(TriangleIndex) 
    {
        assert(TriMeshType::HasFFTopology());
        updateInfo();
    }
    /// Costruct a new RGBTriangle for the triangle of index TriangleIndex
    RgbTriangle(TriMeshType* M, RgbInfo* Info, int TriangleIndex) :
        m(M), rgbInfo(Info), index(TriangleIndex) 
    {
        assert(TriMeshType::HasFFTopology());
        updateInfo();
    }
    
    /// Calculate all non-encoded information
    inline void updateInfo()
    {
        vArray[0] = RgbVertex<TriMeshType>(*m, *rgbInfo, getIndex(face()->V(0)));
        vArray[1] = RgbVertex<TriMeshType>(*m, *rgbInfo, getIndex(face()->V(1)));
        vArray[2] = RgbVertex<TriMeshType>(*m, *rgbInfo, getIndex(face()->V(2)));

        int z;
        switch (getFaceColor())
        {
        case FaceInfo::FACE_GREEN:
            ec[0] = FaceInfo::EDGE_GREEN;
            ec[1] = FaceInfo::EDGE_GREEN;
            ec[2] = FaceInfo::EDGE_GREEN;
            el[0] = getFaceLevel();
            el[1] = getFaceLevel();
            el[2] = getFaceLevel();
            va[0] = 2;
            va[1] = 2;
            va[2] = 2;
            break;
        case FaceInfo::FACE_RED_GGR:
            z = maxLevelVertex();
            ec[(0+z)%3] = FaceInfo::EDGE_GREEN;
            ec[(1+z)%3] = FaceInfo::EDGE_GREEN;
            ec[(2+z)%3] = FaceInfo::EDGE_RED;
            el[(0+z)%3] = getFaceLevel()+1;
            el[(1+z)%3] = getFaceLevel();
            el[(2+z)%3] = getFaceLevel();
            va[(0+z)%3] = 3;
            va[(1+z)%3] = 2;
            va[(2+z)%3] = 1;
            
            break;
        case FaceInfo::FACE_RED_RGG:
            z = maxLevelVertex();
            ec[(0+z)%3] = FaceInfo::EDGE_RED;
            ec[(1+z)%3] = FaceInfo::EDGE_GREEN;
            ec[(2+z)%3] = FaceInfo::EDGE_GREEN;
            el[(0+z)%3] = getFaceLevel();
            el[(1+z)%3] = getFaceLevel();
            el[(2+z)%3] = getFaceLevel()+1;
            va[(0+z)%3] = 3;
            va[(1+z)%3] = 1;
            va[(2+z)%3] = 2;
            break;
        case FaceInfo::FACE_BLUE_GGR:
            z = minLevelVertex();
            ec[(0+z)%3] = FaceInfo::EDGE_GREEN;
            ec[(1+z)%3] = FaceInfo::EDGE_GREEN;
            ec[(2+z)%3] = FaceInfo::EDGE_RED;
            el[(0+z)%3] = getFaceLevel()+1;
            el[(1+z)%3] = getFaceLevel()+1;
            el[(2+z)%3] = getFaceLevel();
            va[(0+z)%3] = 1;
            va[(1+z)%3] = 4;
            va[(2+z)%3] = 1;
            break;
        case FaceInfo::FACE_BLUE_RGG:
            z = minLevelVertex();
            ec[(0+z)%3] = FaceInfo::EDGE_RED;
            ec[(1+z)%3] = FaceInfo::EDGE_GREEN;
            ec[(2+z)%3] = FaceInfo::EDGE_GREEN;
            el[(0+z)%3] = getFaceLevel();
            el[(1+z)%3] = getFaceLevel()+1;
            el[(2+z)%3] = getFaceLevel()+1;
            va[(0+z)%3] = 1;
            va[(1+z)%3] = 1;
            va[(2+z)%3] = 4;
            break;
        }
    }
    
    /// Return the i vertex of the triangle 
    inline RgbVertex<TriMeshType>& V(int i)
    {
        assert(i>=0&& i<= 2);
        return vArray[i];
    }

    /// Get the vertex level of the vertex i
    inline int getVl(int i)
    {
        assert(i>=0&& i<= 2);
        return V(i).info().getLevel();
    }

    /// Set the vertex level of the vertex i, update = false disable the update of non-encoded info
    inline void setVl(int i, int level, bool update = true)
    {
        assert(i>=0&& i<= 2);
        V(i).info().setLevel(level);
        if (update) 
            updateInfo();
    }

    /// Get the vertex coordinates of the vertex i
    inline CoordType& getVertexCoord(int VertexIndex)
    {
    	return face()->V(VertexIndex)->P();
    }
    
    /// Set the vertex coordinates of the vertex i
    inline void setVertexCoord(int VertexIndex, CoordType c)
    {
    	face()->V(VertexIndex)->P() = c;
    }
    
    /// Get the vertex new flag of the vertex i
    inline bool getVertexIsNew(int i)
    {
        assert(i>=0&& i<= 2);
        return V(i).info().getIsNew();
    }
    
    /// Set the vertex new flag of the vertex i
    inline void setVertexIsNew(int i, bool c)
    {
        assert(i>=0&& i<= 2);
        V(i).info().setIsNew(c);
    }
    
    /// Return true if the vertex is on the border
    inline bool getVertexIsBorder(int i)
    {
        assert(i>=0&& i<= 2);
        return V(i).info().getIsBorder();
    }
    
    /// Return the number of border edges incident in v
    inline int getNumberOfBoundaryEdge(RgbVertex<TRI_MESH_TYPE>* v)
    {
    	int t = 0;
    	assert(v);
    	assert(v->index == V(0).index || v->index == V(1).index || v->index == V(2).index);
        for (int i = 0; i < 3; ++i) 
        {
        	if (getEdgeIsBorder(i) && ((V(i).index == v->index) || (V((i+1)%3).index == v->index)))
        		t++;
		}
        assert(t>=0 && t<= 2);
        return t;
    }
    
    /// Set if the vertex is on the border
    inline void setVertexIsBorder(int i,bool c)
    {
        assert(i>=0&& i<= 2);
        V(i).info().setIsBorder(c);
    }
    /// Return true if the edge is on the border
    inline bool getEdgeIsBorder(int i)
    {
        assert(i>=0&& i<= 2);
        return (FF(i).index == index);
    }
    
    
    /// Return the number of vertex at level l present in the triangle
    inline int countVertexAtLevel(int l)
    {
        int count = 0;
        for (int i = 0; i < 3; ++i) 
        {
            if (getVl(i) == l)
                ++count;
        }
        return count;
    }
    
    /// Search for the VertexIndex index in the vertex of the triangle, result is the local index if exist
    inline bool containVertex(int index, int* result = 0)
    {
        for (int i = 0; i < 3; ++i) 
        {
            if (getVIndex(i) == index)
            {
                if (result)
                    *result = i;
                return true;
            }
        }
        return false;
    }
    
    /// Extract an edge as a sorted pair of vertex
    inline VertexPair extractVertexFromEdge(int i)
    {
        assert(i>=0&& i<= 2);
        return VertexPair(getIndex(face()->V(i)),getIndex(face()->V((i+1)%3)));
    }

    /// Check if the edge vp is on the boundary of the triangle
    inline bool containEdge(VertexPair vp, int* index = 0)
    {
        for (int i = 0; i < 3; ++i) 
        {
            if (vp == extractVertexFromEdge(i))
            {
            	if (index)
            		*index = i;
                return true;
            }
        }
        return false;
    }
    
    /// Extract a red edge (use only if the edge exist)
    inline VertexPair getRedEdge()
    {
        assert(getFaceColor() != FaceInfo::FACE_GREEN);
        
        for (int i = 0; i < 3; ++i) 
        {
            if (getEdgeColor(i) == FaceInfo::EDGE_RED)
                return extractVertexFromEdge(i);
        }
        assert(0);
				return VertexPair();
    }
    
    /// Get the face color
    inline FaceColor getFaceColor()
    {
        return info()->getColor();
    }

    /// Set the face color
    inline void setFaceColor(FaceColor fc, bool update = true)
    {
        using vcg::Color4b;
        info()->setColor(fc);
        
#ifdef RGBCOLOR 
        
        Color4b& c = face()->C();

        switch (fc)
        {
        case FaceInfo::FACE_BLUE_GGR:
        case FaceInfo::FACE_BLUE_RGG:
            c.Import(Color4b(0,0,80,255));
            break;
        case FaceInfo::FACE_GREEN:
            c.Import(Color4b(0,80,0,255));
            break;
        case FaceInfo::FACE_RED_GGR:
        case FaceInfo::FACE_RED_RGG:
            c.Import(Color4b(80,0,0,255));
            break;
        }
#endif
        
#ifndef RGBCOLOR 
        
        Color4b& c = face()->C();

        c.Import(Color4b(255,255,255,255));
#endif
        
        
        if (update)
            updateInfo();
    }

    /// Check if the face is blue
    inline bool isBlue()
    {
        return ((getFaceColor() == FaceInfo::FACE_BLUE_GGR) || (getFaceColor() == FaceInfo::FACE_BLUE_RGG)); 
    }

    /// Check if the face is red
    inline bool isRed()
    {
        return ((getFaceColor() == FaceInfo::FACE_RED_GGR) || (getFaceColor() == FaceInfo::FACE_RED_RGG)); 
    }

    /// Check if the face is green
    inline bool isGreen()
    {
        return (getFaceColor() == FaceInfo::FACE_GREEN); 
    }

    /// Get the face level
    inline int getFaceLevel()
    {
        return info()->getLevel();
    }

    /// Set the face level
    inline void setFaceLevel(int level, bool update = true)
    {
        info()->setLevel(level);
        if (update)
            updateInfo();
    }

    /// Return the triangle in relation FF and incident in the i-th edges
    inline RgbTriangle<TriMeshType> FF(int i)
    {
        assert(i>=0&& i<= 2);
        return RgbTriangle<TriMeshType>(m, rgbInfo, getIndex(face()->FFp(i)));
    }

    /// Return the index (in respect of FF(i) of the common edge 
    inline int FFi(int i)
    {
        assert(i>=0&& i<= 2);
        return face()->FFi(i);
    }

    /// Return a Pointer to the RgbInfo Face
    inline FaceInfo* info()
    {
        return &(rgbInfo->face[index]);
    }

    /// Return a Pointer to the VCG Face
    inline FacePointer face()
    {
        return &(m->face[index]);
    }

    /// Return the color of the i-th edge
    inline EdgeColor getEdgeColor(int i)
    {
        assert(i>=0 && i<= 2);
        return ec[i];
    }

    /// Return the level of the i-th edge
    inline int getEdgeLevel(int i)
    {
        assert(i>=0 && i<= 2);
        return el[i];
    }

    /// Return the index of the max level vertex
    inline int maxLevelVertex()
    {
        int level = getVl(0);
        int index = 0;
        if (getVl(1) > level)
        {
            level = getVl(1);
            index = 1;
        }
        if (getVl(2) > level)
        {
            level = getVl(2);
            index = 2;
        }
        return index;
    }
    
    /// Return the index of the min level vertex
    inline int minLevelVertex()
    {
        int level = getVl(0);
        int index = 0;
        if (getVl(1) < level)
        {
            level = getVl(1);
            index = 1;
        }

        if (getVl(2) < level)
        {
            level = getVl(2);
            index = 2;
        }

        return index;
    }
    
    /// Return the index of the min level edge
    inline int minLevelEdge()
    {
        int level = getEdgeLevel(0);
        int index = 0;
        if (getEdgeLevel(1) < level)
        {
            level = getEdgeLevel(1);
            index = 1;
        }

        if (getEdgeLevel(2) < level)
        {
            level = getEdgeLevel(2);
            index = 2;
        }

        return index;
    }

    /// Return the index of the max level edge
    inline int maxLevelEdge()
    {
        int level = getEdgeLevel(0);
        int index = 0;
        if (getEdgeLevel(1) > level)
        {
            level = getEdgeLevel(1);
            index = 1;
        }

        if (getEdgeLevel(2) > level)
        {
            level = getEdgeLevel(2);
            index = 2;
        }

        return index;
    }
    
    /// Return the absolute index of the i-th vertex
    inline int getVIndex(int i)
    {
    	assert(i>=0 && i<= 2);
    	assert (vArray[i].index == getIndex(face()->V(i)));
    	return getIndex(face()->V(i));
    }
    
    /// Return the level of the i-th edge
    inline int getAngle(int i)
    {
        assert(i>=0 && i<= 2);
        return va[i];
    }
    
    /// VCG Mesh
    TriMeshType* m;
    /// Rgb information
    RgbInfo* rgbInfo;
    /// Absolute (and common to m and rgbInfo) index of the face
    int index;
    /// Utilities for extract the index of a face starting from a pointer
    inline int getIndex(FacePointer fp)
    {
        return fp->Index();
    }
    /// Utilities for extract the index of a vertex starting from a pointer
    inline int getIndex(VertexPointer vp)
    {
        return vp - &(m->vert.front());
    }
    
private:
    /// Vertex of the triangle (stored for efficiency in access)
    RgbVertex<TriMeshType> vArray[3];
    /// Color of the edges
    EdgeColor ec[3];
    /// Level of the edges
    int el[3];
    /// Angle size
    int va[3];


};

/// Wrapper for an edge identified by a triangle and an index
template<class TRI_MESH_TYPE>
class RgbEdge
{
public:
	RgbEdge(RgbTriangle<TRI_MESH_TYPE> t, int index) : t(t), index(index) {};
	/// An rgb triangle incident
	RgbTriangle<TRI_MESH_TYPE> t;
	/// Edge index on t
	int index;
	/// additional index used by VF, if first == 0 the common vertex is the one with the lowest index
//	int first;
	
	inline int getLevel()
	{
		return t.getEdgeLevel(index);
	}
	
//	inline 
};

/// Wrapper for RgbInfo and CMeshO that allow a trasparent access to geometrical, topological and rgb informations of a vertex
template<class TRI_MESH_TYPE> class RgbVertex
{
public:

    /// The tetrahedral mesh type
    typedef TRI_MESH_TYPE TriMeshType;
    /// The face type
    typedef typename TriMeshType::FaceType FaceType;
    /// The vertex type
    typedef typename FaceType::VertexType VertexType;
    /// The vertex type pointer
    typedef typename FaceType::VertexType* VertexPointer;
    /// Face Pointer
    typedef typename TriMeshType::FacePointer FacePointer;

    typedef typename VertexType::ScalarType ScalarType;
	typedef typename VertexType::CoordType CoordType;
	/// Standard Costructor for use in STL container
    RgbVertex() {}

    /// Costruct a new RGBVertex for the vertex of index VertexIndex
    RgbVertex(TriMeshType& M, RgbInfo& Info, int VertexIndex) :
        m(&M), rgbInfo(&Info), index(VertexIndex)
    {
    }
    /// Costruct a new RGBVertex for the vertex initialized with a vertex pointer
    RgbVertex(TriMeshType& M, RgbInfo& Info, VertexPointer vp) :
        m(&M), rgbInfo(&Info)
    {
    	index = getIndex(vp);
    }
    /// Restore the RGB info of the Vertex at the original state
    inline void resetInfo()
    {
    	rgbInfo->vert[index].resetInfo();
    }
    /// Return the RGB info associated at the vertex
    inline VertexInfo& info()
    {
        return (rgbInfo->vert[index]);
    }
    /// Return the VCG info associated at the vertex
    inline VertexType& vert()
    {
        return (m->vert[index]);
    }
    /// Get vertex control point at level l
    inline const Point3f& getPl()
    {
    	return info().getPl();
    }
    /// Set vertex control point at level l
    inline void setPl(Point3f& p)
    {
    	info().setPl(p);
    }
    /// Set vertex control point at level inf
    inline const Point3f& getPinf()
    {
    	return info().getPinf();
    }
    /// Set vertex control point at level inf
    inline void setPinf(Point3f& p)
    {
    	info().setPinf(p);
    }
    /// Get number of contribute already given
    inline int getCount()
    {
    	return info().getCount();
    }
    /// Set number of contribute already given
    inline void setCount(int c)
    {
    	info().setCount(c);
    }
    
    /// Get the vertex coordinates
    inline CoordType& getCoord()
    {
    	return vert().P();
    }
    
    /// Set the vertex coordinates
    inline void setCoord(CoordType c)
    {
    	vert().P() = c;
    }
    /// Return true if Pinf is already computed
    inline bool getIsPinfReady()
    {
    	return info().getIsPinfReady();
    }
    /// Return the status of Pinf
    inline void setIsPinfReady(bool c)
    {
    	info().setIsPinfReady(c);
    }
    /// Get the level of the vertex
    inline int getLevel()
    {
        return info().getLevel();
    }
    /// Set the level of the vertex
    inline void setLevel(int level)
    {
        info().setLevel(level);
    }
    /// Return true if the vertex is Marked
    inline bool getIsMarked()
    {
    	return info().getIsMarked();
    }
    /// Set the mark
    inline void setIsMarked(bool c)
    {
    	info().setIsMarked(c);
    }
    /// Return true if the vertex is inserted in the current selective refinement
    inline bool getIsNew()
    {
    	return info().getIsNew();
    }
    /// Set if the vertex is inserted in the current selective refinement
    inline void setIsNew(bool c)
    {
    	info().setIsNew(c);
    }    
    
    /// Return true if the vertex is on the border
    inline bool getIsBorder()
    {
        return info().getIsBorder();
    }
    /// Set if the vertex is on the border
    inline void setIsBorder(bool c)
    {
        info().setIsBorder(c);
    }
    
    /// Utilities for extract the index of a vertex starting from a pointer
    inline int getIndex(VertexPointer vp)
    {
        return vp - &(m->vert.front());
    }
    /// List that contain the index of all the vertexes that has contributed to Pinf
    inline list<int>& taken()
    {
    	return info().taken();
    }
    /// List that contain the index of all the vertexes that depends on this vertex for Pinf
    inline list<int>& given()
    {
    	return info().given();
    }

    inline int VFi()
    {
    	return vert().VFi();
    }
    
    inline VertexType* vp()
    {
    	return &(m->vert[index]);
    }
    
    inline RgbTriangle<TRI_MESH_TYPE> VFp()
    {
    	FacePointer fp = vert().VFp();
    	return RgbTriangleC(m,rgbInfo,fp->Index());
    }
    
    typedef RgbVertex<TRI_MESH_TYPE> RgbVertexC;
    typedef RgbTriangle<TRI_MESH_TYPE> RgbTriangleC;
    typedef RgbEdge<TRI_MESH_TYPE> RgbEdgeC;
    
	void VF(std::vector<RgbEdgeC>& ledge)
	{
		if (ledge.size() == 0)
			ledge.reserve(6);
		RgbTriangleC t = VFp();
		int VertexIndex = VFi();
	    bool isBorder = t.getVertexIsBorder(VertexIndex);
	    
	    vcg::face::Pos<FaceType> pos(t.face(),t.face()->V(VertexIndex));

	    if (t.getNumberOfBoundaryEdge(&(t.V(VertexIndex))) >= 2)
	    {
    		ledge.push_back(RgbEdgeC(t,VertexIndex));
	    	return;
	    }
	    	
	    if (isBorder)       // if is border move cw until the border is found
	    {
	        pos.FlipE();
	        pos.FlipF();
	        
	        while (!pos.IsBorder())
	        {
	            pos.FlipE();
	            pos.FlipF();
	        }
	        
	        pos.FlipE();
	    }
	    
	    CMeshO::FacePointer first = pos.F();

	    RgbTriangleC ttmp = RgbTriangleC(t.m,t.rgbInfo,pos.F()->Index());
	    int itmp = 0;
	    ttmp.containVertex(index,&itmp);
		ledge.push_back(RgbEdgeC(ttmp,itmp));

	    pos.FlipF();
	    pos.FlipE();
	 
	    while(pos.F() != first)
	    {
		    ttmp = RgbTriangleC(t.m,t.rgbInfo,pos.F()->Index());
		    ttmp.containVertex(index,&itmp);
			ledge.push_back(RgbEdgeC(ttmp,itmp));
	        
	        if (pos.IsBorder())
	            break;
	        
	        pos.FlipF();
	        pos.FlipE();
	    }
	    
	    int indexV = t.getVIndex(VertexIndex);
	    int res;
	    for (unsigned int i = 0; i < ledge.size(); ++i) 
	    {
	        assert(ledge[i].t.containVertex(indexV,&res));
	        if (!isBorder)
	        {
	            assert(ledge[i].t.FF((res+2)%3).face() == ledge[(i+1)%ledge.size()].t.face());
	        }
	        assert(!ledge[i].t.face()->IsD());
	    }
	}
    
    friend std::ostream& operator<<(std::ostream& os, RgbVertex<TriMeshType>& v)
    {
      os << "RgbVertexIndex " << v.index << "|";
      
      os << v.getCoord()[0] << " " << v.getCoord()[1] << " " << v.getCoord()[2]; 
      
      os << std::endl;
      return os;
    }
	
    /// Get the number of incident edges in the base mesh
    inline int getBaseArity()
    {
    	return info().getBaseArity();
    }
    /// Set the number of incident edges in the base mesh
    inline void setBaseArity(int a)
    {
    	info().setBaseArity(a);
    }
    
	/// VCG Mesh
    TriMeshType* m;
    /// Rgb information container
    RgbInfo* rgbInfo;
public:
	/// Absolute (and common to m and rgbInfo) index of the vertex
    int index;
};

}

#endif /*RGBINFO_H_*/
