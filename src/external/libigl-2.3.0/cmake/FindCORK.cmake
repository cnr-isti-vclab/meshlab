#
# Try to find CORK library and include path.
# Once done this will define
#
# CORK_FOUND
# CORK_INCLUDE_DIR
# CORK_LIBRARIES
#

if(NOT CORK_FOUND)

FIND_PATH(CORK_INCLUDE_DIR cork.h
  PATHS
    ${PROJECT_SOURCE_DIR}/../../external/cork/include
    ${PROJECT_SOURCE_DIR}/../external/cork/include
    ${PROJECT_SOURCE_DIR}/external/cork/include
    /usr/local/include
    /usr/X11/include
    /usr/include
    /opt/local/include
    NO_DEFAULT_PATH
    )

FIND_LIBRARY( CORK_LIBRARIES NAMES cork
  PATHS
    ${PROJECT_SOURCE_DIR}/../../external/cork/lib/
    ${PROJECT_SOURCE_DIR}/../external/cork/lib/
    ${PROJECT_SOURCE_DIR}/external/cork/lib/
    /usr/local
    /usr/X11
    /usr
    PATH_SUFFIXES
    a
    lib64
    lib
    NO_DEFAULT_PATH
)

SET(CORK_FOUND "NO")
IF (CORK_INCLUDE_DIR AND CORK_LIBRARIES)
	SET(CORK_FOUND "YES")
ENDIF (CORK_INCLUDE_DIR AND CORK_LIBRARIES)

if(CORK_FOUND)
  message(STATUS "Found CORK: ${CORK_INCLUDE_DIR}")
else(CORK_FOUND)
  if (NOT CORK_FIND_QUIETLY)
    message(FATAL_ERROR "could NOT find CORK")
  endif (NOT CORK_FIND_QUIETLY)
endif(CORK_FOUND)

endif(NOT CORK_FOUND)
