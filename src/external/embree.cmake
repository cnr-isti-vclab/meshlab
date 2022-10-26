# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0
option(ALLOW_BUNDLED_EMBREE "Allow use of bundled embree dll (windows only)" ON)
option(ALLOW_SYSTEM_EMBREE "Allow use of system-provided embree" ON)

# looking for embree for the filter_embree
find_package(embree 3.0)
find_package(TBB)

set(TBB_WIN_DIR "${CMAKE_CURRENT_LIST_DIR}/oneapi-tbb-2021.6.0")
set(EMBREE_WIN_DIR "${CMAKE_CURRENT_LIST_DIR}/embree-3.13.5.x64.vc14.windows")
set(EMBREE_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/embree-3.13.5")
set(ISPC_DIR "${CMAKE_CURRENT_LIST_DIR}/ispc-v1.18.1-linux")

if(ALLOW_SYSTEM_EMBREE AND TARGET embree AND TBB_FOUND)
	message(STATUS "- embree - using system-provided library")
	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree tbb)
elseif(ALLOW_BUNDLED_EMBREE AND WIN32 AND EXISTS "${EMBREE_WIN_DIR}/lib/embree3.lib")
	message(STATUS "- embree - using bundled dll")

	set(TBB_DIR "${TBB_WIN_DIR}/lib/cmake/tbb")
	set(embree_DIR "${EMBREE_WIN_DIR}/lib/cmake/embree-3.13.5")
	find_package(embree)

	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree)

	if (DEFINED MESHLAB_LIB_OUTPUT_DIR)
		file(
			COPY
			    ${EMBREE_WIN_DIR}/bin/embree3.dll
				${EMBREE_WIN_DIR}/bin/tbb12.dll
			DESTINATION
			    ${MESHLAB_LIB_OUTPUT_DIR})
	endif()
	if (DEFINED MESHLAB_LIB_INSTALL_DIR)
		install(
			FILES
			    ${EMBREE_WIN_DIR}/bin/embree3.dll
				${EMBREE_WIN_DIR}/bin/tbb12.dll
			DESTINATION
			    ${MESHLAB_LIB_INSTALL_DIR})
	endif()
elseif(ALLOW_BUNDLED_EMBREE AND (UNIX AND NOT APPLE) AND EXISTS "${EMBREE_SRC_DIR}/CMakeLists.txt" )
	message(STATUS "- embree - using bundled source")

	if (EXISTS "${ISPC_DIR}/bin/ispc")
		set(EMBREE_ISPC_EXECUTABLE "${ISPC_DIR}/bin/ispc")
	else()
		set(EMBREE_ISPC_SUPPORT OFF)
	endif()

	set(EMBREE_TUTORIALS OFF)

	set(MESSAGE_QUIET ON)
	add_subdirectory(${EMBREE_SRC_DIR} EXCLUDE_FROM_ALL)
	unset(MESSAGE_QUIET)

	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree)

	install(TARGETS embree DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
else()
	message(STATUS "- embree - skipping embree library")
endif()
