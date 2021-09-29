# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_QHULL "Allow use of bundled Qhull source" ON)
option(ALLOW_SYSTEM_QHULL "Allow use of system-provided QHull" ON)

find_package(Qhull 8 COMPONENTS qhull_r)
set(QHULL_DIR ${CMAKE_CURRENT_LIST_DIR}/qhull-2020.2/src)

if(ALLOW_SYSTEM_QHULL AND TARGET Qhull::qhull_r)
	message(STATUS "- qhull - using system-provided library")
	add_library(external-qhull INTERFACE)
	target_link_libraries(external-qhull INTERFACE Qhull::qhull_r)
	target_compile_definitions(external-qhull INTERFACE SYSTEM_QHULL)
elseif(ALLOW_BUNDLED_QHULL AND EXISTS "${QHULL_DIR}/libqhull_r/libqhull_r.h")
	message(STATUS "- qhull - using bundled source")
	add_library(
		external-qhull STATIC
		"${QHULL_DIR}/libqhull_r/geom2_r.c"
		"${QHULL_DIR}/libqhull_r/io_r.c"
		"${QHULL_DIR}/libqhull_r/io_r.h"
		"${QHULL_DIR}/libqhull_r/mem_r.c"
		"${QHULL_DIR}/libqhull_r/mem_r.h"
		"${QHULL_DIR}/libqhull_r/merge_r.c"
		"${QHULL_DIR}/libqhull_r/merge_r.h"
		"${QHULL_DIR}/libqhull_r/poly_r.c"
		"${QHULL_DIR}/libqhull_r/poly_r.h"
		"${QHULL_DIR}/libqhull_r/poly2_r.c"
		"${QHULL_DIR}/libqhull_r/libqhull_r.c"
		"${QHULL_DIR}/libqhull_r/libqhull_r.h"
		"${QHULL_DIR}/libqhull_r/qset_r.c"
		"${QHULL_DIR}/libqhull_r/qset_r.h"
		"${QHULL_DIR}/libqhull_r/stat_r.c"
		"${QHULL_DIR}/libqhull_r/stat_r.h"
		"${QHULL_DIR}/libqhull_r/geom_r.c"
		"${QHULL_DIR}/libqhull_r/geom_r.h"
		"${QHULL_DIR}/libqhull_r/user_r.c"
		"${QHULL_DIR}/libqhull_r/user_r.h"
		"${QHULL_DIR}/libqhull_r/qhull_ra.h"
		"${QHULL_DIR}/libqhull_r/global_r.c"
		"${QHULL_DIR}/libqhull_r/random_r.h"
		"${QHULL_DIR}/libqhull_r/random_r.c"
		"${QHULL_DIR}/libqhull_r/usermem_r.c"
		"${QHULL_DIR}/libqhull_r/userprintf_r.c"
		"${QHULL_DIR}/libqhull_r/rboxlib_r.c"
		"${QHULL_DIR}/libqhull_r/userprintf_rbox_r.c"
		)
	target_include_directories(external-qhull INTERFACE "${QHULL_DIR}")
	set_property(TARGET external-qhull PROPERTY FOLDER External)
	target_link_libraries(external-qhull PRIVATE external-disable-warnings)
endif()
