//=============================================================================
//
//  CLASS NProblemGmmInterface
//  **************** DEPRECATED -> Please use NProblemInterface ***************//
//=============================================================================


#ifndef COMISO_NPROBLEMGMMINTERFACE_HH
#define COMISO_NPROBLEMGMMINTERFACE_HH


//== INCLUDES =================================================================

#include <gmm/gmm.h>

#include <CoMISo/Config/CoMISoDefines.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class NProblemGmmInterface NProblemGmmInterface.hh <ACG/.../NPRoblemGmmInterface.hh>

    Brief Description.
  
    A more elaborate description follows.
*/

//  *** This class is DEPRECATED -> Please use NProblemInterface ***//
class COMISODLLEXPORT NProblemGmmInterface
{
public:
  
  // ToDo: appropriate MatrixType ???
  typedef gmm::row_matrix< gmm::wsvector<double> > SMatrixNP;

  /// Default constructor
  NProblemGmmInterface()
  {std::cerr << "Warning: NProblemGmmInterface is deprecated -> use NProblemInterface instead!!!" << std::endl;}
 
  /// Destructor
  ~NProblemGmmInterface() {}

  virtual int    n_unknowns   (                                ) = 0;
  virtual void   initial_x    (       double* _x               ) = 0;
  virtual double eval_f       ( const double* _x               ) = 0;
  virtual void   eval_gradient( const double* _x, double*    _g) = 0;
  virtual void   eval_hessian ( const double* _x, SMatrixNP& _H) = 0;
  virtual void   store_result ( const double* _x               ) = 0;
};


//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_NPROBLEMGMMINTERFACE_HH defined
//=============================================================================

