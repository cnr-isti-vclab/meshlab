/*===========================================================================*\
 *                                                                           *
 *                               CoMISo                                      *
 *      Copyright (C) 2008-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.rwth-graphics.de                            *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of CoMISo.                                             *
 *                                                                           *
 *  CoMISo is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  CoMISo is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with CoMISo.  If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                           *
\*===========================================================================*/ 


//=============================================================================
//
//  CLASS MISolver - IMPLEMENTATION
//
//=============================================================================

#define COMISO_CONSTRAINEDSOLVER_C
//== INCLUDES =================================================================

#include "ConstrainedSolver.hh"
#include <gmm/gmm.h>
#include "GMM_Tools.hh"
#include <float.h>
#include <CoMISo/Utils/StopWatch.hh>
#include <CoMISo/Utils/MutablePriorityQueueT.hh>

//== NAMESPACES ===============================================================

namespace COMISO {

//== IMPLEMENTATION ==========================================================


template<class RMatrixT, class CMatrixT, class VectorT, class VectorIT >
void 
ConstrainedSolver::
solve_const(	 const RMatrixT& _constraints,
		 const CMatrixT& _A, 
		 VectorT&  _x,
		 const VectorT&  _rhs,
		 const VectorIT& _idx_to_round,
		 double    _reg_factor,
		 bool      _show_miso_settings,
		 bool      _show_timings )
{
  // copy matrices
  RMatrixT constraints( gmm::mat_nrows(_constraints), gmm::mat_ncols(_constraints));
  gmm::copy(_constraints, constraints);

  CMatrixT A( gmm::mat_nrows(_A), gmm::mat_ncols(_A));
  gmm::copy(_A, A);

  // ... and vectors
  VectorT  rhs(_rhs);
  VectorIT idx_to_round(_idx_to_round);

  // call non-const function
  solve(constraints,
	A,
	_x,
	rhs,
	idx_to_round,
	_reg_factor,
	_show_miso_settings,
	_show_timings);
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorT, class VectorIT >
void 
ConstrainedSolver::
solve_const( const RMatrixT& _constraints,
	     const RMatrixT& _B,
	     VectorT&  _x,
	     const VectorIT& _idx_to_round,
	     double    _reg_factor,
	     bool      _show_miso_settings,
	     bool      _show_timings )
{
  // copy matrices
  RMatrixT constraints( gmm::mat_nrows(_constraints), gmm::mat_ncols(_constraints));
  gmm::copy(_constraints, constraints);

  RMatrixT B( gmm::mat_nrows(_B), gmm::mat_ncols(_B));
  gmm::copy(_B, B);

  // ... and vectors
  VectorIT idx_to_round(_idx_to_round);

  // call non-const function
  solve(constraints,
	B,
	_x,
	idx_to_round,
	_reg_factor,
	_show_miso_settings,
	_show_timings);
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorT, class VectorIT >
void 
ConstrainedSolver::
solve(
    RMatrixT& _constraints,
    RMatrixT& _B, 
    VectorT&  _x,
    VectorIT& _idx_to_round,
    double    _reg_factor,
    bool      _show_miso_settings,
    bool      _show_timings )
{
  // convert into quadratic system
  VectorT rhs;
  gmm::col_matrix< gmm::rsvector< double > > A;
  COMISO_GMM::factored_to_quadratic(_B, A, rhs);

  // solve
  solve( _constraints, A, _x, rhs, 
	 _idx_to_round, _reg_factor, 
	 _show_miso_settings,
	 _show_timings);

//   int nrows = gmm::mat_nrows(_B);
//   int ncols = gmm::mat_ncols(_B);
//   int ncons = gmm::mat_nrows(_constraints);

//   if( _show_timings) std::cerr << __FUNCTION__ << "\n Initial dimension: " << nrows << " x " << ncols << ", number of constraints: " << ncons << std::endl;
 
//   // StopWatch for Timings
//   COMISO::StopWatch sw, sw2; sw.start(); sw2.start();

//   // c_elim[i] = index of variable which is eliminated in condition i
//   // or -1 if condition is invalid
//   std::vector<int> c_elim( ncons);

//   // apply sparse gauss elimination to make subsequent _constraints independent
//   make_constraints_independent( _constraints, _idx_to_round, c_elim);
//   double time_gauss = sw.stop()/1000.0; sw.start();

//   // eliminate conditions and return column matrix Bcol
//   gmm::col_matrix< gmm::rsvector< double > > Bcol( nrows, ncols);

//   // reindexing vector
//   std::vector<int>                          new_idx;

//   eliminate_constraints( _constraints, _B, _idx_to_round, c_elim, new_idx, Bcol);
//   double time_eliminate = sw.stop()/1000.0; 

//   if( _show_timings) std::cerr << "Eliminated dimension: " << gmm::mat_nrows(Bcol) << " x " << gmm::mat_ncols(Bcol) << std::endl;

//   // setup and solve system
//   double time_setup = setup_and_solve_system( Bcol, _x, _idx_to_round, _reg_factor, _show_miso_settings);
//   sw.start();

//   //  double time_setup_solve = sw.stop()/1000.0; sw.start();
  
//   // restore eliminated vars to fulfill the given conditions
//   restore_eliminated_vars( _constraints, _x, c_elim, new_idx);

//   double time_resubstitute = sw.stop()/1000.0; sw.start();

//   //  double time_total = sw2.stop()/1000.0;

//   if( _show_timings) std::cerr << "Timings: \n\t" <<
//     "Gauss Elimination  " << time_gauss          << " s\n\t" <<
//     "System Elimination " << time_eliminate      << " s\n\t" <<
//     "Setup              " << time_setup          << " s\n\t" <<
//    // "Setup + Mi-Solver  " << time_setup_solve    << " s\n\t" <<
//     "Resubstitution     " << time_resubstitute   << " s\n\t" << std::endl << std::endl;
//     //"Total              " << time_total          << std::endl;
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class CMatrixT, class VectorT, class VectorIT>
void 
ConstrainedSolver::
solve(
    RMatrixT& _constraints,
    CMatrixT& _A, 
    VectorT&  _x,
    VectorT&  _rhs,
    VectorIT& _idx_to_round,
    double    _reg_factor,
    bool      _show_miso_settings, 
    bool      _show_timings )
{
  // show options dialog
  if( _show_miso_settings)
    miso_.show_options_dialog();


  int nrows = gmm::mat_nrows(_A);
  int ncols = gmm::mat_ncols(_A);
  int ncons = gmm::mat_nrows(_constraints);

  if( _show_timings) std::cerr << __FUNCTION__ << "\n Initital dimension: " << nrows << " x " << ncols 
			       << ", number of constraints: " << ncons << " use reordering: " << use_constraint_reordering() << std::endl;

  // StopWatch for Timings
  COMISO::StopWatch sw, sw2; sw.start(); sw2.start();

  // c_elim[i] = index of variable which is eliminated in condition i
  // or -1 if condition is invalid
  std::vector<int> c_elim( ncons);

  // apply sparse gauss elimination to make subsequent _conditions independent
  if(use_constraint_reordering())
    make_constraints_independent_reordering( _constraints, _idx_to_round, c_elim);
  else
    make_constraints_independent( _constraints, _idx_to_round, c_elim);

  double time_gauss = sw.stop()/1000.0; sw.start();

  // re-indexing vector
  std::vector<int>                          new_idx;

  gmm::csc_matrix< double > Acsc;
  eliminate_constraints( _constraints, _A, _x, _rhs, _idx_to_round, c_elim, new_idx, Acsc);
  double time_eliminate = sw.stop()/1000.0;

  if( _show_timings)
  {
    std::cerr << "Eliminated dimension: " << Acsc.nr << " x " << Acsc.nc << std::endl;
    std::cerr << "#nonzeros: " << gmm::nnz(Acsc) << std::endl;
  }

  sw.start();
  miso_.solve( Acsc, _x, _rhs, _idx_to_round);
  double time_miso = sw.stop()/1000.0; sw.start();

  rhs_update_table_.store(_constraints, c_elim, new_idx);
  // restore eliminated vars to fulfill the given conditions
  restore_eliminated_vars( _constraints, _x, c_elim, new_idx);

  double time_resubstitute = sw.stop()/1000.0; sw.start();
  double time_total = time_gauss + time_eliminate + time_miso + time_resubstitute;
  if( _show_timings) std::cerr << "Timings: \n\t" <<
    "\tGauss Elimination  " << time_gauss          << " s\n\t" <<
    "\tSystem Elimination " << time_eliminate      << " s\n\t" <<
    "\tMi-Solver          " << time_miso           << " s\n\t" <<
    "\tResubstitution     " << time_resubstitute   << " s\n\t" << 
    "\tTotal              " << time_total          << std::endl << std::endl;
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorT >
void
ConstrainedSolver::
resolve(
    const RMatrixT& _B,
    VectorT&  _x,
    VectorT*  _constraint_rhs,
    bool      _show_timings )
{
  // extract rhs from quadratic system
  VectorT rhs;
 // gmm::col_matrix< gmm::rsvector< double > > A;
 // COMISO_GMM::factored_to_quadratic(_B, A, rhs);
  //TODO only compute rhs, not complete A for efficiency

  unsigned int m = gmm::mat_nrows(_B);
  unsigned int n = gmm::mat_ncols(_B);

  typedef typename gmm::linalg_traits<RMatrixT>::const_sub_row_type CRowT;
  typedef typename gmm::linalg_traits<RMatrixT>::sub_row_type       RowT;
  typedef typename gmm::linalg_traits<CRowT>::const_iterator        RIter;
  typedef typename gmm::linalg_traits<CRowT>::value_type            VecValT;

  gmm::resize(rhs, n-1);
  gmm::clear(rhs);
  for(unsigned int i = 0; i < m; ++i)
  {
    // get current condition row
    CRowT row       = gmm::mat_const_row( _B, i);
    RIter row_it    = gmm::vect_const_begin( row);
    RIter row_end   = gmm::vect_const_end( row);

    if(row_end == row_it) continue;
    --row_end;
    if(row_end.index() != n-1) continue;
    VecValT n_i = *row_end;
    while(row_end != row_it)
    {
      --row_end;
      rhs[row_end.index()] -= (*row_end) * n_i;
    }
  }

  // solve
  resolve(_x, _constraint_rhs, &rhs,
   _show_timings);
}


//-----------------------------------------------------------------------------


template<class VectorT >
void
ConstrainedSolver::
resolve(
     VectorT&  _x,
     VectorT*  _constraint_rhs,
     VectorT*  _rhs           ,
     bool      _show_timings   )
{
  // StopWatch for Timings
  COMISO::StopWatch sw;

  sw.start();
  // apply stored updates and eliminations to exchanged rhs
  if(_constraint_rhs)
  {
    // apply linear transformation of Gaussian elimination
    rhs_update_table_.cur_constraint_rhs_.resize(gmm::mat_nrows(rhs_update_table_.D_));
    gmm::mult(rhs_update_table_.D_, *_constraint_rhs, rhs_update_table_.cur_constraint_rhs_);

    // update rhs of stored constraints
    unsigned int nc = gmm::mat_ncols(rhs_update_table_.constraints_p_);
    for(unsigned int i=0; i<rhs_update_table_.cur_constraint_rhs_.size(); ++i)
      rhs_update_table_.constraints_p_(i,nc-1) = -rhs_update_table_.cur_constraint_rhs_[i];
  }
  if(_rhs)
    rhs_update_table_.cur_rhs_ = *_rhs;

  std::vector<double> rhs_red = rhs_update_table_.cur_rhs_;

  rhs_update_table_.apply(rhs_update_table_.cur_constraint_rhs_, rhs_red);
  rhs_update_table_.eliminate(rhs_red);

  //  std::cerr << "############### Resolve info ##############" << std::endl;
  //  std::cerr << rhs_update_table_.D_ << std::endl;
  //  std::cerr << rhs_update_table_.cur_rhs_ << std::endl;
  //  std::cerr << rhs_update_table_.cur_constraint_rhs_ << std::endl;
  //  std::cerr << rhs_update_table_.table_.size() << std::endl;
  //  std::cerr << "rhs_red: " << rhs_red << std::endl;

  miso_.resolve(_x, rhs_red);

  double time_miso = sw.stop()/1000.0; sw.start();

  // restore eliminated vars to fulfill the given conditions
  restore_eliminated_vars( rhs_update_table_.constraints_p_, _x, rhs_update_table_.c_elim_, rhs_update_table_.new_idx_);

  double time_resubstitute = sw.stop()/1000.0; sw.start();
  double time_total = time_miso + time_resubstitute;
  if( _show_timings) std::cerr << "Timings: \n\t" <<
    "\tMi-Solver          " << time_miso           << " s\n\t" <<
    "\tResubstitution     " << time_resubstitute   << " s\n\t" <<
    "\tTotal              " << time_total          << std::endl << std::endl;
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorIT >
void 
ConstrainedSolver::
make_constraints_independent(
    RMatrixT&         _constraints,
		VectorIT&         _idx_to_round,
		std::vector<int>& _c_elim)
{
  // setup linear transformation for rhs, start with identity
  unsigned int nr = gmm::mat_nrows(_constraints);
  gmm::resize(rhs_update_table_.D_, nr, nr);
  gmm::clear(rhs_update_table_.D_);
  for(unsigned int i=0; i<nr; ++i) rhs_update_table_.D_(i,i) = 1.0;

  //  COMISO::StopWatch sw;
  // number of variables
  int n_vars = gmm::mat_ncols(_constraints);

  // TODO Check: HZ added 14.08.09 
  _c_elim.clear();
  _c_elim.resize( gmm::mat_nrows(_constraints), -1);

  // build round map
  std::vector<bool> roundmap( n_vars, false);
  for(unsigned int i=0; i<_idx_to_round.size(); ++i)
    roundmap[_idx_to_round[i]] = true;

  // copy constraints into column matrix (for faster update via iterators)
  typedef gmm::wsvector<double>      CVector;
  typedef gmm::col_matrix< CVector > CMatrix;
  CMatrix constraints_c;
  gmm::resize(constraints_c, gmm::mat_nrows(_constraints), gmm::mat_ncols(_constraints));
  gmm::copy(_constraints, constraints_c);

  // for all conditions
  for(unsigned int i=0; i<gmm::mat_nrows(_constraints); ++i)
  {
    // get elimination variable
    int elim_j = -1;
    int elim_int_j = -1;

    // iterate over current row, until variable found
    // first search for real valued variable
    // if not found for integers with value +-1
    // and finally take the smallest integer variable

    typedef typename gmm::linalg_traits<RMatrixT>::const_sub_row_type CRowT;
    typedef typename gmm::linalg_traits<RMatrixT>::sub_row_type       RowT;
    typedef typename gmm::linalg_traits<CRowT>::const_iterator        RIter;

    // get current condition row
    CRowT row       = gmm::mat_const_row( _constraints, i);
    RIter row_it    = gmm::vect_const_begin( row);
    RIter row_end   = gmm::vect_const_end( row);
    double elim_val = FLT_MAX;
    double max_elim_val = -FLT_MAX;

    // new: gcd
    std::vector<int> v_gcd;
    v_gcd.resize(gmm::nnz(row),-1);
    int n_ints(0);
    bool gcd_update_valid(true);

    for(; row_it != row_end; ++row_it)
    {
      int cur_j = row_it.index();
      // do not use the constant part
      if(  cur_j != n_vars - 1 )
      {
        // found real valued var? -> finished (UPDATE: no not any more, find biggest real value to avoid x/1e-13)
        if( !roundmap[ cur_j ])
        {
          if( fabs(*row_it) > max_elim_val)
          {
            elim_j = cur_j;
            max_elim_val = fabs(*row_it);
          }
          //break;
        }
        else
        {
          double cur_row_val(fabs(*row_it));
          // gcd
          // if the coefficient of an integer variable is not an integer, then
          // the variable most problably will not be (expect if all coeffs are the same, e.g. 0.5)
          if( (double(int(cur_row_val))- cur_row_val) != 0.0)
	  {
// 	    std::cerr << __FUNCTION__ << " Warning: coefficient of integer variable is NOT integer: " 
// 		      << cur_row_val << std::endl;
	    gcd_update_valid = false;
	  }

          v_gcd[n_ints] = cur_row_val;
          ++n_ints;

          // store integer closest to 1, must be greater than epsilon_
          if( fabs(cur_row_val-1.0) < elim_val && cur_row_val > epsilon_)
          {
            elim_int_j   = cur_j;
            elim_val     = fabs(cur_row_val-1.0);
          }
        }
      }
    }

    // first try to eliminate a valid (>epsilon_) real valued variable (safer)
    if( max_elim_val > epsilon_)
    {}
    else // use the best found integer
      elim_j = elim_int_j;

    // if no integer or real valued variable greater than epsilon_ existed, then
    // elim_j is now -1 and this row is not considered as a valid constraint




    // store result
    _c_elim[i] = elim_j;
    // error check result
    if( elim_j == -1)
    {
      // redundant or incompatible?
      if( noisy_ > 0)
        if( fabs(gmm::mat_const_row(_constraints, i)[n_vars-1]) > epsilon_ )
          std::cerr << "Warning: incompatible condition: "
		    << fabs(gmm::mat_const_row(_constraints, i)[n_vars-1]) << std::endl;
      //         else
      //           std::cerr << "Warning: redundant condition:\n";
    }
    else
      if(roundmap[elim_j] && elim_val > 1e-6) 
      {
        if( do_gcd_ && gcd_update_valid)
        {
          // perform gcd update
          bool gcd_ok = update_constraint_gcd( _constraints, i, elim_j, v_gcd, n_ints);
          if( !gcd_ok)
            if( noisy_ > 0)
              std::cerr << __FUNCTION__ << " Warning: GCD update failed! " << gmm::mat_const_row(_constraints, i) << std::endl;
        }
        else
        {
          if( noisy_ > 0)
	  {
	    if( !do_gcd_)
	      std::cerr << __FUNCTION__ << " Warning: NO +-1 coefficient found, integer rounding cannot be guaranteed. Try using the GCD option! " << gmm::mat_const_row(_constraints, i) << std::endl;
	    else
	      std::cerr << __FUNCTION__ << " Warning: GCD of non-integer cannot be computed! " << gmm::mat_const_row(_constraints, i) << std::endl;

	  }
        }
      }


    // is this condition dependent?
    if( elim_j != -1 )
    {
      // get elim variable value
      double elim_val_cur = _constraints(i, elim_j);

      // copy col
      CVector col = constraints_c.col(elim_j);

      // iterate over column
      typename gmm::linalg_traits<CVector>::const_iterator c_it   = gmm::vect_const_begin(col);
      typename gmm::linalg_traits<CVector>::const_iterator c_end  = gmm::vect_const_end(col);

      for(; c_it != c_end; ++c_it)
        if( c_it.index() > i)
        {
	  //          sw.start();
          double val = -(*c_it)/elim_val_cur;

          add_row_simultaneously( c_it.index(), val, gmm::mat_row(_constraints, i), _constraints, constraints_c);
          // make sure the eliminated entry is 0 on all other rows and not 1e-17
          _constraints( c_it.index(), elim_j) = 0;
          constraints_c(c_it.index(), elim_j) = 0;

          // update linear transition of rhs
          gmm::add(gmm::scaled(gmm::mat_row(rhs_update_table_.D_, i), val),
                   gmm::mat_row(rhs_update_table_.D_, c_it.index()));
        }
    }
  }
}



//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorIT >
void 
ConstrainedSolver::
make_constraints_independent_reordering(
    RMatrixT&         _constraints,
		VectorIT&         _idx_to_round,
		std::vector<int>& _c_elim)
{
  // setup linear transformation for rhs, start with identity
  unsigned int nr = gmm::mat_nrows(_constraints);
  gmm::resize(rhs_update_table_.D_, nr, nr);
  gmm::clear(rhs_update_table_.D_);
  for(unsigned int i=0; i<nr; ++i) rhs_update_table_.D_(i,i) = 1.0;

  //  COMISO::StopWatch sw;
  // number of variables
  int n_vars = gmm::mat_ncols(_constraints);

  // TODO Check: HZ added 14.08.09 
  _c_elim.clear();
  _c_elim.resize( gmm::mat_nrows(_constraints), -1);

  // build round map
  std::vector<bool> roundmap( n_vars, false);
  for(unsigned int i=0; i<_idx_to_round.size(); ++i)
    roundmap[_idx_to_round[i]] = true;

  // copy constraints into column matrix (for faster update via iterators)
  typedef gmm::wsvector<double>      CVector;
  typedef gmm::col_matrix< CVector > CMatrix;
  CMatrix constraints_c;
  gmm::resize(constraints_c, gmm::mat_nrows(_constraints), gmm::mat_ncols(_constraints));
  gmm::copy(_constraints, constraints_c);

  // init priority queue
  MutablePriorityQueueT<unsigned int, unsigned int> queue;
  queue.clear( nr );
  for(unsigned int i=0; i<nr; ++i)
  {
    int cur_nnz = gmm::nnz( gmm::mat_row(_constraints,i));
    if( _constraints(i,n_vars-1) != 0.0) --cur_nnz;

    queue.update(i, cur_nnz);
  }
  
  std::vector<bool> row_visited(nr, false);
  std::vector<unsigned int> row_ordering; row_ordering.reserve(nr);


  // for all conditions
  //  for(unsigned int i=0; i<gmm::mat_nrows(_constraints); ++i)
  while(!queue.empty())
  {
    // get next row
    unsigned int i = queue.get_next();
    row_ordering.push_back(i);
    row_visited[i] = true;

    // get elimination variable
    int elim_j = -1;
    int elim_int_j = -1;

    // iterate over current row, until variable found
    // first search for real valued variable
    // if not found for integers with value +-1
    // and finally take the smallest integer variable

    typedef typename gmm::linalg_traits<RMatrixT>::const_sub_row_type CRowT;
    typedef typename gmm::linalg_traits<RMatrixT>::sub_row_type       RowT;
    typedef typename gmm::linalg_traits<CRowT>::const_iterator        RIter;

    // get current condition row
    CRowT row       = gmm::mat_const_row( _constraints, i);
    RIter row_it    = gmm::vect_const_begin( row);
    RIter row_end   = gmm::vect_const_end( row);
    double elim_val = FLT_MAX;
    double max_elim_val = -FLT_MAX;

    // new: gcd
    std::vector<int> v_gcd;
    v_gcd.resize(gmm::nnz(row),-1);
    int n_ints(0);
    bool gcd_update_valid(true);

    for(; row_it != row_end; ++row_it)
    {
      int cur_j = row_it.index();
      // do not use the constant part
      if(  cur_j != n_vars - 1 )
      {
        // found real valued var? -> finished (UPDATE: no not any more, find biggest real value to avoid x/1e-13)
        if( !roundmap[ cur_j ])
        {
          if( fabs(*row_it) > max_elim_val)
          {
            elim_j = cur_j;
            max_elim_val = fabs(*row_it);
          }
          //break;
        }
        else
        {
          double cur_row_val(fabs(*row_it));
          // gcd
          // if the coefficient of an integer variable is not an integer, then
          // the variable most problably will not be (expect if all coeffs are the same, e.g. 0.5)
          if( (double(int(cur_row_val))- cur_row_val) != 0.0)
	  {
// 	    std::cerr << __FUNCTION__ << " Warning: coefficient of integer variable is NOT integer: " 
// 		      << cur_row_val << std::endl;
	    gcd_update_valid = false;
	  }

          v_gcd[n_ints] = cur_row_val;
          ++n_ints;

          // store integer closest to 1, must be greater than epsilon_
          if( fabs(cur_row_val-1.0) < elim_val && cur_row_val > epsilon_)
          {
            elim_int_j   = cur_j;
            elim_val     = fabs(cur_row_val-1.0);
          }
        }
      }
    }

    // first try to eliminate a valid (>epsilon_) real valued variable (safer)
    if( max_elim_val > epsilon_)
    {}
    else // use the best found integer
      elim_j = elim_int_j;

    // if no integer or real valued variable greater than epsilon_ existed, then
    // elim_j is now -1 and this row is not considered as a valid constraint




    // store result
    _c_elim[i] = elim_j;
    // error check result
    if( elim_j == -1)
    {
      // redundant or incompatible?
      if( noisy_ > 0)
        if( fabs(gmm::mat_const_row(_constraints, i)[n_vars-1]) > epsilon_ )
          std::cerr << "Warning: incompatible condition: "
		    << fabs(gmm::mat_const_row(_constraints, i)[n_vars-1]) << std::endl;
      //         else
      //           std::cerr << "Warning: redundant condition:\n";
    }
    else
      if(roundmap[elim_j] && elim_val > 1e-6) 
      {
        if( do_gcd_ && gcd_update_valid)
        {
          // perform gcd update
          bool gcd_ok = update_constraint_gcd( _constraints, i, elim_j, v_gcd, n_ints);
          if( !gcd_ok)
            if( noisy_ > 0)
              std::cerr << __FUNCTION__ << " Warning: GCD update failed! " << gmm::mat_const_row(_constraints, i) << std::endl;
        }
        else
        {
          if( noisy_ > 0)
	  {
	    if( !do_gcd_)
	      std::cerr << __FUNCTION__ << " Warning: NO +-1 coefficient found, integer rounding cannot be guaranteed. Try using the GCD option! " << gmm::mat_const_row(_constraints, i) << std::endl;
	    else
	      std::cerr << __FUNCTION__ << " Warning: GCD of non-integer cannot be computed! " << gmm::mat_const_row(_constraints, i) << std::endl;

	  }
        }
      }


    // is this condition dependent?
    if( elim_j != -1 )
    {
      // get elim variable value
      double elim_val_cur = _constraints(i, elim_j);

      // copy col
      CVector col = constraints_c.col(elim_j);

      // iterate over column
      typename gmm::linalg_traits<CVector>::const_iterator c_it   = gmm::vect_const_begin(col);
      typename gmm::linalg_traits<CVector>::const_iterator c_end  = gmm::vect_const_end(col);

      for(; c_it != c_end; ++c_it)
	//        if( c_it.index() > i)
	if( !row_visited[c_it.index()])
        {
	  //          sw.start();
          double val = -(*c_it)/elim_val_cur;
          add_row_simultaneously( c_it.index(), val, gmm::mat_row(_constraints, i), _constraints, constraints_c);
          // make sure the eliminated entry is 0 on all other rows and not 1e-17
          _constraints( c_it.index(), elim_j) = 0;
          constraints_c(c_it.index(), elim_j) = 0;

	  int cur_idx = c_it.index();
	  int cur_nnz = gmm::nnz( gmm::mat_row(_constraints,cur_idx));
	  if( _constraints(cur_idx,n_vars-1) != 0.0) --cur_nnz;

	  queue.update(cur_idx, cur_nnz);

          // update linear transition of rhs
          gmm::add(gmm::scaled(gmm::mat_row(rhs_update_table_.D_, i), val),
                   gmm::mat_row(rhs_update_table_.D_, c_it.index()));
        }
    }
  }
  // // check result
  // for(unsigned int i=0; i<row_visited.size(); ++i)
  //   if( !row_visited[i])
  //     std::cerr <<"FAT ERROR: row " << i << " not visited...\n";
  


  // correct ordering
  RMatrixT c_tmp(gmm::mat_nrows(_constraints), gmm::mat_ncols(_constraints));
  gmm::copy(_constraints,c_tmp);
  RowMatrix d_tmp(gmm::mat_nrows(rhs_update_table_.D_),gmm::mat_ncols(rhs_update_table_.D_));
  gmm::copy(rhs_update_table_.D_,d_tmp);

  // std::vector<int> elim_temp2(_c_elim);
  // std::sort(elim_temp2.begin(), elim_temp2.end());
  // std::cerr << elim_temp2 << std::endl;


  std::vector<int> elim_temp(_c_elim);
  _c_elim.resize(0); _c_elim.resize( elim_temp.size(),-1);

  for(unsigned int i=0; i<nr; ++i)
  {
    gmm::copy(gmm::mat_row(c_tmp,row_ordering[i]), gmm::mat_row(_constraints,i));
    gmm::copy(gmm::mat_row(d_tmp,row_ordering[i]), gmm::mat_row(rhs_update_table_.D_,i));

    _c_elim[i] = elim_temp[row_ordering[i]];
  }

  // // hack
  // elim_temp = _c_elim;
  // std::sort(elim_temp.begin(), elim_temp.end());
  // std::cerr << elim_temp << std::endl;

  // std::sort(row_ordering.begin(), row_ordering.end());
  // std::cerr << "row ordering: " << row_ordering << std::endl;
}


//-----------------------------------------------------------------------------

template<class RMatrixT>
bool
ConstrainedSolver::
update_constraint_gcd( RMatrixT& _constraints, 
                       int _row_i, 
                       int& _elim_j,
                       std::vector<int>& _v_gcd,
                       int& _n_ints)
{
  // find gcd
  double i_gcd = find_gcd(_v_gcd, _n_ints);

  if( fabs(i_gcd) == 1.0)
    return false;
  
  // divide by gcd
  typedef typename gmm::linalg_traits<RMatrixT>::const_sub_row_type CRowT;
  typedef typename gmm::linalg_traits<CRowT>::const_iterator        RIter;

  // get current constraint row
  RIter row_it    = gmm::vect_const_begin( gmm::mat_const_row( _constraints, _row_i));
  RIter row_end   = gmm::vect_const_end( gmm::mat_const_row( _constraints, _row_i));

  for( ; row_it!=row_end; ++row_it)
  {
    int cur_j = row_it.index();
    _constraints(_row_i, cur_j) = (*row_it)/i_gcd;
  }
  int elim_coeff = abs(_constraints(_row_i, _elim_j));
  if( elim_coeff != 1)
    std::cerr << __FUNCTION__ << " Error: elimination coefficient " << elim_coeff << " will (most probably) NOT lead to an integer solution!" << std::endl;
  return true;
}

//-----------------------------------------------------------------------------

template<class SVector1T, class SVector2T, class VectorIT, class SVector3T>
void 
ConstrainedSolver::
eliminate_constraints( 
    gmm::row_matrix<SVector1T>& _constraints,
    gmm::row_matrix<SVector2T>& _B, 
    VectorIT&                   _idx_to_round,
    std::vector<int>&           _c_elim,
    std::vector<int>&           _new_idx,
    gmm::col_matrix<SVector3T>& _Bcol)
{
  // copy into column matrix
  gmm::resize(_Bcol, gmm::mat_nrows(_B), gmm::mat_ncols(_B));
  gmm::copy( _B, _Bcol);

  // store columns which should be eliminated
  std::vector<int> elim_cols;
  elim_cols.reserve( _c_elim.size());

  for(unsigned int i=0; i<_c_elim.size(); ++i)
  {
    int cur_j = _c_elim[i];

    if( cur_j != -1)
    {
      double cur_val = _constraints(i,cur_j);

      // store index
      elim_cols.push_back(_c_elim[i]);

      // copy col
      SVector3T col = _Bcol.col(cur_j);

      // iterate over column
      typename gmm::linalg_traits<SVector3T>::const_iterator c_it   = gmm::vect_const_begin(col);
      typename gmm::linalg_traits<SVector3T>::const_iterator c_end  = gmm::vect_const_end(col);

      for(; c_it != c_end; ++c_it)
        add_row( c_it.index(), -(*c_it)/cur_val, gmm::mat_row(_constraints, i), _Bcol);
    }
  }

  // eliminate columns
  eliminate_columns( _Bcol, elim_cols);

  // TODO FIXME Size -1 ?!?!
  // init _new_idx vector
  _new_idx.resize(gmm::mat_ncols(_constraints));
  for(unsigned int i=0; i<_new_idx.size(); ++i)
    _new_idx[i] = i;

  // update _new_idx w.r.t. eliminated cols
  COMISO_GMM::eliminate_vars_idx( elim_cols, _new_idx, -1);

  // update _idx_to_round (in place)
  std::vector<int> round_old(_idx_to_round);
  unsigned int wi = 0;
  for(unsigned int i=0; i<_idx_to_round.size(); ++i)
  {
    if(_new_idx[ _idx_to_round[i]] != -1)
    {
      _idx_to_round[wi] = _new_idx[_idx_to_round[i]];
      ++wi;
    }
  }
  
  // resize, sort and make unique
  _idx_to_round.resize(wi);

  std::sort(_idx_to_round.begin(), _idx_to_round.end());
  _idx_to_round.resize( std::unique(_idx_to_round.begin(), _idx_to_round.end()) -_idx_to_round.begin());


  if( noisy_ > 2 )
  {
    std::cerr << __FUNCTION__ << "remaining         variables: " << gmm::mat_ncols(_Bcol) << std::endl;
    std::cerr << __FUNCTION__ << "remaining integer variables: " << _idx_to_round.size() << std::endl;
    std::cerr << __FUNCTION__ << std::endl;
  }
}


//-----------------------------------------------------------------------------


template<class SVector1T, class SVector2T, class VectorIT, class CSCMatrixT>
void 
ConstrainedSolver::
eliminate_constraints(
    gmm::row_matrix<SVector1T>& _constraints,
    gmm::col_matrix<SVector2T>& _A, 
    std::vector<double>&        _x, 
    std::vector<double>&        _rhs, 
    VectorIT&                   _idx_to_round,
    std::vector<int>&           _v_elim,
    std::vector<int>&           _new_idx,
    CSCMatrixT&                 _Acsc)
{
  COMISO::StopWatch sw;
  sw.start();
  // define iterator on matrix A and on constraints C
  typedef typename gmm::linalg_traits<SVector2T>::const_iterator  AIter;
  typedef typename gmm::linalg_traits<SVector1T>::const_iterator  CIter;

  // store variable indices to be eliminated
  std::vector<int> elim_varids;
  elim_varids.reserve( _v_elim.size());

  rhs_update_table_.clear();
  std::vector<double> constraint_rhs_vec(_constraints.nrows());

  for( unsigned int i=0; i < _v_elim.size(); ++i)
  {
    int cur_j = _v_elim[i];

    if( cur_j != -1)
    {
      double cur_val = _constraints(i, cur_j);

      // store index
      elim_varids.push_back(cur_j);
      rhs_update_table_.add_elim_id(cur_j);

      // copy col
      SVector2T col ( _A.col( cur_j));

      // get a reference to current constraint vector 
      SVector1T& constraint( _constraints.row(i));

      // add cur_j-th row multiplied with constraint[k] to each row k
      // iterator of matrix column
      AIter col_it, col_end;
      
      // constraint rhs
      double constraint_rhs = constraint[ constraint.size()-1];
      constraint_rhs_vec[i] = -constraint_rhs;

      //std::cerr << "constraint_rhs " << constraint_rhs << std::endl;
      // temporarliy set last element to zero (to avoid iterator finding it)
      constraint[ constraint.size()-1] = 0;

      double cur_rhs = _rhs[cur_j];

      // iterator of constraint
      CIter con_it  = gmm::vect_const_begin( constraint);
      CIter con_end = gmm::vect_const_end( constraint);

      // loop over all constraint entries over all column entries
      // should not hit last element (rhs) since set to zero
      for ( ; con_it != con_end; ++con_it )
      {
        col_it  = gmm::vect_const_begin( col );
        col_end = gmm::vect_const_end( col );
        for ( ; col_it != col_end; ++col_it )
          _A( con_it.index(), col_it.index() ) -= ( *col_it )*(( *con_it )/cur_val);

        //_rhs[con_it.index()] -= cur_rhs * (( *con_it )/cur_val);
//        rhs_update_table_.append(con_it.index(), -1.0 * (( *con_it )/cur_val), cur_j);
        rhs_update_table_.append(con_it.index(), -1.0 * (( *con_it )/cur_val), cur_j, false);
        //std::cerr << con_it.index() << " += " << -1.0*(( *con_it )/cur_val) << " * " << cur_rhs << " (["<<cur_j<<"] = "<<_rhs[cur_j]<<") " << std::endl;
      }

      // TODO FIXME must use copy col (referens sometimes yields infinite loop below no col_it++?!?)
      SVector2T col_ref = _A.col(cur_j);
      
      // add cur_j-th col multiplied with condition[k] to each col k
      col_it  = gmm::vect_const_begin( col_ref );
      col_end = gmm::vect_const_end( col_ref );

      // loop over all column entries and over all constraint entries
      for ( ; col_it != col_end; ++col_it )
      {
        con_it  = gmm::vect_const_begin( constraint );
        con_end = gmm::vect_const_end( constraint );

        for ( ; con_it != con_end; ++con_it )
          _A( col_it.index(), con_it.index() ) -= ( *col_it )*(( *con_it )/cur_val);
        //_rhs[col_it.index()] += constraint_rhs*( *col_it )/cur_val;
//        rhs_update_table_.append(col_it.index(), constraint_rhs*( *col_it )/cur_val);
        rhs_update_table_.append(col_it.index(), -( *col_it )/cur_val, i, true);
        //std::cerr << col_it.index() << " += " << constraint_rhs*( *col_it )/cur_val << std::endl;
      }

      // reset last constraint entry to real value
      constraint[ constraint.size()-1] = constraint_rhs;
    }
  }

  // cache current rhs's
  rhs_update_table_.cur_constraint_rhs_ = constraint_rhs_vec;
  rhs_update_table_.cur_rhs_            = _rhs;
  // apply transformation due to elimination
  rhs_update_table_.apply(constraint_rhs_vec, _rhs);

  if( noisy_ > 2)
    std::cerr << __FUNCTION__ << " Constraints integrated " << sw.stop()/1000.0 << std::endl;

  // eliminate vars
  _Acsc.init_with_good_format(_A);
  sw.start();
  std::vector< double > elim_varvals( elim_varids.size(), 0);
  COMISO_GMM::eliminate_csc_vars2( elim_varids, elim_varvals, _Acsc, _x, _rhs);

  if( noisy_ > 2)
    std::cerr << __FUNCTION__ << " Constraints eliminated " << sw.stop()/1000.0 << std::endl;
  sw.start();
  // init _new_idx vector
//  _new_idx.resize( gmm::mat_ncols(_constraints));
  _new_idx.resize( gmm::mat_ncols(_A)+1);
  for( unsigned int i=0; i<_new_idx.size(); ++i)
    _new_idx[i] = i;

  // update _new_idx w.r.t. eliminated cols
  COMISO_GMM::eliminate_vars_idx( elim_varids, _new_idx, -1);

  // update _idx_to_round (in place)
  unsigned int wi = 0;
  for( unsigned int i=0; i<_idx_to_round.size(); ++i)
  {
    if(_new_idx[ _idx_to_round[i]] != -1)
    {
      _idx_to_round[wi] = _new_idx[_idx_to_round[i]];
      ++wi;
    }
  }

  // resize, sort and make unique
  _idx_to_round.resize(wi);

  std::sort(_idx_to_round.begin(), _idx_to_round.end());
  _idx_to_round.resize( std::unique(_idx_to_round.begin(), _idx_to_round.end()) -_idx_to_round.begin());

  if( noisy_ > 2)
    std::cerr << __FUNCTION__ << "Indices reindexed " << sw.stop()/1000.0 << std::endl << std::endl;
}


//-----------------------------------------------------------------------------


template<class RowT, class MatrixT>
void 
ConstrainedSolver::
add_row( int       _row_i,
	 double    _coeff,
	 RowT      _row, 
	 MatrixT&  _mat )
{
  typedef typename gmm::linalg_traits<RowT>::const_iterator RIter;
  RIter r_it  = gmm::vect_const_begin(_row);
  RIter r_end = gmm::vect_const_end(_row);

  for(; r_it!=r_end; ++r_it)
    _mat(_row_i, r_it.index()) += _coeff*(*r_it);
}


//-----------------------------------------------------------------------------


template<class RowT, class RMatrixT, class CMatrixT>
void 
ConstrainedSolver::
add_row_simultaneously(	int       _row_i,
			double    _coeff,
			RowT      _row, 
			RMatrixT& _rmat,
			CMatrixT& _cmat )
{
  typedef typename gmm::linalg_traits<RowT>::const_iterator RIter;
  RIter r_it  = gmm::vect_const_begin(_row);
  RIter r_end = gmm::vect_const_end(_row);

  for(; r_it!=r_end; ++r_it)
  {
    _rmat(_row_i, r_it.index()) += _coeff*(*r_it);
    _cmat(_row_i, r_it.index()) += _coeff*(*r_it);
    //    if( _rmat(_row_i, r_it.index())*_rmat(_row_i, r_it.index()) < epsilon_squared_ )
    if( fabs(_rmat(_row_i, r_it.index())) < epsilon_ )
    {
      _rmat(_row_i, r_it.index()) = 0.0;
      _cmat(_row_i, r_it.index()) = 0.0;
    }
  }
}


//-----------------------------------------------------------------------------


template<class CMatrixT, class VectorT, class VectorIT>
double 
ConstrainedSolver::
setup_and_solve_system( CMatrixT& _B,
			VectorT&  _x,
			VectorIT& _idx_to_round,
			double    _reg_factor,
			bool      _show_miso_settings)
{
  // show options dialog
  if( _show_miso_settings)
    miso_.show_options_dialog();

  COMISO::StopWatch s1;
  COMISO::StopWatch sw; sw.start();
  unsigned int m = gmm::mat_nrows(_B);
  unsigned int n = gmm::mat_ncols(_B);

  s1.start();
  // set up B transposed
  CMatrixT Bt;
  gmm::resize( Bt, n, m);
  gmm::copy( gmm::transposed( _B), Bt);
  if( noisy_ > 1 )
    std::cerr << __FUNCTION__ << " Bt took " << s1.stop()/1000.0 << std::endl;
  s1.start();

  // setup BtB
  CMatrixT BtB;
  gmm::resize( BtB, n, n);
  gmm::mult( Bt, _B, BtB);
  if( noisy_ > 1 )
    std::cerr << __FUNCTION__ << " BtB took " << s1.stop()/1000.0 << std::endl;
  s1.start();

  // extract rhs
  std::vector< double > rhs( n);
  gmm::copy( gmm::scaled(gmm::mat_const_col( BtB, n - 1),-1.0), rhs);
  rhs.resize( n - 1);

  if( noisy_ > 1)
    std::cerr << __FUNCTION__ << " rhs extract resize " << s1.stop()/1000.0 << std::endl;
  s1.start();

  // resize BtB to only contain the actual system matrix (and not the rhs)
  gmm::resize( BtB, n - 1, n - 1);

  if( noisy_ > 1)
    std::cerr << __FUNCTION__ << " BtB resize took " << s1.stop()/1000.0 << std::endl;
  s1.start();
  _x.resize( n - 1);
  if( noisy_ > 1)
    std::cerr << __FUNCTION__ << " x resize took " << s1.stop()/1000.0 << std::endl;

  // regularize if necessary
  if(_reg_factor != 0.0)
    COMISO_GMM::regularize_hack(BtB, _reg_factor);
  s1.start();

  // BtB -> CSC
  CSCMatrix BtBCSC;
  BtBCSC.init_with_good_format( BtB);

  if( noisy_ > 1)
    std::cerr << __FUNCTION__ << " CSC init " << s1.stop()/1000.0 << std::endl;
  double setup_time = sw.stop()/1000.0;
  

  COMISO::StopWatch misw;
  misw.start();
  // miso solve
  miso_.solve( BtBCSC, _x, rhs, _idx_to_round);
  if( noisy_ > 1)
  std::cerr << __FUNCTION__ << " Miso Time " << misw.stop()/1000.0 << "s." << std::endl << std::endl;
  return setup_time;
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorT >
void 
ConstrainedSolver::
restore_eliminated_vars( RMatrixT&         _constraints,
			 VectorT&          _x,
			 std::vector<int>& _c_elim,
			 std::vector<int>& _new_idx)
{
  // restore original ordering of _x
  _x.resize(_new_idx.size());
  // last variable is the constant term 1.0
  _x.back() = 1.0;

  // reverse iterate from prelast element
  for(int i=_new_idx.size()-2; i>= 0; --i)
  {
    if( _new_idx[i] != -1)
    {
      // error handling
      if( i < _new_idx[i] && noisy_ > 0) std::cerr << "Warning: UNSAFE Ordering!!!\n";

      _x[i] = _x[_new_idx[i]];
    }
  }

  // reverse iterate
  for(int i=_c_elim.size()-1; i>=0; --i)
  {
    int cur_var = _c_elim[i];

    if( cur_var != -1)
    {
      // get variable value and set to zero
      double cur_val = _constraints(i, cur_var);

      //_constraints(i, cur_var) = 0.0;
      //_x[cur_var] = -gmm::vect_sp(_x, _constraints.row(i))/cur_val;
      //reformulated to keep _constraints intact for further use:
      _x[cur_var] -= gmm::vect_sp(_x, _constraints.row(i))/cur_val;

    }
  }

  // resize
  _x.resize(_x.size()-1);
}


//-----------------------------------------------------------------------------


template<class RMatrixT, class VectorT, class VectorIT >
void 
ConstrainedSolver::
verify_mi_factored( const RMatrixT& _conditions,
		    const RMatrixT& _B, 
		    const VectorT&  _x,
		    const VectorIT& _idx_to_round )
{
  std::cerr << "######### Verify Constrained Solver Result ############\n";

  // create extended x vector
  std::vector<double> x(_x);
  x.resize(x.size()+1);
  x.back() = 1.0;

  // verify conditions
  std::vector<double> a(gmm::mat_nrows(_conditions));

  gmm::mult(_conditions, x, a);

  int conditions_not_ok = 0;
  for(unsigned int i=0; i<a.size(); ++i)
    if( a[i] > 1e-6)
    {
      ++conditions_not_ok;
    }

  if( conditions_not_ok == 0)
    std::cerr << "all conditions are ok!\n";
  else
    std::cerr << conditions_not_ok 
	      << " conditions are not fullfilled: " 
	      << std::endl;

  // verify rounding
  int roundings_not_ok = 0;
  for(unsigned int i=0; i<_idx_to_round.size(); ++i)
  {
    double d = _x[_idx_to_round[i]];
    if( fabs(d-round(d)) > 1e-6)
      ++roundings_not_ok;
  }
  
  if( roundings_not_ok)
    std::cerr << roundings_not_ok << " Integer variables are not rounded\n";
  else
    std::cerr << "all Integer roundings are ok\n";

  // evaluate energy
  VectorT Bx(x);
  gmm::mult(_B, x, Bx);
  std::cerr << "Total energy: " << gmm::vect_sp(Bx, Bx) << std::endl;

  std::cerr << "######### FINISHED ############\n";
}



//-----------------------------------------------------------------------------


template<class RMatrixT, class CMatrixT, class VectorT>
double 
ConstrainedSolver::verify_constrained_system( 
    const RMatrixT& _conditions,
    const CMatrixT& _A,
    const VectorT&  _x,
    const VectorT&  _rhs,
    double          _eps)
{
  typedef typename gmm::linalg_traits<RMatrixT>::const_sub_row_type RowT;
  typedef typename gmm::linalg_traits<RowT>::const_iterator RIter;

  VectorT Ax( _x.size());
  gmm::mult(_A, _x, Ax);

  gmm::add(_rhs, gmm::scaled(Ax, -1.0), Ax);
  double norm = gmm::vect_norm2(Ax);
  //std::cerr << __FUNCTION__ << ": Error residual: " << norm << " vector : " << Ax << std::endl;

  std::cerr << __FUNCTION__ << ": Checking constraints..." << std::endl;

  unsigned int row_cond = gmm::mat_nrows( _conditions);
  unsigned int col_cond = gmm::mat_ncols( _conditions);
  bool all_conditions_ok = true;
  for( unsigned int r = 0; r < row_cond; ++r)
  {
    double cond_value = 0.0;
    RowT row      = gmm::mat_const_row( _conditions, r);
    RIter row_it  = gmm::vect_const_begin( row);
    RIter row_end = gmm::vect_const_end( row);
    //std::cerr << "\t checking row : " << row << std::endl;

    for( ; row_it != row_end; ++row_it)
    {
      if( row_it.index() == col_cond -1)
        cond_value += (*row_it);
      else
        cond_value += _x[row_it.index()]*(*row_it);
    }
    //std::cerr << "\t Value is : " << cond_value << std::endl;
    //std::cerr << "--- --- --- --- ---\n";
    if( fabs(cond_value) > _eps)
    {
      std::cerr << "\t Error on row " << r << " with vector " << row << " and condition value " << cond_value << std::endl;
      all_conditions_ok = false;
    }
  }
  std::cerr << __FUNCTION__ << (all_conditions_ok? ": All conditions ok!" : ": Some conditions not ok!") << std::endl;
  return norm;
}

//-----------------------------------------------------------------------------


template<class RMatrixT, class CMatrixT, class VectorT, class VectorIT>
double
ConstrainedSolver::
verify_constrained_system_round( 
              const RMatrixT& _conditions,
              const CMatrixT& _A,
              const VectorT&  _x,
              const VectorT&  _rhs,
              const VectorIT& _idx_to_round,
	      double          _eps)
{
  // test integer roundings
  std::cerr << __FUNCTION__ << ": Testing integer roundings..." << std::endl;
  bool all_roundings_ok = true;

  for( unsigned int i = 0; i < _idx_to_round.size(); ++i)
    if(fabs(ROUND(_x[_idx_to_round[i]])-_x[_idx_to_round[i]]) != 0.0)
    {
      std::cerr << "\t Warning: variable " << _idx_to_round[i] << " was not rounded!" << " Value is = " << _x[_idx_to_round[i]] << std::endl;
      all_roundings_ok = false;
    }
  std::cerr << __FUNCTION__ << (all_roundings_ok? ": All roundings ok!" : ": Some roundings not ok!") << std::endl;

  // also test other stuff
  return verify_constrained_system(_conditions, _A, _x, _rhs, _eps);
}

//-----------------------------------------------------------------------------


template<class CMatrixT>
void 
ConstrainedSolver::eliminate_columns( CMatrixT& _M,
				      const std::vector< int >& _columns)
{
  // nothing to do?
  if( _columns.size() == 0) return;

  // eliminate columns in place by first copying to the right place
  // and a subsequent resize
  std::vector< int > columns( _columns);
  std::sort( columns.begin(), columns.end());

  std::vector< int >::iterator col_it  = columns.begin();
  std::vector< int >::iterator col_end = columns.end();

  int next_i = *col_it;
  for( int i = *col_it; i < (int)_M.ncols(); ++i)
  {
    if( col_it != col_end && i == *col_it)
    {
      ++col_it;
    }
    else
    {
      _M.col(next_i) = _M.col(i);
      ++next_i;
    }
  }
  gmm::resize( _M, _M.nrows(), _M.ncols() - columns.size());
}


//=============================================================================
} // namespace COMISO
//=============================================================================
