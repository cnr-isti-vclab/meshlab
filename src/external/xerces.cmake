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
	set(XERCES_C_VER 3.2.4)
	set(XERCES_C_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/xerces-c-${XERCES_C_VER})

	if (NOT EXISTS ${XERCES_C_DIR}/CMakeLists.txt)
		set(XERCES_C_LINK https://dlcdn.apache.org//xerces/c/3/sources/xerces-c-${XERCES_C_VER}.zip)
		download_and_unzip(${XERCES_C_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "Xerces-C")
	endif()

	message(STATUS "- XercesC - using downloaded source")

	set(MESSAGE_QUIET ON)
	add_subdirectory(${XERCES_C_DIR} EXCLUDE_FROM_ALL)
	unset(MESSAGE_QUIET)

	add_library(external-xerces INTERFACE)
	target_link_libraries(external-xerces INTERFACE xerces-c)
	target_include_directories(
		external-xerces INTERFACE
		${XERCES_C_DIR}/src
		${MESHLAB_EXTERNAL_BINARY_DIR}/xerces-c-${XERCES_C_VER}/src)
	add_library(XercesC::XercesC ALIAS external-xerces)
	install(TARGETS xerces-c DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
endif()
