# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

# newuoa - optional and header-only, for several plugins including all that use levmar
set(NEWUOA_DIR ${VCGDIR}/wrap/newuoa)
include(${EXTERNAL_DIR}/newuoa.cmake)

# levmar - optional, for several plugins
set(LEVMAR_DIR ${EXTERNAL_DIR}/levmar-2.3)
include(${EXTERNAL_DIR}/levmar.cmake)

# lib3ds - optional, for io_3ds
set(LIB3DS_DIR ${EXTERNAL_DIR}/lib3ds-1.3.0)
include(${EXTERNAL_DIR}/lib3ds.cmake)

# gmp or mpir - optional, for filter_csg
set(MPIR_DIR ${EXTERNAL_DIR}/mpir)
include(${EXTERNAL_DIR}/gmp-mpir.cmake)

# muparser - optional, for filter_func
set(MUPARSER_DIR ${EXTERNAL_DIR}/muparser_v225)
include(${EXTERNAL_DIR}/muparser.cmake)

# OpenCTM - optional, for io_ctm
set(OPENCTM_DIR ${EXTERNAL_DIR}/OpenCTM-1.0.3)
include(${EXTERNAL_DIR}/openctm.cmake)

# structure-synth - optional, for filter_ssynth
set(SSYNTH_DIR ${EXTERNAL_DIR}/structuresynth-1.5)
include(${EXTERNAL_DIR}/ssynth.cmake)

# qhull - optional, for filter_qhull
set(QHULL_DIR ${EXTERNAL_DIR}/qhull-2003.1)
include(${EXTERNAL_DIR}/qhull.cmake)

# u3d - optional, for io_u3d
set(U3D_DIR ${EXTERNAL_DIR}/u3d)
include(${EXTERNAL_DIR}/u3d.cmake)

# opengr - optional and header-only, for filter_globalregistration
set(OPENGR_DIR ${EXTERNAL_DIR}/OpenGR)
include(${EXTERNAL_DIR}/opengr.cmake)
