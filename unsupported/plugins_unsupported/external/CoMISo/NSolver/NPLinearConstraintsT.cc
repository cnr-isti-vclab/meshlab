//=============================================================================
//
//  CLASS NPLinearConstraints
//
//=============================================================================

#define COMISO_NSOLVERGMMINTERFACE_C

//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>
#include <gmm/gmm.h>
#include "NProblemGmmInterface.hh"
#include "LinearConstraintHandlerElimination.hh"
#include "LinearConstraintHandlerPenalty.hh"
//#include "LinearConstraintHandlerLagrange.hh"


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO {

//== CLASS DEFINITION =========================================================

// initialize constraints
template<class MatrixT, class VectorT>
void NPLinearConstraintsElimination::initialize_constraints( const MatrixT& _C, const VectorT& _c)
{
    initialize(_C, _c);
    cache_initialized_ = false;
}

//=============================================================================

// initialize constraints
template<class MatrixT, class VectorT>
void NPLinearConstraintsPenalty::initialize_constraints( const MatrixT& _C, const VectorT& _c)
{
    initialize(_C, _c);
}


//=============================================================================
} // namespace ACG
//=============================================================================


