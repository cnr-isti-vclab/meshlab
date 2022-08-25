/****************************************************************************
**
** Copyright (c) 2008-2020 C.B. Barber. All rights reserved.
** $Id: //main/2019/qhull/src/libqhullcpp/QhullUser.h#7 $$Change: 3010 $
** $DateTime: 2020/07/30 22:14:11 $$Author: bbarber $
**
****************************************************************************/

#ifndef QhullUser_H
#define QhullUser_H

#include "libqhull_r/qhull_ra.h"
#include "libqhullcpp/QhullPoint.h"
#include "libqhullcpp/PointCoordinates.h"

#include <stdarg.h>
#include <string>
#include <vector>
#include <istream>
#include <ostream>
#include <sstream>

namespace orgQhull{

#//!\name Defined here
    //! QhullUser -- custom C++ interfaces into Qhull (via qh_fprintf)
    class QhullUser;
    // qh_fprintf is defined below.  It replaces libqhull_r/userprintf_r.c

#//!\name Used here
    class QhullQh;

    class QhullUser{

    private:
#//!\name Fields
        QhullQh *       qh_qh;          //!< QhullQh/qhT for access to libqhull_r
        void *          previous_user;  //!< previous qh.cpp_user, restored on deconstruction
        std::vector<std::vector<double> > doubles_vector;  //! vectors for capturing ints and doubles
        std::vector<std::vector<int> >  ints_vector;
        std::vector<int>                fprintf_ints;
        std::vector<double>             fprintf_doubles;
        std::vector<int>                fprintf_codes;
        std::vector<std::string>        fprintf_strings;
        int             num_facets;
        int             num_neighbors;
        int             num_numbers;
        int             num_points;
        int             num_results;
        int             num_ridges;
        int             num_vectors;
        int             num_vertices;
        int             qhull_dim;
        int             delaunay_dim;   //! ints for capturing fprintf fields

    public:
#//!\name Construct
                        QhullUser(QhullQh *qqh);
                        ~QhullUser();
    private:                // Disable default constructor, copy constructor, and assignment
                        QhullUser();
                        QhullUser(const QhullUser &);
        QhullUser &     operator=(const QhullUser &);
    private:

    public:
#//!\name GetSet
        void            appendCode(int msgCode) { fprintf_codes.push_back(msgCode); }
        void            appendDouble(double a) { fprintf_doubles.push_back(a); }
        void            appendInt(int i) { fprintf_ints.push_back(i); }
        void            appendAndClearDoubles() { doubles_vector.push_back(fprintf_doubles); fprintf_doubles.clear(); }
        void            appendAndClearInts() { ints_vector.push_back(fprintf_ints); fprintf_ints.clear(); }
        void            clear();
        void            clearDoubles() { fprintf_doubles.clear(); }
        void            clearDoublesVector() { doubles_vector.clear(); }
        void            clearInts() { fprintf_ints.clear(); }
        void            clearIntsVector() { ints_vector.clear(); }
        const std::vector<int> &codes() const { return fprintf_codes; }
        int             delaunayDim() const { return delaunay_dim; }
        const std::vector<std::vector<double> > &doublesVector() const { return doubles_vector; }
        const std::vector<double> &doubles() const { return fprintf_doubles; }
        int             firstCode() const { return (fprintf_codes.size() == 0 ? -1 : fprintf_codes[0]); }
        const std::vector<int> &ints() const { return fprintf_ints; }
        const std::vector<std::vector<int> > &intsVector() const { return ints_vector; }
        int             numDoubles() const { return (int)doubles_vector.size();  }
        int             numFacets() const { return num_facets; }
        int             numInts() const { return (int)ints_vector.size(); }
        int             numNeighbors() const { return num_neighbors; }
        int             numNumbers() const { return num_numbers; }
        int             numPoints() const { return num_points; }
        int             numResults() const { return num_results; }
        int             numRidges() const { return num_ridges; }
        int             numVectors() const { return num_vectors; }
        int             numVertices() const { return num_vertices; }
        QhullQh *       qh() const { return qh_qh; }
        int             qhullDim() const { return qhull_dim; }
        void            setDelaunayDim(int i) { delaunay_dim= i; }
        void            setNumFacets(int i) { num_facets= i; }
        void            setNumResults(int i) { num_results= i; }
        void            setNumRidges(int i) { num_ridges= i; }
        void            setNumNeighbors(int i) { num_neighbors= i; }
        void            setNumNumbers(int i) { num_numbers= i; }
        void            setNumPoints(int i) { num_points= i; }
        void            setNumVectors(int i) { num_vectors= i; }
        void            setNumVertices(int i) { num_vertices= i; }
        void            setQhullDim(int i) { qhull_dim= i; }

#//!\name Methods
        void            captureOn();
        void            captureOff();
};//class QhullUser

}//namespace orgQhull

#endif // QhullUser_H
