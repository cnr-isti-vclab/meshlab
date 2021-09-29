# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_QHULL "Allow use of bundled Qhull source" ON)
option(ALLOW_SYSTEM_QHULL "Allow use of system-provided QHull" ON)

find_package(Qhull COMPONENTS qhull_r)
set(QHULL_DIR ${CMAKE_CURRENT_LIST_DIR}/qhull-2020.2/src/libqhull_r)

if(ALLOW_SYSTEM_QHULL AND TARGET Qhull::qhull_r)
	message(STATUS "- qhull - using system-provided library")
	add_library(external-qhull INTERFACE)
	target_link_libraries(external-qhull INTERFACE Qhull::qhull_r)
	target_compile_definitions(external-qhull INTERFACE SYSTEM_QHULL)
	target_include_directories(external-qhull INTERFACE ${QHULL_qhull_r_INCLUDE_DIR}/libqhull_r)
elseif(ALLOW_BUNDLED_QHULL AND EXISTS "${QHULL_DIR}/libqhull_r.h")
	message(STATUS "- qhull - using bundled source")
	add_library(
		external-qhull STATIC
		"${QHULL_DIR}/geom2_r.c"
		"${QHULL_DIR}/io_r.c"
		"${QHULL_DIR}/io_r.h"
		"${QHULL_DIR}/mem_r.c"
		"${QHULL_DIR}/mem_r.h"
		"${QHULL_DIR}/merge_r.c"
		"${QHULL_DIR}/merge_r.h"
		"${QHULL_DIR}/poly_r.c"
		"${QHULL_DIR}/poly_r.h"
		"${QHULL_DIR}/poly2_r.c"
#		"${QHULL_DIR}/qconvex.c"
		"${QHULL_DIR}/libqhull_r.c"
		"${QHULL_DIR}/libqhull_r.h"
		"${QHULL_DIR}/qset_r.c"
		"${QHULL_DIR}/qset_r.h"
		"${QHULL_DIR}/stat_r.c"
		"${QHULL_DIR}/stat_r.h"
		"${QHULL_DIR}/geom_r.c"
		"${QHULL_DIR}/geom_r.h"
		"${QHULL_DIR}/user_r.c"
		"${QHULL_DIR}/user_r.h"
		"${QHULL_DIR}/qhull_ra.h"
		"${QHULL_DIR}/global_r.c"
		"${QHULL_DIR}/random_r.h"
		"${QHULL_DIR}/random_r.c"
		"${QHULL_DIR}/usermem_r.c"
		"${QHULL_DIR}/userprintf_r.c"
		"${QHULL_DIR}/rboxlib_r.c"
		"${QHULL_DIR}/userprintf_rbox_r.c"
		)
	target_include_directories(external-qhull INTERFACE "${QHULL_DIR}")
	set_property(TARGET external-qhull PROPERTY FOLDER External)
	target_link_libraries(external-qhull PRIVATE external-disable-warnings)
endif()
