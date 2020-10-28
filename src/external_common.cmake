# GLEW - required
set(GLEW_DIR ${EXTERNAL_DIR}/glew-2.1.0)
add_subdirectory(${GLEW_DIR})

# Eigen3 - required
set(EIGEN_DIR ${VCGDIR}/eigenlib)
if(EIGEN3_INCLUDE_DIR)
    message(STATUS "- Eigen - using system-provided library")
    set(EIGEN_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
else()
    message(STATUS "- Eigen - using bundled source")
    set(EIGEN_INCLUDE_DIRS ${EIGEN_DIR})
endif()
