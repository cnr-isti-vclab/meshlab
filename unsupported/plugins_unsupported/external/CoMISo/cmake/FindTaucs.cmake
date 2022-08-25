# - Find TAUCS
# Find the native TAUCS headers and libraries.
#
#  TAUCS_INCLUDE_DIR -  where to find TAUCS.h, etc.
#  TAUCS_LIBRARY     - List of libraries when using TAUCS.
#  TAUCS_FOUND       - True if TAUCS found.

IF (TAUCS_INCLUDE_DIR)
  # Already in cache, be silent
  SET(TAUCS_FIND_QUIETLY TRUE)
ENDIF (TAUCS_INCLUDE_DIR)


# Look for the header file.
IF(WIN32)
      find_package(CGAL)
	if (NOT CGAL_FOUND)
	  message(STATUS "CGAL not found .. required to use taucs!")
	endif()
	 
	FIND_PATH(TAUCS_INCLUDE_DIR NAMES taucs.h
                PATHS  "${CGAL_TAUCS_DIR}/include" )
				
ELSE(WIN32)

    IF(APPLE)
	FIND_PATH(TAUCS_INCLUDE_DIR NAMES taucs.h
                  PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/taucs/src" 
                        ~/sw/taucs/include
                 )

    ELSE (APPLE)

	FIND_PATH(TAUCS_INCLUDE_DIR NAMES taucs.h
                  PATHS  /ACG/acgdev/gcc-4.3-x86_64/taucs-2.2-gfortran/include
                 )

    ENDIF(APPLE)


ENDIF(WIN32)

# Copy the results to the output variables.
IF(TAUCS_INCLUDE_DIR )
  SET(TAUCS_FOUND 1)
  SET(TAUCS_INCLUDE_DIR ${TAUCS_INCLUDE_DIR})

  IF(WIN32)
#	find_package(CGAL)
	SET(TAUCS_LIBRARY "${CGAL_TAUCS_DIR}/lib/libtaucs.lib" )
  ELSE(WIN32)
    IF(APPLE)
	    FIND_LIBRARY( TAUCS_LIBRARY
        	          NAMES taucs
                	  PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/taucs/lib/darwin9.0" 
                                ~/sw/taucs/lib/darwin11
	               )	
    ELSE (APPLE)
 	   FIND_LIBRARY( TAUCS_LIBRARY
        	          NAMES taucs 
                	  PATHS /ACG/acgdev/gcc-4.3-x86_64/taucs-2.2.new/lib
	               )
    ENDIF( APPLE)
  ENDIF(WIN32)
  
ELSE(TAUCS_INCLUDE_DIR )
  SET(TAUCS_FOUND 0)
  SET(TAUCS_INCLUDE_DIR)
ENDIF(TAUCS_INCLUDE_DIR )

# Report the results.
IF(NOT TAUCS_FOUND)
  SET(TAUCS_DIR_MESSAGE
    "TAUCS was not found. Make sure TAUCS_INCLUDE_DIR is set to the directories containing the include and lib files for TAUCS. .")
  IF(TAUCS_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "${TAUCS_DIR_MESSAGE}")
  ELSEIF(NOT TAUCS_FIND_QUIETLY)
    MESSAGE(STATUS "${TAUCS_DIR_MESSAGE}")
  ELSE(NOT TAUCS_FIND_QUIETLY)
  ENDIF(TAUCS_FIND_REQUIRED)
ELSE (NOT TAUCS_FOUND)
  IF(NOT TAUCS_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for TAUCS - found")
  ENDIF(NOT TAUCS_FIND_QUIETLY)
  IF ( NOT WIN32 )
    SET(TAUCS_LIBRARY "${TAUCS_LIBRARY};metis")
  ELSE  ( NOT WIN32 )
    SET(TAUCS_LIBRARY "${TAUCS_LIBRARY}")
  ENDIF( NOT WIN32 )
ENDIF(NOT TAUCS_FOUND)

