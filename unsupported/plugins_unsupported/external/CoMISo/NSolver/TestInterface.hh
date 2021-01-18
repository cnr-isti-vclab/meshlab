//=============================================================================
//
//  CLASS BaseTaoGmmInterface
//
//=============================================================================


#ifndef COMISO_TESTINTERFACE_HH
#define COMISO_TESTINTERFACE_HH


//== INCLUDES =================================================================

#include "NProblemGmmInterface.hh"
#include <gmm/gmm.h>

#include <CoMISo/Config/CoMISoDefines.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class BaseTaoGmmInterface BaseTaoGmmInterface.hh <ACG/.../BaseTaoGmmInterface.hh>

    Brief Description.
  
    A more elaborate description follows.
*/

class COMISODLLEXPORT TestInterface : public NProblemGmmInterface
{
public:
  
  /// Default constructor
  TestInterface() {}
 
  /// Destructor
  ~TestInterface() {}

  // minimize (x-2.4)^2

  virtual int    n_unknowns  (                              )
  { 
    return 1; 
  }

  virtual void   initial_x        (       double* _x             )
  {
    _x[0] = 100;
  }

  virtual double eval_f       ( const double* _x             )
  {
    return (_x[0]-2.4)*(_x[0]-2.4);
  }

  virtual void   eval_gradient( const double* _x, double*  _g)
  {
    _g[0] = 2.0*(_x[0]-2.4);
  }

  virtual void   eval_hessian( const double* _x, SMatrixNP& _H)
  {
    gmm::resize(_H,1,1);
    _H(0,0) = 2.0;
  }

  virtual void   store_result     ( const double* _x             )
  {
    std::cerr << "result: " << _x[0] << std::endl;
  }
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_TESTINTERFACE_HH defined
//=============================================================================

