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
//  CLASS CholmodSolver
//
//=============================================================================

#ifndef COMISO_SPARSE_QR_SOLVER_HH
#define COMISO_SPARSE_QR_SOLVER_HH


//== INCLUDES =================================================================


#include <CoMISo/Config/CoMISoDefines.hh>
#include <CoMISo/Config/config.hh>
#include "GMM_Tools.hh"

#include <CoMISo/Utils/StopWatch.hh>

#include <iostream>
#include <vector>


//== BUILD-TIME DEPENDENCIES =================================================================
#if(COMISO_SUITESPARSE_SPQR_AVAILABLE)
//============================================================================================

#include "SuiteSparseQR.hpp"
#include "cholmod.h"


//typedef struct cholmod_common_struct               cholmod_common;

//== NAMESPACES ===============================================================

namespace COMISO {

typedef struct SuiteSparseQR_factorization<double> sparseqr_factor;


//== CLASS DEFINITION =========================================================
class COMISODLLEXPORT SparseQRSolver
{
public:

  typedef UF_long Int;


    SparseQRSolver();
    ~SparseQRSolver();
    
    bool calc_system( const std::vector<Int>&    _colptr,
		      const std::vector<Int>&    _rowind,
		      const std::vector<double>& _values );


    template< class GMM_MatrixT>
    bool calc_system_gmm( const GMM_MatrixT& _mat);


    template< class GMM_MatrixT>
    bool update_system_gmm( const GMM_MatrixT& _mat);

    template< class Eigen_MatrixT>
    bool calc_system_eigen( const Eigen_MatrixT& _mat);


    template< class Eigen_MatrixT>
    bool update_system_eigen( const Eigen_MatrixT& _mat);

    bool update_system( const std::vector<Int>&    _colptr,
 			const std::vector<Int>&    _rowind,
 			const std::vector<double>& _values );



    bool solve ( double *             _x0, double *             _b);

    bool solve ( std::vector<double>& _x0, const std::vector<double>& _b)
    {return solve( &(_x0[0]), (double*)&(_b[0]));}

    double tolerance() { return tolerance_; }

    int ordering() { return ordering_; }

    bool& show_timings() { return show_timings_;}
    
    // factorize _A*P = _Q*_R and return rank
    template< class GMM_MatrixT, class GMM_MatrixT2, class GMM_MatrixT3, class IntT>
    int factorize_system_gmm( const GMM_MatrixT& _A, GMM_MatrixT2& _Q, GMM_MatrixT3& _R, std::vector<IntT>& _P);

    // factorize _A*P = _Q*_R and return rank
    template< class Eigen_MatrixT, class IntT >
    int factorize_system_eigen( const Eigen_MatrixT& _A, Eigen_MatrixT& _Q, Eigen_MatrixT& _R, std::vector<IntT>& _P);

    // Solve Ax=b, not the most efficient as it uses SparseMatrices also for _b and _x
    template< class Eigen_MatrixT>
    void solve_system_eigen( const Eigen_MatrixT& _A, const Eigen_MatrixT& _b, Eigen_MatrixT& _x);

    // Solve Ax=b, min( ||x||_2) not the most efficient as it uses SparseMatrices also for _b and _x
    template< class Eigen_MatrixT>
    void solve_system_eigen_min2norm( const Eigen_MatrixT& _A, const Eigen_MatrixT& _b, Eigen_MatrixT& _x);


private:

    cholmod_common * mp_cholmodCommon;

    sparseqr_factor * mp_F;

    double tolerance_;

    // specify Permutation ordering
    // 1. user-provided permutation (only for cholmod analyze p).
    // 2. AMD with default parameters.
    // 3. METIS with default parameters.
    // 4. NESDIS with default parameters
    // 5. natural ordering (with weighted postorder).
    // 6. NESDIS, nd small = 20000, prune dense = 10.
    // 7. NESDIS, nd small = 4, prune dense = 10, no constrained minimum degree.
    // 8. NESDIS, nd small = 200, prune dense = 0.
    // 9. COLAMD for A*A^T or AMD for A
    int ordering_;
    
    // dimension of the mxn matrix
    int m_;
    int n_;

    std::vector<double> values_;
    std::vector<Int>    colptr_;
    std::vector<Int>    rowind_;

    bool show_timings_;
    StopWatch sw_;
};

//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_SPARSE_QR_SOLVER_TEMPLATES_C)
#define COMISO_SPARSE_QR_SOLVER_TEMPLATES
#include "SparseQRSolverT.cc"
#endif
//== BUILD-TIME DEPENDENCIES ==================================================
#endif
//=============================================================================
#endif // COMISO_SPARSE_QR_SOLVER_HH defined
//=============================================================================
