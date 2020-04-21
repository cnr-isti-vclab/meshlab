//=============================================================================
//
//  CLASS TaucsSolver
//
//  Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
//  Version: $Revision: 1$
//  Date:    $Date: 02-04-200X$
//
//=============================================================================

#ifndef COMISO_TAUCS_SOLVER_HH
#define COMISO_TAUCS_SOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_TAUCS_AVAILABLE

//== INCLUDES =================================================================
#include <CoMISo/Config/CoMISoDefines.hh>

#include "GMM_Tools.hh"
#include <iostream>
#include <vector>

#ifdef WIN32
        extern "C"
        {
                #include <taucs.h>
        }
        #undef min
        #undef max
#else
        #include <taucs.h>
#endif


//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================
class COMISODLLEXPORT TaucsSolver
  {
  public:

    // _size is maximal size this instance can handle (smaller problems are possible!!!)
    TaucsSolver(int _size, bool _write_log = false);
    ~TaucsSolver();
    
    void calc_system( const std::vector<int>&    _colptr, 
		      const std::vector<int>&    _rowind, 
		      const std::vector<double>& _values );


    template< class GMM_MatrixT>
    void calc_system_gmm( const GMM_MatrixT& _mat);


    void update_system( const std::vector<int>&    _colptr, 
			const std::vector<int>&    _rowind, 
			const std::vector<double>& _values );


    template< class GMM_MatrixT>
    void update_system_gmm( const GMM_MatrixT& _mat);


    void solve( std::vector<double>& _x0, std::vector<double>& _b) { solve( &(_x0[0]), &(_b[0])); }
    void solve( double* _x0, double* _b);

    void resize       ( int _size);

    // CG support

    template< class GMM_MatrixT>
    void init_system_gmm( const GMM_MatrixT& _mat);

    void solve_cg( std::vector<double>& _x0, std::vector<double>& _b, int _maxiter = 50, double _max_error=1e-6, bool _precond=true) { solve_cg( &(_x0[0]), &(_b[0]), _maxiter, _max_error, _precond); }
    void solve_cg( double* _x0, double* _b, int _maxiter = 50, double _max_error=1e-6, bool _precond=true);
    
    void eliminate_var( int _i, double _xi, std::vector<double>& _x, std::vector<double>& _rhs) {eliminate_var(_i, _xi, &(_x[0]), &(_rhs[0])); _rhs.resize(_rhs.size()-1); _x.resize(_x.size()-1);}
    void eliminate_var( int _i, double _xi, double* _x, double* _rhs);

    template< class GMM_MatrixT>
    void get_matrix_gmm( GMM_MatrixT& _mat);


    void calc_system();
    void update_system();

  private:

    taucs_ccs_matrix   mat_;
    taucs_ccs_matrix  *PAP_;
    
    void              *LSN_;

    int               *perm_;
    int               *invperm_;

    double            *px_;
    double            *pb_;


    std::vector<double> values_;
    std::vector<int>    rowind_;
    std::vector<int>    colptr_;
  };

//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_TAUCS_SOLVER_TEMPLATES_C)
#define COMISO_TAUCS_SOLVER_TEMPLATES
#include "TaucsSolverT.cc"
#endif
//=============================================================================
#endif // COMISO_TAUCS_AVAILABLE
//=============================================================================
#endif // COMISO_TAUCS_SOLVER_HH defined
//=============================================================================
