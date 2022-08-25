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
#if COMISO_SUITESPARSE_AVAILABLE

#include "UMFPACKSolver.hh"


namespace COMISO {

UMFPACKSolver::UMFPACKSolver()
{
  // initialize zero pointers
  symbolic_ = 0;
  numeric_  = 0;

  tolerance_ = 1e-8;
  
  //  ordering_ = CHOLMOD_AMD;
    
  show_timings_ = false;
}


  //-----------------------------------------------------------------------------
  

UMFPACKSolver::~UMFPACKSolver()
{
    if( symbolic_ )
    {
      umfpack_di_free_symbolic( &symbolic_);
      symbolic_ = 0;
    }

    if( numeric_ )
    {
      umfpack_di_free_numeric( &numeric_);
      numeric_ = 0;
    }
}
  

//-----------------------------------------------------------------------------


bool UMFPACKSolver::calc_system( const std::vector<int>&    _colptr, 
				 const std::vector<int>&    _rowind, 
				 const std::vector<double>& _values)
{
  if(show_timings_) sw_.start();

  colptr_ = _colptr;
  rowind_ = _rowind;
  values_ = _values;

  int n   = colptr_.size()-1;

  // clean up
  if( symbolic_ )
  {
    umfpack_di_free_symbolic( &symbolic_);
    symbolic_ = 0;
  }
  if( numeric_ )
  {
    umfpack_di_free_numeric( &numeric_);
    numeric_ = 0;
  }

  if(show_timings_)
  {
    std::cerr << " UMFPACK Timing cleanup: " << sw_.stop()/1000.0 << "s\n";
    sw_.start();
  }

  int status;

  // symbolic factorization
  status = umfpack_di_symbolic(n,n,&(colptr_[0]), &(rowind_[0]), &(values_[0]), &symbolic_, 0, 0);

  if( status != UMFPACK_OK )
  {
    std::cout << "UMFPACK_symbolic failed" << std::endl;
    print_error( status);
    return false;
  }

  if(show_timings_)
  {
    std::cerr << " SuiteSparseQR_symbolic Timing: " << sw_.stop()/1000.0 << "s\n";
    sw_.start();
  }

  // numeric factorization
  status = umfpack_di_numeric ( &(colptr_[0]), &(rowind_[0]), &(values_[0]), symbolic_, &numeric_, 0, 0);

  if( status != UMFPACK_OK )
  {
    std::cout << "UMFPACK_numeric failed" << std::endl;
    print_error( status);
    return false;
  }

  if(show_timings_)
  {
    std::cerr << " SuiteSparseQR_numeric Timing: " << sw_.stop()/1000.0 << "s\n";
    sw_.start();
  }

  return true;
}



  //-----------------------------------------------------------------------------

    
void UMFPACKSolver::print_error( int _status )
{
  switch(_status)
  {
    case UMFPACK_OK                    : std::cerr << "UMFPACK Error: UMFPACK_OK\n";
    case UMFPACK_ERROR_n_nonpositive   : std::cerr << "UMFPACK Error: UMFPACK_ERROR_n_nonpositive\n";
    case UMFPACK_ERROR_invalid_matrix  : std::cerr << "UMFPACK Error: UMFPACK_ERROR_invalid_matrix\n";
    case UMFPACK_ERROR_out_of_memory   : std::cerr << "UMFPACK Error: UMFPACK_ERROR_out_of_memory\n";
    case UMFPACK_ERROR_argument_missing: std::cerr << "UMFPACK Error: UMFPACK_ERROR_argument_missing\n";
    case UMFPACK_ERROR_internal_error  : std::cerr << "UMFPACK Error: UMFPACK_ERROR_internal_error\n";
    default: std::cerr << "UMFPACK Error: UNSPECIFIED ERROR\n";
  }
}

  //-----------------------------------------------------------------------------

    
bool UMFPACKSolver::update_system( const std::vector<int>& _colptr, 
				   const std::vector<int>& _rowind, 
				   const std::vector<double>& _values )
{
  if( !symbolic_ )
    return false;


  if(show_timings_) sw_.start();

  colptr_ = _colptr;
  rowind_ = _rowind;
  values_ = _values;

  if( numeric_ )
  {
    umfpack_di_free_numeric( &numeric_);
    numeric_ = 0;
  }

  if(show_timings_)
  {
    std::cerr << " UMFPACK Timing cleanup: " << sw_.stop()/1000.0 << "s\n";
    sw_.start();
  }

  int status;

  // numeric factorization
  status = umfpack_di_numeric ( &(colptr_[0]), &(rowind_[0]), &(values_[0]), symbolic_, &numeric_, 0, 0);

  if( status != UMFPACK_OK )
  {
    std::cout << "UMFPACK_numeric failed" << std::endl;
    print_error( status);
    return false;
  }

  if(show_timings_)
  {
    std::cerr << " SuiteSparseQR_numeric Timing: " << sw_.stop()/1000.0 << "s\n";
    sw_.start();
  }

  return true;
}


//-----------------------------------------------------------------------------
  

bool UMFPACKSolver::solve( double * _x, double * _b)
{
  if(show_timings_) sw_.start();

  //  const unsigned int n = colptr_.size() - 1;

  int status = umfpack_di_solve( UMFPACK_A, &(colptr_[0]), &(rowind_[0]), &(values_[0]), 
				 _x, _b, numeric_, 0, 0);


  if( status != UMFPACK_OK )
  {
    std::cout << "UMFPACK_solve failed" << std::endl;
    print_error( status);
    return false;
  }

  if(show_timings_)
  {
    std::cerr << " UMFPACK_sove Timing: " << sw_.stop()/1000.0 << "s\n";
    sw_.start();
  }


    return true;
}


//-----------------------------------------------------------------------------


  
}

//=============================================================================
#endif // COMISO_SUITESPARSE_AVAILABLE
//=============================================================================
