# - Try to find ARPACK
# Once done this will define
#
#  ARPACK_FOUND        - system has ARPACK
#  ARPACK_INCLUDE_DIR  - the ARPACK include directories
#  ARPACK_LIBRARY      - Link this to use ARPACK
#  ARPACKpp_LIBRARY    - Link this to use ARPACK++
#  ARPACK_LIBRARIES    - Link this to use ARPACK together with ARPACK++


find_path (ARPACK_INCLUDE_DIR NAMES arpack++/argeig.h
  HINTS ENV ARPACK_INCLUDE_DIR
  PATHS /usr/include/arpack++ $ENV{HOME}/opt/arpack++/include "C:\\libs\\arpack++\\include"
  DOC "ARPACK Include Directory")

IF ( WIN32 )
find_library( ARPACK_LIBRARY arpack.lib
              PATHS "C:\\libs\\arpack++\\lib"
            )
ELSE( WIN32 )
  find_library( ARPACK_LIBRARY arpack
                PATHS /usr/lib /usr/lib64
              )
  find_library( ARPACKpp_LIBRARY arpack++
                PATHS /usr/lib /usr/lib64 ${ARPACK_INCLUDE_DIR}/../src/.libs
              )

  list( APPEND ARPACK_LIBRARIES  ${ARPACK_LIBRARY} )
  IF(ARPACKpp_LIBRARY)
    list( APPEND ARPACK_LIBRARIES  ${ARPACKpp_LIBRARY} )
  ENDIF(ARPACKpp_LIBRARY)
  
ENDIF( WIN32 )

IF (ARPACK_INCLUDE_DIR AND ARPACK_LIBRARY)
  SET(ARPACK_FOUND TRUE)
ELSE ()
  SET(ARPACK_FOUND FALSE)
ENDIF ()




