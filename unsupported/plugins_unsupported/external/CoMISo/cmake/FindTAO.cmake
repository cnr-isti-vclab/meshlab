if (TAO_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(TAO_FIND_QUIETLY TRUE)
endif (TAO_INCLUDE_DIRS)

if (WIN32)
   find_path(TAO_INCLUDE_DIR NAMES tao.h
     PREFIXES SRC
     PATHS
     "C:\\libs\\gurobi45"
     ${TAO_DIR}/include
   )

   find_library( TAO_LIBRARY_RELEASE 
                 SuperLU
                 PATHS "C:\\libs\\gurobi45\\lib" )
   find_library( TAO_LIBRARY_DEBUG
                   SuperLUd
                   PATHS "C:\\libs\\gurobi45\\lib" )


   set ( TAO_LIBRARY "optimized;${TAO_LIBRARY_RELEASE};debug;${TAO_LIBRARY_DEBUG}" CACHE  STRING "TAO Libraries" )

ELSEIF(APPLE)

   find_path(TAO_INCLUDE_DIR NAMES gurobi_c++.h
	     PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40"
	     	   ${TAO_INCLUDE_PATH}
            )

   find_library( TAO_LIBRARY 
                 SuperLU
                 PATHS "${CMAKE_SOURCE_DIR}/MacOS/Libs/gurobi40")

ELSE( WIN32 )
find_path(TAO_INCLUDE_DIR NAMES "include/tao.h"
     PATHS "$ENV{TAO_DIR}"
     /usr/include/tao
   )
   

#   MESSAGE(STATUS "$ENV{TAO_HOME}/include")
   IF(TAO_INCLUDE_DIR)
      SET(TAO_FOUND TRUE)
      SET(TAO_INCLUDE_DIRS "${TAO_INCLUDE_DIR}/include;${TAO_INCLUDE_DIR}")
      SET(TAO_INCLUDE_DIR ${TAO_INCLUDE_DIR}/include;${TAO_INCLUDE_DIR} CACHE PATH "Path to TAO Includes")
      
      #check VERSION 1.x or 2
      IF(IS_DIRECTORY "$ENV{TAO_DIR}/lib/$ENV{PETSC_ARCH}/")
#        MESSAGE(STATUS "TAO Version 1.x")
	SET(TAO_LIBRARY_DIR "$ENV{TAO_DIR}/lib/$ENV{PETSC_ARCH}/" CACHE PATH "Path to TAO Library")
	SET(TAO_LIBRARY "tao;taopetsc;taofortran" CACHE STRING "TAO Libraries")  
      ELSE(IS_DIRECTORY "$ENV{TAO_DIR}/lib/$ENV{PETSC_ARCH}/") #VERSION 2
#        MESSAGE(STATUS "TAO Version 2.x")
	SET(TAO_LIBRARY_DIR "$ENV{TAO_DIR}/$ENV{PETSC_ARCH}/lib" CACHE PATH "Path to TAO Library")
	SET(TAO_LIBRARY "tao" CACHE STRING "TAO Libraries")  
      ENDIF(IS_DIRECTORY "$ENV{TAO_DIR}/lib/$ENV{PETSC_ARCH}/")
      
      #      MESSAGE(STATUS "${TAO_LIBRARY_DIR}")
#      MESSAGE(STATUS "${TAO_LIBRARY}")
    ELSE(TAO_INCLUDE_DIR)
      SET(TAO_FOUND FALSE)
      SET(TAO_INCLUDE_DIR ${TAO_INCLUDE_DIR})
    ENDIF(TAO_INCLUDE_DIR)

   #find_library( TAO_LIBRARY 
    #             gurobi
     #            PATHS "${TAO_HOME}/lib" )
ENDIF()