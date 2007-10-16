#ifndef RGBPRIMITIVES_H_
#define RGBPRIMITIVES_H_

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

#include "rgbInfo.h"
#include <meshlab/meshmodel.h>
#include "topologicalOp.h"
#include <vcg/space/point3.h>
#include "controlPoint.h"

namespace rgbt
{

/// Class that contain static functions for coarsening and refining operations on rgb triangulation
class RgbPrimitives
{
	/// RGB Triangle
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    /// RGB Vertex
    typedef RgbVertex<CMeshO> RgbVertexC;
    
    /// The tetrahedral mesh type
    typedef CMeshO TriMeshType;
    /// The face type
    typedef TriMeshType::FaceType FaceType;
    /// The vertex type
    typedef FaceType::VertexType VertexType;
    /// The vertex type pointer
    typedef FaceType::VertexType* VertexPointer;
    /// The vertex iterator type
    typedef TriMeshType::VertexIterator VertexIterator;
    /// The tetra iterator type
    typedef TriMeshType::FaceIterator FaceIterator;
    /// The coordinate type
    typedef FaceType::VertexType::CoordType CoordType;
    /// The scalar type
    typedef TriMeshType::VertexType::ScalarType ScalarType;
    ///the container of tetrahedron type
    typedef TriMeshType::FaceContainer FaceContainer;
    ///the container of vertex type
    typedef TriMeshType::VertContainer VertContainer;
    ///half edge type
    typedef TriMeshType::FaceType::EdgeType EdgeType;
    /// vector of pos
    typedef std::vector<EdgeType> EdgeVec;
    ///of VFIterator
    typedef vcg::face::VFIterator<FaceType> VFI;
    /// vector of VFIterator
    typedef std::vector<vcg::face::VFIterator<FaceType> > VFIVec;
    /// Face Pointer
    typedef TriMeshType::FacePointer FacePointer;
    /// Topological Operation Class
    typedef TopologicalOp<CMeshO,RgbInfo::VERTEXC,RgbInfo::FACEC > TopologicalOpC;
    /// Vector of FaceColor 
    typedef vector<FaceInfo::FaceColor> vectorFaceColor;
    /// Vector of RgbTriangle 
    typedef vector<RgbTriangleC> vectorRgbTriangle;
    
public:
	/// Test a triangle for correctness in level of its vertexes
	static bool triangleVertexCorrectness(RgbTriangleC& t);
	/// Test a triangle for correctness in respect with adjacent triangle
	static bool triangleAdjCorrectness(RgbTriangleC& t);
	/// Globally test the correctess of a triangle
	static bool triangleCorrectness(RgbTriangleC& t);
	/// Make a green triangle at level level
	static void g_Make(RgbTriangleC& t, int level);
	/// Make a red_rgg triangle at level level
	static void r_rgg_Make(RgbTriangleC& t, int level);
	/// Make a red_ggr triangle at level level
	static void r_ggr_Make(RgbTriangleC& t, int level);
	/// Make a blue_rgg triangle at level level
	static void b_rgg_Make(RgbTriangleC& t, int level);
	/// Make a blue_ggr triangle at level level
	static void b_ggr_Make(RgbTriangleC& t, int level);
	/// Test if it's possible to perform a gg-split
	static bool gg_Split_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Test if it's possible to perform a rg-split
	static bool rg_Split_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Test if it's possible to perform a rr-split
	static bool rr_Split_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Test if it's possible to perfomr an edge split
	static bool edgeSplit_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Perform a single gg-split (do not check if the split is valid)
	static void gg_SplitSingle(RgbTriangleC& t, int EdgeIndex);
	/// Perform a single rg-split (do not check if the split is valid)
	static void rg_SplitSingle(RgbTriangleC& t, int EdgeIndex);
	/// Perform a single rr-split (do not check if the split is valid)
	static void rr_SplitSingle(RgbTriangleC& t, int EdgeIndex);
	/// Perform a single edge split (check if the split is possible, if not the call has no effect)
	static void edgeSplitSingle(RgbTriangleC& t, int EdgeIndex);
	/// Perform a gg-split using to for topology operation (do not check if the split is valid)
    static void gg_Split(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform a rg-split using to for topology operation (do not check if the split is valid)
    static void rg_Split(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform a rr-split using to for topology operation (do not check if the split is valid)
    static void rr_Split(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform an edge  split using to for topology operation (check if the split is possible, if not the call has no effect)
    static bool edgeSplit(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// If possible perform an edge split on a green edge, eventually causing other split (if the edge is red the call has no effect)
    /**
     * vtr (optional) contain all the triangle involved in the operation
     */
    static bool recursiveEdgeSplitVV(RgbVertexC& v1,RgbVertexC& v2, TopologicalOpC& to, vector<RgbTriangleC>* vtr = 0);
    /// Wrapper for recursiveEdgeSplitVV
    static bool recursiveEdgeSplit(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vtr = 0);

    /// Always perform a single edge split on a green edge, eventually causing other split (if the edge is red the call has no effect)
    static bool recursiveEdgeSplitSingle(RgbTriangleC& t, int EdgeIndex);
    
    /// Test if it's possible to perform a bb-swap
	static bool bb_Swap_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Perform a bb-swap (do not check if the edge is valid)
	static void bb_Swap(RgbTriangleC& t, int EdgeIndex, vector<RgbTriangleC>* vt = 0);
	/// Perform a bb-swap only if some triangle in the FF relation form a valid configuration
	static void bb_Swap_If_Needed(RgbTriangleC& t, vector<RgbTriangleC>* vt = 0);
	
	/// Basic operation: perform a g-bisection assigning color and level at rgg and ggr
	static void g_Bisection(int level, RgbTriangleC& rgg, RgbTriangleC& ggr);
	/// Basic operation: perform a r-bisection assigning color and level at t1 and t2
	/*
	 * t1 and t2 are the involved triangle in any order
	 * color: is the color type of red requested
	 * vp: is the red edge
	 */
	static void r_Bisection(int level,FaceInfo::FaceColor color , RgbTriangleC& t1, RgbTriangleC& t2, VertexPair vp);
	
	/// Test if it's possible to perform a r4-merge
	static bool r4_Merge_Possible(RgbTriangleC& t, int VertexIndex);
	/// Test if it's possible to perform a r2gb-merge
	static bool r2gb_Merge_Possible(RgbTriangleC& t, int VertexIndex);
	/// Test if it's possible to perform a gbgb-merge
	static bool gbgb_Merge_Possible(RgbTriangleC& t, int VertexIndex);
	/// Test if it's possible to perform a g2b2-merge
	static bool g2b2_Merge_Possible(RgbTriangleC& t, int VertexIndex);
	/// Test if around the vertex specified is possible to perform some gg-swap to obtain a legal configuration for the removal of the vertex
	static bool gg_Swap_Possible(RgbTriangleC& t, int VertexIndex);
	/// Test if it's possible to remove the specified vertex
	static bool vertexRemoval_Possible(RgbTriangleC& t, int VertexIndex);
	/// Perform a rr merge: set the correct color and level at triangle t
    static void rr_Merge(int level, RgbTriangleC& t);
    /// Perform a gb merge: set the correct color and level at triangle t
    static void gb_Merge(int level, FaceInfo::FaceColor color , RgbTriangleC& t);
    /// Perform a r4 merge (do not check if the merge is valid)
	static void r4_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
	/// Perform a r2gb merge (do not check if the merge is valid)
    static void r2gb_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform a gbgb merge (do not check if the merge is valid)
    static void gbgb_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform a g2b2 merge (do not check if the merge is valid)
    static void g2b2_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform a vertex removal using a gg_Swap (check if the removal is possible, if not the call has no effect)
    static void gg_Swap(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Utilities that perform a gg_swap (without any check)
    static void gg_SwapAux(RgbTriangleC& t, int EdgeIndex, vector<RgbTriangleC>* vt = 0);
    /// Utilities that test if a gg_swap is possible (without any check)
    static bool gg_SwapAuxPossible(RgbTriangleC& t, int EdgeIndex);
    /// Perform a vertex removal (check if the removal is possible, if not the call has no effect)
    static void vertexRemoval(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Perform a single r4_Merge
    static void r4_MergeSingle(RgbTriangleC& t, int VertexIndex);
    /// Perform a single r2gb_Merge
    static void r2gb_MergeSingle(RgbTriangleC& t, int VertexIndex);
    /// Perform a single gbgb_Merge
    static void gbgb_MergeSingle(RgbTriangleC& t, int VertexIndex);
    /// Perform a single g2b2_Merge
    static void g2b2_MergeSingle(RgbTriangleC& t, int VertexIndex);
    /// Perform (if possible) a single vertex removal using a gg_swap
    static void gg_SwapSingle(RgbTriangleC& t, int VertexIndex);
    /// Perform a single vertex removal (check if the removal is possible, if not the call has no effect)
    static void vertexRemovalSingle(RgbTriangleC& t, int VertexIndex);

    /// Auxiliary function for case gg-swap 4g1b
    static void gg_Swap_4g1b(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Auxiliary function for case gg-swap 3g2r
    static void gg_Swap_3g2r(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    /// Auxiliary function for case gg-swap 6g
    static void gg_Swap_6g(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
 
    /// Test if the configuration is valid for the case gg_swap_4g1b
    static bool gg_Swap_4g1b_Possible(RgbTriangleC& t, int VertexIndex);
    /// Test if the configuration is valid for the case gg_swap_3g2r
    static bool gg_Swap_3g2r_Possible(RgbTriangleC& t, int VertexIndex);
    /// Test if the configuration is valid for the case gg_swap_6g
    static bool gg_Swap_6g_Possible(RgbTriangleC& t, int VertexIndex);
    /// Extract the vf relation (in ccw order) starting from t around vertexIndex
    /**
     * the relation is stored in fc
     */
    static void vf(RgbTriangleC& t, int VertexIndex, vectorRgbTriangle& fc);
    /// Check if level are correct in respect at case gg_swap_4g1b
    static bool check_4g1b_LevelCorrectness(vectorRgbTriangle& fc, int l);
    /// Check if level are correct in respect at case gg_swap_3g2r
    static bool check_3g2r_LevelCorrectness(vectorRgbTriangle& fc, int l);

	/// Test if it's possible to perform a 2gbrb-swap
	static bool brb2g_Swap_Possible(RgbTriangleC& t, int VertexIndex);
    /// Perform a r2gb merge (do not check if the merge is valid)
    static void brb2g_Swap(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    
    /// rb_Pattern removal possible
    static bool rb_Pattern_Removal_Possible(RgbTriangleC& t, int VertexIndex);
    /// rb_Pattern removal
    static void rb_Pattern_Removal(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    
    /// Check if the vertex is an internal vertex (the link is a closed chain of edge)
    static bool isVertexInternal(RgbTriangleC& t, int VertexIndex);
    /// Check if the vertex is an internal vertex (the link is a closed chain of edge)
    static bool isVertexInternal(RgbVertexC& v);
    /// Check if the 2 vertexes form an edge of some triangles
    /**
     * t (optional) contain an incident triangle of the edge v1,v2
     * ti (optional) contain the index of the edge v1,v2 in t
     */
    static bool IsValidEdge(RgbVertexC& v1,RgbVertexC& v2, RgbTriangleC* t = 0, int* ti = 0);
    
    /// Check if a boundary g-bisection is possible
    static bool b_g_Bisection_Possible(RgbTriangleC& t, int EdgeIndex);
    /// Perform a boundary g-bisection
    static void b_g_Bisection(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);    

    /// Check if a boundary r-bisection is possible
    static bool b_r_Bisection_Possible(RgbTriangleC& t, int EdgeIndex);
    /// Perform a boundary r-bisection
    static void b_r_Bisection(RgbTriangleC& t, int EdgeIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);    
    
    /// Test if it's possible to perform a b_r2_Merge
    static bool b_r2_Merge_Possible(RgbTriangleC& t, int VertexIndex);
    /// Perform a b_r2_Merge (do not check if the merge is valid)
    static void b_r2_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    
    /// Test if it's possible to perform a b_gb_Merge
    static bool b_gb_Merge_Possible(RgbTriangleC& t, int VertexIndex);
    /// Perform a b_gb_Merge (do not check if the merge is valid)
    static void b_gb_Merge(RgbTriangleC& t, int VertexIndex, TopologicalOpC& to, vector<RgbTriangleC>* vt = 0);
    
    
private:
	
	/// Extract colors from a vector of RgbTriangle 
    static void extractColor(vectorRgbTriangle& f,vectorFaceColor& c);
    /// Find the index of the first face that has its color equal to color
    static int findColorIndex(vectorFaceColor& vc,FaceInfo::FaceColor color);
    /// Auxiliary function that search the edge vp in the triangle vector t and save the results in et and ei
    static bool findEdgeInTriangleV(vector<RgbTriangleC>& vt, VertexPair& vp, RgbTriangleC& et, int& ei);
    /// Auxiliary function for EdgeSplit, split the passed triangle recursively
    static void recursiveEdgeSplitAux(RgbVertexC& v1, RgbVertexC& v2, TopologicalOpC& to, vector<RgbTriangleC>* vt);
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* r4p;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* r2gb1p;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* r2gb2p;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* gbgb1p;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* gbgb2p;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* g2b21p;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* g2b22p;
    
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* s6gp;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* s4g1bggr;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* s4g1brgg;
    /// Color Pattern
    static vector<FaceInfo::FaceColor>* s3g2rp;

};

/// Check if the second vector is equal to the first (the 2 vectors can be not aligned)
template<template<class> class CONTAINER, class ELEMTYPE>
static bool isMatch(CONTAINER<ELEMTYPE>& cont, CONTAINER<ELEMTYPE>& pattern)
{
    if (cont.size() != pattern.size())
        return false;
    int size = cont.size();
    for(int i=0; i<size;++i)
    {
        bool match = true;
        for(int z=0; z<size;++z)
        {
            if (cont[(i+z)%size] != pattern[z])
                match = false;
        }
        if (match) return true;
    }
    return false;
}

}

#endif /*RGBPRIMITIVES_H_*/
