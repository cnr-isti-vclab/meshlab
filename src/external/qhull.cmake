# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_QHULL "Allow use of bundled Qhull source" ON)
option(ALLOW_SYSTEM_QHULL "Allow use of system-provided QHull" ON)

find_package(Qhull COMPONENTS libqhull)

if(ALLOW_SYSTEM_QHULL AND TARGET Qhull::libqhull)
	message(STATUS "- qhull - using system-provided library")
	add_library(external-qhull INTERFACE)
	target_link_libraries(external-qhull INTERFACE Qhull::libqhull)
	target_compile_definitions(external-qhull INTERFACE SYSTEM_QHULL)
	target_include_directories(external-qhull INTERFACE ${QHULL_libqhull_INCLUDE_DIR}/libqhull)
elseif(ALLOW_BUNDLED_QHULL AND EXISTS "${QHULL_DIR}/src/qhull.h")
	message(STATUS "- qhull - using bundled source")
	add_library(
		external-qhull STATIC
		"${QHULL_DIR}/src/geom2.c"
		"${QHULL_DIR}/src/global.c"
		"${QHULL_DIR}/src/io.c"
		"${QHULL_DIR}/src/io.h"
		"${QHULL_DIR}/src/mem.c"
		"${QHULL_DIR}/src/mem.h"
		"${QHULL_DIR}/src/merge.c"
		"${QHULL_DIR}/src/merge.h"
		"${QHULL_DIR}/src/poly.c"
		"${QHULL_DIR}/src/poly.h"
		"${QHULL_DIR}/src/poly2.c"
		"${QHULL_DIR}/src/qconvex.c"
		"${QHULL_DIR}/src/qhull.c"
		"${QHULL_DIR}/src/qhull.h"
		"${QHULL_DIR}/src/qset.c"
		"${QHULL_DIR}/src/qset.h"
		"${QHULL_DIR}/src/stat.c"
		"${QHULL_DIR}/src/stat.h"
		"${QHULL_DIR}/src/geom.c"
		"${QHULL_DIR}/src/geom.h"
		"${QHULL_DIR}/src/user.c"
		"${QHULL_DIR}/src/user.h")
	target_include_directories(external-qhull INTERFACE "${QHULL_DIR}/src")
	set_property(TARGET external-qhull PROPERTY FOLDER External)
	target_link_libraries(external-qhull PRIVATE external-disable-warnings)
endif()
