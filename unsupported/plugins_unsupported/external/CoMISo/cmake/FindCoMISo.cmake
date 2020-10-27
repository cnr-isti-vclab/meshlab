# - Try to find COMISO
# Once done this will define
#  
#  COMISO_FOUND        - system has COMISO
#  COMISO_INCLUDE_DIR  - the COMISO include directory
#  COMISO_LIBRARY_DIR  - where the libraries are
#  COMISO_LIBRARY      - Link these to use COMISO


IF (COMISO_INCLUDE_DIR)
  # Already in cache, be silent
  SET(COMISO_FIND_QUIETLY TRUE)
ENDIF (COMISO_INCLUDE_DIR)


# Find CoMISo config file
FIND_PATH( COMISO_INCLUDE_DIR CoMISo/Config/config.hh
           PATHS "${CMAKE_SOURCE_DIR}/../" "${CMAKE_SOURCE_DIR}/libs/" )

if ( COMISO_INCLUDE_DIR )

  FILE(READ ${COMISO_INCLUDE_DIR}/CoMISo/Config/config.hh CURRENT_COMISO_CONFIG)

  set(COMISO_OPT_DEPS "")


  STRING(REGEX MATCH "\#define COMISO_MPI_AVAILABLE 1" COMISO_MPI_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_MPI_BUILD_TIME_AVAILABLE )

   find_package(MPI)

   if ( NOT MPI_FOUND )
     message(ERROR "COMISO configured with mpi but mpi not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "MPI")

  endif()

  STRING(REGEX MATCH "\#define COMISO_BOOST_AVAILABLE 1" COMISO_BOOST_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_BOOST_BUILD_TIME_AVAILABLE )
   
   find_package( Boost 1.42.0 COMPONENTS system filesystem regex QUIET)

   if ( NOT Boost_FOUND )
     message(ERROR "COMISO configured with Boost but Boost not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "Boost")

  endif()


  STRING(REGEX MATCH "\#define COMISO_SUITESPARSE_AVAILABLE 1" COMISO_SUITESPARSE_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_SUITESPARSE_BUILD_TIME_AVAILABLE )

   find_package(SUITESPARSE)

   if ( NOT SUITESPARSE_FOUND )
     message(ERROR "COMISO configured with Suitesparse but Suitesparse not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "SUITESPARSE")

  endif()
  
  STRING(REGEX MATCH "\#define COMISO_PETSC_AVAILABLE 1" COMISO_PETSC_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_PETSC_BUILD_TIME_AVAILABLE )

   find_package(PETSC)

   if ( NOT PETSC_FOUND )
     message(ERROR "COMISO configured with petsc but petsc not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "PETSC")

  endif()

  STRING(REGEX MATCH "\#define COMISO_IPOPT_AVAILABLE 1" COMISO_IPOPT_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_IPOPT_BUILD_TIME_AVAILABLE )

   find_package(IPOPT)

   if ( NOT IPOPT_FOUND )
     message(ERROR "COMISO configured with ipopt but ipopt not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "IPOPT")

  endif()

  STRING(REGEX MATCH "\#define COMISO_METIS_AVAILABLE 1" COMISO_METIS_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_METIS_BUILD_TIME_AVAILABLE )

   find_package(METIS)

   if ( NOT METIS_FOUND )
     message(ERROR "COMISO configured with Metis but Metis not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "METIS")

  endif()
  
  STRING(REGEX MATCH "\#define COMISO_MUMPS_AVAILABLE 1" COMISO_MUMPS_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_MUMPS_BUILD_TIME_AVAILABLE )

   find_package(MUMPS)

   if ( NOT MUMPS_FOUND )
     message(ERROR "COMISO configured with mumps but mumps not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "MUMPS")

  endif()

  STRING(REGEX MATCH "\#define COMISO_TAO_AVAILABLE 1" COMISO_TAO_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_TAO_BUILD_TIME_AVAILABLE )

   find_package(TAO)

   if ( NOT TAO_FOUND )
     message(ERROR "COMISO configured with tao but tao not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "TAO")
  endif()
  
  STRING(REGEX MATCH "\#define COMISO_TAUCS_AVAILABLE 1" COMISO_TAUCS_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_TAUCS_BUILD_TIME_AVAILABLE )

   find_package(Taucs)

   if ( NOT TAUCS_FOUND )
     message(ERROR "COMISO configured with Taucs but Taucs not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "Taucs")

  endif()

  STRING(REGEX MATCH "\#define COMISO_GUROBI_AVAILABLE 1" COMISO_GUROBI_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_GUROBI_BUILD_TIME_AVAILABLE )

   find_package(GUROBI)

   if ( NOT GUROBI_FOUND )
     message(ERROR "COMISO configured with GUROBI but GUROBI not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "GUROBI")

  endif()


  STRING(REGEX MATCH "\#define COMISO_ARPACK_AVAILABLE 1" COMISO_ARPACK_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_ARPACK_BUILD_TIME_AVAILABLE )

   find_package(ARPACK)

   if ( NOT ARPACK_FOUND )
     message(ERROR "COMISO configured with ARPACK but ARPACK not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "ARPACK")

  endif()
  
  STRING(REGEX MATCH "\#define COMISO_CPLEX_AVAILABLE 1" COMISO_CPLEX_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_CPLEX_BUILD_TIME_AVAILABLE )

   find_package(CPLEX)

   if ( NOT CPLEX_FOUND )
     message(ERROR "COMISO configured with CPLEX but CPLEX not available")
   endif()

   list (APPEND  COMISO_OPT_DEPS "CPLEX")

  endif()
  
  STRING(REGEX MATCH "\#define COMISO_EIGEN3_AVAILABLE 1" COMISO_EIGEN3_BUILD_TIME_AVAILABLE ${CURRENT_COMISO_CONFIG} )

  if ( COMISO_EIGEN3_BUILD_TIME_AVAILABLE )
                                                                          
   find_package(EIGEN3)
                                                                          
   if ( NOT EIGEN3_FOUND )
     message(ERROR "COMISO configured with EIGEN3 but EIGEN3 not available")
   endif()
                                                                          
   list (APPEND  COMISO_OPT_DEPS "EIGEN3")
                                                                          
  endif()

  add_definitions (-DCOMISODLL -DUSECOMISO )

endif(COMISO_INCLUDE_DIR)

IF (COMISO_INCLUDE_DIR)
  include(FindPackageHandleStandardArgs)
  SET(COMISO_FOUND TRUE)
  SET( COMISO_LIBRARY_DIR "${CMAKE_BINARY_DIR}/Build/${ACG_PROJECT_LIBDIR}" )
  SET( COMISO_LIBRARY "CoMISo")
#  SET( COMISO_DEPS "GMM;BLAS;SUITESPARSE" )
  SET( COMISO_DEPS "GMM")
#  SET( COMISO_OPT_DEPS ${COMISO_OPT_DEPS} CACHE STRING "Comiso optional dependecies")
#  mark_as_advanced(COMISO_DEPS COMISO_OPT_DEPS)
ELSE (COMISO_INCLUDE_DIR)
  SET( COMISO_FOUND FALSE )
  SET( COMISO_LIBRARY_DIR )
ENDIF (COMISO_INCLUDE_DIR)

