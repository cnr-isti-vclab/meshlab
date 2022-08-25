#include "TaucsSolver.hh"

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_TAUCS_AVAILABLE

namespace COMISO {

TaucsSolver::TaucsSolver(int _size, bool _write_log)
    : PAP_(0),
      LSN_(0),
      perm_(0),
      invperm_(0),
      px_(0),
      pb_(0)
  {
    if (_write_log)
       taucs_logfile((char*) "taucs.log");
    resize( _size);
  }


  //-----------------------------------------------------------------------------
  

TaucsSolver::~TaucsSolver()
  {
    // free matrices
    if(LSN_)
    {
      taucs_supernodal_factor_free_numeric(LSN_);
      taucs_supernodal_factor_free( LSN_);
    }

    if(perm_)     delete [] perm_;
    if(invperm_)  delete [] invperm_;
    if(px_)       delete [] px_;
    if(pb_)       delete [] pb_;

  }
  

//-----------------------------------------------------------------------------


void TaucsSolver::calc_system( const std::vector<int>&    _colptr, 
		               const std::vector<int>&    _rowind, 
			       const std::vector<double>& _values)
  {
    colptr_ = _colptr;
    rowind_ = _rowind;
    values_ = _values;

    calc_system();
  }

//-----------------------------------------------------------------------------


void TaucsSolver::calc_system()
  {
    int n   = colptr_.size()-1;

    // free matrices
    //    if(PAP_) taucs_ccs_free(PAP_);
    if(LSN_)
    {
      taucs_supernodal_factor_free_numeric(LSN_);
      taucs_supernodal_factor_free( LSN_);
    }

    mat_.n = n;
    mat_.m = mat_.n;
    mat_.flags = TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_LOWER;

    mat_.colptr   = &colptr_[0];
    mat_.rowind   = &rowind_[0];
    mat_.values.d = &values_[0];

    // calc permutation
    taucs_ccs_order(&mat_, &perm_, &invperm_, (char*)"metis");
    
    // permute Matrix
    PAP_ = taucs_ccs_permute_symmetrically(&mat_, perm_, invperm_);

    // factor symbolic
    LSN_ = taucs_ccs_factor_llt_symbolic( PAP_);

    // factor numeric
    taucs_ccs_factor_llt_numeric(PAP_,LSN_);

    // free permuted matrix
    taucs_ccs_free(PAP_);
  }


  //-----------------------------------------------------------------------------


void TaucsSolver::update_system( const std::vector<int>&    _colptr, 
				 const std::vector<int>&    _rowind, 
				 const std::vector<double>& _values)
  {
    colptr_ = _colptr;
    rowind_ = _rowind;
    values_ = _values;
    
    update_system();
  }


  //-----------------------------------------------------------------------------


void TaucsSolver::update_system()
  {
    int n   = colptr_.size()-1;

    mat_.n = n;
    mat_.m = mat_.n;
    mat_.flags = TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_LOWER;

    mat_.colptr   = &colptr_[0];
    mat_.rowind   = &rowind_[0];
    mat_.values.d = &values_[0];

    // free numeric part of matrix
    if(LSN_){taucs_supernodal_factor_free_numeric(LSN_);}

    // permute Matrix
    PAP_ = taucs_ccs_permute_symmetrically(&mat_, perm_, invperm_);

    // factor numeric
    taucs_ccs_factor_llt_numeric(PAP_,LSN_);

    // free permuted matrix
    taucs_ccs_free(PAP_);
  }


  //-----------------------------------------------------------------------------
  

  void TaucsSolver::solve( double * _x, double * _b)
  {
    // solve x coords
    taucs_vec_permute(mat_.n, mat_.flags, _b, pb_, perm_);
    taucs_supernodal_solve_llt(LSN_, px_, pb_);
    taucs_vec_ipermute(mat_.n, mat_.flags, px_, _x, perm_);
  }


  //-----------------------------------------------------------------------------
  

  void TaucsSolver::solve_cg( double* _x0, double* _b, int _maxiter, double _max_error, bool /*_precond*/)
  {
//     // preconditioner
//     taucs_ccs_matrix *M;
//     M = taucs_ccs_factor_llt(&mat_, 1E-1, 1);

//     // solve system
//     taucs_conjugate_gradients( &mat_, taucs_ccs_solve_llt, M, _x0, _b, _maxiter, _max_error );

    // solve system
    taucs_conjugate_gradients( &mat_, 0, 0, _x0, _b, _maxiter, _max_error );
  }


  //-----------------------------------------------------------------------------


  void TaucsSolver::eliminate_var( int _i, double _xi, double* _x, double* _rhs)
  {
    int n = colptr_.size()-1;

    int iv_old(0);
    int iv_new(0);

    // for all columns
    for(int j=0; j<(int)colptr_.size()-1; ++j)
    {
      // update x and rhs
      if( j > _i)
      {
	_rhs[j-1] = _rhs[j];
	_x  [j-1] = _x  [j];
      }

      if( j == _i)
      {
	// update rhs
	for(int i=colptr_[j]; i<colptr_[j+1]; ++i)
	{
	  _rhs[rowind_[iv_old]] -= _xi*values_[iv_old];
	  ++iv_old;
	}
      }
      else
      {
	// store index to update colptr
	int iv_new_save = iv_new;

	for(int i=colptr_[j]; i<colptr_[j+1]; ++i)
	{
	  if( rowind_[iv_old] < _i)
	  {
	    rowind_[iv_new] = rowind_[iv_old];
	    values_[iv_new] = values_[iv_old];
	    ++iv_new;
	  }
	  else if( rowind_[iv_old] > _i)
	  {
	    rowind_[iv_new] = rowind_[iv_old]-1;
	    values_[iv_new] = values_[iv_old];
	    ++iv_new;
	  }
	  else
	  {
	    if( j< _i)
	      _rhs[j] -= _xi*values_[iv_old];
	    else
	      _rhs[j-1] -= _xi*values_[iv_old];

	  }
	  ++iv_old;
	}
	
	if( j<_i)
	  colptr_[j] = iv_new_save;
	else
	  if( j>_i)
	    colptr_[j-1] = iv_new_save;
      }
    }

    // store index to end
    colptr_[colptr_.size()-2] = iv_new;
    // resize vectors
    colptr_.resize( colptr_.size()-1);
    values_.resize( iv_new);
    rowind_.resize( iv_new);

    mat_.n = n-1;
    mat_.m = n-1;
  }


  //-----------------------------------------------------------------------------


  void TaucsSolver::resize( int _size)
  {
    if(perm_)     delete [] perm_;
    if(invperm_)  delete [] invperm_;
    if(px_)       delete [] px_;
    if(pb_)       delete [] pb_;

    perm_    = new int[_size];
    invperm_ = new int[_size];
    px_      = new double[_size];
    pb_      = new double[_size];
  }

  
}

//=============================================================================
#endif // COMISO_TAUCS_AVAILABLE