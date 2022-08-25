//=============================================================================
//
//  CLASS LeastSquaresProblem
//
//=============================================================================


#ifndef COMISO_LeastSquaresProblem_HH
#define COMISO_LeastSquaresProblem_HH


//== INCLUDES =================================================================
#include <CoMISo/Config/config.hh>
#include <CoMISo/Config/CoMISoDefines.hh>
#include <CoMISo/NSolver/NProblemInterface.hh>
#include <CoMISo/NSolver/NConstraintInterface.hh>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class newClass newClass.hh 

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT LeastSquaresProblem : public NProblemInterface
{
public:
   
  /// Default constructor
  LeastSquaresProblem(const int _n_unknowns) :n_(_n_unknowns), x_(_n_unknowns, 0.0) {}
 
  /// Destructor
  ~LeastSquaresProblem() {}


  void add_term(NConstraintInterface* _term);

  double eval_term(const unsigned int _i, const double* _x);
  double eval_term(const unsigned int _i);

  // get reference to solution vector
  const std::vector<double>& x() const {return x_;}
        std::vector<double>& x()       {return x_;}

  // problem definition
  virtual int    n_unknowns   (                                );
  virtual void   initial_x    (       double* _x               );
  virtual double eval_f       ( const double* _x               );
  virtual void   eval_gradient( const double* _x, double*    _g);
  virtual void   eval_hessian ( const double* _x, SMatrixNP& _H);
  virtual void   store_result ( const double* _x               );

  // advanced properties
  virtual bool   constant_hessian();

private:

  // #unknowns
  int n_;

  // current solution vector
  std::vector<double> x_;

  // pointer to terms
  std::vector<NConstraintInterface*> terms_;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_LeastSquaresProblem_HH defined
//=============================================================================

