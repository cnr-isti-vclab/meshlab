# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_XERCES "Allow download and use of Xerces-C sources" ON)
option(MESHLAB_ALLOW_SYSTEM_XERCES "Allow use of system-provided Xerces-C" ON)

find_package(XercesC)

# https://dlcdn.apache.org//xerces/c/3/sources/xerces-c-3.2.4.zip

if(MESHLAB_ALLOW_SYSTEM_XERCES AND TARGET XercesC::XercesC)

	message(STATUS "- XercesC - using system-provided library")
	add_library(external-xerces INTERFACE)
	target_link_libraries(external-xerces INTERFACE XercesC::XercesC)

elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_XERCES)

	set(XERCES_DIR ${CMAKE_CURRENT_LIST_DIR}/xerces-c-3.2.4)

	if (NOT EXISTS ${XERCES_DIR}/CMakeLists.txt)
		set(XERCES_C_LINK https://dlcdn.apache.org//xerces/c/3/sources/xerces-c-3.2.4.zip)
		download_and_unzip(${XERCES_C_LINK} ${CMAKE_CURRENT_LIST_DIR} "Xerces-C")
	endif()

	message(STATUS "- XercesC - using downloaded source")

	set(MESSAGE_QUIET ON)
	add_subdirectory(${XERCES_DIR} EXCLUDE_FROM_ALL)
	unset(MESSAGE_QUIET)

	add_library(external-xerces INTERFACE)
	target_link_libraries(external-xerces INTERFACE xerces-c)
	target_include_directories(
		external-xerces INTERFACE
		${XERCES_DIR}/src
		${CMAKE_CURRENT_BINARY_DIR}/xerces/src)
	add_library(XercesC::XercesC ALIAS external-xerces)

endif()
