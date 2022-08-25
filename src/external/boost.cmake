# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_BOOST "Allow use of bundled boost source" ON)
option(ALLOW_SYSTEM_BOOST "Allow use of system-provided boost" ON)

find_package(Boost COMPONENTS thread)
set(BOOST_DIR ${CMAKE_CURRENT_LIST_DIR}/boost_1_75_0)

if(ALLOW_SYSTEM_BOOST AND TARGET Boost::boost)
	message(STATUS "- Boost - using system-provided library")
	add_library(external-boost INTERFACE)
	target_link_libraries(external-boost INTERFACE Boost::boost)
	if (TARGET Boost::thread)
		target_link_libraries(external-boost INTERFACE Boost::thread)
	endif()
elseif(ALLOW_BUNDLED_BOOST AND EXISTS "${BOOST_DIR}/boost/version.hpp")
	message(STATUS "- Boost - using bundled source")
	add_library(external-boost INTERFACE)
	target_include_directories(external-boost INTERFACE "${BOOST_DIR}")
endif()
