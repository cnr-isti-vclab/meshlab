#ifndef TOPOLOGICALOP_H_
#define TOPOLOGICALOP_H_

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


#include <vcg/simplex/face/topology.h>
#include <vcg/complex/trimesh/base.h>
#include <vector>
#include <list>
#include <vcg/complex/trimesh/allocate.h>
#include <iostream>

#include <vcg/space/color4.h> 

namespace rgbt
{

using std::list;
using namespace vcg;
using std::vector;

/// Identify an Edge by the pair Face, Index on the face
template<class FacePointer> class EdgeFI
{
public:
    EdgeFI(FacePointer fp, int i)
    {
        assert(i>=0 && i<= 2);
        assert(fp);
        this->fp = fp;
        this->i = i;
    }
    
    EdgeFI() {}
    
    FacePointer fp;
    int i;
};

/// Contains the operation to update the topological structure of the triangulation
/*
 * Efficiently implement a garbage collect mechanism for vertex and triangle
 */
template <class TRI_MESH_TYPE, class VERTEXC = vector<int>, class FACEC = vector<int> >
class TopologicalOp
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
    /// The vertex iterator type
    typedef typename TriMeshType::VertexIterator VertexIterator;
    /// The tetra iterator type
    typedef typename TriMeshType::FaceIterator FaceIterator;
    /// The coordinate type
    typedef typename FaceType::VertexType::CoordType CoordType;
    /// The scalar type
    typedef typename TriMeshType::VertexType::ScalarType ScalarType;
    ///the container of tetrahedron type
    typedef typename TriMeshType::FaceContainer FaceContainer;
    ///the container of vertex type
    typedef typename TriMeshType::VertContainer VertContainer;
    ///half edge type
    typedef typename TriMeshType::FaceType::EdgeType EdgeType;
    /// vector of pos
    typedef typename std::vector<EdgeType> EdgeVec;
    ///of VFIterator
    typedef typename vcg::face::VFIterator<FaceType> VFI;
    /// vector of VFIterator
    typedef typename std::vector<vcg::face::VFIterator<FaceType> > VFIVec;
    /// Face Pointer
    typedef typename TriMeshType::FacePointer FacePointer;
    /// Edge defined by Face and Index
    typedef EdgeFI<FacePointer> EdgeFIType;
    
    
private:
	/// Working mesh (needed for the insert and garbage collect of faces and vertexes)
    TriMeshType& m;
    /// List of deleted faces that can be reused
    list<FacePointer> listFp;
    /// Size of listFp (size() is in O(n)) 
    int sizelistFp;
    /// List of deleted vertexes that can be reused
    list<VertexPointer> listVp;
    /// Size of listVp (size() is in O(n))
    int sizelistVp;
    /// Additional container that is reallocated if the vertex container is reallocated
    VERTEXC *vc;
    /// Additional container that is reallocated if the face container is reallocated
    FACEC *fc;
    //! Faces to add when the faces container is reallocated. The number is calculated by growNumberFace * oldNumberOfFace
    static const float growNumberFace() { return 2; }
    //! Vertexes to add when the vertexes container is reallocated.  The number is calculated by growNumberVertex * oldNumberOfVertex
    static const float growNumberVertex() { return  2; } 
    
public:
    /// Create a new TopologicalOp
    /**
     * vc and fc are container for other information linked with the vertex and face container 
     * that need the same resize of the mesh containers
     */ 
    TopologicalOp(TriMeshType& m, VERTEXC* vc = 0, FACEC* fc = 0) : m(m), vc(vc), fc(fc)
    {
        updateLists();
    }
    
    //! Perform an edge collapse on the specified edge
    /*
     * If vfp is given push on the vector the pointer at the face (in order) f00,f01,f10,f11
     */
    
    
    template<bool BOUNDARY>
    void doCollapse(EdgeFIType Edge, const Point3<ScalarType> *p, vector<FacePointer> *vfp = 0)
    {
        
        assert(FaceType::HasFFAdjacency());
        
        assert(Edge.fp);
        assert(Edge.i>= 0 && Edge.i <= 2);
        
        FacePointer f0p = Edge.fp;
        int f0i = Edge.i;
        
        std::vector<FacePointer> vec;
        vec.reserve(6);
        getAllFacesAroundVertex(f0p,(f0i+1)%3,vec,BOUNDARY);
        
        FacePointer f00p = 0;
        int f00i = -1;
        FacePointer f01p = 0;
        int f01i = -1;

        FacePointer f1p = 0;
        int f1i;
        FacePointer f10p = 0;
        int f10i = -1;
        FacePointer f11p = 0;
        int f11i = -1;

        if (f0p->FFp((f0i+2)%3) != f0p) // it exists a triangle f00
        {
            f00p = f0p->FFp((f0i+2)%3);
            f00i = f0p->FFi((f0i+2)%3);
        }
        
        if (f0p->FFp((f0i+1)%3) != f0p) // it exists a triangle f01
        {
            f01p = f0p->FFp((f0i+1)%3);
            f01i = f0p->FFi((f0i+1)%3);
        }        
        
        if (!BOUNDARY)
        {
            assert(f0p->FFp(f0i) != f0p); // it must exists a triangle f1 or this is a boundary configuration
            f1p = f0p->FFp(f0i);
            f1i = f0p->FFi(f0i);

            assert(f1p);            
            assert(f1i>= 0 && f1i <= 2);
            
            if (f1p->FFp((f1i+1)%3) != f1p)
            {
                f10p = f1p->FFp((f1i+1)%3);
                f10i = f1p->FFi((f1i+1)%3);
            }
            if (f1p->FFp((f1i+2)%3) != f1p)
            {
                f11p = f1p->FFp((f1i+2)%3);
                f11i = f1p->FFi((f1i+2)%3);
            }
        }
        
        if (f00p)
        {
            if (f01p)
            {
                f00p->FFp(f00i) = f01p;
                f00p->FFi(f00i) = f01i;
            }
            else
            {
                f00p->FFp(f00i) = f00p;
                f00p->FFi(f00i) = f00i;
            }
            
        }

        if (f01p)
        {
            if (f00p)
            {
                f01p->FFp(f01i) = f00p;
                f01p->FFi(f01i) = f00i;
            }
            else
            {
                f01p->FFp(f01i) = f01p;
                f01p->FFi(f01i) = f01i;
            }
                
        }
        
        if (!BOUNDARY)
        {
            if (f10p)
            {
                if (f11p)
                {
                    f10p->FFp(f10i) = f11p;
                    f10p->FFi(f10i) = f11i;
                }
                else
                {
                    f10p->FFp(f10i) = f10p;
                    f10p->FFi(f10i) = f10i;
                }
            }
    
            if (f11p)
            {
                if (f10p)
                {
                    f11p->FFp(f11i) = f10p;
                    f11p->FFi(f11i) = f10i;
                }
                else
                {
                    f11p->FFp(f11i) = f11p;
                    f11p->FFi(f11i) = f11i;
                }
                    
            }
        }        
        
        // warning: this update at the relation VF* of the vertex
        // break the relation VF on the face
        
        if (VertexType::HasVFAdjacency())
        {
            //assert(!FaceType::HasVFAdjacency());
            assert(f01p||f00p);
            if (f01p)
            {
                assert(f01p);
                f0p->V((f0i+2)%3)->VFp() = f01p;
                f0p->V((f0i+2)%3)->VFi() = f01i;
                
                f0p->V(f0i)->VFp() = f01p;
                f0p->V(f0i)->VFi() = (f01i+1)%3;
            }
            else
            {
                assert(f00p);
                f0p->V((f0i+2)%3)->VFp() = f00p;
                f0p->V((f0i+2)%3)->VFi() = (f00i+1)%3;
                f0p->V(f0i)->VFp() = f00p;
                f0p->V(f0i)->VFi() = f00i;
            }
            
            //f0p->V((f0i+1)%3)->VFp() = f11p;
            //f0p->V((f0i+1)%3)->VFi() = f11i;
            if (!BOUNDARY)
            {
                assert(f11p || f10p);
                assert(f11p);
                f1p->V((f1i+2)%3)->VFp() = f11p;
                f1p->V((f1i+2)%3)->VFi() = (f11i+1)%3;
            }
        }
        
        f0p->SetD();
        if (!BOUNDARY)
            f1p->SetD();
        
        if (!BOUNDARY)
            m.fn -= 2;
        else
            m.fn -= 1;
        
        if (!BOUNDARY)
            assert(f0p->V(f0i) == f1p->V((f1i+1)%3));
        
        VertexPointer v = f0p->V(f0i);
        VertexPointer v1 = f0p->V((f0i + 1)%3);
        if (p)
            v->P() = *p;
        
        // update all face around vertex v1
        typedef typename std::vector<FacePointer>::iterator FaceIt;
        FaceIt iter;
        for (iter = vec.begin(); iter != vec.end(); ++iter) 
        {
            for (int j = 0; j < 3; ++j) 
            {
                if ((*iter)->V(j) == v1)
                    (*iter)->V(j) = v;
            }
            
        }
        
        v1->SetD();
        --(m.vn);
        
        if (vfp)
        {
            if (f00p)
                vfp->push_back(f00p);
            if (f01p)
                vfp->push_back(f01p);
            
            if (!BOUNDARY)
            {
                vfp->push_back(f10p);
                vfp->push_back(f11p);
            }
        }
        if (f00p)
            assert(FFCorrectness(f00p));
        if (f01p)
            assert(FFCorrectness(f01p));
        if (!BOUNDARY)
        {
            if (f10p)
                assert(FFCorrectness(f10p));
            if (f11p)
                assert(FFCorrectness(f11p));
        }
        if (f00p)
            assert(VFCorrectness(f00p));
        if (f01p)
            assert(VFCorrectness(f01p));
        if (!BOUNDARY)
        {
            if (f10p)
                assert(VFCorrectness(f10p));
            if (f11p)
                assert(VFCorrectness(f11p));
        }
    }
    //! Perform an edge collaps
    void doCollapse(FacePointer fp, int EdgeIndex, Point3<ScalarType> *p = 0, vector<FacePointer> *vfp = 0)
    {
        //const Point3<ScalarType> p2 = p;
        EdgeFIType e = EdgeFIType(fp,EdgeIndex);
        doCollapse<false>(e,p,vfp);
    }
    //! Perform an edge collaps on the boundary
    void doCollapseBoundary(FacePointer fp, int EdgeIndex, Point3<ScalarType> *p = 0, vector<FacePointer> *vfp = 0)
    {
        //const Point3<ScalarType> p2 = p;
        EdgeFIType e = EdgeFIType(fp,EdgeIndex);
        doCollapse<true>(e,p,vfp);
    }
    
    //! Extract in v all faces around the specified vertex
    /// It is necessary to specify if the vertex is on the boundary
    void getAllFacesAroundVertex(FacePointer fp, int i,std::vector<FacePointer> &v,bool isBoundary)
    {
        v.clear();
        if (!fp) return;
        assert(i>= 0 && i<=2);
        vcg::face::Pos<CMeshO::FaceType> pos(fp,fp->V(i));
        
        if (isBoundary)       // if is border move cw until the border is found
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
        v.push_back(pos.F());
        pos.FlipF();
        pos.FlipE();
        while(pos.F() != first)
        {
            v.push_back(pos.F());
            if (pos.IsBorder())
                break;
            pos.FlipF();
            pos.FlipE();
        }
    }

    //! Perform an edge split on the specified edge
    /*
     * If vfp is given push on the vector the pointer at the face (in order) f0,f1,f2,f3
     */
    template<bool BOUNDARY>
    void doSplit(EdgeFIType Edge, const Point3<ScalarType> &p, vector<FacePointer> *vfp = 0, vector<VertexPointer> *vvp = 0)
    {
        assert(FaceType::HasFFAdjacency());
        
        assert(Edge.fp);
        assert(Edge.i>= 0 && Edge.i <= 2);
        
        int index = Edge.fp->Index();
        
        FacePointer f2p = getNewFace(1); // This line must allocate the space also for the next call
        int f2i = 0;
        
        FacePointer f3p = 0;
        int f3i = 0;
        
        if (!BOUNDARY)
        {
            f3p = getNewFace(0); 
            f3i = 0;
        }

        VertexPointer v2 = getNewVertex();
        v2->P() = p;
        
        Edge.fp = &m.face[index];
        
        FacePointer f0p = Edge.fp;
        int f0i = (Edge.i + 1)%3;
        
        VertexPointer v1 = f0p->V(f0i);
        
        FacePointer f00p = f0p->FFp((f0i+1)%3);
        //int f00i = f0p->FFi((f0i+1)%3);
        
        FacePointer f01p = f0p->FFp((f0i)%3);
        int f01i = f0p->FFi((f0i)%3);

        FacePointer f1p = 0;
        int f1i = 0;
        
        FacePointer f10p = 0;
        int f10i = 0;
        
        FacePointer f11p = 0;
        int f11i = 0;
        if (!BOUNDARY)
        {
            f1p = f0p->FFp((f0i+2)%3);
            f1i = f0p->FFi((f0i+2)%3);
            assert(f1p);
            assert(f0p->V(f0i) == f1p->V(f1i));
        
            f10p = f1p->FFp((f1i+1)%3);
            f10i = f1p->FFi((f1i+1)%3);
            f11p = f1p->FFp((f1i+2)%3);
            f11i = f1p->FFi((f1i+2)%3);

        }
        
        if (!BOUNDARY)
        {
            // FF triangle f2
            f2p->FFp((f2i)%3) = f3p;
            f2p->FFi((f2i)%3) = (f3i+2)%3;
        }
        else
        {
            // FF triangle f2
            f2p->FFp((f2i)%3) = f2p;
            f2p->FFi((f2i)%3) = f2i;
        }
        
        
        if (f0p->FFp(f0i) != f0p) // it exists a face f01     
        {
            f2p->FFp((f2i+1)%3) = f01p;
            f2p->FFi((f2i+1)%3) = f01i;
        }
        else
        {
            f2p->FFp((f2i+1)%3) = f2p;
            f2p->FFi((f2i+1)%3) = (f2i+1)%3;
        }
        f2p->FFp((f2i+2)%3) = f0p;
        f2p->FFi((f2i+2)%3) = f0i;
        if (!BOUNDARY)
        {

            // FF triangle f3
            f3p->FFp((f3i)%3) = f1p;
            f3p->FFi((f3i)%3) = (f1i+2)%3;

            if (f1p->FFp((f1i+2)%3) != f1p) // it exists a face f11
            {
                f3p->FFp((f3i+1)%3) = f11p;
                f3p->FFi((f3i+1)%3) = f11i;
            }
            else
            {
                f3p->FFp((f3i+1)%3) = f3p;
                f3p->FFi((f3i+1)%3) = (f3i+1)%3;
            }
            
            f3p->FFp((f3i+2)%3) = f2p;
            f3p->FFi((f3i+2)%3) = f2i;
        }        
        // FF triangle f01
        f01p->FFp(f01i) = f2p;
        f01p->FFi(f01i) = (f2i+1)%3;

        if (!BOUNDARY)
        {

            // FF triangle f11
            f11p->FFp(f11i) = f3p;
            f11p->FFi(f11i) = (f3i+1)%3;
        }        
        // FF triangle f0
        f0p->FFp(f0i) = f2p;
        f0p->FFi(f0i) = (f2i+2)%3;

        if (!BOUNDARY)
        {

            // FF triangle f1
            f1p->FFp((f1i+2)%3) = f3p;
            f1p->FFi((f1i+2)%3) = f3i;
        }
        
        // FV
        f0p->V(f0i) = v2;
        if (!BOUNDARY)
        {
            f1p->V(f1i) = v2;
        }
        
        f2p->V(f2i) = v2;
        f2p->V((f2i+1)%3) = v1;
        f2p->V((f2i+2)%3) = f0p->V((f0i+1)%3);

        assert(f2p->V(f2i) == f0p->V(f0i));
        
        if (!BOUNDARY)
        {

            f3p->V(f3i) = v2;
            f3p->V((f3i+1)%3) = f1p->V((f1i+2)%3);
            f3p->V((f3i+2)%3) = v1;
        }
        // Detach old vertex from f0 and f1
        f0p->V(f0i) = v2;
        
        if (!BOUNDARY)
        {
            f1p->V(f1i) = v2;
        }
        
        if (VertexType::HasVFAdjacency())
        {
            //assert(!FaceType::HasVFAdjacency());
            v2->VFp() = f0p;
            v2->VFi() = f0i;
            v1->VFp() = f2p;
            v1->VFi() = (f2i+1)%3;
        }
       
        if (vfp)
        {
            vfp->push_back(f0p);
            if (!BOUNDARY)
            {
                vfp->push_back(f1p);
            }
            vfp->push_back(f2p);
            if (!BOUNDARY)
            {
                vfp->push_back(f3p);
            }
        }

        if (vvp)
        {
            vvp->push_back(v2);
        }
        
        assert(FFCorrectness(f0p));
        
        if (!BOUNDARY)
        {
            assert(FFCorrectness(f1p));
        }
        assert(FFCorrectness(f2p));
        if (!BOUNDARY)
        { 
            assert(FFCorrectness(f3p));
        }
        assert(FFCorrectness(f00p));
        assert(FFCorrectness(f01p));
        if (!BOUNDARY)
        {
            assert(FFCorrectness(f10p));
            assert(FFCorrectness(f11p));
        }

        assert(VFCorrectness(f0p));
        if (!BOUNDARY)
        {
            assert(VFCorrectness(f1p));
        }
        assert(VFCorrectness(f2p));
        if (!BOUNDARY)
        {
            assert(VFCorrectness(f3p));
        }
        assert(VFCorrectness(f00p));
        assert(VFCorrectness(f01p));
        if (!BOUNDARY)
        {
            assert(VFCorrectness(f10p));
            assert(VFCorrectness(f11p));
        }
        
    }
    
    //! Perform an edge split
    void doSplit(FacePointer fp, int EdgeIndex, const Point3<ScalarType> &p = 0, vector<FacePointer> *vfp = 0, vector<VertexPointer> *vvp = 0)
    {
        //const Point3<ScalarType> p2 = p;
        EdgeFIType e = EdgeFIType(fp,EdgeIndex);
        doSplit<false>(e,p,vfp,vvp);
    }

    //! Perform an edge split
    void doSplitBoundary(FacePointer fp, int EdgeIndex, const Point3<ScalarType> &p = 0, vector<FacePointer> *vfp = 0, vector<VertexPointer> *vvp = 0)
    {
        //const Point3<ScalarType> p2 = p;
        EdgeFIType e = EdgeFIType(fp,EdgeIndex);
        doSplit<true>(e,p,vfp,vvp);
    }
    
    
    
    
private:
    //! Update the list of vertexes and triangles that can be garbage-collected
    void updateLists()
    {
        listFp.clear();
        sizelistFp = 0;
        listVp.clear();
        sizelistVp = 0;
        
        FaceIterator fit = m.face.begin();
        while(fit != m.face.end())
        {
            if (fit->IsD()) 
            {
            	listFp.push_back(&*fit);
            	sizelistFp++;
            }
            ++fit;
        }

        VertexIterator vit = m.vert.begin();
        while(vit != m.vert.end())
        {
            if (vit->IsD()) 
            {
            	listVp.push_back(&*vit);
            	sizelistVp++;
            }
            ++vit;
        }
    }
    //! Return a new face (if necessary the container is reallocated)
    // The container must be reallocated also if otherneeded is greater than the free face
    FacePointer getNewFace(int otherneeded = 0)
    {
        assert(otherneeded >= 0);
        if (sizelistFp <= otherneeded)
        {
            list<int> l;
            
            for (typename list<FacePointer>::iterator lit2 = listFp.begin(); lit2 != listFp.end(); ++lit2) 
            {
                l.push_back((*lit2)->Index());
            }
            
            int newFaces = (int)(growNumberFace() * m.face.size()); // (float)fc->size());
            newFaces += otherneeded + 1;
            FaceIterator it = vcg::tri::Allocator<TriMeshType>::AddFaces(m,newFaces);
            if (fc)
            	fc->resize(fc->size()+newFaces);
                       
            listFp.clear();
            sizelistFp = 0;
            for (list<int>::iterator lit = l.begin(); lit != l.end(); ++lit) 
            {
                listFp.push_back(&m.face[*lit]);
                sizelistFp++;
            }
            
            while(it != m.face.end())
            {
                listFp.push_back(&*it);
                sizelistFp++;
                it->SetD();
                --(m.fn);
                ++it;
            }
            
        }
        
        assert(sizelistFp > otherneeded);
        
        FacePointer fp = listFp.front();
        listFp.pop_front();
        sizelistFp--;
        assert(fp->IsD());
        fp->ClearD();
        ++(m.fn);
        return fp;
    }

    //! Return a new Vertex (if necessary the container is reallocated)
    VertexPointer getNewVertex()
    {
        if (sizelistVp <= 0)
        {
            int newVertexes = (int)(growNumberVertex() * m.vert.size());//(float)vc->size());
            ++newVertexes;
            VertexIterator it = vcg::tri::Allocator<TriMeshType>::AddVertices(m,newVertexes);
            if (vc)
            	vc->resize(vc->size()+newVertexes);
            while(it != m.vert.end())
            {
                listVp.push_back(&*it);
                sizelistVp++;
                it->SetD();
                --(m.vn);
                ++it;
            }
        }
        
        assert(sizelistVp > 0);
        
        VertexPointer vp = listVp.front();
        listVp.pop_front();
        sizelistVp--;
        assert(vp->IsD());
        vp->ClearD();
        ++(m.vn);
        return vp;
    }

    
    
    
};
//! Test for FFCorrectess the face fp
template <class FacePointer>
bool FFCorrectness(FacePointer fp)
{
    return (
            vcg::face::FFCorrectness(*fp,0) &&
            vcg::face::FFCorrectness(*fp,1) &&
            vcg::face::FFCorrectness(*fp,2)
            );
}
//! Test for the VF Correctness of a vertex
template <class FacePointer>
static bool VFCorrectness(FacePointer fp)
{
	return (VFCorrectnessP(fp->V(0)) && VFCorrectnessP(fp->V(1)) && VFCorrectnessP(fp->V(2))); 
}

//! Test for the VF Correctness of a vertex
template <class VertexPointer>
static bool VFCorrectnessP(VertexPointer vp)
{
	if (vp->IsD())
		return false;
	if (vp->VFp()->IsD())
		return false;
	int index = vp->VFi();
	
	return (vp->VFp()->V(index) == vp);
}


/*!
* Check if the z-th edge of the face f can be flipped.
*   \param f    pointer to the face
*   \param z    the edge index
*/
template <class FaceType>
static bool CheckFlipEdge(FaceType &f, int z)
{
    if (z<0 || z>2)
        return false;

    // boundary edges cannot be flipped
    if (face::IsBorder(f, z))
        return false;

    FaceType *g = f.FFp(z);
    int      w = f.FFi(z);

    // check if the vertices of the edge are the same
    if (g->V(w)!=f.V1(z) || g->V1(w)!=f.V(z) )
        return false;

    // check if the flipped edge is already present in the mesh
    typedef typename FaceType::VertexType VertexType;
    VertexType *f_v2 = f.V2(z);
    VertexType *g_v2 = g->V2(w);
    if (f_v2 == g_v2)
        return false;

    vcg::face::Pos< FaceType > pos(&f, (z+2)%3, f.V2(z));
    do
    {
        pos.NextE();
        if (g_v2==pos.f->V1(pos.z))
            return false;
    }
    while (&f!=pos.f);

    return true;
}

/*!
* Flip the z-th edge of the face f.
* Check for topological correctness first using <CODE>CheckFlipFace()</CODE>.
*   \param f    pointer to the face
*   \param z    the edge index
*
* Note: For <em>edge flip</em> we intend the swap of the diagonal of the rectangle 
*       formed by the face \a f and the face adjacent to the specified edge.
*/
template <class FaceType>
static void FlipEdge(FaceType &f, const int z)
{   
    assert(z>=0);
    assert(z<3);
    assert( !IsBorder(f,z) );
    assert( face::IsManifold<FaceType>(f, z));

    FaceType *g = f.FFp(z);
    int      w = f.FFi(z);
    
    assert( g->V(w) == f.V1(z) );
    assert( g->V1(w)== f.V(z) );
    assert( g->V2(w)!= f.V(z) );
    assert( g->V2(w)!= f.V1(z) );
    assert( g->V2(w)!= f.V2(z) );

    f.V1(z) = g->V2(w);
    g->V1(w) = f.V2(z);
    
    f.FFp(z)                = g->FFp((w+1)%3);
    f.FFi(z)                = g->FFi((w+1)%3);
    g->FFp(w)               = f.FFp((z+1)%3);
    g->FFi(w)               = f.FFi((z+1)%3);
    f.FFp((z+1)%3)          = g;
    f.FFi((z+1)%3)          = (w+1)%3;
    g->FFp((w+1)%3)         = &f;
    g->FFi((w+1)%3)         = (z+1)%3;

    if(f.FFp(z)==g)
    {
        f.FFp(z) = &f;
        f.FFi(z) = z;
    }
    else
    {
        f.FFp(z)->FFp( f.FFi(z) ) = &f;
        f.FFp(z)->FFi( f.FFi(z) ) = z;
    }
    if(g->FFp(w)==&f)
    {
        g->FFp(w)=g;
        g->FFi(w)=w;
    }
    else
    {
        g->FFp(w)->FFp( g->FFi(w) ) = g;
        g->FFp(w)->FFi( g->FFi(w) ) = w;
    }
    
    if (g->V(0)->HasVFAdjacency()) // Vertex has vf adjacency
    {
        //assert(!FaceType::HasVFAdjacency());
        
        f.V((z+0)%3)->VFp() = &f;
        f.V((z+0)%3)->VFi() = z;

        g->V((w+0)%3)->VFp() = g;
        g->V((w+0)%3)->VFi() = w;
        
    }
    
    
    assert(FFCorrectness(&f));
    assert(FFCorrectness(g));
    assert(VFCorrectness(&f));
    assert(VFCorrectness(g));
}

}

#endif /*TOPOLOGICALOP_H_*/
