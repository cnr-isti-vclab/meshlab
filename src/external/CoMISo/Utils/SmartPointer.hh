#ifndef SMARTPOINTER_HH
#define SMARTPOINTER_HH


//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

//== DEFINITION =========================================================

/** This set of defines maps the pointer namespaces to the namespace ptr depending on
 *  the current architecture and compilers.
 */
#if (__cplusplus >= 201103L)
   // C++11:
   #include <memory>
   namespace ptr = std;
   #define ACG_UNIQUE_POINTER_SUPPORTED 1
#elif defined(__GXX_EXPERIMENTAL_CXX0X__)
   // C++11 via -std=c++0x on gcc:
   #include <memory>
   namespace ptr = std;
   #define ACG_UNIQUE_POINTER_SUPPORTED 1
#else
   // C++98 and TR1:
   #if (_MSC_VER >= 1600)
     // VStudio 2010 supports some C++11 features
     #include <memory>
     namespace ptr = std;
     #define ACG_UNIQUE_POINTER_SUPPORTED 1
   #elif (_MSC_VER >= 1500)
     // hope for TR1 equivalents
    #if(_HAS_TR1)
     #include <memory>
     namespace ptr = std::tr1;
     #define ACG_UNIQUE_POINTER_SUPPORTED 0
    #else
     #pragma warning "TR1 not available! Please install Visual Studio Service Pack 1!"
    #endif
   #else
     // hope for TR1 equivalents
     #include <tr1/memory>
     namespace ptr = std::tr1;
     #define ACG_UNIQUE_POINTER_SUPPORTED 0
   #endif
#endif


//=============================================================================
#endif // SMARTPOINTER_HH defined
//=============================================================================

