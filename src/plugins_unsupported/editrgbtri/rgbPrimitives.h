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
#include <common/meshmodel.h>
#include "topologicalOp.h"
#include "controlPoint.h"
#include "modButterfly.h"

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
    /// Pos
        typedef vcg::face::Pos<FaceType> Pos;
        

    
public:
	
    typedef enum {
    	LOOP,
    	MODBUTFLY
    } subtype;
	
	/// Test a triangle for correctness in level of its vertexes
	static bool triangleVertexCorrectness(RgbTriangleC& t);
	/// Test a triangle for correctness in respect with adjacent triangle
	static bool triangleAdjCorrectness(RgbTriangleC& t);
	/// Globally test the correctess of a triangle
	static bool triangleCorrectness(RgbTriangleC& t);
	/// Test a triangle correctness in respect to the angles around its vertexes
	static bool triangleVertexAngleCorrectness(RgbTriangleC& t);
	/// Test if it's possible to perform a gg-split
	static bool gg_Split_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Test if it's possible to perform a rg-split
	static bool rg_Split_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Test if it's possible to perform a rr-split
	static bool rr_Split_Possible(RgbTriangleC& t, int EdgeIndex);
	/// Test if it's possible to perfomr an edge split
	static bool edgeSplit_Possible(RgbTriangleC& t, int EdgeIndex);
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
    
	/// Find the 3 vertexes of the same level of the corresponding triangle
	static RgbVertexC findOppositeVertex(RgbTriangleC& t, int EdgeIndex, vector<RgbVertexC>* firstVertexes);
    
	/// Split all the green edges incident in v if they have a level < of the parameter level - 1
	static void splitGreenEdgeIfNeeded(RgbVertexC& v, int level, TopologicalOpC& to);
	
	/// Split all the red edges incident in v if they have a level < of the parameter level - 1
	static void splitRedEdgeIfNeeded(RgbVertexC& v, int level, TopologicalOpC& to);

    /// Extract the face in ccw order around the vertex v
    static void VF(RgbVertexC& v,vector<FacePointer>& vfp);

    /// Update the normal of v
    static void updateNormal(RgbVertexC& v);
    
    //! Return the VV relation
    static void VV(RgbVertexC& v, vector<RgbVertexC>& vv, bool onlyGreenEdge = false);

    //! Return the number of incident edges in the base mesh (the BaseArity fields in RgbVertex must have been initialized)
    static unsigned int baseIncidentEdges(RgbVertexC& v);

    /// Type of Subdivision Surface
    static subtype stype;

private:

    //! Wrapper for the true edgeSplit. This function choose the type of subdivision surface to use
	/* Return true if on the current edge was performed only a topological split
	 * return false if a complete split (with update on rgb Info) was performed.
	 */ 
    static bool doSplit(RgbTriangleC& fp, int EdgeIndex, int level, TopologicalOpC& to , vector<FacePointer> *vfp = 0, RgbVertexC* vNewInserted = 0, vector<RgbVertexC>* vcont = 0, vector<RgbVertexC>* vupd = 0);
    //! Wrapper for the edge collapse. This function choose the type of subdivision surface to use
    static void doCollapse(RgbTriangleC& fp, int EdgeIndex, TopologicalOpC& to, Point3<ScalarType> *p = 0, vector<FacePointer> *vfp = 0);
	
	/// Extract colors from a vector of RgbTriangle 
    static void extractColor(vectorRgbTriangle& f,vectorFaceColor& c);
    /// Find the index of the first face that has its color equal to color
    static int findColorIndex(vectorFaceColor& vc,FaceInfo::FaceColor color);
    /// Auxiliary function for EdgeSplit, split the passed triangle recursively
    static void recursiveEdgeSplitAux(RgbVertexC& v1, RgbVertexC& v2, TopologicalOpC& to, vector<RgbTriangleC>* vt);
    /// Auxiliary function used after every split 
    static void distributeContribute(vector<RgbVertexC>& vCont,RgbVertexC& vNew,vector<RgbVertexC>& vUpd);
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
template<class CONTAINER>
static bool isMatch(CONTAINER& cont, CONTAINER& pattern)
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
