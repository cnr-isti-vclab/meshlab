#
# Try to find MOSEK
# Once done this will define
#
# MOSEK_FOUND           - system has MOSEK
# MOSEK_INCLUDE_DIRS    - the MOSEK include directories
# MOSEK_LIBRARIES       - Link these to use MOSEK
#

# Hardcoded search paths
set(SEARCH_PATHS
  ${CMAKE_SOURCE_DIR}/mosek/9.2/tools/platform/osx64x86/ 
  /usr/local/mosek/7/tools/platform/osx64x86/
  /usr/local/mosek/8/tools/platform/osx64x86/
  /usr/local/mosek/9.2/tools/platform/osx64x86/
  /opt/mosek/7/tools/platform/linux64x86/
)

find_path(MOSEK_INCLUDE_DIR mosek.h
  PATHS ${SEARCH_PATHS}
  PATH_SUFFIXES h
)

set(MOSEK_LIBRARIES)
find_library(MOSEK_LIBRARIES NAMES mosek64
  HINT
    "${MOSEK_INCLUDE_DIR}"
    "${MOSEK_INCLUDE_DIR}/../bin"
    "${MOSEK_INCLUDE_DIR}/lib"
  PATHS
    ${SEARCH_PATHS}
  NO_DEFAULT_PATH
  PATH_SUFFIXES a bin lib dylib)

# Check that Mosek was successfully found
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  MOSEK DEFAULT_MSG MOSEK_LIBRARIES MOSEK_INCLUDE_DIR
)
set(MOSEK_INCLUDE_DIRS ${MOSEK_INCLUDE_DIR})

# Hide variables from CMake-Gui options
mark_as_advanced(MOSEK_LIBRARIES MOSEK_INCLUDE_DIRS MOSEK_INCLUDE_DIR)
