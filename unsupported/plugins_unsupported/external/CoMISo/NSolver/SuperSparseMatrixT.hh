//=============================================================================
//
//  CLASS SuperSparseMatrixT
//
//=============================================================================


#ifndef COMISO_SUPERSPARSEMATRIXT_HH
#define COMISO_SUPERSPARSEMATRIXT_HH


//== INCLUDES =================================================================

#include <iostream>
#include <map>
#include <math.h>
#include <Eigen/Dense>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================



	      
/** \class SuperSparseMatrixT SuperSparseMatrixT.hh <COMISO/.../SuperSparseMatrixT.hh>

    Brief Description.
  
    A more elaborate description follows.
*/

template <class VType>
class SuperSparseMatrixT
{
public:

  // value type
  typedef VType                                  VT;
  typedef std::pair<unsigned int, unsigned int> PII;

  // iterate over elements
  class iterator
  {
  public:

    // default constructor
    iterator() {}
    // copy constructor
    iterator(const iterator& _it) { m_it_ = _it.m_it_; }
    // map iterator constructor
    iterator(const typename std::map<PII,VType>::iterator& _m_it) { m_it_ = _m_it; }

    // get reference to data
    VT& operator*() { return m_it_->second; }

    // get row and col of value
    unsigned int row() {return m_it_->first.first; }
    unsigned int col() {return m_it_->first.second; }

    // post-increment
    iterator& operator++()    { ++m_it_; return(*this);}
    iterator  operator++(int) { return iterator(++m_it_); }

    bool operator== (const iterator& _it) { return (m_it_ == _it.m_it_);}
    bool operator!= (const iterator& _it) { return (m_it_ != _it.m_it_);}

    // get raw iterator of map
    typename std::map<PII,VType>::iterator& map_iterator() {return m_it_;}

  private:
    typename std::map<PII,VType>::iterator m_it_;
  };

  /// Constructor
  SuperSparseMatrixT(const int _n_rows = 0, const int _n_cols = 0)
  : n_rows_(_n_rows), n_cols_(_n_cols)
  {}
 
  /// Destructor
  ~SuperSparseMatrixT() {}

  // iterate over non-zeros
  iterator begin() { return iterator(data_.begin()); }
  iterator end()   { return iterator(data_.end());   }
  // erase element
  void     erase( iterator _it) { data_.erase(_it.map_iterator()); }

  // element access
  VT& operator()(const unsigned int _i, const unsigned int _j)
  {  return data_[PII(_i,_j)]; }

  // const element access
  const VT& operator()(const unsigned int _i, const unsigned int _j) const
  {  return data_[PII(_i,_j)]; }

  // get number of stored elements
  unsigned int nonZeros() { return data_.size(); }

  // get dimensions
  unsigned int rows() const { return n_rows_;}
  unsigned int cols() const { return n_cols_;}


  // resize matrix -> delete invalid elements
  void resize(const unsigned int _n_rows, const unsigned int _n_cols)
  {
    n_rows_ = _n_rows;
    n_cols_ = _n_cols;

    // delete out of range elements
    typename std::map<PII, VT>::iterator m_it = data_.begin();
    for(; m_it != data_.end();)
    {
      if(m_it->first.first >=n_rows_ || m_it->first.second >= n_cols_)
        data_.erase(m_it++);
      else
        ++m_it;
    }
  }

  // clear data
  void clear()
  { data_.clear(); }


  // removes all values whose absolut value is smaller than _eps
  void prune(const VT _eps)
  {
    typename std::map<PII, VT>::iterator m_it = data_.begin();
    for(; m_it != data_.end();)
    {
      if(fabs(m_it->second) < _eps)
        data_.erase(m_it++);
      else
        ++m_it;
    }
  }

  // scale matrix by scalar
  void scale(const VT _s)
  {
    typename std::map<PII, VT>::iterator m_it = data_.begin();
    for(; m_it != data_.end(); ++m_it)
      m_it->second *=_s;
  }

  void print()
  {
    iterator it  = begin();
    iterator ite = end();

    std::cerr << "######## SuperSparseMatrix ########" << std::endl;
    std::cerr << "dimension : " << rows()   << " x " << cols() << std::endl;
    std::cerr << "#non-zeros: " << nonZeros() << std::endl;
    for(; it!=ite; ++it)
      std::cerr << "(" << it.row() << "," << it.col() << ") -> " << *it << std::endl;
  }

  void print_eigenvalues()
  {
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(n_rows_, n_cols_);

    iterator it = begin();
    for(; it != end(); ++it)
      A(it.row(),it.col()) = *it;

    Eigen::EigenSolver<Eigen::MatrixXd> eigensolver(A);
    if (eigensolver.info() != Eigen::Success) abort();
    std::cout << "The eigenvalues of A are:\n" << eigensolver.eigenvalues() << std::endl;
//    std::cout << "Here's a matrix whose columns are eigenvectors of A \n"
//    << "corresponding to these eigenvalues:\n"
//    << eigensolver.eigenvectors() << std::endl;
  }

  
private:

  // dimension of matrix
  unsigned int n_rows_;
  unsigned int n_cols_;
  
  typename std::map<PII, VT> data_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_SUPERSPARSEMATRIXT_C)
#define COMISO_SUPERSPARSEMATRIXT_TEMPLATES
#include "SuperSparseMatrixT.cc"
#endif
//=============================================================================
#endif // COMISO_SUPERSPARSEMATRIXT_HH defined
//=============================================================================

