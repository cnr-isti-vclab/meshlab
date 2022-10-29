# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_BOOST "Allow download and use of boost source" ON)
option(MESHLAB_ALLOW_SYSTEM_BOOST "Allow use of system-provided boost" ON)

find_package(Boost COMPONENTS thread)

if(MESHLAB_ALLOW_SYSTEM_BOOST AND TARGET Boost::boost)
	message(STATUS "- Boost - using system-provided library")
	add_library(external-boost INTERFACE)
	target_link_libraries(external-boost INTERFACE Boost::boost)
	if (TARGET Boost::thread)
		target_link_libraries(external-boost INTERFACE Boost::thread)
	endif()
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_BOOST)
	set(BOOST_DIR ${CMAKE_CURRENT_LIST_DIR}/boost_1_75_0)
	set(BOOST_ZIP ${CMAKE_CURRENT_LIST_DIR}/boost_1_75_0.zip)

	if (NOT EXISTS "${BOOST_DIR}/boost/version.hpp")
		message("Downloading Boost...")
		set(BOOST_LINK https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.zip)

		file(DOWNLOAD ${BOOST_LINK} ${BOOST_ZIP})
		message("Boost downloaded.")
		message("Extracting Boost archive...")
		file(ARCHIVE_EXTRACT
			INPUT ${BOOST_ZIP}
			DESTINATION ${CMAKE_CURRENT_LIST_DIR})
		message("Boost archive extracted.")
		file(REMOVE ${BOOST_ZIP})
	endif()

	message(STATUS "- Boost - using downloaded source")
	add_library(external-boost INTERFACE)
	target_include_directories(external-boost INTERFACE "${BOOST_DIR}")
endif()
