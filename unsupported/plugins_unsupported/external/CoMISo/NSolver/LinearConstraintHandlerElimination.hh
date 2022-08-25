//=============================================================================
//
//  CLASS LinearConstraintHandlerElimination
//
//=============================================================================


#ifndef COMISO_LINEARCONSTRAINTHANDLERELIMINATION_HH
#define COMISO_LINEARCONSTRAINTHANDLERELIMINATION_HH


//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <iostream>
#include <gmm/gmm.h>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================



	      
/** \class LinearConstraintHandler LinearConstraintHandler.hh <ACG/.../LinearConstraintHandler.hh>

    Brief Description.
  
    A more elaborate description follows.
*/


class COMISODLLEXPORT LinearConstraintHandlerElimination
{
public:
   
  typedef gmm::col_matrix< gmm::wsvector< double> > CMatrix;
  typedef gmm::row_matrix< gmm::wsvector< double> > RMatrix;

  // use c-arrays as vectors for gmm
  typedef gmm::array1D_reference<double*> VectorPT;

  /// Constructor
  LinearConstraintHandlerElimination() {initialize_identity(0);}

  // initialize Constructor
  template<class MatrixT, class VectorT>
  LinearConstraintHandlerElimination( const MatrixT& _C, const VectorT& _c)
  {initialize(_C, _c); }

 
  /// Destructor
  ~LinearConstraintHandlerElimination() {}

  // number of variables
  int n() {return n_;}
  // number of reduced variables (after elimination)
  int n_reduced() {return n_red_;}
  // number of linearly independent constraints (n-n_reduced)
  int n_constraints() { return m_;}

  // initialize new constraints
  template<class MatrixT, class VectorT>
  void initialize( const MatrixT& _C, const VectorT& _c);

  // no constraints
  void initialize_identity(int _n);

  // initialize new constraints rhs only
  void initialize( const std::vector<double>& _c);
  void initialize( double* _c);

  // transform x vector (least squares solution, fulfilling the constraints)
  void transform_x( const std::vector<double>& _x, std::vector<double>& _xC);
  void transform_x( double* _x, double* _xC);

  // inverse transform x ( x_reduced -> x)
  void inv_transform_x( const std::vector<double>& _xC, std::vector<double>& _x);
  void inv_transform_x( double* _xC, double* _x);

  // project x to closest one fullfilling the constraints: inv_transform_x(transform_x(x))
  void project_x( const std::vector<double>& _x, std::vector<double>& _xp);
  void project_x( double* _x, double* _xp);

  // project dx to closest one that A(x0+dx) = b
  void project_dx( const std::vector<double>& _x, std::vector<double>& _xp);
  void project_dx( double* _x, double* _xp);

  // transform gradient
  void transform_gradient( const std::vector<double>& _g, std::vector<double>& _gC);
  void transform_gradient( double* _g, double* _gC);

  // transform hessian
  void transform_hessian( const RMatrix& _H, RMatrix& _HC);

private:
  
  // Constraints in basis transformation form x_orig = b_ + C_*x_reduced
  // notice that C_ is a basis of the nullspace of the constraints
  RMatrix             C_;
  RMatrix             Ct_;
  std::vector<double> b_;

  // number of variables
  int n_;
  // number of constraints (linear independent)
  int m_;
  // number of reduced variables
  int n_red_;

  // temp matrix to transform hessian and temp vectors
  RMatrix             Mtemp_;
  std::vector<double> Vtemp_;

  // hack -> store initial linear system
  RMatrix              A_orig_;
  std::vector<double>  b_orig_;
};

//=============================================================================
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(COMISO_LINEARCONSTRAINTHANDLERELIMINATION_C)
#define COMISO_LINEARCONSTRAINTHANDLERELIMINATION_TEMPLATES
#include "LinearConstraintHandlerEliminationT.cc"
#endif
//=============================================================================
#endif // COMISO_LINEARCONSTRAINTHANDLERELIMINATION_HH defined
//=============================================================================

