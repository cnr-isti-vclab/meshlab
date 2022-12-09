# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_BUNDLED_NEWUOA "Allow use of bundled newuoa source" ON)

set(NEWUOA_DIR ${VCGDIR}/wrap/newuoa)

if(MESHLAB_ALLOW_BUNDLED_NEWUOA AND EXISTS "${NEWUOA_DIR}/include/newuoa.h")
	message(STATUS "- newuoa - using bundled source")
	add_library(external-newuoa INTERFACE)
	target_include_directories(external-newuoa INTERFACE ${NEWUOA_DIR}/include)
endif()
