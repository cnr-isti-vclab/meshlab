# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0
option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_EMBREE "Allow download and use of embree source (linux only)" ON)
option(MESHLAB_ALLOW_DOWNLOAD_DLL_EMBREE "Allow download and use of embree dll (windows only)" ON)
option(MESHLAB_ALLOW_SYSTEM_EMBREE "Allow use of system-provided embree" ON)

set(EMBREE_MAJOR 4)
set(EMBREE_MINOR 3.0)
set(EMBREE_VER "${EMBREE_MAJOR}.${EMBREE_MINOR}")

# looking for embree for the filter_embree
find_package(embree ${EMBREE_MAJOR})
find_package(TBB)

if(MESHLAB_ALLOW_SYSTEM_EMBREE AND TARGET embree AND TBB_FOUND)
	message(STATUS "- embree - using system-provided library")
	add_library(external-embree INTERFACE)
	target_link_libraries(external-embree INTERFACE embree)
elseif(MESHLAB_ALLOW_DOWNLOAD_DLL_EMBREE AND WIN32)
	set(EMBREE_WIN_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/embree-${EMBREE_VER}.x64.windows")
	set(EMBREE_WIN_CHECK "${EMBREE_WIN_DIR}/lib/embree${EMBREE_MAJOR}.lib")
	set(TBB_WIN_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/oneapi-tbb-2021.6.0")
	set(TBB_WIN_CHECK "${TBB_WIN_DIR}/lib/cmake/tbb/TBBConfig.cmake")

	if (NOT EXISTS ${TBB_WIN_CHECK})
		set(TBB_WIN_LINK https://github.com/oneapi-src/oneTBB/releases/download/v2021.6.0/oneapi-tbb-2021.6.0-win.zip)
		#set(TBB_WIN_MD5 )
		download_and_unzip(
			NAME "TBB"
			LINK ${TBB_WIN_LINK}
			#MD5 ${TBB_WIN_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- TBB - download failed.")
		endif()
	endif()

	if(NOT EXISTS ${EMBREE_WIN_CHECK})
		set(EMBREE_WIN_LINK https://github.com/embree/embree/releases/download/v${EMBREE_VER}/embree-${EMBREE_VER}.x64.windows.zip)
		#set(EMBREE_WIN_MD5 )
		download_and_unzip(
			NAME "embree dll"
			LINK ${EMBREE_WIN_LINK}
			#MD5 ${EMBREE_WIN_MD5}
			DIR ${EMBREE_WIN_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- embree dll - download failed.")
		endif()
	endif()

	if (EXISTS ${TBB_WIN_CHECK} AND EXISTS ${EMBREE_WIN_CHECK})
		message(STATUS "- embree - using downloaded dll")

		set(TBB_DIR "${TBB_WIN_DIR}/lib/cmake/tbb")
		set(embree_DIR "${EMBREE_WIN_DIR}/lib/cmake/embree-${EMBREE_VER}")
		find_package(embree)

		add_library(external-embree INTERFACE)
		target_link_libraries(external-embree INTERFACE embree)

		if (DEFINED MESHLAB_LIB_OUTPUT_DIR)
			file(
				COPY
					${EMBREE_WIN_DIR}/bin/embree${EMBREE_MAJOR}.dll
					${EMBREE_WIN_DIR}/bin/tbb12.dll
				DESTINATION
					${MESHLAB_LIB_OUTPUT_DIR})
		endif()
		if (DEFINED MESHLAB_LIB_INSTALL_DIR)
			install(
				FILES
					${EMBREE_WIN_DIR}/bin/embree${EMBREE_MAJOR}.dll
					${EMBREE_WIN_DIR}/bin/tbb12.dll
				DESTINATION
					${MESHLAB_LIB_INSTALL_DIR})
		endif()
	endif()
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_EMBREE AND (UNIX AND NOT APPLE) AND TBB_FOUND)
	set(EMBREE_SRC_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/embree-${EMBREE_VER}")
	set(EMBREE_CHECK "${EMBREE_SRC_DIR}/CMakeLists.txt")
	set(ISPC_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/ispc-v1.18.1-linux")
	set(ISPC_CHECK "${ISPC_DIR}/bin/ispc")

	if (NOT EXISTS ${ISPC_CHECK})
		set(ISPC_LINK
			https://github.com/ispc/ispc/releases/download/v1.18.1/ispc-v1.18.1-linux.tar.gz)
		#set(ISPC_MD5 )
		download_and_unzip(
			NAME "embree ispc"
			LINK ${ISPC_LINK}
			#MD5 ${ISPC_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- embree ispc - download failed.")
		endif()
	endif()

	if (NOT EXISTS ${EMBREE_CHECK})
		set(EMBREE_LINK
			https://github.com/embree/embree/archive/refs/tags/v${EMBREE_VER}.zip)
		#set(EMBREE_MD5 )
		download_and_unzip(
			NAME "embree"
			LINK ${EMBREE_LINK}
			#MD5 ${EMBREE_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- embree - download failed.")
		endif()
	endif()

	if (EXISTS ${EMBREE_CHECK})
		message(STATUS "- embree - using downloaded source")

		if (EXISTS ${ISPC_CHECK})
			set(EMBREE_ISPC_EXECUTABLE ${ISPC_CHECK})
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
	endif()
else()
	message(STATUS "- embree - skipping embree library")
endif()
