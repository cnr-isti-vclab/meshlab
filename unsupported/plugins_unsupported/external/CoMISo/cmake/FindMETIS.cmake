# - Try to find METIS
# Once done this will define
#  METIS_FOUND - System has Metis
#  METIS_INCLUDE_DIRS - The Metis include directories
#  METIS_LIBRARY_DIRS - The library directories needed to use Metis
#  METIS_LIBRARIES    - The libraries needed to use Metis

if (METIS_INCLUDE_DIR)
  # in cache already
  SET(METIS_FIND_QUIETLY TRUE)
endif (METIS_INCLUDE_DIR)

find_path(METIS_INCLUDE_DIR NAMES metis.h
     PATHS "$ENV{IPOPT_HOME}/ThirdParty/Metis/metis-4.0/Lib/"
           "/usr/include/"
           "/usr/include/metis"
           "/opt/local/include"
           "/opt/local/include/metis"
           
           
   )
   
find_library( METIS_LIBRARY 
              metis coinmetis
              PATHS "$ENV{IPOPT_HOME}/lib/"
                    "/usr/lib"
                    "/opt/local/lib" )

set(METIS_INCLUDE_DIRS "${METIS_INCLUDE_DIR}" )
set(METIS_LIBRARIES "${METIS_LIBRARY}" )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(METIS  DEFAULT_MSG
                                  METIS_LIBRARY METIS_INCLUDE_DIR)

mark_as_advanced(METIS_INCLUDE_DIR METIS_LIBRARY )
