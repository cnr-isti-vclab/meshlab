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
	set(BOOST_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/boost_1_75_0")
	set(BOOST_CHECK "${BOOST_DIR}/boost/version.hpp")

	if (NOT EXISTS ${BOOST_CHECK})
		set(BOOST_LINK https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.zip)
		set(BOOST_MD5 d3b276f6d22246171f93282910a1d583)
		download_and_unzip(
			NAME "Boost"
			MD5 ${BOOST_MD5}
			LINK ${BOOST_LINK}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- Boost - download failed.")
		endif()
	endif()

	if (EXISTS ${BOOST_CHECK})
		message(STATUS "- Boost - using downloaded source")
		add_library(external-boost INTERFACE)
		target_include_directories(external-boost INTERFACE "${BOOST_DIR}")
	endif()
endif()
