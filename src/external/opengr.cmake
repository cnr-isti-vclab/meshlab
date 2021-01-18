# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_OPENGR "Allow use of bundled OpenGR source" ON)

set(OPENGR_DIR ${EXTERNAL_DIR}/OpenGR)

if(ALLOW_BUNDLED_OPENGR AND EXISTS "${OPENGR_DIR}/src/gr/algorithms/match4pcsBase.h")
	message(STATUS "- OpenGR - using bundled source")
	add_library(external-opengr INTERFACE)
	target_include_directories(external-opengr INTERFACE ${OPENGR_DIR}/src/)
endif()
