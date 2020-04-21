# - Try to find IPOPT
# Once done this will define
#  IPOPT_FOUND - System has IpOpt
#  IPOPT_INCLUDE_DIRS - The IpOpt include directories
#  IPOPT_LIBRARY_DIRS - The library directories needed to use IpOpt
#  IPOPT_LIBRARIES    - The libraries needed to use IpOpt


if (IPOPT_INCLUDE_DIR)
  # in cache already
  SET(IPOPT_FIND_QUIETLY TRUE)
endif (IPOPT_INCLUDE_DIR)

if (WIN32)
   find_path(IPOPT_INCLUDE_DIR NAMES IpNLP.hpp
     PATHS
     "C:\\libs\\Ipopt-3.8.2\\include\\coin"
     ${IPOPT_DIR}/include
   )

   IF(IPOPT_INCLUDE_DIR)
      find_library( IPOPT_LIBRARY_RELEASE 
                    Ipopt
                    PATHS "C:\\libs\\Ipopt-3.8.2\\lib\\win32\\release" )
      find_library( IPOPT_LIBRARY_DEBUG
                    Ipopt
                    PATHS "C:\\libs\\Ipopt-3.8.2\\lib\\win32\\debug" )

      set ( IPOPT_LIBRARY "optimized;${IPOPT_LIBRARY_RELEASE};debug;${IPOPT_LIBRARY_DEBUG}" CACHE  STRING "IPOPT Libraries" )

      SET(IPOPT_FOUND TRUE)
      SET(IPOPT_INCLUDE_DIR ${IPOPT_INCLUDE_DIR})
	  # Todo, set right version depending on build type (debug/release)
	  #GET_FILENAME_COMPONENT( IPOPT_LIBRARY_DIR ${GLEW_LIBRARY} PATH )
    ELSE(IPOPT_INCLUDE_DIR)
      SET(IPOPT_FOUND FALSE)
      SET(IPOPT_INCLUDE_DIR ${IPOPT_INCLUDE_DIR})
    ENDIF(IPOPT_INCLUDE_DIR)

ELSE( WIN32 )
   find_path(IPOPT_INCLUDE_DIR NAMES IpNLP.hpp
     PATHS  "$ENV{IPOPT_HOME}/include/coin"
            "/usr/include/coin"
    
   )

   find_library( IPOPT_LIBRARY 
                 ipopt
                 PATHS "$ENV{IPOPT_HOME}/lib"
                       "/usr/lib" )   
    
    #wrong config under Debian workaround
    add_definitions( -DHAVE_CSTDDEF )

   
   # set optional path to HSL Solver
   find_path(IPOPT_HSL_LIBRARY_DIR 
             NAMES libhsl.so
                   libhsl.dylib
             PATHS "$ENV{IPOPT_HSL_LIBRARY_PATH}"
                   "$ENV{HOME}/opt/HSL/lib"
   )
   
   IF( IPOPT_HSL_LIBRARY_DIR)
     IF( NOT IPOPT_FIND_QUIETLY )
        message ( "IPOPT_HSL_LIBRARY_DIR found at ${IPOPT_HSL_LIBRARY_DIR} ")
     ENDIF()
     set(IPOPT_LIBRARY_DIR ${IPOPT_HSL_LIBRARY_DIR})
     LIST( APPEND IPOPT_LIBRARY_DIRS "${IPOPT_HSL_LIBRARY_DIR}")
   ENDIF(IPOPT_HSL_LIBRARY_DIR)
   
   
   set(IPOPT_INCLUDE_DIRS "${IPOPT_INCLUDE_DIR}" )
   set(IPOPT_LIBRARIES "${IPOPT_LIBRARY}" )

   include(FindPackageHandleStandardArgs)
   # handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
   # if all listed variables are TRUE
   find_package_handle_standard_args(IPOPT  DEFAULT_MSG
                                     IPOPT_LIBRARY IPOPT_INCLUDE_DIR)

   mark_as_advanced(IPOPT_INCLUDE_DIR IPOPT_LIBRARY )
   
ENDIF()
