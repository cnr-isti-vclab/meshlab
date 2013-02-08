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



#include "SparseQRSolver.hh"

//== BUILD-TIME DEPENDENCIES =================================================================
#if(COMISO_SUITESPARSE_SPQR_AVAILABLE)
//============================================================================================


namespace COMISO {

SparseQRSolver::SparseQRSolver()
{
    mp_cholmodCommon = new cholmod_common;
    cholmod_l_start( mp_cholmodCommon );

    tolerance_ = 1e-8;

    mp_F = 0;

    ordering_ = CHOLMOD_AMD;
    
    show_timings_ = false;
}


  //-----------------------------------------------------------------------------
  

SparseQRSolver::~SparseQRSolver()
{
    if( mp_F )
    {
      SuiteSparseQR_free<double>( &mp_F, mp_cholmodCommon);
    }

    cholmod_l_finish( mp_cholmodCommon );
    delete mp_cholmodCommon;
    mp_cholmodCommon = NULL;
}
  

//-----------------------------------------------------------------------------


bool SparseQRSolver::calc_system( const std::vector<Int>&    _colptr,
				  const std::vector<Int>&    _rowind,
				  const std::vector<double>& _values)
{
    if(show_timings_) sw_.start();

    colptr_ = _colptr;
    rowind_ = _rowind;
    values_ = _values;

    int n   = colptr_.size()-1;

    cholmod_sparse matA;

    matA.nrow = n;
    matA.ncol = n;
    matA.nzmax = _values.size();

    matA.p = &colptr_[0];
    matA.i = &rowind_[0];
    matA.x = &values_[0];
    matA.nz = 0;
    matA.z = 0;
    
    matA.stype = 0;
    matA.itype = CHOLMOD_LONG;
    matA.xtype = CHOLMOD_REAL;
    matA.dtype = CHOLMOD_DOUBLE;
//    matA.sorted = 1;
    matA.sorted = 1;
    matA.packed = 1;

//    cholmod_l_print_common("CholmodCommon A", mp_cholmodCommon);
    cholmod_l_print_sparse(&matA, "Matrix A", mp_cholmodCommon);

    // clean up
    if( mp_F )
    {
      SuiteSparseQR_free<double>( &mp_F, mp_cholmodCommon);
    }

    if(show_timings_)
    {
      std::cerr << " SuiteSparseQR Timing cleanup: " << sw_.stop()/1000.0 << "s\n";
      sw_.start();
    }

//    if( !(mp_F = SuiteSparseQR_symbolic<double>(ordering_, int(tolerance_!=0.0), &matA, mp_cholmodCommon)))
//    {
//	std::cerr << "SuiteSparseQR_symbolic failed" << std::endl;
//	cholmod_l_print_common("CholmodCommon A", mp_cholmodCommon);
//	return false;
//    }
//    if(show_timings_)
//    {
//      std::cerr << " SuiteSparseQR_symbolic Timing: " << sw_.stop()/1000.0 << "s\n";
//      sw_.start();
//    }
//
//    if( !SuiteSparseQR_numeric<double>( tolerance_, &matA, mp_F, mp_cholmodCommon))
//    {
//	std::cout << "SuiteSparseQR_numeric failed" << std::endl;
//	return false;
//    }

    if( !(mp_F = SuiteSparseQR_factorize<double>(ordering_, tolerance_, &matA, mp_cholmodCommon)))
    {
      std::cerr << "SuiteSparseQR_factorize failed" << std::endl;
      cholmod_l_print_common("CholmodCommon A", mp_cholmodCommon);
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

    
bool SparseQRSolver::update_system( const std::vector<Int>& _colptr,
				   const std::vector<Int>& _rowind,
				   const std::vector<double>& _values )
{
    if( !mp_F )
	return false;

    colptr_ = _colptr;
    rowind_ = _rowind;
    values_ = _values;
    int n   = colptr_.size()-1;

    cholmod_sparse matA;

    matA.nrow = n;
    matA.ncol = n;
    matA.nzmax = _values.size();

    matA.p = &colptr_[0];
    matA.i = &rowind_[0];
    matA.x = &values_[0];
    matA.nz = 0;
    matA.z = 0;
    
    matA.stype = 0;
    matA.itype = CHOLMOD_LONG;
    matA.xtype = CHOLMOD_REAL;
    matA.dtype = CHOLMOD_DOUBLE;
    matA.sorted = 1;
    matA.packed = 1;


    if( !(mp_F = SuiteSparseQR_symbolic<double>(ordering_, int(tolerance_==0.0), &matA, mp_cholmodCommon)))
    {
	std::cout << "SuiteSparseQR_symbolic failed" << std::endl;
	return false;
    }

    return true;
}


//-----------------------------------------------------------------------------
  

bool SparseQRSolver::solve( double * _x, double * _b)
{
    const unsigned int n = colptr_.size() - 1;

    cholmod_dense *x, *Qtb, b;


    b.nrow = n;
    b.ncol = 1;
    b.nzmax = n;
    b.d = b.nrow;
    b.x = _b;
    b.z = 0;
    b.xtype = CHOLMOD_REAL;
    b.dtype = CHOLMOD_DOUBLE;

    // mult by Q^t
    if( !(Qtb = SuiteSparseQR_qmult<double>(0,mp_F, &b, mp_cholmodCommon)))
    {
      std::cout << "sparseqr_solve failed" << std::endl;
          return false;
    }

    // solve triangular system and permute
    if( !(x = SuiteSparseQR_solve<double>( 1, mp_F, Qtb, mp_cholmodCommon )) )
    {
	std::cout << "sparseqr_solve failed" << std::endl;
	return false;
    }
    
    for( unsigned int i = 0; i < n; ++i )
	_x[i] = ((double*)x->x)[i];

    cholmod_l_free_dense( &Qtb, mp_cholmodCommon );
    cholmod_l_free_dense( &x, mp_cholmodCommon );

    return true;
}


//-----------------------------------------------------------------------------


  
}

//== BUILD-TIME DEPENDENCIES ==================================================
#endif
//=============================================================================
