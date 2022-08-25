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



#define COMISO_UMFPACK_SOLVER_TEMPLATES_C

#include "UMFPACKSolver.hh"


namespace COMISO {


template< class GMM_MatrixT>
bool UMFPACKSolver::calc_system_gmm( const GMM_MatrixT& _mat)
{
//   std::vector<int>    colptr;
//   std::vector<int>    rowind;
//   std::vector<double> values;
    

    if(show_timings_) sw_.start();

    COMISO_GMM::get_ccs_symmetric_data( _mat,
					'c',
					values_, 
					rowind_, 
					colptr_ );
    
    if(show_timings_)
    {
      std::cerr << "UMFPACK Timing GMM convert: " << sw_.stop()/1000.0 << "s\n";
    }

    return calc_system( colptr_, rowind_, values_);
}
  

  //-----------------------------------------------------------------------------


template< class GMM_MatrixT>
bool UMFPACKSolver::update_system_gmm( const GMM_MatrixT& _mat)
{
//   std::vector<int>    colptr;
//   std::vector<int>    rowind;
//   std::vector<double> values;
    
  COMISO_GMM::get_ccs_symmetric_data( _mat,
				      'c',
				      values_, 
				      rowind_, 
				      colptr_ );

    return update_system( colptr_, rowind_, values_);
}


}
