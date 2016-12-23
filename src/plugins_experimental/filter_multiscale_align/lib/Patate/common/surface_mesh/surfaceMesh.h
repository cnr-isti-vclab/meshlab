//=============================================================================
// Copyright (C) 2001-2005 by Computer Graphics Group, RWTH Aachen
// Copyright (C) 2011-2013 by Graphics & Geometry Group, Bielefeld University
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation, version 2.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================


#ifndef _PATATE_COMMON_SURFACE_MESH_SURFACE_MESH_
#define _PATATE_COMMON_SURFACE_MESH_SURFACE_MESH_


//== INCLUDES =================================================================


#include "properties.h"
#include <string.h>
#include <cmath>
#include <clocale>
#include <cstdio>
#include <cfloat>
#include <map>


//== NAMESPACE ================================================================


namespace PatateCommon {


//== CLASS DEFINITION =========================================================


/// A halfedge data structure for polygonal meshes.
class SurfaceMesh
{

public: //------------------------------------------------------ topology types


    /// Base class for all topology types (internally it is basically an index)
    /// \sa Vertex, Halfedge, Edge, Face
    class BaseHandle
    {
    public:

        /// constructor
        explicit BaseHandle(int _idx=-1) : idx_(_idx) {}

        /// Get the underlying index of this handle
        int idx() const { return idx_; }

        /// reset handle to be invalid (index=-1)
        void reset() { idx_=-1; }

        /// return whether the handle is valid, i.e., the index is not equal to -1.
        bool isValid() const { return idx_ != -1; }

        /// are two handles equal?
        bool operator==(const BaseHandle& _rhs) const {
            return idx_ == _rhs.idx_;
        }

        /// are two handles different?
        bool operator!=(const BaseHandle& _rhs) const {
            return idx_ != _rhs.idx_;
        }

        /// compare operator useful for sorting handles
        bool operator<(const BaseHandle& _rhs) const {
            return idx_ < _rhs.idx_;
        }

    private:
        friend class VertexIterator;
        friend class HalfedgeIterator;
        friend class EdgeIterator;
        friend class FaceIterator;
        friend class SurfaceMesh;
        int idx_;
    };


    /// this type represents a vertex (internally it is basically an index)
    ///  \sa Halfedge, Edge, Face
    struct Vertex : public BaseHandle
    {
        /// default constructor (with invalid index)
        explicit Vertex(int _idx=-1) : BaseHandle(_idx) {}
        std::ostream& operator<<(std::ostream& os) const { return os << 'v' << idx(); }
    };


    /// this type represents a halfedge (internally it is basically an index)
    /// \sa Vertex, Edge, Face
    struct Halfedge : public BaseHandle
    {
        /// default constructor (with invalid index)
        explicit Halfedge(int _idx=-1) : BaseHandle(_idx) {}
    };


    /// this type represents an edge (internally it is basically an index)
    /// \sa Vertex, Halfedge, Face
    struct Edge : public BaseHandle
    {
        /// default constructor (with invalid index)
        explicit Edge(int _idx=-1) : BaseHandle(_idx) {}
    };


    /// this type represents a face (internally it is basically an index)
    /// \sa Vertex, Halfedge, Edge
    struct Face : public BaseHandle
    {
        /// default constructor (with invalid index)
        explicit Face(int _idx=-1) : BaseHandle(_idx) {}
    };




public: //-------------------------------------------------- connectivity types

    /// This type stores the vertex connectivity
    /// \sa HalfedgeConnectivity, FaceConnectivity
    struct VertexConnectivity
    {
        /// an outgoing halfedge per vertex (it will be a boundary halfedge for boundary vertices)
        Halfedge  halfedge;
    };


    /// This type stores the halfedge connectivity
    /// \sa VertexConnectivity, FaceConnectivity
    struct HalfedgeConnectivity
    {
        /// face incident to halfedge
        Face      face;
        /// vertex the halfedge points to
        Vertex    vertex;
        /// next halfedge within a face (or along a boundary)
        Halfedge  nextHalfedge;
        /// previous halfedge within a face (or along a boundary)
        Halfedge  prevHalfedge;
    };


    /// This type stores the face connectivity
    /// \sa VertexConnectivity, HalfedgeConnectivity
    struct FaceConnectivity
    {
        /// a halfedge that is part of the face
        Halfedge  halfedge;
    };




public: //------------------------------------------------------ property types

    /// Vertex property of type T
    /// \sa HalfedgeProperty, EdgeProperty, FaceProperty
    template <class T> class VertexProperty : public Property<T>
    {
    public:

        /// default constructor
        explicit VertexProperty() {}
        explicit VertexProperty(Property<T> p) : Property<T>(p) {}

        /// access the data stored for vertex \c v
        typename Property<T>::Reference operator[](Vertex v)
        {
            return Property<T>::operator[](v.idx());
        }

        /// access the data stored for vertex \c v
        typename Property<T>::ConstReference operator[](Vertex v) const
        {
            return Property<T>::operator[](v.idx());
        }
    };


    /// Halfedge property of type T
    /// \sa VertexProperty, EdgeProperty, FaceProperty
    template <class T> class HalfedgeProperty : public Property<T>
    {
    public:

        /// default constructor
        explicit HalfedgeProperty() {}
        explicit HalfedgeProperty(Property<T> p) : Property<T>(p) {}

        /// access the data stored for halfedge \c h
        typename Property<T>::Reference operator[](Halfedge h)
        {
            return Property<T>::operator[](h.idx());
        }

        /// access the data stored for halfedge \c h
        typename Property<T>::ConstReference operator[](Halfedge h) const
        {
            return Property<T>::operator[](h.idx());
        }
    };


    /// Edge property of type T
    /// \sa VertexProperty, HalfedgeProperty, FaceProperty
    template <class T> class EdgeProperty : public Property<T>
    {
    public:

        /// default constructor
        explicit EdgeProperty() {}
        explicit EdgeProperty(Property<T> p) : Property<T>(p) {}

        /// access the data stored for edge \c e
        typename Property<T>::Reference operator[](Edge e)
        {
            return Property<T>::operator[](e.idx());
        }

        /// access the data stored for edge \c e
        typename Property<T>::ConstReference operator[](Edge e) const
        {
            return Property<T>::operator[](e.idx());
        }
    };


    /// Face property of type T
    /// \sa VertexProperty, HalfedgeProperty, EdgeProperty
    template <class T> class FaceProperty : public Property<T>
    {
    public:

        /// default constructor
        explicit FaceProperty() {}
        explicit FaceProperty(Property<T> p) : Property<T>(p) {}

        /// access the data stored for face \c f
        typename Property<T>::Reference operator[](Face f)
        {
            return Property<T>::operator[](f.idx());
        }

        /// access the data stored for face \c f
        typename Property<T>::ConstReference operator[](Face f) const
        {
            return Property<T>::operator[](f.idx());
        }
    };




public: //------------------------------------------------------ iterator types

    /// this class iterates linearly over all vertices
    /// \sa verticesBegin(), verticesEnd()
    /// \sa HalfedgeIterator, EdgeIterator, FaceIterator
    class VertexIterator
    {
    public:

        /// Default constructor
        VertexIterator(Vertex v=Vertex(), const SurfaceMesh* m=NULL) : hnd_(v), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
        }

        /// get the vertex the iterator refers to
        Vertex operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const VertexIterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const VertexIterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        VertexIterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        VertexIterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Vertex  hnd_;
        const SurfaceMesh* mesh_;
    };


    /// this class iterates linearly over all halfedges
    /// \sa halfedgesBegin(), halfedgesEnd()
    /// \sa VertexIterator, EdgeIterator, FaceIterator
    class HalfedgeIterator
    {
    public:

        /// Default constructor
        HalfedgeIterator(Halfedge h=Halfedge(), const SurfaceMesh* m=NULL) : hnd_(h), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
        }

        /// get the halfedge the iterator refers to
        Halfedge operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const HalfedgeIterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const HalfedgeIterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        HalfedgeIterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        HalfedgeIterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Halfedge  hnd_;
        const SurfaceMesh* mesh_;
    };


    /// this class iterates linearly over all edges
    /// \sa edgesBegin(), edgesEnd()
    /// \sa VertexIterator, HalfedgeIterator, FaceIterator
    class EdgeIterator
    {
    public:

        /// Default constructor
        EdgeIterator(Edge e=Edge(), const SurfaceMesh* m=NULL) : hnd_(e), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
        }

        /// get the edge the iterator refers to
        Edge operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const EdgeIterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const EdgeIterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        EdgeIterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        EdgeIterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Edge  hnd_;
        const SurfaceMesh* mesh_;
    };


    /// this class iterates linearly over all faces
    /// \sa facesBegin(), facesEnd()
    /// \sa VertexIterator, HalfedgeIterator, EdgeIterator
    class FaceIterator
    {
    public:

        /// Default constructor
        FaceIterator(Face f=Face(), const SurfaceMesh* m=NULL) : hnd_(f), mesh_(m)
        {
            if (mesh_ && mesh_->garbage()) while (mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
        }

        /// get the face the iterator refers to
        Face operator*()  const { return  hnd_; }

        /// are two iterators equal?
        bool operator==(const FaceIterator& rhs) const
        {
            return (hnd_==rhs.hnd_);
        }

        /// are two iterators different?
        bool operator!=(const FaceIterator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment iterator
        FaceIterator& operator++()
        {
            ++hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) ++hnd_.idx_;
            return *this;
        }

        /// pre-decrement iterator
        FaceIterator& operator--()
        {
            --hnd_.idx_;
            assert(mesh_);
            while (mesh_->garbage() && mesh_->isValid(hnd_) && mesh_->isDeleted(hnd_)) --hnd_.idx_;
            return *this;
        }

    private:
        Face  hnd_;
        const SurfaceMesh* mesh_;
    };



public: //-------------------------- containers for C++11 range-based for loops

    /// this helper class is a container for iterating through all
    /// vertices using C++11 range-based for-loops.
    /// \sa vertices()
    class VertexContainer
    {
    public:
        VertexContainer(VertexIterator _begin, VertexIterator _end) : begin_(_begin), end_(_end) {}
        VertexIterator begin() const { return begin_; }
        VertexIterator end()   const { return end_;   }
    private:
        VertexIterator begin_, end_;
    };



    /// this helper class is a container for iterating through all
    /// halfedge using C++11 range-based for-loops.
    /// \sa halfedges()
    class HalfedgeContainer
    {
    public:
        HalfedgeContainer(HalfedgeIterator _begin, HalfedgeIterator _end) : begin_(_begin), end_(_end) {}
        HalfedgeIterator begin() const { return begin_; }
        HalfedgeIterator end()   const { return end_;   }
    private:
        HalfedgeIterator begin_, end_;
    };



    /// this helper class is a container for iterating through all
    /// edges using C++11 range-based for-loops.
    /// \sa edges()
    class EdgeContainer
    {
    public:
        EdgeContainer(EdgeIterator _begin, EdgeIterator _end) : begin_(_begin), end_(_end) {}
        EdgeIterator begin() const { return begin_; }
        EdgeIterator end()   const { return end_;   }
    private:
        EdgeIterator begin_, end_;
    };



    /// this helper class is a container for iterating through all
    /// faces using C++11 range-based for-loops.
    /// \sa faces()
    class FaceContainer
    {
    public:
        FaceContainer(FaceIterator _begin, FaceIterator _end) : begin_(_begin), end_(_end) {}
        FaceIterator begin() const { return begin_; }
        FaceIterator end()   const { return end_;   }
    private:
        FaceIterator begin_, end_;
    };





public: //---------------------------------------------------- circulator types

    /// this class circulates through all one-ring neighbors of a vertex.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa HalfedgeAroundVertexCirculator, FaceAroundVertexCirculator, vertices(Vertex)
    class VertexAroundVertexCirculator
    {
    public:

        /// default constructor
        VertexAroundVertexCirculator(const SurfaceMesh* m=NULL, Vertex v=Vertex())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(v);
        }

        /// are two circulators equal?
        bool operator==(const VertexAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const VertexAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotate couter-clockwise)
        VertexAroundVertexCirculator& operator++()
        {
            assert(mesh_);
            halfedge_ = mesh_->ccwRotatedHalfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotate clockwise)
        VertexAroundVertexCirculator& operator--()
        {
            assert(mesh_);
            halfedge_ = mesh_->cwRotatedHalfedge(halfedge_);
            return *this;
        }

        /// get the vertex the circulator refers to
        Vertex operator*()  const
        {
            assert(mesh_);
            return mesh_->toVertex(halfedge_);
        }

        /// cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.isValid(); }

        /// return current halfedge
        Halfedge halfedge() const { return halfedge_; }

        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculator& begin() { active_=!halfedge_.isValid(); return *this; }
        // helper for C++11 range-based for-loops
        VertexAroundVertexCirculator& end()   { active_=true;  return *this; }

    private:
        const SurfaceMesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through all outgoing halfedges of a vertex.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa VertexAroundVertexCirculator, FaceAroundVertexCirculator, halfedges(Vertex)
    class HalfedgeAroundVertexCirculator
    {
    public:

        /// default constructor
        HalfedgeAroundVertexCirculator(const SurfaceMesh* m=NULL, Vertex v=Vertex())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(v);
        }

        /// are two circulators equal?
        bool operator==(const HalfedgeAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const HalfedgeAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotate couter-clockwise)
        HalfedgeAroundVertexCirculator& operator++()
        {
            assert(mesh_);
            halfedge_ = mesh_->ccwRotatedHalfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotate clockwise)
        HalfedgeAroundVertexCirculator& operator--()
        {
            assert(mesh_);
            halfedge_ = mesh_->cwRotatedHalfedge(halfedge_);
            return *this;
        }

        /// get the halfedge the circulator refers to
        Halfedge operator*() const { return halfedge_; }

        /// cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.isValid(); }

        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculator& begin() { active_=!halfedge_.isValid(); return *this; }
        // helper for C++11 range-based for-loops
        HalfedgeAroundVertexCirculator& end()   { active_=true;  return *this; }

    private:
        const SurfaceMesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through all incident faces of a vertex.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa VertexAroundVertexCirculator, HalfedgeAroundVertexCirculator, faces(Vertex)
    class FaceAroundVertexCirculator
    {
    public:

        /// construct with mesh and vertex (vertex should not be isolated!)
        FaceAroundVertexCirculator(const SurfaceMesh* m=NULL, Vertex v=Vertex())
        : mesh_(m), active_(true)
        {
            if (mesh_)
            {
                halfedge_ = mesh_->halfedge(v);
                if (halfedge_.isValid() && mesh_->isBoundary(halfedge_))
                    operator++();
            }
        }

        /// are two circulators equal?
        bool operator==(const FaceAroundVertexCirculator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const FaceAroundVertexCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotates counter-clockwise)
        FaceAroundVertexCirculator& operator++()
        {
            assert(mesh_ && halfedge_.isValid());
            do {
                halfedge_ = mesh_->ccwRotatedHalfedge(halfedge_);
            } while (mesh_->isBoundary(halfedge_));
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotate clockwise)
        FaceAroundVertexCirculator& operator--()
        {
            assert(mesh_ && halfedge_.isValid());
            do
                halfedge_ = mesh_->cwRotatedHalfedge(halfedge_);
            while (mesh_->isBoundary(halfedge_));
            return *this;
        }

        /// get the face the circulator refers to
        Face operator*() const
        {
            assert(mesh_ && halfedge_.isValid());
            return mesh_->face(halfedge_);
        }

        /// cast to bool: true if vertex is not isolated
        operator bool() const { return halfedge_.isValid(); }

        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculator& begin() { active_=!halfedge_.isValid(); return *this; }
        // helper for C++11 range-based for-loops
        FaceAroundVertexCirculator& end()   { active_=true;  return *this; }

    private:
        const SurfaceMesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through the vertices of a face.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa HalfedgeAroundFaceCirculator, vertices(Face)
    class VertexAroundFaceCirculator
    {
    public:

        /// default constructor
        VertexAroundFaceCirculator(const SurfaceMesh* m=NULL, Face f=Face())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(f);
        }

        /// are two circulators equal?
        bool operator==(const VertexAroundFaceCirculator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const VertexAroundFaceCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotates counter-clockwise)
        VertexAroundFaceCirculator& operator++()
        {
            assert(mesh_ && halfedge_.isValid());
            halfedge_ = mesh_->nextHalfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotates clockwise)
        VertexAroundFaceCirculator& operator--()
        {
            assert(mesh_ && halfedge_.isValid());
            halfedge_ = mesh_->prevHalfedge(halfedge_);
            return *this;
        }

        /// get the vertex the circulator refers to
        Vertex operator*() const
        {
            assert(mesh_ && halfedge_.isValid());
            return mesh_->toVertex(halfedge_);
        }

        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculator& begin() { active_=false; return *this; }
        // helper for C++11 range-based for-loops
        VertexAroundFaceCirculator& end()   { active_=true;  return *this; }

    private:
        const SurfaceMesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


    /// this class circulates through all halfedges of a face.
    /// it also acts as a container-concept for C++11 range-based for loops.
    /// \sa VertexAroundFaceCirculator, halfedges(Face)
    class HalfedgeAroundFaceCirculator
    {
    public:

        /// default constructur
        HalfedgeAroundFaceCirculator(const SurfaceMesh* m=NULL, Face f=Face())
        : mesh_(m), active_(true)
        {
            if (mesh_) halfedge_ = mesh_->halfedge(f);
        }

        /// are two circulators equal?
        bool operator==(const HalfedgeAroundFaceCirculator& rhs) const
        {
            assert(mesh_);
            return (active_ && (mesh_==rhs.mesh_) && (halfedge_==rhs.halfedge_));
        }

        /// are two circulators different?
        bool operator!=(const HalfedgeAroundFaceCirculator& rhs) const
        {
            return !operator==(rhs);
        }

        /// pre-increment (rotates counter-clockwise)
        HalfedgeAroundFaceCirculator& operator++()
        {
            assert(mesh_ && halfedge_.isValid());
            halfedge_ = mesh_->nextHalfedge(halfedge_);
            active_ = true;
            return *this;
        }

        /// pre-decrement (rotates clockwise)
        HalfedgeAroundFaceCirculator& operator--()
        {
            assert(mesh_ && halfedge_.isValid());
            halfedge_ = mesh_->prevHalfedge(halfedge_);
            return *this;
        }

        /// get the halfedge the circulator refers to
        Halfedge operator*() const { return halfedge_; }

        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculator& begin() { active_=false; return *this; }
        // helper for C++11 range-based for-loops
        HalfedgeAroundFaceCirculator& end()   { active_=true;  return *this; }

    private:
        const SurfaceMesh*  mesh_;
        Halfedge         halfedge_;
        // helper for C++11 range-based for-loops
        bool active_;
    };


public:
    enum
    {
        GC_DONT_RELEASE_INDEX_MAPS = 0x01
    };


public: //-------------------------------------------- constructor / destructor

    /// \name Construct, destruct, assignment
    //@{

    /// default constructor
    inline SurfaceMesh();

    inline ~SurfaceMesh();

    /// copy constructor: copies \c rhs to \c *this. performs a deep copy of all properties.
    SurfaceMesh(const SurfaceMesh& rhs) { operator=(rhs); }

    /// assign \c rhs to \c *this. performs a deep copy of all properties.
    inline SurfaceMesh& operator=(const SurfaceMesh& rhs);

    /// assign \c rhs to \c *this. does not copy custom properties.
    inline SurfaceMesh& assign(const SurfaceMesh& rhs);

    //@}




//public: //------------------------------------------------------------- file IO

//    /// \name File IO
//    //@{

//    /// read mesh from file \c filename. file extension determines file type.
//    /// \sa write(const std::string& filename)
//    inline bool read(const std::string& filename);

//    /// write mesh to file \c filename. file extensions determines file type.
//    /// \sa read(const std::string& filename)
//    inline bool write(const std::string& filename) const;

//    //@}




public: //----------------------------------------------- add new vertex / face

    /// \name Add new elements by hand
    //@{

    /// add a new vertex
    inline Vertex addVertex();

    /// add a new face with vertex list \c vertices
    /// \sa addTriangle, addQuad
    inline Face addFace(const std::vector<Vertex>& vertices);

    /// add a new triangle connecting vertices \c v1, \c v2, \c v3
    /// \sa addFace, addQuad
    inline Face addTriangle(Vertex v1, Vertex v2, Vertex v3);

    /// add a new quad connecting vertices \c v1, \c v2, \c v3, \c v4
    /// \sa addTriangle, addFace
    inline Face addQuad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);

    //@}




public: //--------------------------------------------------- memory management

    /// \name Memory Management
    //@{

    /// returns number of (deleted and valid) vertices in the mesh
    unsigned int verticesSize() const { return (unsigned int) m_vprops.size(); }
    /// returns number of (deleted and valid)halfedge in the mesh
    unsigned int halfedgesSize() const { return (unsigned int) m_hprops.size(); }
    /// returns number of (deleted and valid)edges in the mesh
    unsigned int edgesSize() const { return (unsigned int) m_eprops.size(); }
    /// returns number of (deleted and valid)faces in the mesh
    unsigned int facesSize() const { return (unsigned int) m_fprops.size(); }


    /// returns number of vertices in the mesh
    unsigned int nVertices() const { return verticesSize() - m_deletedVertices; }
    /// returns number of halfedge in the mesh
    unsigned int nHalfedges() const { return halfedgesSize() - 2*m_deletedEdges; }
    /// returns number of edges in the mesh
    unsigned int nEdges() const { return edgesSize() - m_deletedEdges; }
    /// returns number of faces in the mesh
    unsigned int nFaces() const { return facesSize() - m_deleteFaces; }


    /// returns true iff the mesh is empty, i.e., has no vertices
    unsigned int empty() const { return nVertices() == 0; }


    /// clear mesh: remove all vertices, edges, faces
    inline void clear();

    /// remove unused memory from vectors
    inline void freeMemory();

    /// reserve memory (mainly used in file readers)
    inline void reserve(unsigned int nvertices,
                 unsigned int nedges,
                 unsigned int nfaces );


    /// remove deleted vertices/edges/faces
    inline void garbageCollection(unsigned flags = 0);

    inline void releaseGCIndexMaps()
    {
        std::vector<Vertex>   vMap;
        std::vector<Halfedge> hMap;
        std::vector<Face>     fMap;
        m_gcVertexMap  .swap(vMap);
        m_gcHalfedgeMap.swap(hMap);
        m_gcFaceMap    .swap(fMap);
    }

    inline Vertex gcMap(Vertex v)
    {
        assert(v.isValid() && v.idx() < int(m_gcVertexMap.size()));
        return m_gcVertexMap[v.idx()];
    }

    inline Halfedge gcMap(Halfedge h)
    {
        assert(h.isValid() && h.idx() < int(m_gcHalfedgeMap.size()));
        return m_gcHalfedgeMap[h.idx()];
    }

    inline Face gcMap(Face f)
    {
        assert(f.isValid() && f.idx() < int(m_gcFaceMap.size()));
        return m_gcFaceMap[f.idx()];
    }


    /// returns whether vertex \c v is deleted
    /// \sa garbageCollection()
    bool isDeleted(Vertex v) const
    {
        return m_vdeleted[v];
    }
    /// returns whether halfedge \c h is deleted
    /// \sa garbageCollection()
    bool isDeleted(Halfedge h) const
    {
        return m_edeleted[edge(h)];
    }
    /// returns whether edge \c e is deleted
    /// \sa garbageCollection()
    bool isDeleted(Edge e) const
    {
        return m_edeleted[e];
    }
    /// returns whether face \c f is deleted
    /// \sa garbageCollection()
    bool isDeleted(Face f) const
    {
        return m_fdeleted[f];
    }


    /// return whether vertex \c v is valid, i.e. the index is stores it within the array bounds.
    bool isValid(Vertex v) const
    {
        return (0 <= v.idx()) && (v.idx() < (int)verticesSize());
    }
    /// return whether halfedge \c h is valid, i.e. the index is stores it within the array bounds.
    bool isValid(Halfedge h) const
    {
        return (0 <= h.idx()) && (h.idx() < (int)halfedgesSize());
    }
    /// return whether edge \c e is valid, i.e. the index is stores it within the array bounds.
    bool isValid(Edge e) const
    {
        return (0 <= e.idx()) && (e.idx() < (int)edgesSize());
    }
    /// return whether face \c f is valid, i.e. the index is stores it within the array bounds.
    bool isValid(Face f) const
    {
        return (0 <= f.idx()) && (f.idx() < (int)facesSize());
    }

    //@}




public: //---------------------------------------------- low-level connectivity

    /// \name Low-level connectivity
    //@{

    /// returns an outgoing halfedge of vertex \c v.
    /// if \c v is a boundary vertex this will be a boundary halfedge.
    Halfedge halfedge(Vertex v) const
    {
        return m_vconn[v].halfedge;
    }

    /// set the outgoing halfedge of vertex \c v to \c h
    void setHalfedge(Vertex v, Halfedge h)
    {
        m_vconn[v].halfedge = h;
    }

    /// returns whether \c v is a boundary vertex
    bool isBoundary(Vertex v) const
    {
        Halfedge h(halfedge(v));
        return (!(h.isValid() && face(h).isValid()));
    }

    /// returns whether \c v is isolated, i.e., not incident to any face
    bool isIsolated(Vertex v) const
    {
        return !halfedge(v).isValid();
    }

    /// returns whether \c v is a manifold vertex (not incident to several patches)
    bool isManifold(Vertex v) const
    {
        // The vertex is non-manifold if more than one gap exists, i.e.
        // more than one outgoing boundary halfedge.
        int n(0);
        HalfedgeAroundVertexCirculator hit = halfedges(v), hend=hit;
        if (hit) do
        {
            if (isBoundary(*hit))
                ++n;
        }
        while (++hit!=hend);
        return n<2;
    }

    /// returns the vertex the halfedge \c h points to
    Vertex toVertex(Halfedge h) const
    {
        return m_hconn[h].vertex;
    }

    /// returns the vertex the halfedge \c h emanates from
    Vertex fromVertex(Halfedge h) const
    {
        return toVertex(oppositeHalfedge(h));
    }

    /// sets the vertex the halfedge \c h points to to \c v
    void setVertex(Halfedge h, Vertex v)
    {
        m_hconn[h].vertex = v;
    }

    /// returns the face incident to halfedge \c h
    Face face(Halfedge h) const
    {
        return m_hconn[h].face;
    }

    /// sets the incident face to halfedge \c h to \c f
    void setFace(Halfedge h, Face f)
    {
        m_hconn[h].face = f;
    }

    /// returns the next halfedge within the incident face
    Halfedge nextHalfedge(Halfedge h) const
    {
        return m_hconn[h].nextHalfedge;
    }

    /// sets the next halfedge of \c h within the face to \c nh
    void setNextHalfedge(Halfedge h, Halfedge nh)
    {
        m_hconn[h].nextHalfedge = nh;
        m_hconn[nh].prevHalfedge = h;
    }

    /// returns the previous halfedge within the incident face
    Halfedge prevHalfedge(Halfedge h) const
    {
        return m_hconn[h].prevHalfedge;
    }

    /// returns the opposite halfedge of \c h
    Halfedge oppositeHalfedge(Halfedge h) const
    {
        return Halfedge((h.idx() & 1) ? h.idx()-1 : h.idx()+1);
    }

    /// returns the halfedge that is rotated counter-clockwise around the
    /// start vertex of \c h. it is the opposite halfedge of the previous halfedge of \c h.
    Halfedge ccwRotatedHalfedge(Halfedge h) const
    {
        return oppositeHalfedge(prevHalfedge(h));
    }

    /// returns the halfedge that is rotated clockwise around the
    /// start vertex of \c h. it is the next halfedge of the opposite halfedge of \c h.
    Halfedge cwRotatedHalfedge(Halfedge h) const
    {
        return nextHalfedge(oppositeHalfedge(h));
    }

    /// return the edge that contains halfedge \c h as one of its two halfedges.
    Edge edge(Halfedge h) const
    {
        return Edge(h.idx() >> 1);
    }

    /// returns whether h is a boundary halfege, i.e., if its face does not exist.
    bool isBoundary(Halfedge h) const
    {
        return !face(h).isValid();
    }


    /// returns the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    Halfedge halfedge(Edge e, unsigned int i) const
    {
        assert(i<=1);
        return Halfedge((e.idx() << 1) + i);
    }

    /// returns the \c i'th vertex of edge \c e. \c i has to be 0 or 1.
    Vertex vertex(Edge e, unsigned int i) const
    {
        assert(i<=1);
        return toVertex(halfedge(e, i));
    }

    /// returns the face incident to the \c i'th halfedge of edge \c e. \c i has to be 0 or 1.
    Face face(Edge e, unsigned int i) const
    {
        assert(i<=1);
        return face(halfedge(e, i));
    }

    /// returns whether \c e is a boundary edge, i.e., if one of its
    /// halfedges is a boundary halfedge.
    bool isBoundary(Edge e) const
    {
        return (isBoundary(halfedge(e, 0)) || isBoundary(halfedge(e, 1)));
    }

    /// returns a halfedge of face \c f
    Halfedge halfedge(Face f) const
    {
        return m_fconn[f].halfedge;
    }

    /// sets the halfedge of face \c f to \c h
    void setHalfedge(Face f, Halfedge h)
    {
        m_fconn[f].halfedge = h;
    }

    /// returns whether \c f is a boundary face, i.e., it one of its edges is a boundary edge.
    bool isBoundary(Face f) const
    {
        Halfedge h  = halfedge(f);
        Halfedge hh = h;
        do
        {
            if (isBoundary(oppositeHalfedge(h)))
                return true;
            h = nextHalfedge(h);
        }
        while (h != hh);
        return false;
    }

    //@}




public: //--------------------------------------------------- property handling

    /// \name Property handling
    //@{

    /** add a vertex property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> VertexProperty<T> addVertexProperty(const std::string& name, const T t=T())
    {
        return VertexProperty<T>(m_vprops.add<T>(name, t));
    }
    /** add a halfedge property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> HalfedgeProperty<T> addHalfedgeProperty(const std::string& name, const T t=T())
    {
        return HalfedgeProperty<T>(m_hprops.add<T>(name, t));
    }
    /** add a edge property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> EdgeProperty<T> addEdgeProperty(const std::string& name, const T t=T())
    {
        return EdgeProperty<T>(m_eprops.add<T>(name, t));
    }
    /** add a face property of type \c T with name \c name and default value \c t.
     fails if a property named \c name exists already, since the name has to be unique.
     in this case it returns an invalid property */
    template <class T> FaceProperty<T> addFaceProperty(const std::string& name, const T t=T())
    {
        return FaceProperty<T>(m_fprops.add<T>(name, t));
    }


    /** get the vertex property named \c name of type \c T. returns an invalid
     VertexProperty if the property does not exist or if the type does not match. */
    template <class T> VertexProperty<T> getVertexProperty(const std::string& name) const
    {
        return VertexProperty<T>(m_vprops.get<T>(name));
    }
    /** get the halfedge property named \c name of type \c T. returns an invalid
     VertexProperty if the property does not exist or if the type does not match. */
    template <class T> HalfedgeProperty<T> getHalfedgeProperty(const std::string& name) const
    {
        return HalfedgeProperty<T>(m_hprops.get<T>(name));
    }
    /** get the edge property named \c name of type \c T. returns an invalid
     VertexProperty if the property does not exist or if the type does not match. */
    template <class T> EdgeProperty<T> getEdgeProperty(const std::string& name) const
    {
        return EdgeProperty<T>(m_eprops.get<T>(name));
    }
    /** get the face property named \c name of type \c T. returns an invalid
     VertexProperty if the property does not exist or if the type does not match. */
    template <class T> FaceProperty<T> getFaceProperty(const std::string& name) const
    {
        return FaceProperty<T>(m_fprops.get<T>(name));
    }


    /** if a vertex property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> VertexProperty<T> vertexProperty(const std::string& name, const T t=T())
    {
        return VertexProperty<T>(m_vprops.getOrAdd<T>(name, t));
    }
    /** if a halfedge property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> HalfedgeProperty<T> halfedgeProperty(const std::string& name, const T t=T())
    {
        return HalfedgeProperty<T>(m_hprops.getOrAdd<T>(name, t));
    }
    /** if an edge property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> EdgeProperty<T> edgeProperty(const std::string& name, const T t=T())
    {
        return EdgeProperty<T>(m_eprops.getOrAdd<T>(name, t));
    }
    /** if a face property of type \c T with name \c name exists, it is returned.
     otherwise this property is added (with default value \c t) */
    template <class T> FaceProperty<T> faceProperty(const std::string& name, const T t=T())
    {
        return FaceProperty<T>(m_fprops.getOrAdd<T>(name, t));
    }


    /// remove the vertex property \c p
    template <class T> void removeVertexProperty(VertexProperty<T>& p)
    {
        m_vprops.remove(p);
    }
    /// remove the halfedge property \c p
    template <class T> void removeHalfedgeProperty(HalfedgeProperty<T>& p)
    {
        m_hprops.remove(p);
    }
    /// remove the edge property \c p
    template <class T> void removeEdgeProperty(EdgeProperty<T>& p)
    {
        m_eprops.remove(p);
    }
    /// remove the face property \c p
    template <class T> void removeFaceProperty(FaceProperty<T>& p)
    {
        m_fprops.remove(p);
    }


    /** get the type_info \c T of vertex property named \c name. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& getVertexPropertyType(const std::string& name)
    {
        return m_vprops.getType(name);
    }
    /** get the type_info \c T of halfedge property named \c name. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& getHalfedgePropertyType(const std::string& name)
    {
        return m_hprops.getType(name);
    }
    /** get the type_info \c T of edge property named \c name. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& getEdgePropertyType(const std::string& name)
    {
        return m_eprops.getType(name);
    }
    /** get the type_info \c T of face property named \c name. returns an typeid(void)
     if the property does not exist or if the type does not match. */
    const std::type_info& getFacePropertyType(const std::string& name)
    {
        return m_fprops.getType(name);
    }


    /// returns the names of all vertex properties
    std::vector<std::string> vertexProperties() const
    {
        return m_vprops.properties();
    }
    /// returns the names of all halfedge properties
    std::vector<std::string> halfedgeProperties() const
    {
        return m_hprops.properties();
    }
    /// returns the names of all edge properties
    std::vector<std::string> edgeProperties() const
    {
        return m_eprops.properties();
    }
    /// returns the names of all face properties
    std::vector<std::string> faceProperties() const
    {
        return m_fprops.properties();
    }
    /// prints the names of all properties
    inline void propertyStats() const;

    //@}




public: //--------------------------------------------- iterators & circulators

    /// \name Iterators & Circulators
    //@{

    /// returns start iterator for vertices
    VertexIterator verticesBegin() const
    {
        return VertexIterator(Vertex(0), this);
    }

    /// returns end iterator for vertices
    VertexIterator verticesEnd() const
    {
        return VertexIterator(Vertex(verticesSize()), this);
    }

    /// returns vertex container for C++11 range-based for-loops
    VertexContainer vertices() const
    {
        return VertexContainer(verticesBegin(), verticesEnd());
    }

    /// returns start iterator for halfedges
    HalfedgeIterator halfedgesBegin() const
    {
        return HalfedgeIterator(Halfedge(0), this);
    }

    /// returns end iterator for halfedges
    HalfedgeIterator halfedgesEnd() const
    {
        return HalfedgeIterator(Halfedge(halfedgesSize()), this);
    }

    /// returns halfedge container for C++11 range-based for-loops
    HalfedgeContainer halfedges() const
    {
        return HalfedgeContainer(halfedgesBegin(), halfedgesEnd());
    }

    /// returns start iterator for edges
    EdgeIterator edgesBegin() const
    {
        return EdgeIterator(Edge(0), this);
    }

    /// returns end iterator for edges
    EdgeIterator edgesEnd() const
    {
        return EdgeIterator(Edge(edgesSize()), this);
    }

    /// returns edge container for C++11 range-based for-loops
    EdgeContainer edges() const
    {
        return EdgeContainer(edgesBegin(), edgesEnd());
    }

    /// returns start iterator for faces
    FaceIterator facesBegin() const
    {
        return FaceIterator(Face(0), this);
    }

    /// returns end iterator for faces
    FaceIterator facesEnd() const
    {
        return FaceIterator(Face(facesSize()), this);
    }

    /// returns face container for C++11 range-based for-loops
    FaceContainer faces() const
    {
        return FaceContainer(facesBegin(), facesEnd());
    }

    /// returns circulator for vertices around vertex \c v
    VertexAroundVertexCirculator vertices(Vertex v) const
    {
        return VertexAroundVertexCirculator(this, v);
    }

    /// returns circulator for outgoing halfedges around vertex \c v
    HalfedgeAroundVertexCirculator halfedges(Vertex v) const
    {
        return HalfedgeAroundVertexCirculator(this, v);
    }

    /// returns circulator for faces around vertex \c v
    FaceAroundVertexCirculator faces(Vertex v) const
    {
        return FaceAroundVertexCirculator(this, v);
    }

    /// returns circulator for vertices of face \c f
    VertexAroundFaceCirculator vertices(Face f) const
    {
        return VertexAroundFaceCirculator(this, f);
    }

    /// returns circulator for halfedges of face \c f
    HalfedgeAroundFaceCirculator halfedges(Face f) const
    {
        return HalfedgeAroundFaceCirculator(this, f);
    }

    //@}





public: //--------------------------------------------- higher-level operations

    /// \name Higher-level Topological Operations
    //@{

    /// returns whether the mesh a triangle mesh. this function simply tests
    /// each face, and therefore is not very efficient.
    /// \sa trianglate(), triangulate(Face)
    inline bool isTriangleMesh() const;

    /// returns whether the mesh a quad mesh. this function simply tests
    /// each face, and therefore is not very efficient.
    inline bool isQuadMesh() const;

    /// triangulate the entire mesh, by calling triangulate(Face) for each face.
    /// \sa trianglate(Face)
    inline void triangulate();

    /// triangulate the face \c f
    /// \sa trianglate()
    inline void triangulate(Face f);


    /// returns whether collapsing the halfedge \c h is topologically legal.
    /// \attention This function is only valid for triangle meshes.
    inline bool isCollapseOk(Halfedge h);

    /** Collapse the halfedge \c h by moving its start vertex into its target
     vertex. For non-boundary halfedges this function removes one vertex, three
     edges, and two faces. For boundary halfedges it removes one vertex, two
     edges and one face.
     \attention This function is only valid for triangle meshes.
     \attention Halfedge collapses might lead to invalid faces. Call
     isCollapseOk(Halfedge) to be sure the collapse is legal.
     \attention The removed items are only marked as deleted. You have
     to call garbageCollection() to finally remove them.
     */
    inline void collapse(Halfedge h);


    /** Split the face \c f by inserting edges between \c p and the vertices
     of \c f. For a triangle this is a standard one-to-three split.
     */
    inline void split(Face f, Vertex v);


    /** Split the edge \c e by connecting vertex \c v it to the two vertices
     of the adjacent triangles that are opposite to edge \c e.
     \attention This function is only valid for triangle meshes.
     */
    inline void split(Edge e, Vertex v);


    /** Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
     (v0,v) and (v,v1). Note that this function does not introduce any other
     edge or faces. It simply splits the edge. Returns halfedge that points to \c p.
     \sa insertVertex(Halfedge, Vertex)
     */
    Halfedge insertVertex(Edge e, Vertex v)
    {
        return insertVertex(halfedge(e,0), v);
    }

    /** Subdivide the edge \c e = (v0,v1) by splitting it into the two edge
     (v0,v) and (v,v1). Note that this function does not introduce any other
     edge or faces. It simply splits the edge. Returns halfedge that points to \c p.
     \sa insertVertex(Edge, Vertex)
     */
    inline Halfedge insertVertex(Halfedge h, Vertex v);


    /// insert edge between the to-vertices v0 of h0 and v1 of h1.
    /// returns the new halfedge from v0 to v1.
    /// \attention h0 and h1 have to belong to the same face
    inline Halfedge insertEdge(Halfedge h0, Halfedge h1);


    /** Check whether flipping edge \c e is topologically
     \attention This function is only valid for triangle meshes.
     \sa flip(Edge)
     */
    inline bool isFlipOk(Edge e) const;

    /** Flip edge \c e: Remove edge \c e and add an edge between the two vertices
     opposite to edge \c e of the two incident triangles.
     \attention This function is only valid for triangle meshes.
     \sa isFlipOk(Edge)
     */
    inline void flip(Edge e);


    /** returns the valence (number of incident edges or neighboring vertices)
     of vertex \c v. */
    inline unsigned int valence(Vertex v) const;

    /// returns the valence of face \c f (its number of vertices)
    inline unsigned int valence(Face f) const;

    /// find the halfedge from start to end
    inline Halfedge findHalfedge(Vertex start, Vertex end) const;

    /// find the edge (a,b)
    inline Edge findEdge(Vertex a, Vertex b) const;

    /// deletes the vertex \c v from the mesh
    inline void deleteVertex(Vertex v);

    /// deletes the edge \c e from the mesh
    inline void deleteEdge(Edge e);

    /// deletes the face \c f from the mesh
    inline void deleteFace(Face f);

    //@}


protected: //-------------------------------------------- allocate new elements

    /// allocate a new vertex, resize vertex properties accordingly.
    Vertex newVertex()
    {
        m_vprops.pushBack();
        return Vertex(verticesSize()-1);
    }

    /// allocate a new edge, resize edge and halfedge properties accordingly.
    Halfedge newEdge(Vertex start, Vertex end)
    {
        assert(start != end);

        m_eprops.pushBack();
        m_hprops.pushBack();
        m_hprops.pushBack();

        Halfedge h0(halfedgesSize()-2);
        Halfedge h1(halfedgesSize()-1);

        setVertex(h0, end);
        setVertex(h1, start);

        return h0;
    }

    /// allocate a new face, resize face properties accordingly.
    Face newFace()
    {
        m_fprops.pushBack();
        return Face(facesSize()-1);
    }




protected: //------------------------------------------------- helper functions

    /** make sure that the outgoing halfedge of vertex v is a boundary halfedge
     if v is a boundary vertex. */
    inline void adjustOutgoingHalfedge(Vertex v);

    /// Helper for halfedge collapse
    inline void removeEdge(Halfedge h);

    /// Helper for halfedge collapse
    inline void removeLoop(Halfedge h);

    /// are there deleted vertices, edges or faces?
    inline bool garbage() const { return m_garbage; }



protected: //----------------------------------------------------- private data

//    friend inline bool read_poly(SurfaceMesh& mesh, const std::string& filename);

    PropertyContainer m_vprops;
    PropertyContainer m_hprops;
    PropertyContainer m_eprops;
    PropertyContainer m_fprops;

    VertexProperty<VertexConnectivity>      m_vconn;
    HalfedgeProperty<HalfedgeConnectivity>  m_hconn;
    FaceProperty<FaceConnectivity>          m_fconn;

    VertexProperty<bool>  m_vdeleted;
    EdgeProperty<bool>    m_edeleted;
    FaceProperty<bool>    m_fdeleted;

    unsigned int m_deletedVertices;
    unsigned int m_deletedEdges;
    unsigned int m_deleteFaces;
    bool m_garbage;
    std::vector<Vertex>   m_gcVertexMap;
    std::vector<Halfedge> m_gcHalfedgeMap;
    std::vector<Face>     m_gcFaceMap;

    // helper data for add_face()
    typedef std::pair<Halfedge, Halfedge>  NextCacheEntry;
    typedef std::vector<NextCacheEntry>    NextCache;
    std::vector<Vertex>      m_addFaceVertices;
    std::vector<Halfedge>    m_addFaceHalfedges;
    std::vector<bool>        m_addFaceIsNew;
    std::vector<bool>        m_addFaceNeedsAdjust;
    NextCache                m_addFaceNextCache;
};
//------------------------------------------------------------ output operators


inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Vertex v)
{
    return (os << 'v' << v.idx());
}

inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Halfedge h)
{
    return (os << 'h' << h.idx());
}

inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Edge e)
{
    return (os << 'e' << e.idx());
}

inline std::ostream& operator<<(std::ostream& os, SurfaceMesh::Face f)
{
    return (os << 'f' << f.idx());
}

//inline bool read_mesh(SurfaceMesh& mesh, const std::string& filename);
//inline bool read_off(SurfaceMesh& mesh, const std::string& filename);
//inline bool read_obj(SurfaceMesh& mesh, const std::string& filename);
//inline bool read_stl(SurfaceMesh& mesh, const std::string& filename);

//inline bool write_mesh(const SurfaceMesh& mesh, const std::string& filename);
//inline bool write_off(const SurfaceMesh& mesh, const std::string& filename);
//inline bool write_obj(const SurfaceMesh& mesh, const std::string& filename);


//== IMPLEMENTATION ===========================================================


SurfaceMesh::
SurfaceMesh()
{
    // allocate standard properties
    // same list is used in operator=() and assign()
    m_vconn    = addVertexProperty<VertexConnectivity>("v:connectivity");
    m_hconn    = addHalfedgeProperty<HalfedgeConnectivity>("h:connectivity");
    m_fconn    = addFaceProperty<FaceConnectivity>("f:connectivity");
    m_vdeleted = addVertexProperty<bool>("v:deleted", false);
    m_edeleted = addEdgeProperty<bool>("e:deleted", false);
    m_fdeleted = addFaceProperty<bool>("f:deleted", false);

    m_deletedVertices = m_deletedEdges = m_deleteFaces = 0;
    m_garbage = false;
}


//-----------------------------------------------------------------------------


SurfaceMesh::
~SurfaceMesh()
{
}


//-----------------------------------------------------------------------------


SurfaceMesh&
SurfaceMesh::
operator=(const SurfaceMesh& rhs)
{
    if (this != &rhs)
    {
        // deep copy of property containers
        m_vprops = rhs.m_vprops;
        m_hprops = rhs.m_hprops;
        m_eprops = rhs.m_eprops;
        m_fprops = rhs.m_fprops;

        // property handles contain pointers, have to be reassigned
        m_vconn    = vertexProperty<VertexConnectivity>("v:connectivity");
        m_hconn    = halfedgeProperty<HalfedgeConnectivity>("h:connectivity");
        m_fconn    = faceProperty<FaceConnectivity>("f:connectivity");
        m_vdeleted = vertexProperty<bool>("v:deleted");
        m_edeleted = edgeProperty<bool>("e:deleted");
        m_fdeleted = faceProperty<bool>("f:deleted");

        // how many elements are deleted?
        m_deletedVertices = rhs.m_deletedVertices;
        m_deletedEdges    = rhs.m_deletedEdges;
        m_deleteFaces    = rhs.m_deleteFaces;
        m_garbage          = rhs.m_garbage;
    }

    return *this;
}


//-----------------------------------------------------------------------------


SurfaceMesh&
SurfaceMesh::
assign(const SurfaceMesh& rhs)
{
    if (this != &rhs)
    {
        // clear properties
        m_vprops.clear();
        m_hprops.clear();
        m_eprops.clear();
        m_fprops.clear();

        // allocate standard properties
        m_vconn    = addVertexProperty<VertexConnectivity>("v:connectivity");
        m_hconn    = addHalfedgeProperty<HalfedgeConnectivity>("h:connectivity");
        m_fconn    = addFaceProperty<FaceConnectivity>("f:connectivity");
        m_vdeleted = addVertexProperty<bool>("v:deleted", false);
        m_edeleted = addEdgeProperty<bool>("e:deleted", false);
        m_fdeleted = addFaceProperty<bool>("f:deleted", false);

        // copy properties from other mesh
        m_vconn.array()     = rhs.m_vconn.array();
        m_hconn.array()     = rhs.m_hconn.array();
        m_fconn.array()     = rhs.m_fconn.array();
        m_vdeleted.array()  = rhs.m_vdeleted.array();
        m_edeleted.array()  = rhs.m_edeleted.array();
        m_fdeleted.array()  = rhs.m_fdeleted.array();

        // resize (needed by property containers)
        m_vprops.resize(rhs.verticesSize());
        m_hprops.resize(rhs.halfedgesSize());
        m_eprops.resize(rhs.edgesSize());
        m_fprops.resize(rhs.facesSize());

        // how many elements are deleted?
        m_deletedVertices = rhs.m_deletedVertices;
        m_deletedEdges    = rhs.m_deletedEdges;
        m_deleteFaces    = rhs.m_deleteFaces;
        m_garbage          = rhs.m_garbage;
    }

    return *this;
}


//-----------------------------------------------------------------------------


//bool
//SurfaceMesh::
//read(const std::string& filename)
//{
//    return read_mesh(*this, filename);
//}


//-----------------------------------------------------------------------------


//bool
//SurfaceMesh::
//write(const std::string& filename) const
//{
//    return write_mesh(*this, filename);
//}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
clear()
{
    m_vprops.resize(0);
    m_hprops.resize(0);
    m_eprops.resize(0);
    m_fprops.resize(0);

    m_vprops.freeMemory();
    m_hprops.freeMemory();
    m_eprops.freeMemory();
    m_fprops.freeMemory();

    m_deletedVertices = m_deletedEdges = m_deleteFaces = 0;
    m_garbage = false;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
freeMemory()
{
    m_vprops.freeMemory();
    m_hprops.freeMemory();
    m_eprops.freeMemory();
    m_fprops.freeMemory();
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
reserve(unsigned int nvertices,
        unsigned int nedges,
        unsigned int nfaces )
{
    m_vprops.reserve(nvertices);
    m_hprops.reserve(2*nedges);
    m_eprops.reserve(nedges);
    m_fprops.reserve(nfaces);
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
propertyStats() const
{
    std::vector<std::string> props;

    std::cout << "vertex properties:\n";
    props = vertexProperties();
    for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;

    std::cout << "halfedge properties:\n";
    props = halfedgeProperties();
    for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;

    std::cout << "edge properties:\n";
    props = edgeProperties();
    for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;

    std::cout << "face properties:\n";
    props = faceProperties();
    for (unsigned int i=0; i<props.size(); ++i)
        std::cout << "\t" << props[i] << std::endl;
}


//-----------------------------------------------------------------------------


SurfaceMesh::Vertex
SurfaceMesh::
addVertex()
{
    Vertex v = newVertex();
    return v;
}


//-----------------------------------------------------------------------------


SurfaceMesh::Halfedge
SurfaceMesh::
findHalfedge(Vertex start, Vertex end) const
{
    assert(isValid(start) && isValid(end));

    Halfedge h  = halfedge(start);
    const Halfedge hh = h;

    if (h.isValid())
    {
        do
        {
            if (toVertex(h) == end)
                return h;
            h = cwRotatedHalfedge(h);
        }
        while (h != hh);
    }

    return Halfedge();
}


//-----------------------------------------------------------------------------


SurfaceMesh::Edge
SurfaceMesh::
findEdge(Vertex a, Vertex b) const
{
    Halfedge h = findHalfedge(a,b);
    return h.isValid() ? edge(h) : Edge();
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
adjustOutgoingHalfedge(Vertex v)
{
    Halfedge h  = halfedge(v);
    const Halfedge hh = h;

    if (h.isValid())
    {
        do
        {
            if (isBoundary(h))
            {
                setHalfedge(v, h);
                return;
            }
            h = cwRotatedHalfedge(h);
        }
        while (h != hh);
    }
}


//-----------------------------------------------------------------------------


SurfaceMesh::Face
SurfaceMesh::
addTriangle(Vertex v0, Vertex v1, Vertex v2)
{
    m_addFaceVertices.resize(3);
    m_addFaceVertices[0] = v0;
    m_addFaceVertices[1] = v1;
    m_addFaceVertices[2] = v2;
    return addFace(m_addFaceVertices);
}


//-----------------------------------------------------------------------------


SurfaceMesh::Face
SurfaceMesh::
addQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3)
{
    m_addFaceVertices.resize(4);
    m_addFaceVertices[0] = v0;
    m_addFaceVertices[1] = v1;
    m_addFaceVertices[2] = v2;
    m_addFaceVertices[3] = v3;
    return addFace(m_addFaceVertices);
}


//-----------------------------------------------------------------------------


SurfaceMesh::Face
SurfaceMesh::
addFace(const std::vector<Vertex>& vertices)
{
    const unsigned int n(vertices.size());
    assert (n > 2);

    Vertex        v;
    unsigned int  i, ii, id;
    Halfedge      inner_next, inner_prev, outer_next, outer_prev, boundary_next, boundary_prev, patch_start, patch_end;


    // use global arrays to avoid new/delete of local arrays!!!
    std::vector<Halfedge>&  halfedges    = m_addFaceHalfedges;
    std::vector<bool>&      is_new       = m_addFaceIsNew;
    std::vector<bool>&      needs_adjust = m_addFaceNeedsAdjust;
    NextCache&              next_cache   = m_addFaceNextCache;
    halfedges.clear();
    halfedges.resize(n);
    is_new.clear();
    is_new.resize(n);
    needs_adjust.clear();
    needs_adjust.resize(n, false);
    next_cache.clear();
    next_cache.reserve(3*n);


    // test for topological errors
    for (i=0, ii=1; i<n; ++i, ++ii, ii%=n)
    {
        if ( !isBoundary(vertices[i]) )
        {
            // std::cerr << "SurfaceMeshT::addFace: complex vertex\n";
            return Face();
        }

        halfedges[i] = findHalfedge(vertices[i], vertices[ii]);
        is_new[i]    = !halfedges[i].isValid();

        if (!is_new[i] && !isBoundary(halfedges[i]))
        {
            // std::cerr << "Surface_meshT::add_face: complex edge\n";
            return Face();
        }
    }


    // re-link patches if necessary
    for (i=0, ii=1; i<n; ++i, ++ii, ii%=n)
    {
        if (!is_new[i] && !is_new[ii])
        {
            inner_prev = halfedges[i];
            inner_next = halfedges[ii];

            if (nextHalfedge(inner_prev) != inner_next)
            {
                // here comes the ugly part... we have to relink a whole patch

                // search a free gap
                // free gap will be between boundary_prev and boundary_next
                outer_prev = oppositeHalfedge(inner_next);
                outer_next = oppositeHalfedge(inner_prev);
                boundary_prev = outer_prev;
                do
                {
                    boundary_prev = oppositeHalfedge(nextHalfedge(boundary_prev));
                }
                while (!isBoundary(boundary_prev) || boundary_prev==inner_prev);
                boundary_next = nextHalfedge(boundary_prev);
                assert(isBoundary(boundary_prev));
                assert(isBoundary(boundary_next));


                // ok ?
                if (boundary_next == inner_next)
                {
                    // std::cerr << "SurfaceMeshT::addFace: patch re-linking failed\n";
                    return Face();
                }

                // other halfedges' handles
                patch_start = nextHalfedge(inner_prev);
                patch_end   = prevHalfedge(inner_next);

                // relink
                next_cache.push_back(NextCacheEntry(boundary_prev, patch_start));
                next_cache.push_back(NextCacheEntry(patch_end, boundary_next));
                next_cache.push_back(NextCacheEntry(inner_prev, inner_next));
            }
        }
    }



    // create missing edges
    for (i=0, ii=1; i<n; ++i, ++ii, ii%=n)
    {
        if (is_new[i])
        {
            halfedges[i] = newEdge(vertices[i], vertices[ii]);
        }
    }



    // create the face
    Face f(newFace());
    setHalfedge(f, halfedges[n-1]);



    // setup halfedges
    for (i=0, ii=1; i<n; ++i, ++ii, ii%=n)
    {
        v          = vertices[ii];
        inner_prev = halfedges[i];
        inner_next = halfedges[ii];

        id = 0;
        if (is_new[i])  id |= 1;
        if (is_new[ii]) id |= 2;

        if (id)
        {
            outer_prev = oppositeHalfedge(inner_next);
            outer_next = oppositeHalfedge(inner_prev);

            // set outer links
            switch (id)
            {
                case 1: // prev is new, next is old
                    boundary_prev = prevHalfedge(inner_next);
                    next_cache.push_back(NextCacheEntry(boundary_prev, outer_next));
                    setHalfedge(v, outer_next);
                    break;

                case 2: // next is new, prev is old
                    boundary_next = nextHalfedge(inner_prev);
                    next_cache.push_back(NextCacheEntry(outer_prev, boundary_next));
                    setHalfedge(v, boundary_next);
                    break;

                case 3: // both are new
                    if (!halfedge(v).isValid())
                    {
                        setHalfedge(v, outer_next);
                        next_cache.push_back(NextCacheEntry(outer_prev, outer_next));
                    }
                    else
                    {
                        boundary_next = halfedge(v);
                        boundary_prev = prevHalfedge(boundary_next);
                        next_cache.push_back(NextCacheEntry(boundary_prev, outer_next));
                        next_cache.push_back(NextCacheEntry(outer_prev, boundary_next));
                    }
                    break;
            }

            // set inner link
            next_cache.push_back(NextCacheEntry(inner_prev, inner_next));
        }
        else needs_adjust[ii] = (halfedge(v) == inner_next);


        // set face handle
        setFace(halfedges[i], f);
    }



    // process next halfedge cache
    NextCache::const_iterator ncIt(next_cache.begin()), ncEnd(next_cache.end());
    for (; ncIt != ncEnd; ++ncIt)
    {
        setNextHalfedge(ncIt->first, ncIt->second);
    }



    // adjust vertices' halfedge handle
    for (i=0; i<n; ++i)
    {
        if (needs_adjust[i])
        {
            adjustOutgoingHalfedge(vertices[i]);
        }
    }


    return f;
}


//-----------------------------------------------------------------------------


unsigned int
SurfaceMesh::
valence(Vertex v) const
{
    unsigned int count(0);

    VertexAroundVertexCirculator vvit = vertices(v);
    VertexAroundVertexCirculator vvend = vvit;
    if (vvit) do
    {
        ++count;
    } while (++vvit != vvend);

    return count;
}


//-----------------------------------------------------------------------------


unsigned int
SurfaceMesh::
valence(Face f) const
{
    unsigned int count(0);

    VertexAroundFaceCirculator fvit = vertices(f);
    VertexAroundFaceCirculator fvend = fvit;
    do {
        ++count;
    } while (++fvit != fvend);

    return count;
}


//-----------------------------------------------------------------------------


bool
SurfaceMesh::
isTriangleMesh() const
{
    FaceIterator fit=facesBegin(), fend=facesEnd();
    for (; fit!=fend; ++fit)
        if (valence(*fit) != 3)
            return false;

    return true;
}


//-----------------------------------------------------------------------------


bool
SurfaceMesh::
isQuadMesh() const
{
    FaceIterator fit=facesBegin(), fend=facesEnd();
    for (; fit!=fend; ++fit)
        if (valence(*fit) != 4)
            return false;

    return true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
triangulate()
{
    /* The iterators will stay valid, even though new faces are added,
     because they are now implemented index-based instead of
     pointer-based.
     */
    FaceIterator fit=facesBegin(), fend=facesEnd();
    for (; fit!=fend; ++fit)
        triangulate(*fit);
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
triangulate(Face f)
{
    /*
     Split an arbitrary face into triangles by connecting
     each vertex of fh after its second to vh.

     - fh will remain valid (it will become one of the
     triangles)
     - the halfedge handles of the new triangles will
     point to the old halfedges
     */

    Halfedge base_h  = halfedge(f);
    Vertex   start_v = fromVertex(base_h);
    Halfedge next_h  = nextHalfedge(base_h);

    while (toVertex(nextHalfedge(next_h)) != start_v)
    {
        Halfedge next_next_h(nextHalfedge(next_h));

        Face new_f = newFace();
        setHalfedge(new_f, base_h);

        Halfedge new_h = newEdge(toVertex(next_h), start_v);

        setNextHalfedge(base_h, next_h);
        setNextHalfedge(next_h, new_h);
        setNextHalfedge(new_h,  base_h);

        setFace(base_h, new_f);
        setFace(next_h, new_f);
        setFace(new_h,  new_f);

        base_h = oppositeHalfedge(new_h);
        next_h = next_next_h;
    }
    setHalfedge(f, base_h);  //the last face takes the handle _fh

    setNextHalfedge(base_h, next_h);
    setNextHalfedge(nextHalfedge(next_h), base_h);

    setFace(base_h, f);
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
split(Face f, Vertex v)
{
    /*
     Split an arbitrary face into triangles by connecting each vertex of fh to vh.
     - fh will remain valid (it will become one of the triangles)
     - the halfedge handles of the new triangles will point to the old halfeges
     */

    Halfedge hend = halfedge(f);
    Halfedge h    = nextHalfedge(hend);

    Halfedge hold = newEdge(toVertex(hend), v);

    setNextHalfedge(hend, hold);
    setFace(hold, f);

    hold = oppositeHalfedge(hold);

    while (h != hend)
    {
        Halfedge hnext = nextHalfedge(h);

        Face fnew = newFace();
        setHalfedge(fnew, h);

        Halfedge hnew = newEdge(toVertex(h), v);

        setNextHalfedge(hnew, hold);
        setNextHalfedge(hold, h);
        setNextHalfedge(h,    hnew);

        setFace(hnew, fnew);
        setFace(hold, fnew);
        setFace(h,    fnew);

        hold = oppositeHalfedge(hnew);

        h = hnext;
    }

    setNextHalfedge(hold, hend);
    setNextHalfedge(nextHalfedge(hend), hold);

    setFace(hold, f);

    setHalfedge(v, hold);
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
split(Edge e, Vertex v)
{
    Halfedge h0 = halfedge(e, 0);
    Halfedge o0 = halfedge(e, 1);

    Vertex   v2 = toVertex(o0);

    Halfedge e1 = newEdge(v, v2);
    Halfedge t1 = oppositeHalfedge(e1);

    Face     f0 = face(h0);
    Face     f3 = face(o0);

    setHalfedge(v, h0);
    setVertex(o0, v);

    if (!isBoundary(h0))
    {
        Halfedge h1 = nextHalfedge(h0);
        Halfedge h2 = nextHalfedge(h1);

        Vertex   v1 = toVertex(h1);

        Halfedge e0 = newEdge(v, v1);
        Halfedge t0 = oppositeHalfedge(e0);

        Face f1 = newFace();
        setHalfedge(f0, h0);
        setHalfedge(f1, h2);

        setFace(h1, f0);
        setFace(t0, f0);
        setFace(h0, f0);

        setFace(h2, f1);
        setFace(t1, f1);
        setFace(e0, f1);

        setNextHalfedge(h0, h1);
        setNextHalfedge(h1, t0);
        setNextHalfedge(t0, h0);

        setNextHalfedge(e0, h2);
        setNextHalfedge(h2, t1);
        setNextHalfedge(t1, e0);
    }
    else
    {
        setNextHalfedge(prevHalfedge(h0), t1);
        setNextHalfedge(t1, h0);
        // halfedge handle of _vh already is h0
    }


    if (!isBoundary(o0))
    {
        Halfedge o1 = nextHalfedge(o0);
        Halfedge o2 = nextHalfedge(o1);

        Vertex v3 = toVertex(o1);

        Halfedge e2 = newEdge(v, v3);
        Halfedge t2 = oppositeHalfedge(e2);

        Face f2 = newFace();
        setHalfedge(f2, o1);
        setHalfedge(f3, o0);

        setFace(o1, f2);
        setFace(t2, f2);
        setFace(e1, f2);

        setFace(o2, f3);
        setFace(o0, f3);
        setFace(e2, f3);

        setNextHalfedge(e1, o1);
        setNextHalfedge(o1, t2);
        setNextHalfedge(t2, e1);

        setNextHalfedge(o0, e2);
        setNextHalfedge(e2, o2);
        setNextHalfedge(o2, o0);
    }
    else
    {
        setNextHalfedge(e1, nextHalfedge(o0));
        setNextHalfedge(o0, e1);
        setHalfedge(v, e1);
    }

    if (halfedge(v2) == h0)
        setHalfedge(v2, t1);
}


//-----------------------------------------------------------------------------


SurfaceMesh::Halfedge
SurfaceMesh::
insertVertex(Halfedge h0, Vertex v)
{
    // before:
    //
    // v0      h0       v2
    //  o--------------->o
    //   <---------------
    //         o0
    //
    // after:
    //
    // v0  h0   v   h1   v2
    //  o------>o------->o
    //   <------ <-------
    //     o0       o1

    Halfedge h2 = nextHalfedge(h0);
    Halfedge o0 = oppositeHalfedge(h0);
    Halfedge o2 = prevHalfedge(o0);
    Vertex   v2 = toVertex(h0);
    Face     fh = face(h0);
    Face     fo = face(o0);

    Halfedge h1 = newEdge(v, v2);
    Halfedge o1 = oppositeHalfedge(h1);

    // adjust halfedge connectivity
    setNextHalfedge(h1, h2);
    setNextHalfedge(h0, h1);
    setVertex(h0, v);
    setVertex(h1, v2);
    setFace(h1, fh);

    setNextHalfedge(o1, o0);
    setNextHalfedge(o2, o1);
    setVertex(o1, v);
    setFace(o1, fo);

    // adjust vertex connectivity
    setHalfedge(v2, o1);
    adjustOutgoingHalfedge(v2);
    setHalfedge(v, h1);
    adjustOutgoingHalfedge(v);

    // adjust face connectivity
    if (fh.isValid()) setHalfedge(fh, h0);
    if (fo.isValid()) setHalfedge(fo, o1);

    return h0;
}


//-----------------------------------------------------------------------------


SurfaceMesh::Halfedge
SurfaceMesh::
insertEdge(Halfedge h0, Halfedge h1)
{
    assert(face(h0) == face(h1));
    assert(face(h0).isValid());

    Vertex   v0 = toVertex(h0);
    Vertex   v1 = toVertex(h1);

    Halfedge h2 = nextHalfedge(h0);
    Halfedge h3 = nextHalfedge(h1);

    Halfedge h4 = newEdge(v0, v1);
    Halfedge h5 = oppositeHalfedge(h4);

    Face     f0 = face(h0);
    Face     f1 = newFace();

    setHalfedge(f0, h0);
    setHalfedge(f1, h1);

    setNextHalfedge(h0, h4);
    setNextHalfedge(h4, h3);
    setFace(h4, f0);

    setNextHalfedge(h1, h5);
    setNextHalfedge(h5, h2);
    Halfedge h = h2;
    do
    {
        setFace(h, f1);
        h = nextHalfedge(h);
    }
    while (h != h2);

    return h4;
}


//-----------------------------------------------------------------------------


bool
SurfaceMesh::
isFlipOk(Edge e) const
{
    // boundary edges cannot be flipped
    if (isBoundary(e)) return false;

    // check if the flipped edge is already present in the mesh

    Halfedge h0 = halfedge(e, 0);
    Halfedge h1 = halfedge(e, 1);

    Vertex v0 = toVertex(nextHalfedge(h0));
    Vertex v1 = toVertex(nextHalfedge(h1));

    if (v0 == v1)   // this is generally a bad sign !!!
        return false;

    if (findHalfedge(v0, v1).isValid())
        return false;

    return true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
flip(Edge e)
{
    // CAUTION : Flipping a halfedge may result in
    // a non-manifold mesh, hence check for yourself
    // whether this operation is allowed or not!

    //let's make it sure it is actually checked
    assert(isFlipOk(e));

    Halfedge a0 = halfedge(e, 0);
    Halfedge b0 = halfedge(e, 1);

    Halfedge a1 = nextHalfedge(a0);
    Halfedge a2 = nextHalfedge(a1);

    Halfedge b1 = nextHalfedge(b0);
    Halfedge b2 = nextHalfedge(b1);

    Vertex   va0 = toVertex(a0);
    Vertex   va1 = toVertex(a1);

    Vertex   vb0 = toVertex(b0);
    Vertex   vb1 = toVertex(b1);

    Face     fa  = face(a0);
    Face     fb  = face(b0);

    setVertex(a0, va1);
    setVertex(b0, vb1);

    setNextHalfedge(a0, a2);
    setNextHalfedge(a2, b1);
    setNextHalfedge(b1, a0);

    setNextHalfedge(b0, b2);
    setNextHalfedge(b2, a1);
    setNextHalfedge(a1, b0);

    setFace(a1, fb);
    setFace(b1, fa);

    setHalfedge(fa, a0);
    setHalfedge(fb, b0);

    if (halfedge(va0) == b0)
        setHalfedge(va0, a1);
    if (halfedge(vb0) == a0)
        setHalfedge(vb0, b1);
}


//-----------------------------------------------------------------------------


bool
SurfaceMesh::
isCollapseOk(Halfedge v0v1)
{
    Halfedge  v1v0(oppositeHalfedge(v0v1));
    Vertex    v0(toVertex(v1v0));
    Vertex    v1(toVertex(v0v1));
    Vertex    vv, vl, vr;
    Halfedge  h1, h2;


    // the edges v1-vl and vl-v0 must not be both boundary edges
    if (!isBoundary(v0v1))
    {
        vl = toVertex(nextHalfedge(v0v1));
        h1 = nextHalfedge(v0v1);
        h2 = nextHalfedge(h1);
        if (isBoundary(oppositeHalfedge(h1)) && isBoundary(oppositeHalfedge(h2)))
            return false;
    }


    // the edges v0-vr and vr-v1 must not be both boundary edges
    if (!isBoundary(v1v0))
    {
        vr = toVertex(nextHalfedge(v1v0));
        h1 = nextHalfedge(v1v0);
        h2 = nextHalfedge(h1);
        if (isBoundary(oppositeHalfedge(h1)) && isBoundary(oppositeHalfedge(h2)))
            return false;
    }


    // if vl and vr are equal or both invalid -> fail
    if (vl == vr) return false;


    // edge between two boundary vertices should be a boundary edge
    if ( isBoundary(v0) && isBoundary(v1) &&
        !isBoundary(v0v1) && !isBoundary(v1v0))
        return false;


    // test intersection of the one-rings of v0 and v1
    VertexAroundVertexCirculator vv_it, vv_end;
    vv_it = vv_end = vertices(v0);
    do
    {
        vv = *vv_it;
        if (vv != v1 && vv != vl && vv != vr)
            if (findHalfedge(vv, v1).isValid())
                return false;
    }
    while (++vv_it != vv_end);


    // passed all tests
    return true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
collapse(Halfedge h)
{
    Halfedge h0 = h;
    Halfedge h1 = prevHalfedge(h0);
    Halfedge o0 = oppositeHalfedge(h0);
    Halfedge o1 = nextHalfedge(o0);

    // remove edge
    removeEdge(h0);

    // remove loops
    if (nextHalfedge(nextHalfedge(h1)) == h1)
        removeLoop(h1);
    if (nextHalfedge(nextHalfedge(o1)) == o1)
        removeLoop(o1);
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
removeEdge(Halfedge h)
{
    Halfedge  hn = nextHalfedge(h);
    Halfedge  hp = prevHalfedge(h);

    Halfedge  o  = oppositeHalfedge(h);
    Halfedge  on = nextHalfedge(o);
    Halfedge  op = prevHalfedge(o);

    Face      fh = face(h);
    Face      fo = face(o);

    Vertex    vh = toVertex(h);
    Vertex    vo = toVertex(o);



    // halfedge -> vertex
    HalfedgeAroundVertexCirculator vh_it, vh_end;
    vh_it = vh_end = halfedges(vo);
    do
    {
        setVertex(oppositeHalfedge(*vh_it), vh);
    }
    while (++vh_it != vh_end);


    // halfedge -> halfedge
    setNextHalfedge(hp, hn);
    setNextHalfedge(op, on);


    // face -> halfedge
    if (fh.isValid())  setHalfedge(fh, hn);
    if (fo.isValid())  setHalfedge(fo, on);


    // vertex -> halfedge
    if (halfedge(vh) == o)  setHalfedge(vh, hn);
    adjustOutgoingHalfedge(vh);
    setHalfedge(vo, Halfedge());


    // delete stuff
    if (!m_vdeleted) m_vdeleted = vertexProperty<bool>("v:deleted", false);
    if (!m_edeleted) m_edeleted = edgeProperty<bool>("e:deleted", false);
    m_vdeleted[vo]      = true; ++m_deletedVertices;
    m_edeleted[edge(h)] = true; ++m_deletedEdges;
    m_garbage = true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
removeLoop(Halfedge h)
{
    Halfedge  h0 = h;
    Halfedge  h1 = nextHalfedge(h0);

    Halfedge  o0 = oppositeHalfedge(h0);
    Halfedge  o1 = oppositeHalfedge(h1);

    Vertex    v0 = toVertex(h0);
    Vertex    v1 = toVertex(h1);

    Face      fh = face(h0);
    Face      fo = face(o0);



    // is it a loop ?
    assert ((nextHalfedge(h1) == h0) && (h1 != o0));


    // halfedge -> halfedge
    setNextHalfedge(h1, nextHalfedge(o0));
    setNextHalfedge(prevHalfedge(o0), h1);


    // halfedge -> face
    setFace(h1, fo);


    // vertex -> halfedge
    setHalfedge(v0, h1);  adjustOutgoingHalfedge(v0);
    setHalfedge(v1, o1);  adjustOutgoingHalfedge(v1);


    // face -> halfedge
    if (fo.isValid() && halfedge(fo) == o0)
        setHalfedge(fo, h1);


    // delete stuff
    if (!m_edeleted) m_edeleted = edgeProperty<bool>("e:deleted", false);
    if (!m_fdeleted) m_fdeleted = faceProperty<bool>("f:deleted", false);
    if (fh.isValid()) { m_fdeleted[fh] = true; ++m_deleteFaces; }
    m_edeleted[edge(h0)] = true; ++m_deletedEdges;
    m_garbage = true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
deleteVertex(Vertex v)
{
    if (m_vdeleted[v])  return;

    // collect incident faces
    std::vector<Face> incident_faces;
    incident_faces.reserve(6);

    FaceAroundVertexCirculator fc, fc_end;
    fc = fc_end = faces(v);

    if (fc)
    do
    {
        incident_faces.push_back(*fc);
    } while (++fc != fc_end);

    // delete incident faces
    std::vector<Face>::iterator fit(incident_faces.begin()),
                                fend(incident_faces.end());

    for (; fit != fend; ++fit)
        deleteFace(*fit);

    m_vdeleted[v] = true;
    m_deletedVertices++;
    m_garbage = true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
deleteEdge(Edge e)
{
    if (m_edeleted[e])  return;

    Face f0 = face(halfedge(e, 0));
    Face f1 = face(halfedge(e, 1));

    if (f0.isValid()) deleteFace(f0);
    if (f1.isValid()) deleteFace(f1);
}


//-----------------------------------------------------------------------------

void
SurfaceMesh::
deleteFace(Face f)
{
    if (m_fdeleted[f])  return;

    // mark face deleted
    if (!m_fdeleted[f])
    {
        m_fdeleted[f] = true;
        m_deleteFaces++;
    }

    // boundary edges of face f to be deleted
    std::vector<Edge> deleted_edges;
    deleted_edges.reserve(3);


    // vertices of face f for updating their outgoing halfedge
    std::vector<Vertex> vertices;
    vertices.reserve(3);


    // for all halfedges of face f do:
    //   1) invalidate face handle.
    //   2) collect all boundary halfedges, set them deleted
    //   3) store vertex handles
    HalfedgeAroundFaceCirculator hc, hc_end;
    hc = hc_end = halfedges(f);

    do
    {
        setFace(*hc, Face());

        if (isBoundary(oppositeHalfedge(*hc)))
            deleted_edges.push_back(edge(*hc));

        vertices.push_back(toVertex(*hc));

    } while (++hc != hc_end);


    // delete all collected (half)edges
    // delete isolated vertices
    if (!deleted_edges.empty())
    {
        std::vector<Edge>::iterator del_it(deleted_edges.begin()),
                                    del_end(deleted_edges.end());

        Halfedge h0, h1, next0, next1, prev0, prev1;
        Vertex   v0, v1;

        for (; del_it!=del_end; ++del_it)
        {
            h0    = halfedge(*del_it, 0);
            v0    = toVertex(h0);
            next0 = nextHalfedge(h0);
            prev0 = prevHalfedge(h0);

            h1    = halfedge(*del_it, 1);
            v1    = toVertex(h1);
            next1 = nextHalfedge(h1);
            prev1 = prevHalfedge(h1);

            // adjust next and prev handles
            setNextHalfedge(prev0, next1);
            setNextHalfedge(prev1, next0);

            // mark edge deleted
            if (!m_edeleted[*del_it])
            {
                m_edeleted[*del_it] = true;
                m_deletedEdges++;
            }

            // update v0
            if (halfedge(v0) == h1)
            {
                if (next0 == h1)
                {
                    if (!m_vdeleted[v0])
                    {
                        m_vdeleted[v0] = true;
                        m_deletedVertices++;
                    }
                }
                else setHalfedge(v0, next0);
            }

            // update v1
            if (halfedge(v1) == h0)
            {
                if (next1 == h0)
                {
                    if (!m_vdeleted[v1])
                    {
                        m_vdeleted[v1] = true;
                        m_deletedVertices++;
                    }
                }
                else  setHalfedge(v1, next1);
            }
        }
    }


    // update outgoing halfedge handles of remaining vertices
    std::vector<Vertex>::iterator v_it(vertices.begin()),
                                  v_end(vertices.end());
    for (; v_it!=v_end; ++v_it)
        adjustOutgoingHalfedge(*v_it);

    m_garbage = true;
}


//-----------------------------------------------------------------------------


void
SurfaceMesh::
garbageCollection(unsigned flags)
{
    int  i, i0, i1,
    nV(verticesSize()),
    nE(edgesSize()),
    nH(halfedgesSize()),
    nF(facesSize());

    Vertex    v;
    Halfedge  h;
    Face      f;


    // setup handle mapping
    m_gcVertexMap  .resize(nV);
    m_gcHalfedgeMap.resize(nH);
    m_gcFaceMap    .resize(nF);

    for (i=0; i<nV; ++i)
        m_gcVertexMap[i]   = Vertex(i);
    for (i=0; i<nH; ++i)
        m_gcHalfedgeMap[i] = Halfedge(i);
    for (i=0; i<nF; ++i)
        m_gcFaceMap[i]     = Face(i);



    // remove deleted vertices
    if (nV > 0)
    {
        i0=0;  i1=nV-1;

        while (1)
        {
            // find first deleted and last un-deleted
            while (!m_vdeleted[Vertex(i0)] && i0 < i1)  ++i0;
            while ( m_vdeleted[Vertex(i1)] && i0 < i1)  --i1;
            if (i0 >= i1) break;

            // swap
            m_vprops.swap(i0, i1);
            std::swap(m_gcVertexMap[i0], m_gcVertexMap[i1]);
        };

        // remember new size
        nV = m_vdeleted[Vertex(i0)] ? i0 : i0+1;
    }


    // remove deleted edges
    if (nE > 0)
    {
        i0=0;  i1=nE-1;

        while (1)
        {
            // find first deleted and last un-deleted
            while (!m_edeleted[Edge(i0)] && i0 < i1)  ++i0;
            while ( m_edeleted[Edge(i1)] && i0 < i1)  --i1;
            if (i0 >= i1) break;

            // swap
            m_eprops.swap(i0, i1);
            m_hprops.swap(2*i0,   2*i1);
            m_hprops.swap(2*i0+1, 2*i1+1);
            std::swap(m_gcHalfedgeMap[2*i0 + 0], m_gcHalfedgeMap[2*i1 + 0]);
            std::swap(m_gcHalfedgeMap[2*i0 + 1], m_gcHalfedgeMap[2*i1 + 1]);
        };

        // remember new size
        nE = m_edeleted[Edge(i0)] ? i0 : i0+1;
        nH = 2*nE;
    }


    // remove deleted faces
    if (nF > 0)
    {
        i0=0;  i1=nF-1;

        while (1)
        {
            // find 1st deleted and last un-deleted
            while (!m_fdeleted[Face(i0)] && i0 < i1)  ++i0;
            while ( m_fdeleted[Face(i1)] && i0 < i1)  --i1;
            if (i0 >= i1) break;

            // swap
            m_fprops.swap(i0, i1);
            std::swap(m_gcFaceMap[i0], m_gcFaceMap[i1]);
        };

        // remember new size
        nF = m_fdeleted[Face(i0)] ? i0 : i0+1;
    }


    // update vertex connectivity
    for (i=0; i<nV; ++i)
    {
        v = Vertex(i);
        if (!isIsolated(v))
            setHalfedge(v, gcMap(halfedge(v)));
    }


    // update halfedge connectivity
    for (i=0; i<nH; ++i)
    {
        h = Halfedge(i);
        setVertex(h, gcMap(toVertex(h)));
        setNextHalfedge(h, gcMap(nextHalfedge(h)));
        if (!isBoundary(h))
            setFace(h, gcMap(face(h)));
    }


    // update handles of faces
    for (i=0; i<nF; ++i)
    {
        f = Face(i);
        setHalfedge(f, gcMap(halfedge(f)));
    }


    // remove handle maps
    if(!(flags & GC_DONT_RELEASE_INDEX_MAPS))
    {
        releaseGCIndexMaps();
    }

    // finally resize arrays
    m_vprops.resize(nV); m_vprops.freeMemory();
    m_hprops.resize(nH); m_hprops.freeMemory();
    m_eprops.resize(nE); m_eprops.freeMemory();
    m_fprops.resize(nF); m_fprops.freeMemory();

    m_deletedVertices = m_deletedEdges = m_deleteFaces = 0;
    m_garbage = false;
}

//bool read_mesh(Surface_mesh& mesh, const std::string& filename)
//{
//    std::setlocale(LC_NUMERIC, "C");

//    // clear mesh before reading from file
//    mesh.clear();

//    // extract file extension
//    std::string::size_type dot(filename.rfind("."));
//    if (dot == std::string::npos) return false;
//    std::string ext = filename.substr(dot+1, filename.length()-dot-1);
//    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

//    // extension determines reader
//    if (ext == "off")
//    {
//        return read_off(mesh, filename);
//    }
//    else if (ext == "obj")
//    {
//        return read_obj(mesh, filename);
//    }
//    else if (ext == "stl")
//    {
//        return read_stl(mesh, filename);
//    }

//    // we didn't find a reader module
//    return false;
//}

//bool write_mesh(const Surface_mesh& mesh, const std::string& filename)
//{
//    // extract file extension
//    std::string::size_type dot(filename.rfind("."));
//    if (dot == std::string::npos) return false;
//    std::string ext = filename.substr(dot+1, filename.length()-dot-1);
//    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);


//    // extension determines reader
//    if (ext == "off")
//    {
//        return write_off(mesh, filename);
//    }
//    else if(ext=="obj")
//    {
//        return write_obj(mesh, filename);
//    }

//    // we didn't find a writer module
//    return false;
//}


//bool read_obj(Surface_mesh& mesh, const std::string& filename)
//{
//    char   s[200];
//    float  x, y, z;
//    std::vector<Surface_mesh::Vertex>  vertices;
//    std::vector<Texture_coordinate> all_tex_coords;   //individual texture coordinates
//    std::vector<int> halfedge_tex_idx; //texture coordinates sorted for halfedges
//    Surface_mesh::Halfedge_property <Texture_coordinate> tex_coords = mesh.halfedge_property<Texture_coordinate>("h:texcoord");
//    bool with_tex_coord=false;

//    // clear mesh
//    mesh.clear();


//    // open file (in ASCII mode)
//    FILE* in = fopen(filename.c_str(), "r");
//    if (!in) return false;


//    // clear line once
//    memset(&s, 0, 200);


//    // parse line by line (currently only supports vertex positions & faces
//    while(in && !feof(in) && fgets(s, 200, in))
//    {
//        // comment
//        if (s[0] == '#' || isspace(s[0])) continue;

//        // vertex
//        else if (strncmp(s, "v ", 2) == 0)
//        {
//            if (sscanf(s, "v %f %f %f", &x, &y, &z))
//            {
//                mesh.add_vertex(Point(x,y,z));
//            }
//        }
//        // normal
//        else if (strncmp(s, "vn ", 3) == 0)
//        {
//            if (sscanf(s, "vn %f %f %f", &x, &y, &z))
//            {
//                // problematic as it can be either a vertex property when interpolated
//                // or a halfedge property for hard edges
//            }
//        }

//        // texture coordinate
//        else if (strncmp(s, "vt ", 3) == 0)
//        {
//            if (sscanf(s, "vt %f %f", &x, &y))
//            {
//                z=1;
//                all_tex_coords.push_back(Texture_coordinate(x,y,z));
//            }
//        }

//        // face
//        else if (strncmp(s, "f ", 2) == 0)
//        {
//            int component(0), nV(0);
//            bool endOfVertex(false);
//            char *p0, *p1(s+1);

//            vertices.clear();
//            halfedge_tex_idx.clear();

//            // skip white-spaces
//            while (*p1==' ') ++p1;

//            while (p1)
//            {
//                p0 = p1;

//                // overwrite next separator

//                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
//                while (*p1!='/' && *p1!='\r' && *p1!='\n' && *p1!=' ' && *p1!='\0') ++p1;

//                // detect end of vertex
//                if (*p1 != '/')
//                {
//                    endOfVertex = true;
//                }

//                // replace separator by '\0'
//                if (*p1 != '\0')
//                {
//                    *p1 = '\0';
//                    p1++; // point to next token
//                }

//                // detect end of line and break
//                if (*p1 == '\0' || *p1 == '\n')
//                {
//                    p1 = 0;
//                }

//                // read next vertex component
//                if (*p0 != '\0')
//                {
//                    switch (component)
//                    {
//                    case 0: // vertex
//                        {
//                            vertices.push_back( Surface_mesh::Vertex(atoi(p0) - 1) );
//                            break;
//                        }
//                    case 1: // texture coord
//                        {
//                            int idx = atoi(p0)-1;
//                            halfedge_tex_idx.push_back(idx);
//                            with_tex_coord=true;
//                            break;
//                        }
//                    case 2: // normal
//                        break;
//                    }
//                }

//                ++component;

//                if (endOfVertex)
//                {
//                    component = 0;
//                    nV++;
//                    endOfVertex = false;
//                }
//            }

//            Surface_mesh::Face f=mesh.add_face(vertices);


//            // add texture coordinates
//            if(with_tex_coord)
//            {
//                Surface_mesh::Halfedge_around_face_circulator h_fit = mesh.halfedges(f);
//                Surface_mesh::Halfedge_around_face_circulator h_end = h_fit;
//                unsigned v_idx =0;
//                do
//                {
//                    tex_coords[*h_fit]=all_tex_coords.at(halfedge_tex_idx.at(v_idx));
//                    ++v_idx;
//                    ++h_fit;
//                }
//                while(h_fit!=h_end);
//            }
//        }
//        // clear line
//        memset(&s, 0, 200);
//    }

//    fclose(in);
//    return true;
//}


////-----------------------------------------------------------------------------


//bool write_obj(const Surface_mesh& mesh, const std::string& filename)
//{
//    FILE* out = fopen(filename.c_str(), "w");
//    if (!out)
//        return false;

//    // comment
//    fprintf(out, "# OBJ export from Surface_mesh\n");

//    //vertices
//    Surface_mesh::Vertex_property<Point> points = mesh.get_vertex_property<Point>("v:point");
//    for (Surface_mesh::Vertex_iterator vit=mesh.vertices_begin(); vit!=mesh.vertices_end(); ++vit)
//    {
//        const Point& p = points[*vit];
//        fprintf(out, "v %.10f %.10f %.10f\n", p[0], p[1], p[2]);
//    }

//    //normals
//    Surface_mesh::Vertex_property<Point> normals = mesh.get_vertex_property<Point>("v:normal");
//    for (Surface_mesh::Vertex_iterator vit=mesh.vertices_begin(); vit!=mesh.vertices_end(); ++vit)
//    {
//        const Point& p = normals[*vit];
//        fprintf(out, "vn %.10f %.10f %.10f\n", p[0], p[1], p[2]);
//    }

//    //optionally texture coordinates
//    // do we have them?
//    std::vector<std::string> h_props= mesh.halfedge_properties();
//    bool with_tex_coord = false;
//    std::vector<std::string>::iterator h_prop_end = h_props.end();
//    std::vector<std::string>::iterator h_prop_start= h_props.begin();
//    while(h_prop_start!=h_prop_end)
//    {
//        if(0==(*h_prop_start).compare("h:texcoord"))
//        {
//            with_tex_coord=true;
//        }
//        ++h_prop_start;
//    }

//    //if so then add
//    if(with_tex_coord)
//    {
//        Surface_mesh::Halfedge_property<Texture_coordinate> tex_coord = mesh.get_halfedge_property<Texture_coordinate>("h:texcoord");
//        for (Surface_mesh::Halfedge_iterator hit=mesh.halfedges_begin(); hit!=mesh.halfedges_end(); ++hit)
//        {
//            const Texture_coordinate& pt = tex_coord[*hit];
//            fprintf(out, "vt %.10f %.10f %.10f\n", pt[0], pt[1], pt[2]);
//        }
//    }

//    //faces
//    for (Surface_mesh::Face_iterator fit=mesh.faces_begin(); fit!=mesh.faces_end(); ++fit)
//    {
//        fprintf(out, "f");
//        Surface_mesh::Vertex_around_face_circulator fvit=mesh.vertices(*fit), fvend=fvit;
//        Surface_mesh::Halfedge_around_face_circulator fhit=mesh.halfedges(*fit);
//        do
//        {
//            if(with_tex_coord)
//            {
//                // write vertex index, tex_coord index and normal index
//                fprintf(out, " %d/%d/%d", (*fvit).idx()+1, (*fhit).idx()+1, (*fvit).idx()+1);
//                ++fhit;
//            }
//            else
//            {
//                // write vertex index and normal index
//                fprintf(out, " %d//%d", (*fvit).idx()+1, (*fvit).idx()+1);
//            }
//        }
//        while (++fvit != fvend);
//        fprintf(out, "\n");
//    }

//    fclose(out);
//    return true;
//}

//// helper function
//template <typename T> inline void readOff(FILE* in, T& t)
//{
//    int err = 0;
//    err = fread(&t, 1, sizeof(t), in);
//}


////-----------------------------------------------------------------------------


//inline bool read_off_ascii(Surface_mesh& mesh,
//                    FILE* in,
//                    const bool has_normals,
//                    const bool has_texcoords,
//                    const bool has_colors)
//{
//    char                 line[200], *lp;
//    int                  nc;
//    unsigned int         i, j, items, idx;
//    unsigned int         nV, nF, nE;
//    Vec3f                p, n, c;
//    Vec2f                t;
//    Surface_mesh::Vertex v;


//    // properties
//    Surface_mesh::Vertex_property<Normal>              normals;
//    Surface_mesh::Vertex_property<Texture_coordinate>  texcoords;
//    Surface_mesh::Vertex_property<Color>               colors;
//    if (has_normals)   normals   = mesh.vertex_property<Normal>("v:normal");
//    if (has_texcoords) texcoords = mesh.vertex_property<Texture_coordinate>("v:texcoord");
//    if (has_colors)    colors    = mesh.vertex_property<Color>("v:color");


//    // #Vertice, #Faces, #Edges
//    items = fscanf(in, "%d %d %d\n", (int*)&nV, (int*)&nF, (int*)&nE);
//    mesh.clear();
//    mesh.reserve(nV, std::max(3*nV, nE), nF);


//    // read vertices: pos [normal] [color] [texcoord]
//    for (i=0; i<nV && !feof(in); ++i)
//    {
//        // read line
//        lp = fgets(line, 200, in);
//        lp = line;

//        // position
//        items = sscanf(lp, "%f %f %f%n", &p[0], &p[1], &p[2], &nc);
//        assert(items==3);
//        v = mesh.add_vertex((Point)p);
//        lp += nc;

//        // normal
//        if (has_normals)
//        {
//            if (sscanf(lp, "%f %f %f%n", &n[0], &n[1], &n[2], &nc) == 3)
//            {
//                normals[v] = n;
//            }
//            lp += nc;
//        }

//        // color
//        if (has_colors)
//        {
//            if (sscanf(lp, "%f %f %f%n", &c[0], &c[1], &c[2], &nc) == 3)
//            {
//                if (c[0]>1.0f || c[1]>1.0f || c[2]>1.0f) c *= (1.0/255.0);
//                colors[v] = c;
//            }
//            lp += nc;
//        }

//        // tex coord
//        if (has_texcoords)
//        {
//            items = sscanf(lp, "%f %f%n", &t[0], &t[1], &nc);
//            assert(items == 2);
//            texcoords[v][0] = t[0];
//            texcoords[v][1] = t[1];
//            lp += nc;
//        }
//    }



//    // read faces: #N v[1] v[2] ... v[n-1]
//    std::vector<Surface_mesh::Vertex> vertices;
//    for (i=0; i<nF; ++i)
//    {
//        // read line
//        lp = fgets(line, 200, in);
//        lp = line;

//        // #vertices
//        items = sscanf(lp, "%d%n", (int*)&nV, &nc);
//        assert(items == 1);
//        vertices.resize(nV);
//        lp += nc;

//        // indices
//        for (j=0; j<nV; ++j)
//        {
//            items = sscanf(lp, "%d%n", (int*)&idx, &nc);
//            assert(items == 1);
//            vertices[j] = Surface_mesh::Vertex(idx);
//            lp += nc;
//        }
//        mesh.add_face(vertices);
//    }


//    return true;
//}


////-----------------------------------------------------------------------------


//inline bool read_off_binary(Surface_mesh& mesh,
//                     FILE* in,
//                     const bool has_normals,
//                     const bool has_texcoords,
//                     const bool has_colors)
//{
//    unsigned int       i, j, idx;
//    unsigned int       nV, nF, nE;
//    Vec3f              p, n, c;
//    Vec2f              t;
//    Surface_mesh::Vertex  v;


//    // binary cannot (yet) read colors
//    if (has_colors) return false;


//    // properties
//    Surface_mesh::Vertex_property<Normal>              normals;
//    Surface_mesh::Vertex_property<Texture_coordinate>  texcoords;
//    if (has_normals)   normals   = mesh.vertex_property<Normal>("v:normal");
//    if (has_texcoords) texcoords = mesh.vertex_property<Texture_coordinate>("v:texcoord");


//    // #Vertice, #Faces, #Edges
//    readOff(in, nV);
//    readOff(in, nF);
//    readOff(in, nE);
//    mesh.clear();
//    mesh.reserve(nV, std::max(3*nV, nE), nF);


//    // read vertices: pos [normal] [color] [texcoord]
//    for (i=0; i<nV && !feof(in); ++i)
//    {
//        // position
//        readOff(in, p);
//        v = mesh.add_vertex((Point)p);

//        // normal
//        if (has_normals)
//        {
//            readOff(in, n);
//            normals[v] = n;
//        }

//        // tex coord
//        if (has_texcoords)
//        {
//            readOff(in, t);
//            texcoords[v][0] = t[0];
//            texcoords[v][1] = t[1];
//        }
//    }


//    // read faces: #N v[1] v[2] ... v[n-1]
//    std::vector<Surface_mesh::Vertex> vertices;
//    for (i=0; i<nF; ++i)
//    {
//        readOff(in, nV);
//        vertices.resize(nV);
//        for (j=0; j<nV; ++j)
//        {
//            readOff(in, idx);
//            vertices[j] = Surface_mesh::Vertex(idx);
//        }
//        mesh.add_face(vertices);
//    }


//    return true;
//}


////-----------------------------------------------------------------------------


//bool read_off(Surface_mesh& mesh, const std::string& filename)
//{
//    char  line[200];
//    bool  has_texcoords = false;
//    bool  has_normals   = false;
//    bool  has_colors    = false;
//    bool  has_hcoords   = false;
//    bool  has_dim       = false;
//    bool  is_binary     = false;


//    // open file (in ASCII mode)
//    FILE* in = fopen(filename.c_str(), "r");
//    if (!in) return false;


//    // read header: [ST][C][N][4][n]OFF BINARY
//    char *c = fgets(line, 200, in);
//    assert(c != NULL);
//    c = line;
//    if (c[0] == 'S' && c[1] == 'T') { has_texcoords = true; c += 2; }
//    if (c[0] == 'C') { has_colors  = true; ++c; }
//    if (c[0] == 'N') { has_normals = true; ++c; }
//    if (c[0] == '4') { has_hcoords = true; ++c; }
//    if (c[0] == 'n') { has_dim     = true; ++c; }
//    if (strncmp(c, "OFF", 3) != 0) { fclose(in); return false; } // no OFF
//    if (strncmp(c+4, "BINARY", 6) == 0) is_binary = true;


//    // homogeneous coords, and vertex dimension != 3 are not supported
//    if (has_hcoords || has_dim)
//    {
//        fclose(in);
//        return false;
//    }


//    // if binary: reopen file in binary mode
//    if (is_binary)
//    {
//        fclose(in);
//        in = fopen(filename.c_str(), "rb");
//        c = fgets(line, 200, in);
//        assert(c != NULL);
//    }


//    // read as ASCII or binary
//    bool ok = (is_binary ?
//        read_off_binary(mesh, in, has_normals, has_texcoords, has_colors) :
//        read_off_ascii(mesh, in, has_normals, has_texcoords, has_colors));


//    fclose(in);
//    return ok;
//}


////-----------------------------------------------------------------------------


//bool write_off(const Surface_mesh& mesh, const std::string& filename)
//{
//    FILE* out = fopen(filename.c_str(), "w");
//    if (!out)
//        return false;


//    bool  has_normals   = false;
//    bool  has_texcoords = false;
//    Surface_mesh::Vertex_property<Normal> normals = mesh.get_vertex_property<Normal>("v:normal");
//    Surface_mesh::Vertex_property<Texture_coordinate>  texcoords = mesh.get_vertex_property<Texture_coordinate>("v:texcoord");
//    if (normals)   has_normals = true;
//    if (texcoords) has_texcoords = true;


//    // header
//    if(has_texcoords)
//        fprintf(out, "ST");
//    if(has_normals)
//        fprintf(out, "N");
//    fprintf(out, "OFF\n%d %d 0\n", mesh.n_vertices(), mesh.n_faces());


//    // vertices, and optionally normals and texture coordinates
//    Surface_mesh::Vertex_property<Point> points = mesh.get_vertex_property<Point>("v:point");
//    for (Surface_mesh::Vertex_iterator vit=mesh.vertices_begin(); vit!=mesh.vertices_end(); ++vit)
//    {
//        const Point& p = points[*vit];
//        fprintf(out, "%.10f %.10f %.10f", p[0], p[1], p[2]);

//        if (has_normals)
//        {
//            const Normal& n = normals[*vit];
//            fprintf(out, " %.10f %.10f %.10f", n[0], n[1], n[2]);
//        }

//        if (has_texcoords)
//        {
//            const Texture_coordinate& t = texcoords[*vit];
//            fprintf(out, "% .10f %.10f", t[0], t[1]);
//        }

//        fprintf(out, "\n");
//    }


//    // faces
//    for (Surface_mesh::Face_iterator fit=mesh.faces_begin(); fit!=mesh.faces_end(); ++fit)
//    {
//        int nV = mesh.valence(*fit);
//        fprintf(out, "%d", nV);
//        Surface_mesh::Vertex_around_face_circulator fvit=mesh.vertices(*fit), fvend=fvit;
//        do
//        {
//            fprintf(out, " %d", (*fvit).idx());
//        }
//        while (++fvit != fvend);
//        fprintf(out, "\n");
//    }

//    fclose(out);
//    return true;
//}

//// helper function
//template <typename T> inline size_t readPoly(FILE* in, T& t)
//{
//    return fread((char*)&t, 1, sizeof(t), in);
//}


////-----------------------------------------------------------------------------


//bool read_poly(Surface_mesh& mesh, const std::string& filename)
//{
//    unsigned int n_items;

//    // open file (in binary mode)
//    FILE* in = fopen(filename.c_str(), "rb");
//    if (!in) return false;


//    // clear mesh
//    mesh.clear();


//    // how many elements?
//    unsigned int nv, ne, nh, nf;
//    readPoly(in, nv);
//    readPoly(in, ne);
//    readPoly(in, nf);
//    nh = 2*ne;


//    // resize containers
//    mesh.vprops_.resize(nv);
//    mesh.hprops_.resize(nh);
//    mesh.eprops_.resize(ne);
//    mesh.fprops_.resize(nf);


//    // get properties
//    Surface_mesh::Vertex_property<Surface_mesh::Vertex_connectivity>      vconn = mesh.vertex_property<Surface_mesh::Vertex_connectivity>("v:connectivity");
//    Surface_mesh::Halfedge_property<Surface_mesh::Halfedge_connectivity>  hconn = mesh.halfedge_property<Surface_mesh::Halfedge_connectivity>("h:connectivity");
//    Surface_mesh::Face_property<Surface_mesh::Face_connectivity>          fconn = mesh.face_property<Surface_mesh::Face_connectivity>("f:connectivity");
//    Surface_mesh::Vertex_property<Point>                                  point = mesh.vertex_property<Point>("v:point");

//    // read properties from file
//    n_items = fread((char*)vconn.data(), sizeof(Surface_mesh::Vertex_connectivity),   nv, in);
//    n_items = fread((char*)hconn.data(), sizeof(Surface_mesh::Halfedge_connectivity), nh, in);
//    n_items = fread((char*)fconn.data(), sizeof(Surface_mesh::Face_connectivity),     nf, in);
//    n_items = fread((char*)point.data(), sizeof(Point),                               nv, in);

//    fclose(in);
//    return true;
//}

//// helper function
//template <typename T> inline void readStl(FILE* in, T& t)
//{
//    size_t n_items(0);
//    n_items = fread((char*)&t, 1, sizeof(t), in);
//    assert(n_items > 0);
//}


////-----------------------------------------------------------------------------


//// helper class for STL reader
//class CmpVec
//{
//public:

//    CmpVec(float _eps=FLT_MIN) : eps_(_eps) {}

//    bool operator()(const Vec3f& v0, const Vec3f& v1) const
//    {
//        if (fabs(v0[0] - v1[0]) <= eps_)
//        {
//            if (fabs(v0[1] - v1[1]) <= eps_)
//            {
//                return (v0[2] < v1[2] - eps_);
//            }
//            else return (v0[1] < v1[1] - eps_);
//        }
//        else return (v0[0] < v1[0] - eps_);
//    }

//private:
//    float eps_;
//};


////-----------------------------------------------------------------------------


//bool read_stl(Surface_mesh& mesh, const std::string& filename)
//{
//    char                            line[100], *c;
//    unsigned int                    i, nT;
//    Vec3f                           p;
//    Surface_mesh::Vertex               v;
//    std::vector<Surface_mesh::Vertex>  vertices(3);
//    size_t n_items(0);

//    CmpVec comp(FLT_MIN);
//    std::map<Vec3f, Surface_mesh::Vertex, CmpVec>            vMap(comp);
//    std::map<Vec3f, Surface_mesh::Vertex, CmpVec>::iterator  vMapIt;


//    // clear mesh
//    mesh.clear();


//    // open file (in ASCII mode)
//    FILE* in = fopen(filename.c_str(), "r");
//    if (!in) return false;


//    // ASCII or binary STL?
//    c = fgets(line, 6, in);
//    assert(c != NULL);
//    const bool binary = ((strncmp(line, "SOLID", 5) != 0) &&
//        (strncmp(line, "solid", 5) != 0));


//    // parse binary STL
//    if (binary)
//    {
//        // re-open file in binary mode
//        fclose(in);
//        in = fopen(filename.c_str(), "rb");
//        if (!in) return false;

//        // skip dummy header
//        n_items = fread(line, 1, 80, in);
//        assert(n_items > 0);

//        // read number of triangles
//        readStl(in, nT);

//        // read triangles
//        while (nT)
//        {
//            // skip triangle normal
//            n_items = fread(line, 1, 12, in);
//            assert(n_items > 0);
//            // triangle's vertices
//            for (i=0; i<3; ++i)
//            {
//                readStl(in, p);

//                // has vector been referenced before?
//                if ((vMapIt=vMap.find(p)) == vMap.end())
//                {
//                    // No : add vertex and remember idx/vector mapping
//                    v = mesh.add_vertex((Point)p);
//                    vertices[i] = v;
//                    vMap[p] = v;
//                }
//                else
//                {
//                    // Yes : get index from map
//                    vertices[i] = vMapIt->second;
//                }
//            }

//            // Add face only if it is not degenerated
//            if ((vertices[0] != vertices[1]) &&
//                (vertices[0] != vertices[2]) &&
//                (vertices[1] != vertices[2]))
//                mesh.add_face(vertices);

//            n_items = fread(line, 1, 2, in);
//            assert(n_items > 0);
//            --nT;
//        }
//    }


//    // parse ASCII STL
//    else
//    {
//        // parse line by line
//        while (in && !feof(in) && fgets(line, 100, in))
//        {
//            // skip white-space
//            for (c=line; isspace(*c) && *c!='\0'; ++c) {};

//            // face begins
//            if ((strncmp(c, "outer", 5) == 0) ||
//                (strncmp(c, "OUTER", 5) == 0))
//            {
//                // read three vertices
//                for (i=0; i<3; ++i)
//                {
//                    // read line
//                    c = fgets(line, 100, in);
//                    assert(c != NULL);

//                    // skip white-space
//                    for (c=line; isspace(*c) && *c!='\0'; ++c) {};

//                    // read x, y, z
//                    sscanf(c+6, "%f %f %f", &p[0], &p[1], &p[2]);

//                    // has vector been referenced before?
//                    if ((vMapIt=vMap.find(p)) == vMap.end())
//                    {
//                        // No : add vertex and remember idx/vector mapping
//                        v = mesh.add_vertex((Point)p);
//                        vertices[i] = v;
//                        vMap[p] = v;
//                    }
//                    else
//                    {
//                        // Yes : get index from map
//                        vertices[i] = vMapIt->second;
//                    }
//                }

//                // Add face only if it is not degenerated
//                if ((vertices[0] != vertices[1]) &&
//                    (vertices[0] != vertices[2]) &&
//                    (vertices[1] != vertices[2]))
//                    mesh.add_face(vertices);
//            }
//        }
//    }


//    fclose(in);
//    return true;
//}

//=============================================================================
} // namespace Patate
//=============================================================================
#endif // SURFACE_MESH_H
//=============================================================================
