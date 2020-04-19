# - Try to find GMM
# Once done this will define
#  GMM_FOUND          - System has GMM
#  GMM_INCLUDE_DIRS   - The GMM include directories
#  GMM_COMPILER_FLAGS - Compiler flags that are necessary for GMM

if (GMM_INCLUDE_DIR)
  # in cache already
  set(GMM_FOUND TRUE)
  set(GMM_INCLUDE_DIRS "${GMM_INCLUDE_DIR}" )
else (GMM_INCLUDE_DIR)

find_path( GMM_INCLUDE_DIR 
           NAMES gmm/gmm.h 
           PATHS $ENV{GMM_DIR}
                 /usr/include
                 /usr/include
                 /usr/local/include
                 ~/sw/gmm-4.1/include
		 "d:\\code\\gmm-4.2\\include"
                 "c:\\libs\\gmm-3.0\\include"
		 ${PROJECT_SOURCE_DIR}/MacOS/Libs/gmm-3.1/include
                 ../../External/include
                 /ACG/acgdev/gcc-4.3-i686/gmm-4.1/include/
                 /ACG/acgdev/gcc-4.3-i686/gmm-3/include/
                 ${module_file_path}/../../../External/include
                 $ENV{HOME}/opt/gmm/gmm-4.2/include
          )

set(GMM_INCLUDE_DIRS "${GMM_INCLUDE_DIR}" )


# use c++ headers as default
IF (WIN32)
  set(GMM_COMPILER_FLAGS "-D_SCL_SECURE_NO_DEPRECATE" CACHE STRING "GMM Compiler Flags")
#  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
ELSE(WIN32)
  set(GMM_COMPILER_FLAGS "" CACHE STRING "GMM Compiler Flags")
ENDIF(WIN32)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GMM  DEFAULT_MSG
                                  GMM_INCLUDE_DIR)

mark_as_advanced(GMM_INCLUDE_DIR GMM_COMPILER_FLAGS)

endif(GMM_INCLUDE_DIR)

