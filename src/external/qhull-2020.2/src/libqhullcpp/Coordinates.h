/****************************************************************************
**
** Copyright (c) 2009-2020 C.B. Barber. All rights reserved.
** $Id: //main/2019/qhull/src/libqhullcpp/Coordinates.h#5 $$Change: 3001 $
** $DateTime: 2020/07/24 20:43:28 $$Author: bbarber $
**
****************************************************************************/

#ifndef QHCOORDINATES_H
#define QHCOORDINATES_H

#include "libqhull_r/qhull_ra.h"
#include "libqhullcpp/QhullError.h"

#include <cstddef> // ptrdiff_t, size_t
#include <ostream>
// Requires STL vector class.  Can use with another vector class such as QList.
#include <vector>

namespace orgQhull {

#//!\name Defined here
    //! An std::vector of point coordinates independent of dimension
    //! Used by PointCoordinates for RboxPoints and by Qhull for feasiblePoint
    //! A QhullPoint refers to previously allocated coordinates
    class Coordinates;

    //! Java-style iterators are not implemented for Coordinates.  std::vector has an expensive copy constructor and copy assignment.
    //!    A pointer to Coordinates is vulnerable to mysterious overwrites (e.g., deleting a returned value and reusing its memory)
    //! Qt's 'foreach' should not be used.  It makes a copy of the std::vector

class Coordinates {

private:
#//!\name Fields
    std::vector<coordT> coordinate_array;

public:
#//!\name Subtypes

    class const_iterator;
    class iterator;
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;

    typedef coordT              value_type;
    typedef const value_type   *const_pointer;
    typedef const value_type &  const_reference;
    typedef value_type *        pointer;
    typedef value_type &        reference;
    typedef ptrdiff_t           difference_type;
    typedef countT              size_type;

#//!\name Construct
                        Coordinates() : coordinate_array() {}
    explicit            Coordinates(const std::vector<coordT> &other) : coordinate_array(other) {}
                        Coordinates(const Coordinates &other) : coordinate_array(other.coordinate_array) {}
    Coordinates &       operator=(const Coordinates &other) { coordinate_array= other.coordinate_array; return *this; }
    Coordinates &       operator=(const std::vector<coordT> &other) { coordinate_array= other; return *this; }
                        ~Coordinates() {}

#//!\name Conversion

#ifndef QHULL_NO_STL
    std::vector<coordT> toStdVector() const { return coordinate_array; }
#endif //QHULL_NO_STL
#ifdef QHULL_USES_QT
    QList<coordT>       toQList() const;
#endif //QHULL_USES_QT

#//!\name GetSet
    countT              count() const { return static_cast<countT>(size()); }
    coordT *            data() { return (isEmpty() ? NULL : &at(0)); }
    const coordT *      data() const { return (isEmpty() ? NULL : &at(0)); }
    bool                isEmpty() const { return coordinate_array.empty(); }
    bool                operator==(const Coordinates &other) const  { return coordinate_array==other.coordinate_array; }
    bool                operator!=(const Coordinates &other) const  { return coordinate_array!=other.coordinate_array; }
    size_t              size() const { return coordinate_array.size(); }

#//!\name Element access
    coordT &            at(countT idx) { return coordinate_array.at(idx); }
    const coordT &      at(countT idx) const { return coordinate_array.at(idx); }
    coordT &            back() { return coordinate_array.back(); }
    const coordT &      back() const { return coordinate_array.back(); }
    coordT &            first() { return front(); }
    const coordT &      first() const { return front(); }
    coordT &            front() { return coordinate_array.front(); }
    const coordT &      front() const { return coordinate_array.front(); }
    coordT &            last() { return back(); }
    const coordT &      last() const { return back(); }
    Coordinates         mid(countT idx, countT length= -1) const; //!<\todo countT -1 indicates
    coordT &            operator[](countT idx) { return coordinate_array.operator[](idx); }
    const coordT &      operator[](countT idx) const { return coordinate_array.operator[](idx); }
    coordT              value(countT idx, const coordT &defaultValue) const;

#//!\name Iterator
    iterator            begin() { return iterator(coordinate_array.begin()); }
    const_iterator      begin() const { return const_iterator(coordinate_array.begin()); }
    const_iterator      constBegin() const { return begin(); }
    const_iterator      constEnd() const { return end(); }
    iterator            end() { return iterator(coordinate_array.end()); }
    const_iterator      end() const { return const_iterator(coordinate_array.end()); }

#//!\name GetSet
    Coordinates         operator+(const Coordinates &other) const;

#//!\name Modify
    void                append(int pointDimension, coordT *c);
    void                append(const coordT &c) { push_back(c); }
    void                clear() { coordinate_array.clear(); }
    iterator            erase(iterator idx) { return iterator(coordinate_array.erase(idx.base())); }
    iterator            erase(iterator beginIterator, iterator endIterator) { return iterator(coordinate_array.erase(beginIterator.base(), endIterator.base())); }
    void                insert(countT before, const coordT &c) { insert(begin()+before, c); }
    iterator            insert(iterator before, const coordT &c) { return iterator(coordinate_array.insert(before.base(), c)); }
    void                move(countT from, countT to) { insert(to, takeAt(from)); }
    Coordinates &       operator+=(const Coordinates &other);
    Coordinates &       operator+=(const coordT &c) { append(c); return *this; }
    Coordinates &       operator<<(const Coordinates &other) { return *this += other; }
    Coordinates &       operator<<(const coordT &c) { return *this += c; }
    void                pop_back() { coordinate_array.pop_back(); }
    void                pop_front() { removeFirst(); }
    void                prepend(const coordT &c) { insert(begin(), c); }
    void                push_back(const coordT &c) { coordinate_array.push_back(c); }
    void                push_front(const coordT &c) { insert(begin(), c); }
                        //removeAll below
    void                removeAt(countT idx) { erase(begin()+idx); }
    void                removeFirst() { erase(begin()); }
    void                removeLast() { erase(--end()); }
    void                replace(countT idx, const coordT &c) { (*this)[idx]= c; }
    void                reserve(countT i) { coordinate_array.reserve(i); }
    void                swap(countT idx, countT other);
    coordT              takeAt(countT idx);
    coordT              takeFirst() { return takeAt(0); }
    coordT              takeLast();

#//!\name Search
    bool                contains(const coordT &t) const;
    countT              count(const coordT &t) const;
    countT              indexOf(const coordT &t, countT from= 0) const;
    countT              lastIndexOf(const coordT &t, countT from= -1) const;
    void                removeAll(const coordT &t);

#//!\name Coordinates::iterator -- from QhullPoints, forwarding to coordinate_array
    // before const_iterator for conversion with comparison operators
    // Reviewed corelib/tools/qlist.h and corelib/tools/qvector.h w/o QT_STRICT_ITERATORS
    class iterator {

    private:
        std::vector<coordT>::iterator i;
        friend class const_iterator;

    public:
        typedef std::random_access_iterator_tag  iterator_category;
        typedef coordT      value_type;
        typedef value_type *pointer;
        typedef value_type &reference;
        typedef ptrdiff_t   difference_type;

                        iterator() : i() {}
                        iterator(const iterator &other) : i() { i= other.i; }
        explicit        iterator(const std::vector<coordT>::iterator &vi) : i() { i= vi; }
        iterator &      operator=(const iterator &other) { i= other.i; return *this; }
        std::vector<coordT>::iterator &base() { return i; }
        coordT &        operator*() const { return *i; }
        // No operator->() when the base type is double
        coordT &        operator[](countT idx) const { return i[idx]; }

        bool            operator==(const iterator &other) const { return i==other.i; }
        bool            operator!=(const iterator &other) const { return i!=other.i; }
        bool            operator<(const iterator &other) const { return i<other.i; }
        bool            operator<=(const iterator &other) const { return i<=other.i; }
        bool            operator>(const iterator &other) const { return i>other.i; }
        bool            operator>=(const iterator &other) const { return i>=other.i; }
              // reinterpret_cast to break circular dependency
        bool            operator==(const Coordinates::const_iterator &other) const { return *this==reinterpret_cast<const iterator &>(other); }
        bool            operator!=(const Coordinates::const_iterator &other) const { return *this!=reinterpret_cast<const iterator &>(other); }
        bool            operator<(const Coordinates::const_iterator &other) const { return *this<reinterpret_cast<const iterator &>(other); }
        bool            operator<=(const Coordinates::const_iterator &other) const { return *this<=reinterpret_cast<const iterator &>(other); }
        bool            operator>(const Coordinates::const_iterator &other) const { return *this>reinterpret_cast<const iterator &>(other); }
        bool            operator>=(const Coordinates::const_iterator &other) const { return *this>=reinterpret_cast<const iterator &>(other); }

        iterator &      operator++() { ++i; return *this; }
        iterator        operator++(int) { return iterator(i++); }
        iterator &      operator--() { --i; return *this; }
        iterator        operator--(int) { return iterator(i--); }
        iterator &      operator+=(countT idx) { i += idx; return *this; }
        iterator &      operator-=(countT idx) { i -= idx; return *this; }
        iterator        operator+(countT idx) const { return iterator(i+idx); }
        iterator        operator-(countT idx) const { return iterator(i-idx); }
        difference_type operator-(iterator other) const { return i-other.i; }
    };//Coordinates::iterator

#//!\name Coordinates::const_iterator
    class const_iterator {

    private:
        std::vector<coordT>::const_iterator i;

    public:
        typedef std::random_access_iterator_tag  iterator_category;
        typedef coordT            value_type;
        typedef const value_type *pointer;
        typedef const value_type &reference;
        typedef ptrdiff_t         difference_type;

                        const_iterator() : i() {}
                        const_iterator(const const_iterator &other) : i() { i= other.i; }
                        const_iterator(const iterator &o) : i(o.i) {}
        explicit        const_iterator(const std::vector<coordT>::const_iterator &vi) : i() { i= vi; }
        const_iterator &operator=(const const_iterator &other) { i= other.i; return *this; }
        const coordT &  operator*() const { return *i; }
        // No operator->() when the base type is double
        const coordT &  operator[](countT idx) const { return i[idx]; }

        bool            operator==(const const_iterator &other) const { return i==other.i; }
        bool            operator!=(const const_iterator &other) const { return i!=other.i; }
        bool            operator<(const const_iterator &other) const { return i<other.i; }
        bool            operator<=(const const_iterator &other) const { return i<=other.i; }
        bool            operator>(const const_iterator &other) const { return i>other.i; }
        bool            operator>=(const const_iterator &other) const { return i>=other.i; }

        const_iterator & operator++() { ++i; return *this; }
        const_iterator  operator++(int) { return const_iterator(i++); }
        const_iterator & operator--() { --i; return *this; }
        const_iterator  operator--(int) { return const_iterator(i--); }
        const_iterator & operator+=(countT idx) { i += idx; return *this; }
        const_iterator & operator-=(countT idx) { i -= idx; return *this; }
        const_iterator  operator+(countT idx) const { return const_iterator(i+idx); }
        const_iterator  operator-(countT idx) const { return const_iterator(i-idx); }
        difference_type operator-(const_iterator other) const { return i-other.i; }
    };//Coordinates::const_iterator

};//Coordinates

}//namespace orgQhull

#//!\name Global

std::ostream &operator<<(std::ostream &os, const orgQhull::Coordinates &c);

#endif // QHCOORDINATES_H
