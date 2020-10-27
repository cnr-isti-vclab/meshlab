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



#include "EigenLDLTSolver.hh"

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#if (COMISO_EIGEN3_AVAILABLE)
//== INCLUDES =================================================================

namespace COMISO {

  EigenLDLTSolver::EigenLDLTSolver() : n_(0)
{

}


  //-----------------------------------------------------------------------------
  

  EigenLDLTSolver::~EigenLDLTSolver()
{
}
  

//-----------------------------------------------------------------------------


bool EigenLDLTSolver::calc_system( const std::vector<int>&    _colptr,
				 const std::vector<int>&    _rowind, 
				 const std::vector<double>& _values)
{
  std::cerr << "Info: EigenLDLTSolver::calc_system( const std::vector<int> ...) not implemented yet..." << std::endl;
  return false;
}


  //-----------------------------------------------------------------------------

    
bool EigenLDLTSolver::update_system( const std::vector<int>& _colptr,
				     const std::vector<int>& _rowind,
				     const std::vector<double>& _values )
{
  std::cerr << "Info: EigenLDLTSolver::update_system( const std::vector<int> ...) not implemented yet..." << std::endl;
  return false;
}


//-----------------------------------------------------------------------------
  

bool EigenLDLTSolver::solve( double * _x, double * _b)
{
  // map arrays to Eigen-Vectors
  Eigen::Map<Eigen::VectorXd> x(_x,n_);
  Eigen::Map<Eigen::VectorXd> b(_b,n_);

  // solve for another right hand side:
  x = ldlt_.solve(b);

  return (ldlt_.info()==Eigen::Success);
}


//-----------------------------------------------------------------------------

int EigenLDLTSolver::dimension()
{
  return n_;
}

//-----------------------------------------------------------------------------

bool EigenLDLTSolver::
solve ( std::vector<double>& _x0, std::vector<double>& _b)
{
  return solve( &(_x0[0]), &(_b[0]));
}

//-----------------------------------------------------------------------------

bool& EigenLDLTSolver::
show_timings()
{
  return show_timings_;
}


}

//=============================================================================
#endif // COMISO_EIGEN3_AVAILABLE
//=============================================================================
