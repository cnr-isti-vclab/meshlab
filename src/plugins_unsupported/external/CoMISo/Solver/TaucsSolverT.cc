#define COMISO_TAUCS_SOLVER_TEMPLATES_C

#include "TaucsSolver.hh"


//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_TAUCS_AVAILABLE

namespace COMISO {


template< class GMM_MatrixT>
void TaucsSolver::calc_system_gmm( const GMM_MatrixT& _mat)
  {
    COMISO_GMM::get_ccs_symmetric_data( _mat,
				 'l',
				   values_, 
				 rowind_, 
				 colptr_ );

    calc_system();
  }
  

  //-----------------------------------------------------------------------------


template< class GMM_MatrixT>
void TaucsSolver::update_system_gmm( const GMM_MatrixT& _mat)
  {
    COMISO_GMM::get_ccs_symmetric_data( _mat,
				 'l',
				   values_, 
				 rowind_, 
				 colptr_ );

    update_system();
  }



  //-----------------------------------------------------------------------------


template< class GMM_MatrixT>
void TaucsSolver::init_system_gmm( const GMM_MatrixT& _mat)
  {
    COMISO_GMM::get_ccs_symmetric_data( _mat,
				 'l',
				   values_, 
				 rowind_, 
				 colptr_ );
    int n   = colptr_.size()-1;

    mat_.n = n;
    mat_.m = n;
    mat_.flags = TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_LOWER;

    mat_.colptr   = &(colptr_[0]);
    mat_.rowind   = &(rowind_[0]);
    mat_.values.d = &(values_[0]);

  }


//-----------------------------------------------------------------------------

template< class GMM_MatrixT>
void TaucsSolver::get_matrix_gmm( GMM_MatrixT& _mat)
  {
    gmm::resize( _mat, mat_.n, mat_.n);
    gmm::clear( _mat);
 
    // for all columns
    for(int j=0; j<(int)colptr_.size()-1; ++j)
    {
      // for all lower triangular values
      for(int i=colptr_[j]; i<colptr_[j+1]; ++i)
      {
	// set value
	_mat(rowind_[i],j) = values_[i];
	// symmetric part
	_mat(j,rowind_[i]) = values_[i];
      }
    }
  }

}

//=============================================================================
#endif // COMISO_TAUCS_AVAILABLE
//=============================================================================