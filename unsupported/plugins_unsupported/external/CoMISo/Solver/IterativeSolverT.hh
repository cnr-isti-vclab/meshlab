//=============================================================================
//
//  CLASS IterativeSolverT
//
//=============================================================================


#ifndef COMISO_ITERATIVESOLVERT_HH
#define COMISO_ITERATIVESOLVERT_HH


//== INCLUDES =================================================================

#include <gmm/gmm.h>
#include <deque>
#include <queue>
#include <set>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================



	      
/** \class IterativeSolverT IterativeSolverT.hh <COMISO/.../IterativeSolverT.hh>

    Brief Description.
  
    A more elaborate description follows.
*/

template <class RealT>
class IterativeSolverT
{
public:
  
  typedef RealT Real;
 
  /// Constructor
  IterativeSolverT() {}
 
  /// Destructor
  ~IterativeSolverT() {}


  // local gauss_seidel
  bool gauss_seidel_local( typename gmm::csc_matrix<Real>&  _A,
   			   std::vector<Real>&               _x, 
			   std::vector<Real>&               _rhs, 
  			   std::vector<unsigned int>&       _idxs, 
  			   int&                             _max_iter, 
  			   Real&                            _tolerance );

  // local gauss_seidel
  bool gauss_seidel_local2( typename gmm::csc_matrix<Real>&  _A,
			    std::vector<Real>&               _x, 
			    std::vector<Real>&               _rhs, 
			    std::vector<unsigned int>&       _idxs, 
			    int&                             _max_iter, 
			    Real&                            _tolerance );

  // conjugate gradient
  bool conjugate_gradient( typename gmm::csc_matrix<Real>&  _A,
			   std::vector<Real>&               _x, 
			   std::vector<Real>&               _rhs, 
			   int&                             _max_iter, 
			   Real&                            _tolerance );
  
private:
  // compute relative norm
  Real vect_norm_rel(const std::vector<Real>& _v, const std::vector<Real>& _diag) const;


private:

  // helper for conjugate gradient
  std::vector<Real> p_;
  std::vector<Real> q_;
  std::vector<Real> r_;
  std::vector<Real> d_;

  //  helper for local gauss seidel
  std::vector<unsigned int>  i_temp;
  std::deque<unsigned int> q;
  std::set<int> s;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_ITERATIVESOLVERT_C)
#define COMISO_ITERATIVESOLVERT_TEMPLATES
#include "IterativeSolverT.cc"
#endif
//=============================================================================
#endif // COMISO_ITERATIVESOLVERT_HH defined
//=============================================================================

