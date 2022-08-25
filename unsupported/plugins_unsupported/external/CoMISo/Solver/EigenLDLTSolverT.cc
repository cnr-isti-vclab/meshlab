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

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if (COMISO_EIGEN3_AVAILABLE)
//== INCLUDES =================================================================

#define COMISO_EIGEN_LDLT_SOLVER_TEMPLATES_C

#include <CoMISo/Solver/GMM_Tools.hh>
#include <CoMISo/Solver/Eigen_Tools.hh>
#include <CoMISo/Solver/EigenLDLTSolver.hh>


namespace COMISO {


template< class GMM_MatrixT>
bool EigenLDLTSolver::calc_system_gmm( const GMM_MatrixT& _mat)
{
  if(show_timings_) sw_.start();

  Eigen::SparseMatrix<double> E;
  COMISO_EIGEN::gmm_to_eigen(_mat, E);

  if(show_timings_)
  {
      std::cerr << "EigenLDLT Timing GMM convert: " << sw_.stop()/1000.0 << "s\n";
      std::cerr << "#nnz: " << E.nonZeros() << std::endl;
  }

  return calc_system_eigen( E);
}
  

//-----------------------------------------------------------------------------


template< class GMM_MatrixT>
bool EigenLDLTSolver::update_system_gmm( const GMM_MatrixT& _mat)
{
  if(show_timings_) sw_.start();

  Eigen::SparseMatrix<double> E;
  COMISO_EIGEN::gmm_to_eigen(_mat, E);

  if(show_timings_)
  {
      std::cerr << "EigenLDLT Timing GMM convert: " << sw_.stop()/1000.0 << "s\n";
      std::cerr << "#nnz: " << E.nonZeros() << std::endl;
  }

  return update_system_eigen( E);
}

//-----------------------------------------------------------------------------
  
template< class Eigen_MatrixT>
bool EigenLDLTSolver::calc_system_eigen( const Eigen_MatrixT& _mat)
{
    n_ = _mat.rows();

    if(show_timings_) sw_.start();

    ldlt_.compute(_mat);

    if(show_timings_)
    {
      std::cerr << "EigenLDLT Timing EIGEN compute: " << sw_.stop()/1000.0 << "s\n";
    }

    return (ldlt_.info()==Eigen::Success);
}
  
//-----------------------------------------------------------------------------

template< class Eigen_MatrixT>
bool EigenLDLTSolver::update_system_eigen( const Eigen_MatrixT& _mat)
{
  if(show_timings_) sw_.start();

  ldlt_.factorize(_mat);

  if(show_timings_)
  {
    std::cerr << "EigenLDLT Timing EIGEN factorize: " << sw_.stop()/1000.0 << "s\n";
  }

  return (ldlt_.info()==Eigen::Success );
}


}

//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
