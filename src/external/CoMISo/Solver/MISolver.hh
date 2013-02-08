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
//  CLASS MISolver
//
//=============================================================================


#ifndef COMISO_MISOLVER_HH
#define COMISO_MISOLVER_HH


//== INCLUDES =================================================================
#include <CoMISo/Config/CoMISoDefines.hh>
#include <CoMISo/Config/config.hh>

#if COMISO_SUITESPARSE_AVAILABLE
  #include "CholmodSolver.hh"
#elif COMISO_EIGEN3_AVAILABLE
  #include "EigenLDLTSolver.hh"
#else
  #print "Warning: MISolver requires Suitesparse or Eigen3 support"
#endif

#include "GMM_Tools.hh"
#include "IterativeSolverT.hh"

#include <vector>

#define ROUND_MI(x) ((x)<0?int((x)-0.5):int((x)+0.5))


//== FORWARDDECLARATIONS ======================================================


namespace COMISO {
class MISolverDialog;
}

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================



/** \class MISolver MISolver.hh 

    Mixed-Integer Solver.
    Approximates the solution of a (mixed-)integer problem
    by iteratively computing a continuous(real) minimizer x followed by a
    rounding of the one variable x_i which is subsequently eliminated from the
    system, and the system is solved again ...
*/

class COMISODLLEXPORT MISolver
{
public:
   
  // typedefs
  typedef gmm::csc_matrix< double >       CSCMatrix;
  typedef std::vector< double >           Vecd;
  typedef std::vector< int >              Veci;
  typedef std::vector< unsigned int >     Vecui;

  // gmm Column and ColumnIterator of CSC matrix
  typedef gmm::linalg_traits< CSCMatrix >::const_sub_col_type Col;
  typedef gmm::linalg_traits< Col >::const_iterator           ColIter;


  /// default Constructor
  MISolver();

  /// Destructor
  ~MISolver() {}


  /// Compute greedy approximation to a mixed integer problem.
	/** @param _A symmetric positive semi-definite CSC matrix (Will be \b destroyed!)
	 *  @param _x vector holding solution at the end
   *  @param _rhs right hand side of system Ax=rhs (Will be \b destroyed!)
   *  @param _to_round vector with variable indices to round to integers
   *  @param _fixed_order specifies if _to_round indices shall be rounded in the
   *  given order (\b true) or be greedily selected (\b false)
	 *  */
  inline void solve(
    CSCMatrix& _A, 
    Vecd&      _x, 
    Vecd&      _rhs, 
    Veci&      _to_round,
    bool       _fixed_order = false );

  void resolve(
    Vecd&      _x,
    Vecd&      _rhs );

  /// Compute greedy approximation to a mixed integer problem.
	/** @param _B mx(n+1) matrix with (still non-squared) equations of the energy,
   * including the right hand side (Will be \b destroyed!)
	 *  @param _x vector holding solution at the end
   *  @param _to_round vector with variable indices to round to integers
   *  @param _fixed_order specifies if _to_round indices shall be rounded in the
   *  given order (\b true) or be greedily selected (\b false)
	 *  */
  template<class CMatrixT>
  void solve( 
    CMatrixT& _B,
    Vecd&     _x,
    Veci&     _to_round,
    bool      _fixed_order = false );


  /// show Qt-Options-Dialog for setting algorithm parameters
  /** Requires a Qt Application running and COMISO_GUI to be defined */
  void show_options_dialog();

  /** @name Get/Set functions for algorithm parameters 
	 * Besides being used by the Qt-Dialog these can also be called explicitly
   * to set parameters of the algorithm. */
	/*@{*/
	/// Shall an initial full solution be computed?
  void set_inital_full( bool _b) {        initial_full_solution_=_b;}
	/// Will an initial full solution be computed?
  bool get_inital_full()         { return initial_full_solution_;}

	/// Shall an full solution be computed if iterative methods did not converged?
  void set_iter_full( bool _b) {        iter_full_solution_=_b;}
	/// Will an full solution be computed if iterative methods did not converged?
  bool get_iter_full()         { return iter_full_solution_;}

	/// Shall a final full solution be computed?
  void set_final_full( bool _b) {        final_full_solution_=_b;}
	/// Will a final full solution be computed?
  bool get_final_full()         { return final_full_solution_;}

  /// Shall direct (or greedy) rounding be used?
  void set_direct_rounding( bool _b) {        direct_rounding_=_b;}
  /// Will direct rounding be used?
  bool get_direct_rounding()         { return direct_rounding_;}

  /// Shall no rounding be performed?
  void set_no_rounding( bool _b) {        no_rounding_=_b;}
  /// Will no rounding be performed?
  bool get_no_rounding()         { return no_rounding_;}

  /// Shall multiple rounding be performed?
  void set_multiple_rounding( bool _b) {       multiple_rounding_=_b;}
  /// Will multiple rounding be performed?
  bool get_multiple_rounding()         { return multiple_rounding_;}

  /// Shall gurobi solver be used?
  void set_gurobi_rounding( bool _b) {        gurobi_rounding_=_b;}
  /// Will gurobi rounding be performed?
  bool get_gurobi_rounding()         { return gurobi_rounding_;}

  /// Shall cplex solver be used?
  void set_cplex_rounding( bool _b) {        cplex_rounding_=_b;}
  /// Will cplex rounding be performed?
  bool get_cplex_rounding()         { return cplex_rounding_;}

  /// Set number of maximum Gauss-Seidel iterations
  void         set_local_iters( unsigned int _i) { max_local_iters_ = _i;}
  /// Get number of maximum Gauss-Seidel iterations
  unsigned int get_local_iters()                 { return max_local_iters_;}

  /// Set error threshold for Gauss-Seidel solver
  void   set_local_error( double _d) { max_local_error_ = _d;}
  /// Get error threshold for Gauss-Seidel solver
  double get_local_error()           { return max_local_error_;}

  /// Set number of maximum Conjugate Gradient iterations 
  void         set_cg_iters( unsigned int _i) { max_cg_iters_ = _i;}
  /// Get number of maximum Conjugate Gradient iterations 
  unsigned int get_cg_iters()                 { return max_cg_iters_;}

  /// Set error threshold for Conjugate Gradient
  void   set_cg_error( double _d) { max_cg_error_ = _d;}
  /// Get error threshold for Conjugate Gradient
  double get_cg_error()           { return max_cg_error_;}

  /// Set multiple rounding threshold (upper bound of rounding performed in each iteration)
  void   set_multiple_rounding_threshold( double _d) { multiple_rounding_threshold_ = _d;}
  /// Get multiple rounding  threshold (upper bound of rounding performed in each iteration)
  double get_multiple_rounding_threshold()           { return multiple_rounding_threshold_;}

  /// Set noise level of algorithm. 0 - quiet, 1 - more noise, 2 - even more, 100 - all noise
  void         set_noise( unsigned int _i) { noisy_ = _i;}
  /// Get noise level of algorithm
  unsigned int get_noise()                 { return noisy_;}

  /// Set time limit for gurobi solver (in seconds)
  void   set_gurobi_max_time( double _d) { gurobi_max_time_ = _d;}
  /// Get time limit for gurobi solver (in seconds)
  double get_gurobi_max_time()          { return gurobi_max_time_;}

  /// Set output statistics of solver
  void set_stats( bool _stats) { stats_ = _stats; }
  /// Get output statistics of solver
  bool get_stats( )            { return stats_; }
	/*@}*/

  /// Set/Get use_constraint_reordering for constraint solver (default = true)
  bool& use_constraint_reordering() { return use_constraint_reordering_;}

 private:

  // find set of variables for simultaneous rounding
  class RoundingSet
  {
  public:
    typedef std::pair<double,int> PairDI;
    
    RoundingSet() : threshold_(0.5), cur_sum_(0.0) {}

    void clear() { rset_.clear(); cur_sum_ = 0.0;}

    bool add( int _id, double _rd_val)
    {
      // empty set? -> always add one element
      if( rset_.empty() || cur_sum_+_rd_val <= threshold_)
      {
	rset_.insert( PairDI(_rd_val,_id) );
	cur_sum_ += _rd_val;
	return true;
      }
      else
      {
	// move to last element
	std::set<PairDI>::iterator s_it = rset_.end();
	--s_it;

	if( s_it->first > _rd_val)
	{
	  cur_sum_ -= s_it->first;
	  rset_.erase(s_it);
	  rset_.insert( PairDI(_rd_val,_id) );
	  cur_sum_ += _rd_val;
	  return true;
	}
      }
      return false;
    }
    
    void set_threshold( double _thres) { threshold_ = _thres; }

    void get_ids( std::vector<int>& _ids )
    {
      _ids.clear();
      _ids.reserve( rset_.size());
      std::set<PairDI>::iterator s_it = rset_.begin();
      for(; s_it != rset_.end(); ++s_it)
	_ids.push_back( s_it->second);
    }

  private:

    double threshold_;
    double cur_sum_;

    std::set<PairDI> rset_;

    std::set<PairDI> test_;
  };

private:

  void solve_no_rounding( 
    CSCMatrix& _A, 
    Vecd&      _x, 
    Vecd&      _rhs );

  void solve_direct_rounding( 
    CSCMatrix& _A, 
    Vecd&      _x, 
    Vecd&      _rhs, 
    Veci&      _to_round);

  void solve_multiple_rounding( 
    CSCMatrix& _A, 
    Vecd&      _x, 
    Vecd&      _rhs, 
    Veci&      _to_round );

  void solve_iterative(
    CSCMatrix& _A, 
    Vecd&      _x, 
    Vecd&      _rhs, 
    Veci&      _to_round,
    bool       _fixed_order );

  void solve_gurobi(
    CSCMatrix& _A,
    Vecd&      _x,
    Vecd&      _rhs,
    Veci&      _to_round );

  inline void solve_cplex(
    CSCMatrix& _A,
    Vecd&      _x,
    Vecd&      _rhs,
    Veci&      _to_round );


void update_solution( 
    CSCMatrix& _A, 
    Vecd&      _x, 
    Vecd&      _rhs, 
    Vecui&     _neigh_i );

private:

  /// Copy constructor (not used)
  MISolver(const MISolver& _rhs);

  /// Assignment operator (not used)
  MISolver& operator=(const MISolver& _rhs);

  // parameters used by the MiSo
  bool initial_full_solution_;
  bool iter_full_solution_;
  bool final_full_solution_;

  bool direct_rounding_;
  bool no_rounding_;
  bool multiple_rounding_;
  bool gurobi_rounding_;
  bool cplex_rounding_;

  double multiple_rounding_threshold_;

  unsigned int max_local_iters_;
  double       max_local_error_;
  unsigned int max_cg_iters_;
  double       max_cg_error_;
  double       max_full_error_;
  unsigned int noisy_;
  bool         stats_;

  // time limit for gurobi solver (in seconds)
  double       gurobi_max_time_;

  // flag
  bool         cholmod_step_done_;

  // declar direct solver depending on availability
#if COMISO_SUITESPARSE_AVAILABLE
  COMISO::CholmodSolver   direct_solver_;
#elif COMISO_EIGEN3_AVAILABLE
  COMISO::EigenLDLTSolver direct_solver_;
#else
  #print "Warning: MISolver requires Suitesparse or Eigen3 support"
#endif

  IterativeSolverT<double> siter_;

  // statistics
  unsigned int n_local_;
  unsigned int n_cg_;
  unsigned int n_full_;

  bool use_constraint_reordering_;

#if(COMISO_QT4_AVAILABLE)
  friend class COMISO::MISolverDialog;
#endif
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_MISOLVER_C)
#define COMISO_MISOLVER_TEMPLATES
#include "MISolverT.cc"
#endif
//=============================================================================
#endif // COMISO_MISOLVER_HH defined
//=============================================================================

