//=============================================================================
//
//  ENUM VariableType
//
//=============================================================================


#ifndef COMISO_VARIABLETYPE_HH
#define COMISO_VARIABLETYPE_HH


//== INCLUDES =================================================================

#include <CoMISo/Config/CoMISoDefines.hh>


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace COMISO { 

//== CLASS DEFINITION =========================================================

enum VariableType { Real, Integer, Binary};

typedef std::pair<unsigned int, VariableType> PairIndexVtype;

//=============================================================================
} // namespace COMISO
//=============================================================================
#endif // COMISO_VARIABLETYPE_HH defined
//=============================================================================

