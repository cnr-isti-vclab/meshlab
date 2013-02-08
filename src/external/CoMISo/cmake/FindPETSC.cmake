if (PETSC_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(PETSC_FIND_QUIETLY TRUE)
endif(PETSC_INCLUDE_DIRS)

if (WIN32)
   find_path(PETSC_INCLUDE_DIR NAMES petsc.h
     PREFIXES SRC
     PATHS
     "C:\\libs\\gurobi45"
     ${PETSC_DIR}/include
   )

   find_library( PETSC_LIBRARY_RELEASE 
                 SuperLU
                 PATHS "C:\\libs\\gurobi45\\lib" )
   find_library( PETSC_LIBRARY_DEBUG
                   SuperLUd
                   PATHS "C:\\libs\\gurobi45\\lib" )


   set ( PETSC_LIBRARY "optimized;${PETSC_LIBRARY_RELEASE};debug;${PETSC_LIBRARY_DEBUG}" CACHE  STRING "PETSC Libraries" )

ELSEIF(APPLE)

   find_path(PETSC_INCLUDE_DIR NAMES gurobi_c++.h
	     PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40"
	     	   ${PETSC_INCLUDE_PATH}
            )

   find_library( PETSC_LIBRARY 
                 SuperLU
                 PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40")

ELSE( WIN32 )
   find_path(PETSC_INCLUDE_DIR1 NAMES petsc.h
     PATHS "$ENV{PETSC_DIR}/include"
     ${PETSC_INCLUDE_DIR1}
   )

   find_path(PETSC_INCLUDE_DIR2 NAMES petscconf.h
     PATHS "$ENV{PETSC_DIR}/$ENV{PETSC_ARCH}/include"
     ${PETSC_INCLUDE_DIR2}
   )

#   MESSAGE(STATUS "$ENV{PETSC_HOME}/include")
   IF(PETSC_INCLUDE_DIR1 AND PETSC_INCLUDE_DIR2)
      SET(PETSC_FOUND TRUE)
      SET(PETSC_INCLUDE_DIRS "${PETSC_INCLUDE_DIR1};${PETSC_INCLUDE_DIR2}")
      SET(PETSC_LIBRARY_DIR "$ENV{PETSC_DIR}/$ENV{PETSC_ARCH}/lib" CACHE PATH "Path to PETSC Library")
      SET(PETSC_LIBRARY "petsc" CACHE STRING "PETSC Libraries")  
    ELSE(PETSC_INCLUDE_DIR1 AND PETSC_INCLUDE_DIR2)
      SET(PETSC_FOUND FALSE)
      SET(PETSC_INCLUDE_DIR ${PETSC_INCLUDE_DIR})
    ENDIF(PETSC_INCLUDE_DIR1 AND PETSC_INCLUDE_DIR2)

   #find_library( PETSC_LIBRARY 
    #             gurobi
     #            PATHS "${PETSC_HOME}/lib" )
ENDIF()