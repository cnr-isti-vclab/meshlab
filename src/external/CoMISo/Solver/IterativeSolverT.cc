//=============================================================================
//
//  CLASS IterativeSolverT - IMPLEMENTATION
//
//=============================================================================

#define COMISO_ITERATIVESOLVERT_C

//== INCLUDES =================================================================

#include "IterativeSolverT.hh"

//== NAMESPACES ===============================================================

namespace COMISO{

//== IMPLEMENTATION ==========================================================

template <class RealT>
bool
IterativeSolverT<RealT>::
gauss_seidel_local( typename gmm::csc_matrix<Real>&  _A,
		    std::vector<Real>&               _x, 
		    std::vector<Real>&               _rhs, 
		    std::vector<unsigned int>&       _idxs, 
		    int&                             _max_iter, 
		    Real&                            _tolerance )
{
  if( _max_iter == 0) return false;

  typedef typename gmm::linalg_traits< gmm::csc_matrix<Real> >::const_sub_col_type ColT;
  typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

  // clear old data
  i_temp.clear();
  q.clear();

  for ( unsigned int i=0; i<_idxs.size(); ++i )
    q.push_back( _idxs[i] );

  int it_count = 0;

  while ( !q.empty() && it_count < _max_iter )
  {
    ++it_count;
    unsigned int cur_i = q.front();
    q.pop_front();
    i_temp.clear();

    ColT col = mat_const_col( _A, cur_i );

    CIter it  = gmm::vect_const_begin( col );
    CIter ite = gmm::vect_const_end( col );

    double res_i   = -_rhs[cur_i];
    double x_i_new = _rhs[cur_i];
    double diag    = 1.0;
    for ( ; it!=ite; ++it )
    {
      res_i   += ( *it ) * _x[it.index()];
      x_i_new -= ( *it ) * _x[it.index()];
      if( it.index() != cur_i)
  	i_temp.push_back( it.index() );
      else
  	diag = *it;
    }

    // take inverse of diag
    diag = 1.0/diag;

    // compare relative residuum normalized by diagonal entry
    if ( fabs(res_i*diag) > _tolerance )
    {
      _x[cur_i] += x_i_new*diag;

      for ( unsigned int j=0; j<i_temp.size(); ++j )
	q.push_back( i_temp[j] );
    }
  }

  // converged?
  return q.empty();
}


//-----------------------------------------------------------------------------


template <class RealT>
bool
IterativeSolverT<RealT>::
gauss_seidel_local2( typename gmm::csc_matrix<Real>&  _A,
		     std::vector<Real>&               _x, 
		     std::vector<Real>&               _rhs, 
		     std::vector<unsigned int>&       _idxs, 
		     int&                             _max_iter, 
		     Real&                            _tolerance )
{
  typedef typename gmm::linalg_traits< gmm::csc_matrix<Real> >::const_sub_col_type ColT;
  typedef typename gmm::linalg_traits<ColT>::const_iterator CIter;

  double t2 = _tolerance*_tolerance;

  // clear old data
  i_temp.clear();
  s.clear();

  for ( unsigned int i=0; i<_idxs.size(); ++i )
    s.insert( _idxs[i] );

  int it_count = 0;

  bool finished = false;

  while ( !finished && it_count < _max_iter )
  {
    finished = true;
    std::set<int>::iterator s_it = s.begin();
    for(; s_it != s.end(); ++s_it)
    {
      ++it_count;
      unsigned int cur_i = *s_it;
      i_temp.clear();

      ColT col = mat_const_col( _A, cur_i );

      CIter it  = gmm::vect_const_begin( col );
      CIter ite = gmm::vect_const_end( col );
      
      double res_i   = -_rhs[cur_i];
      double x_i_new = _rhs[cur_i];
      double diag    = 1.0;
      for ( ; it!=ite; ++it )
      {
	res_i   += ( *it ) * _x[it.index()];
	x_i_new -= ( *it ) * _x[it.index()];
	if( it.index() != cur_i)
	  i_temp.push_back( it.index() );
	else
	  diag = *it;
      }

      // compare relative residuum normalized by diagonal entry
      if ( res_i*res_i/diag > t2 )
      {
	_x[cur_i] += x_i_new/_A( cur_i, cur_i );
	
	for ( unsigned int j=0; j<i_temp.size(); ++j )
	{
	  finished = false;
	  s.insert( i_temp[j] );
	}
      }
    }
  }

  // converged?
  return finished;
}
    
//-----------------------------------------------------------------------------

template <class RealT>
bool
IterativeSolverT<RealT>::
conjugate_gradient( typename gmm::csc_matrix<Real>&  _A,
		    std::vector<Real>&               _x, 
		    std::vector<Real>&               _rhs, 
		    int&                             _max_iter, 
		    Real&                            _tolerance )
{
  Real rho, rho_1(0), a;

  // initialize vectors
  p_.resize(_x.size());
  q_.resize(_x.size());
  r_.resize(_x.size());
  d_.resize(_x.size());
  gmm::copy( _x, p_);

  // initialize diagonal (for relative norm)
  for(unsigned int i=0; i<_x.size(); ++i)
    d_[i] = 1.0/_A(i,i);

  // start with iteration 0
  int cur_iter(0);

  gmm::mult(_A, gmm::scaled(_x, Real(-1)), _rhs, r_);
  rho = gmm::vect_sp( r_, r_);
  gmm::copy(r_, p_);
    
  bool not_converged = true;
  Real res_norm(0);

  // while not converged
  while( (not_converged = ( (res_norm=vect_norm_rel(r_, d_)) > _tolerance)) && 
	 cur_iter < _max_iter)
  {
    //    std::cerr << "iter " << cur_iter << "  res " << res_norm << std::endl;

    if (cur_iter != 0) 
    { 
      rho = gmm::vect_sp( r_, r_);
      gmm::add(r_, gmm::scaled(p_, rho / rho_1), p_);
    }

    gmm::mult(_A, p_, q_);

    a = rho / gmm::vect_sp( q_, p_);	
    gmm::add(gmm::scaled(p_, a), _x);
    gmm::add(gmm::scaled(q_, -a), r_);
    rho_1 = rho;

    ++cur_iter;
  }
  
  _max_iter  = cur_iter;
  _tolerance = res_norm;

  return (!not_converged);
}



//-----------------------------------------------------------------------------


template <class RealT>
typename IterativeSolverT<RealT>::Real
IterativeSolverT<RealT>::
vect_norm_rel(const std::vector<Real>& _v, const std::vector<Real>& _diag) const
{
  Real res = 0.0;

  for(unsigned int i=0; i<_v.size(); ++i)
  {
    res = std::max(fabs(_v[i]*_diag[i]), res);

//     Real cur = fabs(_v[i]*_diag[i]);
//     if(cur > res)
//       res = cur;
  }
  return res;
}


//-----------------------------------------------------------------------------



//=============================================================================
} // namespace COMISO
//=============================================================================
