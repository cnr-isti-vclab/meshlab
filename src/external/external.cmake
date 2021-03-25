# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

# newuoa - optional and header-only, for several plugins including all that use levmar
include(${EXTERNAL_DIR}/newuoa.cmake)

# levmar - optional, for several plugins
include(${EXTERNAL_DIR}/levmar.cmake)

# lib3ds - optional, for io_3ds
include(${EXTERNAL_DIR}/lib3ds.cmake)

# gmp or mpir - optional, for filter_csg
include(${EXTERNAL_DIR}/gmp-mpir.cmake)

# muparser - optional, for filter_func
include(${EXTERNAL_DIR}/muparser.cmake)

# OpenCTM - optional, for io_ctm
include(${EXTERNAL_DIR}/openctm.cmake)

# structure-synth - optional, for filter_ssynth
include(${EXTERNAL_DIR}/ssynth.cmake)

# qhull - optional, for filter_qhull
include(${EXTERNAL_DIR}/qhull.cmake)

# u3d - optional, for io_u3d
include(${EXTERNAL_DIR}/u3d.cmake)

# libe57Format - optional, for io_e57
include(${EXTERNAL_DIR}/e57.cmake)