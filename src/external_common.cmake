# GLEW - required
set(GLEW_DIR ${EXTERNAL_DIR}/glew-2.1.0)
add_subdirectory(${GLEW_DIR})


# VCGLIb -- required
if(NOT VCGDIR)
	get_filename_component(VCGDIR "${CMAKE_CURRENT_SOURCE_DIR}/vcglib" ABSOLUTE)
	if(NOT EXISTS ${VCGDIR})
		set(VCGDIR NOTFOUND)
	endif()
endif()
set(VCGDIR
	"${VCGDIR}")

if(NOT VCGDIR)
	message(FATAL_ERROR "VCGLib not found. Please clone recursively the MeshLab repo.")
endif()
include_directories(${VCGDIR} ${CMAKE_CURRENT_SOURCE_DIR})


# Eigen3 - required
set(EIGEN_DIR ${VCGDIR}/eigenlib)
if(EIGEN3_INCLUDE_DIR)
	message(STATUS "- Eigen - using system-provided library")
	set(EIGEN_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
else()
	message(STATUS "- Eigen - using bundled source")
	set(EIGEN_INCLUDE_DIRS ${EIGEN_DIR})
endif()
include_directories(${EIGEN_INCLUDE_DIRS})
