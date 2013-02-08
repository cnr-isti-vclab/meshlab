# - Try to find EIGEN3
# Once done this will define
#  EIGEN3_FOUND         - System has EIGEN3
#  EIGEN3_INCLUDE_DIRS  - The EIGEN3 include directories

if (EIGEN3_INCLUDE_DIR)
  # in cache already
  set(EIGEN3_FOUND TRUE)
  set(EIGEN3_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIR}" )
else (EIGEN3_INCLUDE_DIR)

find_path( EIGEN3_INCLUDE_DIR 
	   NAMES Eigen/Dense 
           PATHS $ENV{EIGEN_DIR}
                 /usr/include/eigen3
                 /usr/local/include
                 /usr/local/include/eigen3/
                 /opt/local/include/eigen3/
                 "d:\\code\\vcglib\\eigenlib\\"
		 "c:\\libs\\eigen\\include"
		  ${PROJECT_SOURCE_DIR}/MacOS/Libs/eigen3/include
                  ../../External/include
                  ${module_file_path}/../../../External/include
          )

set(EIGEN3_INCLUDE_DIRS "${EIGEN3_INCLUDE_DIR}" )


include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(EIGEN3  DEFAULT_MSG
                                  EIGEN3_INCLUDE_DIR)

mark_as_advanced(EIGEN3_INCLUDE_DIR)

endif(EIGEN3_INCLUDE_DIR)
