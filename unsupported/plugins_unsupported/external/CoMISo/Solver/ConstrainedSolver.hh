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
//  CLASS ConstrainedSolver
//
//=============================================================================


#ifndef COMISO_CONSTRAINEDSOLVER_HH
#define COMISO_CONSTRAINEDSOLVER_HH


//== INCLUDES =================================================================
#include <CoMISo/Config/CoMISoDefines.hh>

#include "GMM_Tools.hh"
#include "MISolver.hh"
#include <vector>

//== FORWARDDECLARATIONS ======================================================

//== DEFINES ==================================================================
#define ROUND(x) ((x)<0?int((x)-0.5):int((x)+0.5))
//== NAMESPACES ===============================================================

namespace COMISO {
//== CLASS DEFINITION =========================================================

/** \class ConstrainedSolver ConstrainedSolver.hh <ACG/.../ConstrainedSolver.hh>

  Takes a linear (symmetric) system of equations and a set of linear constraints and solves it.
 */

class COMISODLLEXPORT ConstrainedSolver
{
public:
  typedef gmm::csc_matrix<double>                    CSCMatrix;
  typedef gmm::row_matrix< gmm::wsvector< double > > RowMatrix;


  /// default Constructor
  /** _do_gcd specifies if a greatest common devisor correction should be used when no (+-)1-coefficient is found*/
  ConstrainedSolver( bool _do_gcd = true): do_gcd_(_do_gcd)
  { epsilon_ = 1e-8; noisy_ = 1; }

  /// Destructor
  ~ConstrainedSolver() { }

/** @name Contrained solvers
 * Functions to solve constrained linear systems of the form Ax=b (stemming from quadratic energies). 
 * The constraints can be linear constraints of the form \f$ x_1*c_1+ \cdots +x_n*c_n=c \f$ as well as integer constraints \f$x_i\in \mathbf{Z}\f$. 
 * The system is then solved with these constraints in mind. For solving the system the Mixed-Integer Solver \a MISolver is used. 
 */
/*@{*/

/// Quadratic matrix constrained solver
/**  
  *  Takes a system of the form Ax=b, a constraint matrix C and a set of variables _to_round to be rounded to integers. \f$ A\in \mathbf{R}^{n\times n}\f$
  *  @param _constraints row matrix with rows of the form \f$ [ c_1, c_2, \cdots, c_n, c_{n+1} ] \f$ corresponding to the linear equation \f$ c_1*x_1+\cdots+c_n*x_n + c_{n+1}=0 \f$.
  *  @param _A nxn-dimensional column matrix of the system 
  *  @param _x n-dimensional variable vector
  *  @param _rhs n-dimensional right hand side.
  *  @param _idx_to_round indices i of variables x_i that shall be rounded
  *  @param _reg_factor regularization factor. Helps unstable, low rank system to become solvable. Adds \f$ \_reg\_factor*mean(trace(_A))*Id \f$ to A.
  *  @param _show_miso_settings should the (QT) dialog of the Mixed-Integer solver pop up?
  *  @param _show_timings shall some timings be printed?
  */
  template<class RMatrixT, class CMatrixT, class VectorT, class VectorIT >
  void solve(
      RMatrixT& _constraints,
      CMatrixT& _A, 
      VectorT&  _x,
      VectorT&  _rhs,
      VectorIT& _idx_to_round,
      double    _reg_factor = 0.0,
      bool      _show_miso_settings = true,
      bool      _show_timings = true );

  // const version of above function
  template<class RMatrixT, class CMatrixT, class VectorT, class VectorIT >
  void solve_const(
      const RMatrixT& _constraints,
      const CMatrixT& _A, 
            VectorT&  _x,
      const VectorT&  _rhs,
      const VectorIT& _idx_to_round,
            double    _reg_factor = 0.0,
            bool      _show_miso_settings = true,
            bool      _show_timings = true );

  // efficent re-solve with modified _constraint_rhs and/or _rhs (if not modified use 0 pointer)
  // by keeping previous _constraints and _A fixed
  // _constraint_rhs and _rhs are constant, i.e. not changed
  template<class VectorT >
  void resolve(
       VectorT&  _x,
       VectorT*  _constraint_rhs = 0,
       VectorT*  _rhs            = 0,
       bool      _show_timings = true );


/// Non-Quadratic matrix constrained solver
/**  
  *  Same as above, but performs the elimination of the constraints directly on the B matrix of \f$ x^\top B^\top Bx \f$, where B has m rows (equations) and (n+1) columns \f$ [ x_1, x_2, \cdots, x_n, -rhs ] \f$.
  *  \note This function might be more efficient in some cases, but generally the solver for the quadratic matrix above is a safer bet. Needs further testing.
  *  \note Internally the \f$ A=B^\top B \f$ matrix is formed.
  *  @param _constraints row matrix with rows of the form \f$ [ c_1, c_2, \cdots, c_n, c_{n+1} ] \f$ corresponding to the linear equation \f$ c_1*x_1+\cdots+c_n*x_n + c_{n+1}=0 \f$.
  *  @param _B mx(n+1)-dimensional column matrix of the system 
  *  @param _x n-dimensional variable vector
  *  @param _idx_to_round indices i of variables x_i that shall be rounded
  *  @param _reg_factor regularization factor. Helps unstable, low rank system to become solvable.
  *  @param _show_miso_settings should the (QT) dialog of the Mixed-Integer solver pop up?
  *  @param _show_timings shall some timings be printed?
  */
  template<class RMatrixT, class VectorT, class VectorIT >
  void solve(
      RMatrixT& _constraints,
      RMatrixT& _B, 
      VectorT&  _x,
      VectorIT& _idx_to_round,
      double    _reg_factor = 0.0,
      bool      _show_miso_settings = true,
      bool      _show_timings = true );

  // const version of above function
  template<class RMatrixT, class VectorT, class VectorIT >
  void solve_const(
      const RMatrixT& _constraints,
      const RMatrixT& _B,
      VectorT&  _x,
      const VectorIT& _idx_to_round,
      double    _reg_factor = 0.0,
      bool      _show_miso_settings = true,
      bool      _show_timings = true );

  // efficent re-solve with modified _rhs by keeping previous _constraints and _A fixed
  // ATTENTION: only the rhs resulting from B^TB can be changed!!! otherwise use solve
  template<class RMatrixT, class VectorT >
    void resolve(
      const RMatrixT& _B,
      VectorT&  _x,
      VectorT*  _constraint_rhs = 0,
      bool      _show_timings = true );

/*@}*/


/** @name Eliminate constraints
 * Functions to eliminate (or integrate) linear constraints from an equation system. These functions are used internally by the \a solve functions.
 */
/*@{*/

/// Make constraints independent
/**  
  *  This function performs a Gauss elimination on the constraint matrix making the constraints easier to eliminate. 
  *  \note A certain amount of independence of the constraints is assumed.
  *  \note contradicting constraints will be ignored.
  *  \warning care must be taken when non-trivial constraints occur where some of the variables contain integer-variables (to be rounded) as the optimal result might not always occur.
  *  @param _constraints  row matrix with constraints
  *  @param _idx_to_round indices of variables to be rounded (these must be considered.)
  *  @param _c_elim the "returned" vector of variable indices and the order in which the can be eliminated.
  */
  template<class RMatrixT, class VectorIT >
  void make_constraints_independent(
      RMatrixT&         _constraints,
			VectorIT&         _idx_to_round,
			std::vector<int>& _c_elim );

  template<class RMatrixT, class VectorIT >
  void make_constraints_independent_reordering(
      RMatrixT&         _constraints,
			VectorIT&         _idx_to_round,
			std::vector<int>& _c_elim );

/// Eliminate constraints on a factored matrix B
/**  
  *  \note Constraints are assumed to have been made independent by \a make_constraints_independent.
  *  @param _constraints row matrix with constraints (n+1 columns) 
  *  @param _B system row matrix mx(n+1)
  *  @param _idx_to_round indices to be rounded
  *  @param _c_elim the indices of the variables to be eliminated.
  *  @param _new_idx the created re-indexing map. new_idx[i] = -1 means x_i eliminated, new_idx[i] = j means x_i is now at index j.
  *  @param _Bcol resulting (smaller) column matrix to be used for future computations. (e.g. convert to CSC and solve)
  */
  template<class SVector1T, class SVector2T, class VectorIT, class SVector3T>
  void eliminate_constraints(
      gmm::row_matrix<SVector1T>& _constraints,
			gmm::row_matrix<SVector2T>& _B, 
			VectorIT&                   _idx_to_round,
			std::vector<int>&           _c_elim,
			std::vector<int>&           _new_idx,
			gmm::col_matrix<SVector3T>& _Bcol);

/// Eliminate constraints on a quadratic matrix A
/**  
  *  \note Constraints are assumed to have been made independent by \a make_constraints_independent.
  *  \note _x must have correct size (same as _rhs)
  *  @param _constraints row matrix with constraints (n+1 columns) 
  *  @param _A system row matrix nxn)
  *  @param _x variable vector
  *  @param _rhs right hand side
  *  @param _idx_to_round indices to be rounded
  *  @param _c_elim the indices of the variables to be eliminated.
  *  @param _new_idx the created re-indexing map. new_idx[i] = -1 means x_i eliminated, new_idx[i] = j means x_i is now at index j.
  *  @param _Acsc resulting (smaller) column (csc) matrix to be used for future computations.
  */
 
  template<class SVector1T, class SVector2T, class VectorIT, class CSCMatrixT>
  void eliminate_constraints(
      gmm::row_matrix<SVector1T>& _constraints,
      gmm::col_matrix<SVector2T>& _A, 
      std::vector<double>&        _x, 
      std::vector<double>&        _rhs, 
      VectorIT&                   _idx_to_round,
      std::vector<int>&           _c_elim,
      std::vector<int>&           _new_idx,
      CSCMatrixT&                 _Acsc);

/// Restore a solution vector to the un-eliminated size
/**  
  *  @param _constraints row matrix with constraints (n+1 columns) 
  *  @param _x solution vector to reduced/eliminated system (result will also be written here)
  *  @param _c_elim vector of eliminated indices 
  *  @param _new_idx re-indexing vector
  */
 
  template<class RMatrixT, class VectorT >
  void restore_eliminated_vars( RMatrixT&         _constraints,
				VectorT&          _x,
				std::vector<int>& _c_elim,
				std::vector<int>& _new_idx);


/*@}*/

  /// Set numerical epsilon for valid constraint coefficient
  void set_epsilon( double _epsilon) { epsilon_ = _epsilon; }

  /// Set noise-level (how much std output is given) 0 basically none, 1 important stuff (warning/timing, is default), 2+ not so important
  void set_noisy( int _noisy) { noisy_ = _noisy;}

  // Get/Set whether the constraint reordering is used (default true)
  bool& use_constraint_reordering() { return miso_.use_constraint_reordering(); }

/** @name Verify the result.
 * Functions to verify the result of the constrained solver. Are the constraints met, are the correct variables correctly rounded ...
 */
/*@{*/


  template<class RMatrixT, class CMatrixT, class VectorT>
  double verify_constrained_system( 
				   const RMatrixT& _conditions,
				   const CMatrixT& _A,
				   const VectorT&  _x,
				   const VectorT&  _rhs,
				   double          _eps = 1e-9);

  template<class RMatrixT, class CMatrixT, class VectorT, class VectorIT>
  double verify_constrained_system_round( 
					 const RMatrixT& _conditions,
					 const CMatrixT& _A,
					 const VectorT&  _x,
					 const VectorT&  _rhs,
					 const VectorIT& _idx_to_round,
					 double          _eps = 1e-9);

  template<class RMatrixT, class VectorT, class VectorIT>
  void verify_mi_factored( const RMatrixT& _conditions,
			   const RMatrixT& _B, 
			   const VectorT&  _x,
			   const VectorIT& _idx_to_round );
/*@}*/


  /// Access the MISolver (e.g. to change settings)
  COMISO::MISolver& misolver() { return miso_;}

private:

  template<class RowT, class MatrixT>
  void add_row( int       _row_i,
		double    _coeff,
		RowT      _row, 
		MatrixT&  _mat );

  template<class RowT, class RMatrixT, class CMatrixT>
  void add_row_simultaneously( int       _row_i,
			       double    _coeff,
			       RowT      _row, 
			       RMatrixT& _rmat,
			       CMatrixT& _cmat );


  template<class CMatrixT, class VectorT, class VectorIT>
  double setup_and_solve_system( CMatrixT& _B,
			       VectorT&  _x,
			       VectorIT& _idx_to_round,
			       double    _reg_factor,
			       bool      _show_miso_settings);


  // warning: order of replacement not the same as in _columns (internal sort)
  template<class CMatrixT>
  void eliminate_columns( CMatrixT& _M,
			  const std::vector< int >& _columns);

  inline int gcd( int _a, int _b)
  {
    while( _b != 0)
    {
      int t(_b);
      _b = _a%_b;
      _a = t;
    }
    return _a;
  }

  int find_gcd(std::vector<int>& _v_gcd, int& _n_ints);
  // TODO if no gcd correction was possible, at least use a variable divisible by 2 as new elim_j (to avoid in-exactness e.g. 1/3)
  template<class RMatrixT>
  bool update_constraint_gcd( RMatrixT& _constraints, 
                              int _row_i,
                              int& _elim_j,
                              std::vector<int>& _v_gcd,
                              int& _n_ints);

private:

  /// Copy constructor (not used)
  ConstrainedSolver(const ConstrainedSolver& _rhs);

  /// Assignment operator (not used)
  ConstrainedSolver& operator=(const ConstrainedSolver& _rhs);

  // MISO solver
  COMISO::MISolver miso_;

  double epsilon_;
  int    noisy_;
  bool   do_gcd_;

  // --------------- Update by Marcel to enable efficient re-solve with changed rhs ----------------------
  // Store for symbolic elimination information for rhs
  class rhsUpdateTable {
  public:

    void append(int _i, double _f, int _j, bool _flag)
    {
//      std::cerr << "append " << _i << ", " << _j << ", " << _f << ", " << int(_flag) << std::endl;
      table_.push_back(rhsUpdateTableEntry(_i, _j, _f, _flag));
    }
    void add_elim_id(int _i) { elim_var_ids_.push_back(_i); }
    void clear() { table_.clear(); elim_var_ids_.clear(); }
    // apply stored transformations to _rhs
    void apply(std::vector<double>& _constraint_rhs, std::vector<double>& _rhs)
    {
      std::vector<rhsUpdateTableEntry>::const_iterator t_it, t_end;
      t_end = table_.end();
      int cur_j = -1;
      double cur_rhs = 0.0;
      for(t_it = table_.begin(); t_it != t_end; ++t_it)
      {
        if(t_it->rhs_flag)
            _rhs[t_it->i] += t_it->f*_constraint_rhs[t_it->j];
        else
        {
          if(t_it->j != cur_j) { cur_j = t_it->j; cur_rhs = _rhs[cur_j]; }
          _rhs[t_it->i] += t_it->f * cur_rhs;
        }
      }
    }
    // remove eliminated elements from _rhs
    void eliminate(std::vector<double>& _rhs)
    {
      std::vector<int> evar( elim_var_ids_ );
      std::sort( evar.begin(), evar.end() );
      evar.push_back( std::numeric_limits<int>::max() );

      int cur_evar_idx=0;
      unsigned int nc = _rhs.size();
      for( unsigned int i=0; i<nc; ++i )
      {
        unsigned int next_i = evar[cur_evar_idx];

        if ( i != next_i ) _rhs[i-cur_evar_idx] = _rhs[i];
        else ++cur_evar_idx;
      }
      _rhs.resize( nc - cur_evar_idx );
    }
    // store transformed constraint matrix and index map to allow for later re-substitution
    template<class RMatrixT>
    void store(const RMatrixT& _constraints, const std::vector<int>& _c_elim, const std::vector<int>& _new_idx)
    {
      constraints_p_.resize( gmm::mat_nrows(_constraints), gmm::mat_ncols(_constraints));
      gmm::copy(_constraints, constraints_p_);
      c_elim_ = _c_elim;
      new_idx_ = _new_idx;
    }

  private:
    class rhsUpdateTableEntry {
    public:
      rhsUpdateTableEntry(int _i, int _j, double _f, bool _rhs_flag) : i(_i), j(_j), f(_f), rhs_flag(_rhs_flag) {}
      int i;
      int j;
      double f;
      bool rhs_flag;
    };

    std::vector<rhsUpdateTableEntry> table_;
    std::vector<int> elim_var_ids_;

  public:
    std::vector<int> c_elim_;
    std::vector<int> new_idx_;
    RowMatrix constraints_p_;

    // cache current rhs_ and constraint_rhs_ and linear transformation of constraint_rhs_ D_
    RowMatrix D_;
    std::vector<double> cur_rhs_;
    // constraint_rhs after Gaussian elimination update D*constraint_rhs_orig_
    std::vector<double> cur_constraint_rhs_;
  } rhs_update_table_;

};


//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_CONSTRAINEDSOLVER_C)
#define COMISO_CONSTRAINEDSOLVER_TEMPLATES
#include "ConstrainedSolverT.cc"
#endif
//=============================================================================
#endif // COMISO_CONSTRAINEDSOLVER_HH defined
//=============================================================================

