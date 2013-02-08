//=============================================================================
//
//  CLASS TAOSolver
//
//=============================================================================


#ifndef COMISO_TAOSOLVER_HH
#define COMISO_TAOSOLVER_HH

//== COMPILE-TIME PACKAGE REQUIREMENTS ========================================
#include <CoMISo/Config/config.hh>
#if COMISO_TAO_AVAILABLE

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include "NProblemGmmInterface.hh"
#include "NProblemInterface.hh"

#include <mpi.h>
#include <tao.h>
#include <gmm/gmm.h>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

	      

/** \class TAOSolver TAOSolver.hh <ACG/.../TAOSolver.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class COMISODLLEXPORT TAOSolver
{
public:
   
  /// Default constructor
  TAOSolver() {}


  /// Destructor
  ~TAOSolver() {}

  // solve problem
  static int solve( NProblemInterface*    _base);
  static int solve( NProblemGmmInterface* _base);


private:

  static void initialize();

  // ToDo: cleanup has to be started automatically
  static void cleanup();

#if (TAO_VERSION_MAJOR < 2)
  // declar TAO function prototypes
  static int objective(TAO_APPLICATION,Vec,double*,void*);
  static int gradient (TAO_APPLICATION,Vec,Vec    ,void*);
  static int hessian  (TAO_APPLICATION,Vec,Mat*,Mat*,MatStructure*,void*);

  static int objective2(TAO_APPLICATION,Vec,double*,void*);
  static int gradient2 (TAO_APPLICATION,Vec,Vec    ,void*);
  static int hessian2  (TAO_APPLICATION,Vec,Mat*,Mat*,MatStructure*,void*);
#else
  static PetscErrorCode objective(TaoSolver,Vec,double*,void*);
  static PetscErrorCode gradient (TaoSolver,Vec,Vec    ,void*);
  static PetscErrorCode hessian  (TaoSolver,Vec,Mat*,Mat*,MatStructure*,void*);

  static PetscErrorCode objective2(TaoSolver,Vec,double*,void*);
  static PetscErrorCode gradient2 (TaoSolver,Vec,Vec    ,void*);
  static PetscErrorCode hessian2  (TaoSolver,Vec,Mat*,Mat*,MatStructure*,void*);
#endif

private:
  // initialized?
  static bool initialized_;
};


//=============================================================================
} // namespace COMISO

//=============================================================================
#endif // COMISO_TAO_AVAILABLE
//=============================================================================
#endif // ACG_TAOSOLVER_HH defined
//=============================================================================

