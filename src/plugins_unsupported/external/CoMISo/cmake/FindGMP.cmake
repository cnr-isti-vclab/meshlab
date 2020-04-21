# Try to find the GMP libraries
# GMP_FOUND - system has GMP lib
# GMP_INCLUDE_DIR - the GMP include directory
# GMP_LIBRARY_DIR - Directory where the GMP libraries are located
# GMP_LIBRARIES - the GMP libraries
# GMP_IN_CGAL_AUXILIARY - TRUE if the GMP found is the one distributed with CGAL in the auxiliary folder

# TODO: support MacOSX

include(CGAL_FindPackageHandleStandardArgs)
include(CGAL_GeneratorSpecificSettings)

# Is it already configured?
if (GMP_INCLUDE_DIR AND GMP_LIBRARY_DIR ) 
   
  set(GMP_FOUND TRUE)
  
else()  
  find_package(CGAL)
  if (NOT CGAL_FOUND)
    message(STATUS "CGAL not found .. required to use taucs!")
    set(GMP_FOUND FALSE)
  else ()
    find_path(GMP_INCLUDE_DIR 
              NAMES gmp.h
              PATHS "${CGAL_INCLUDE_DIR}/../auxiliary/gmp/include"
                  DOC "The directory containing the GMP header files"
             )

    if ( GMP_INCLUDE_DIR STREQUAL "${CMAKE_SOURCE_DIR}/auxiliary/gmp/include" )
      cache_set( GMP_IN_CGAL_AUXILIARY TRUE )
    endif()
    
    if ( CGAL_AUTO_LINK_ENABLED )
		message("AAAA")
      find_path(GMP_LIBRARY_DIR 
                NAMES "gmp-${CGAL_TOOLSET}-mt.lib" "gmp-${CGAL_TOOLSET}-mt-gd.lib" libgmp-10.lib
                PATHS "${CGAL_INCLUDE_DIR}/../auxiliary/gmp/lib"
                DOC "Directory containing the GMP library"
               )
      
	    list ( APPEND GMP_LIBRARIES libgmp-10.lib;libmpfr-4.lib )	    
    else()
    
      find_library(GMP_LIBRARIES NAMES gmp 
                   PATHS ENV GMP_LIB_DIR
                   DOC "Path to the GMP library"
                  )
                  
      if ( GMP_LIBRARIES ) 
        get_filename_component(GMP_LIBRARY_DIR ${GMP_LIBRARIES} PATH CACHE )
      endif()
      
    endif()  
      
    # Attempt to load a user-defined configuration for GMP if couldn't be found
    if ( NOT GMP_INCLUDE_DIR OR NOT GMP_LIBRARY_DIR )
      include( GMPConfig OPTIONAL )
    endif()
    
    find_package_handle_standard_args(GMP "DEFAULT_MSG" GMP_INCLUDE_DIR GMP_LIBRARY_DIR)

  endif()
endif()
