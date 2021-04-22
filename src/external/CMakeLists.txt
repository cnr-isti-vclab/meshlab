# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

message(STATUS "Searching for optional components")

# gmp or mpir - optional, for filter_csg
include(${CMAKE_CURRENT_SOURCE_DIR}/gmp-mpir.cmake)

# levmar - optional, for several plugins
include(${CMAKE_CURRENT_SOURCE_DIR}/levmar.cmake)

# lib3ds - optional, for io_3ds
include(${CMAKE_CURRENT_SOURCE_DIR}/lib3ds.cmake)

# muparser - optional, for filter_func
include(${CMAKE_CURRENT_SOURCE_DIR}/muparser.cmake)

# newuoa - optional and header-only, for several plugins including all that use levmar
include(${CMAKE_CURRENT_SOURCE_DIR}/newuoa.cmake)

# OpenCTM - optional, for io_ctm
include(${CMAKE_CURRENT_SOURCE_DIR}/openctm.cmake)

# qhull - optional, for filter_qhull
include(${CMAKE_CURRENT_SOURCE_DIR}/qhull.cmake)

# structure-synth - optional, for filter_ssynth
include(${CMAKE_CURRENT_SOURCE_DIR}/ssynth.cmake)

# u3d - optional, for io_u3d
include(${CMAKE_CURRENT_SOURCE_DIR}/u3d.cmake)
