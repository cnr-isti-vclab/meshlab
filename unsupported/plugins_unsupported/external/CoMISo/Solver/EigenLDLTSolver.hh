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

#ifndef COMISO_EIGEN_LDLT_SOLVER_HH
#define COMISO_EIGEN_LDLT_SOLVER_HH


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if (COMISO_EIGEN3_AVAILABLE)
//== INCLUDES =================================================================


#include <CoMISo/Config/CoMISoDefines.hh>
#include <CoMISo/Utils/StopWatch.hh>

#include <iostream>
#include <vector>


#include <Eigen/Eigen>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>


//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================
class COMISODLLEXPORT EigenLDLTSolver
{
public:

    // _size is maximal size this instance can handle (smaller problems are possible!!!)
  EigenLDLTSolver();
  ~EigenLDLTSolver();
    
    bool calc_system( const std::vector<int>&    _colptr,
		      const std::vector<int>&    _rowind,
		      const std::vector<double>& _values );


    template< class GMM_MatrixT>
    bool calc_system_gmm( const GMM_MatrixT& _mat);

    template< class Eigen_MatrixT>
    bool calc_system_eigen( const Eigen_MatrixT& _mat);


    bool update_system( const std::vector<int>&    _colptr,
 			const std::vector<int>&    _rowind,
 			const std::vector<double>& _values );


    template< class GMM_MatrixT>
    bool update_system_gmm( const GMM_MatrixT& _mat);

    template< class Eigen_MatrixT>
    bool update_system_eigen( const Eigen_MatrixT& _mat);


    bool solve ( double *             _x0, double *             _b);

    bool solve ( std::vector<double>& _x0, std::vector<double>& _b);

    bool& show_timings();
    
    int dimension();
    
private:

    // dimension n_
    unsigned int n_;

    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > ldlt_;

    bool show_timings_;
    StopWatch sw_;
};

//=============================================================================
} // namespace COMISO
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_EIGEN_LDLT_SOLVER_TEMPLATES_C)
#define COMISO_EIGEN_LDLT_SOLVER_TEMPLATES
#include "EigenLDLTSolverT.cc"
#endif
//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
#endif // COMISO_EIGEN_LDLT_SOLVER_HH defined
//=============================================================================
