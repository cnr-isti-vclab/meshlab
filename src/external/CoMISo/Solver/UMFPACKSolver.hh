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
//  CLASS UMPFACKSolver
//
//=============================================================================

#ifndef COMISO_UMFPACK_SOLVER_HH
#define COMISO_UMFPACK_SOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_SUITESPARSE_AVAILABLE

//== INCLUDES =================================================================


#include <CoMISo/Config/CoMISoDefines.hh>
#include "GMM_Tools.hh"

#include <CoMISo/Utils/StopWatch.hh>

#include <iostream>
#include <vector>

#include "umfpack.h"


//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================
class COMISODLLEXPORT UMFPACKSolver
{
public:


    // _size is maximal size this instance can handle (smaller problems are possible!!!)
    UMFPACKSolver();
    ~UMFPACKSolver();
    
    bool calc_system( const std::vector<int>&    _colptr, 
		      const std::vector<int>&    _rowind, 
		      const std::vector<double>& _values );


    template< class GMM_MatrixT>
    bool calc_system_gmm( const GMM_MatrixT& _mat);


    bool update_system( const std::vector<int>&    _colptr, 
 			const std::vector<int>&    _rowind, 
 			const std::vector<double>& _values );


    template< class GMM_MatrixT>
    bool update_system_gmm( const GMM_MatrixT& _mat);


    bool solve ( double *             _x0, double *             _b);

    bool solve ( std::vector<double>& _x0, std::vector<double>& _b)
    {return solve( &(_x0[0]), &(_b[0]));}

    double tolerance() { return tolerance_; }

    int ordering() { return ordering_; }

    bool& show_timings() { return show_timings_;}
    

 private:    
    void print_error( int _status );

 private:
    
    void *symbolic_;
    void *numeric_;

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
    // 9. COLAMD for A*A’ or AMD for A
    int ordering_;
    
    std::vector<double> values_;
    std::vector<int>    colptr_;
    std::vector<int>    rowind_;

    bool show_timings_;
    StopWatch sw_;
};

//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_UMFPACK_SOLVER_TEMPLATES_C)
#define COMISO_UMFPACK_SOLVER_TEMPLATES
#include "UMFPACKSolverT.cc"
#endif
//=============================================================================
#endif // COMISO_SUITESPARSE_AVAILABLE
//=============================================================================
#endif // COMISO_UMFPACK_SOLVER_HH defined
//=============================================================================
