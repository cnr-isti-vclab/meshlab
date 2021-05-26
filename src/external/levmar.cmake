# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_LEVMAR "Allow use of bundled levmar source" ON)

set(LEVMAR_DIR ${CMAKE_CURRENT_LIST_DIR}/levmar-2.3)

if(ALLOW_BUNDLED_LEVMAR AND EXISTS "${LEVMAR_DIR}/lm.h")
	message(STATUS "- levmar - using bundled source")
	add_library(
		external-levmar STATIC
		"${LEVMAR_DIR}/compiler.h"
		"${LEVMAR_DIR}/lm.h"
		"${LEVMAR_DIR}/misc.h"
		"${LEVMAR_DIR}/Axb.c"
		"${LEVMAR_DIR}/lm.c"
		"${LEVMAR_DIR}/lmbc.c"
		"${LEVMAR_DIR}/lmblec.c"
		"${LEVMAR_DIR}/lmlec.c"
		"${LEVMAR_DIR}/misc.c")
	target_include_directories(external-levmar PUBLIC ${LEVMAR_DIR})
	set_property(TARGET external-levmar PROPERTY FOLDER External)
	target_link_libraries(external-levmar PRIVATE external-disable-warnings)
endif()
