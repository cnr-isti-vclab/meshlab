# Default installation directory, based on operating system
IF (PROJECT_OS_WIN)
    SET (CMAKE_INSTALL_PREFIX "C:\\Program Files\\libfreenect" CACHE PATH "Installation directory")
ELSE (PROJECT_OS_WIN)
    SET (CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation directory")
ENDIF (PROJECT_OS_WIN)

MESSAGE (STATUS "${PROJECT_NAME} will be installed to ${CMAKE_INSTALL_PREFIX}")

# Installation prefix for include files
STRING (TOLOWER ${PROJECT_NAME} projectNameLower)
SET (PROJECT_INCLUDE_INSTALL_DIR "include/${projectNameLower}")

IF (PROJECT_PROC_64BIT)
    SET (LIB_SUFFIX "64" CACHE STRING "Suffix for library installation directory")
ELSE (PROJECT_PROC_64BIT)
    SET (LIB_SUFFIX "" CACHE STRING "Suffix for library installation directory")
ENDIF (PROJECT_PROC_64BIT)

SET (PROJECT_LIBRARY_INSTALL_DIR "lib${LIB_SUFFIX}")

MESSAGE (STATUS "Headers will be installed to ${CMAKE_INSTALL_PREFIX}/${PROJECT_INCLUDE_INSTALL_DIR}")
MESSAGE (STATUS "Libraries will be installed to ${CMAKE_INSTALL_PREFIX}/${PROJECT_LIBRARY_INSTALL_DIR}")

