# - Try to find ADOLC
# Once done this will define
#  ADOLC_FOUND         - System has ADOLC
#  ADOLC_INCLUDE_DIRS  - The ADOLC include directories
#  ADOLC_LIBRARIES     - The libraries needed to use ADOLC

if (ADOLC_INCLUDE_DIR)
  # in cache already
  set(ADOLC_FOUND TRUE)
  set(ADOLC_INCLUDE_DIRS "${ADOLC_INCLUDE_DIR}" )
  set(ADOLC_LIBRARIES "${ADOLC_LIBRARY}" )
else (ADOLC_INCLUDE_DIR)

message( status "adolc include dir: ${ADOLC_DIR}/include/")

find_path( ADOLC_INCLUDE_DIR 
	   NAMES adolc/adolc.h 
           PATHS $ENV{ADOLC_DIR}/include/
                 /usr/include/adolc
                 /usr/local/include
                 /usr/local/include/adolc/
                 /opt/local/include/adolc/
                 "c:\\libs\\adolc\\include"
	         "c:\\libs\\adolc\\include"
	         ${PROJECT_SOURCE_DIR}/MacOS/Libs/adolc/include
                 ../../External/include
                 ${module_file_path}/../../../External/include
          )
          
find_library( ADOLC_LIBRARY 
              adolc
              PATHS $ENV{ADOLC_DIR}/lib
                    $ENV{ADOLC_DIR}/lib64
                   /usr/lib/adolc
                   /usr/local/lib
                   /usr/local/lib/adolc/
                   /opt/local/lib/adolc/
                   "c:\\libs\\adolc\\lib"
                   "c:\\libs\\adolc\\lib"
                   ${PROJECT_SOURCE_DIR}/MacOS/Libs/adolc/lib
                   /usr/lib64/adolc
                   /usr/local/lib64
                   /usr/local/lib64/adolc/
                   /opt/local/lib64/adolc/
                   "c:\\libs\\adolc\\lib64"
                   "c:\\libs\\adolc\\lib64"
                   ${PROJECT_SOURCE_DIR}/MacOS/Libs/adolc/lib64
                   DOC "ADOL-C library"
              )

set(ADOLC_INCLUDE_DIRS "${ADOLC_INCLUDE_DIR}" )
set(ADOLC_LIBRARIES "${ADOLC_LIBRARY}" )


include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCPLEX_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(ADOLC  DEFAULT_MSG
                                  ADOLC_LIBRARY ADOLC_INCLUDE_DIR)

mark_as_advanced(ADOLC_INCLUDE_DIR ADOLC_LIBRARY)

endif(ADOLC_INCLUDE_DIR)
