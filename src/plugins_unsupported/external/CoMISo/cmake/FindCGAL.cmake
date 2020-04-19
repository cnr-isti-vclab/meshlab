# - Find CGAL
# Find the CGAL autolinking headers.
#
#  CGAL_INCLUDE_DIR -  where to find CGAL.h, etc.
#  CGAL_FOUND        - True if CGAL found.

IF (CGAL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(CGAL_FIND_QUIETLY TRUE)
ENDIF (CGAL_INCLUDE_DIR)

GET_FILENAME_COMPONENT(module_file_path ${CMAKE_CURRENT_LIST_FILE} PATH )

IF (NOT APPLE )
	# Look for the header file.
	FIND_PATH(CGAL_INCLUDE_DIR NAMES CGAL/auto_link/auto_link.h
        	                   PATHS /usr/include
        	                   ../../External/include
                                 "C:/libs/CGAL-3.6/include"
								 "C:/Program Files/CGAL-3.5/include"
                                 "C:/Programme/CGAL-3.5/include"
                                 "C:/libs/CGAL-3.5/include"
                	             "C:/Program Files/CGAL-3.4/include"
                                 "C:/Programme/CGAL-3.4/include"
                                 "C:/libs/CGAL-3.4/include"
								 "C:/Programme/CGAL-3.9/include"
								 "C:/Program Files/CGAL-3.9/include"
								 "C:/Program Files (x86)/CGAL-3.9/include"
	                             ${module_file_path}/../../../External/include)
ELSE( NOT APPLE)
	# Look for the header file.
	FIND_PATH(CGAL_INCLUDE_DIR NAMES CGAL/auto_link/auto_link.h
        	                   PATHS "/opt/local/include"
                 )

ENDIF( NOT APPLE )



MARK_AS_ADVANCED(CGAL_INCLUDE_DIR)


# Copy the results to the output variables.
IF(CGAL_INCLUDE_DIR )
  SET(CGAL_FOUND 1)
  SET(CGAL_INCLUDE_DIR ${CGAL_INCLUDE_DIR})
  # Look for the taucs dir.
  FIND_PATH(CGAL_TAUCS_DIR NAMES include/taucs.h
                           PATHS ${CGAL_INCLUDE_DIR}/../auxiliary/taucs)

  IF(WIN32)
    include(CGAL_GeneratorSpecificSettings)
    find_path(CGAL_LIBRARY_DIR 
                NAMES "CGAL-${CGAL_TOOLSET}-mt.lib" "CGAL-${CGAL_TOOLSET}-mt-gd.lib"
                PATHS "${CGAL_INCLUDE_DIR}/../lib"
                DOC "Directory containing the CGAL library"
               ) 
    #add_definitions (-DCGAL_AUTOLINK)
  ELSEIF( APPLE)
    find_path(CGAL_LIBRARY_DIR 
                NAMES "libCGAL.dylib"
                PATHS "/opt/local/lib/"
                DOC "Directory containing the CGAL library"
               ) 
    list ( APPEND CGAL_LIBRARIES CGAL CGAL_Core CGAL_ImageIO mpfr )
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -frounding-math")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math")

  ELSE( WIN32 )
    find_path(CGAL_LIBRARY_DIR
              NAMES "libCGAL.so"
              PATHS "/usr/lib/" "/usr/lib64" 
              DOC "Directory containing the CGAL library"
             )
    list ( APPEND CGAL_LIBRARIES CGAL CGAL_Core CGAL_ImageIO CGAL_Qt4) 
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -frounding-math")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -frounding-math")


    # This is needed to link correctly against lapack
    add_definitions (-DCGAL_USE_F2C)
  ENDIF(WIN32)


 

ELSE(CGAL_INCLUDE_DIR )
  SET(CGAL_FOUND 0)
  SET(CGAL_INCLUDE_DIR)
ENDIF(CGAL_INCLUDE_DIR )

# Report the results.
IF(NOT CGAL_FOUND)
  SET(CGAL_DIR_MESSAGE
    "CGAL was not found. Make sure CGAL_INCLUDE_DIR is set to the directories containing the include files for CGAL. .")
  IF(CGAL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "${CGAL_DIR_MESSAGE}")
  ELSEIF(NOT CGAL_FIND_QUIETLY)
    MESSAGE(STATUS "${CGAL_DIR_MESSAGE}")
  ELSE(NOT CGAL_FIND_QUIETLY)
  ENDIF(CGAL_FIND_REQUIRED)
ELSE (NOT CGAL_FOUND)
  IF(NOT CGAL_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for CGAL - found")
  ENDIF(NOT CGAL_FIND_QUIETLY)
ENDIF(NOT CGAL_FOUND)

