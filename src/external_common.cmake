# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

# GLEW - required
set(GLEW_DIR ${EXTERNAL_DIR}/glew-2.1.0)
include(${EXTERNAL_DIR}/glew.cmake)


# VCGLib -- required
if(NOT VCGDIR)
	get_filename_component(VCGDIR "${CMAKE_CURRENT_LIST_DIR}/vcglib" ABSOLUTE)
	if(NOT EXISTS ${VCGDIR})
		set(VCGDIR NOTFOUND)
	endif()
endif()
set(VCGDIR "${VCGDIR}")

if(NOT VCGDIR)
	message(FATAL_ERROR "VCGLib not found. Please clone recursively the MeshLab repo.")
endif()


# Eigen3 - required
set(EIGEN_DIR ${VCGDIR}/eigenlib)
include(${EXTERNAL_DIR}/eigen.cmake)
